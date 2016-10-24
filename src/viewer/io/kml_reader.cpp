#include "kml_reader.hpp"
#include "map_overlay.hpp"
#include "map_overlay_manager.hpp"
#include "overlays/gps_overlays.hpp"

#include <QStringList>
#include <QDebug>
#include <QDomDocument>
#include <QXmlDefaultHandler>
#include <QFile>
#include <QBuffer>
#include <QStack>
#include <QFileInfo>

#include "minizip/unzip.h"

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192

bool getKmzFileList(unzFile zipfile, QStringList &res)
{
    // Get info about the zip file

    unz_global_info global_info;

    if ( unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK ) return false ;

    // Loop to extract all files
    uLong i;

    for ( i = 0; i < global_info.number_entry; ++i )
    {
        // Get info about current file.
        unz_file_info file_info;

        char filename[ MAX_FILENAME ];

        if ( unzGetCurrentFileInfo( zipfile,  &file_info, filename, MAX_FILENAME,  NULL, 0, NULL, 0 ) != UNZ_OK )
            return false ;

        QString qfileName = QString::fromUtf8(filename) ;

        if ( qfileName.endsWith('/') || qfileName.endsWith('\\') ) ;
        else
            res.append(qfileName) ;
    }

    return true ;
}

QByteArray readKmlFile(unzFile zipfile, const QString &fileName)
{
    QByteArray res ;

    if ( unzLocateFile(zipfile, fileName.toUtf8().data(), 0) != UNZ_OK ) return res ;
    if ( unzOpenCurrentFile( zipfile ) != UNZ_OK ) return res ;

    int error = UNZ_OK ;

    // Buffer to hold data read from the zip file.

    char read_buffer[ READ_SIZE ];

    do
    {
        error = unzReadCurrentFile( zipfile, read_buffer, READ_SIZE );
        if ( error < 0 ) {
            unzCloseCurrentFile( zipfile );
            return res ;
        }

        res.append(QByteArray(read_buffer, error)) ;
    } while ( error > 0 );


    unzCloseCurrentFile( zipfile );

    return res ;
}

CollectionTreeNode *KMLReader::importKmz(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)
{
    CollectionTreeNode *res = 0 ;

    // Open the zip file
    unzFile zipfile = unzOpen( fileName.toUtf8().data() );
    if ( zipfile == NULL ) return 0 ;

    QStringList files ;

    if ( !getKmzFileList(zipfile, files) ) {
        unzClose(zipfile) ;
        return 0 ;
    }

    Q_FOREACH( const QString &kml_file, files) {
        QByteArray data = readKmlFile(zipfile, kml_file) ;

        if ( !data.isEmpty() )
        {
            QBuffer buffer(&data) ;
            res = importKml(&buffer, QFileInfo(fileName).baseName(), folder_id, fidx) ;
        }
    }

    unzClose( zipfile );

    return res ;
}

static bool isZip(const QString &fileName) {
    QFile file(fileName) ;
    if ( !file.open(QIODevice::ReadOnly) ) return false ;
    QByteArray ba = file.read(4) ;
    return ( ba.at(0) == 0x50 ) && ( ba.at(1) == 0x4b ) && ( ba.at(2) == 0x03 ) && ( ba.at(3) == 0x04 ) ;
}

CollectionTreeNode *KMLReader::import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)
{
    if ( isZip(fileName) ) return importKmz(fileName, folder_id, fidx) ;
    else {
        QFile file(fileName) ;
        if ( !file.open(QIODevice::ReadOnly) ) return nullptr ;
        return importKml(&file,  QFileInfo(fileName).baseName(), folder_id, fidx) ;
    }
}

struct KmlPlacemark {

    KmlPlacemark(): type_(-1), is_outer_boundary_(true) {}
    QString name_, style_, description_ ;
        QVector<QPointF> geometry_ ;
    QVector<float> altitude_ ;
    int type_ ;
    QString time_ ;
    bool is_outer_boundary_ ;

    bool parseCoordinates(const QString &coords) ;

};

bool KmlPlacemark::parseCoordinates(const QString &coords)
{
    QStringList tokens = coords.split(QRegExp("[\\s,]+"), QString::SkipEmptyParts) ;

    if ( tokens.size() %3 ) return false ;

    for(int i=0 ; i<tokens.size() ; i+=3)
    {
        bool ok ;
        float lat = tokens.at(i).toFloat(&ok) ;
        if ( !ok ) return false ;
        float lon = tokens.at(i+1).toFloat(&ok) ;
        if ( !ok ) return false ;
        float ele = tokens.at(i+2).toFloat(&ok) ;
        if ( !ok ) return false ;
        geometry_.push_back(QPointF(lat, lon)) ;
        altitude_.push_back(ele) ;
    }


}

class KmlHandler : public QXmlDefaultHandler
{
public:
    KmlHandler(QSharedPointer<MapOverlayManager> index, const QString &dname): index_(index), default_name_(dname) {
        root_node_ = new CollectionTreeNode ;
        nodes_.push_back(root_node_) ;
    }
    ~KmlHandler() {}

    bool parse(QIODevice *dev);

    CollectionTreeNode *root_node_ ;

protected:

    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);

    bool characters(const QString &str);

    bool fatalError(const QXmlParseException &exception);


private:

    enum NodeType { FolderNode, PlacemarkNode, OtherNode } current_node_type_ ;
    QString current_text_, default_name_;

    QStack<CollectionTreeNode *> nodes_ ;
    QStack<NodeType> types_ ;
    QSharedPointer<MapOverlayManager> index_ ;
    KmlPlacemark *placemark_ ;
};

bool KmlHandler::parse(QIODevice *source)
{
    QXmlInputSource inputSource(source);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    return reader.parse(inputSource);
}

bool KmlHandler::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes &attributes)
{
    current_text_.clear();

    if ( qName == "Folder" ) {

        CollectionTreeNode *child = new CollectionTreeNode ;
        CollectionTreeNode *current = nodes_.back() ;

        current->children_.push_back(child) ;
        child->parent_ = current ;

        nodes_.append(child) ;
        types_.push_back(FolderNode) ;
    }
    else if ( qName == "Document" )
    {
        types_.push_back(FolderNode) ;
    }
    else if (qName == "Placemark") {
        types_.push_back(PlacemarkNode) ;
        placemark_ = new KmlPlacemark ;
    }
    else if ( qName == "Point" ) {
        placemark_->type_ = 0 ;
        types_.push_back(OtherNode) ;
    }
    else if ( qName == "LineString" ) {
        placemark_->type_ = 1 ;
        types_.push_back(OtherNode) ;
    }
    else if ( qName == "Polygon" ) {
        placemark_->type_ = 2 ;
        types_.push_back(OtherNode) ;
    }
    else if ( qName == "IsOuterBoundary" ) {
        placemark_->is_outer_boundary_ = true ;
        types_.push_back(OtherNode) ;
    }
    else if ( qName == "IsInnerBoundary" ) {
        placemark_->is_outer_boundary_ = false ;
        types_.push_back(OtherNode) ;
    }
    else {
        types_.push_back(OtherNode) ;
    }
    return true;
}

bool KmlHandler::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString &qName)
{
    types_.pop_back() ;

    if ( types_.empty() ) return true ;

    NodeType type = types_.back() ;

    if (qName == "Folder" || qName == "Document") {
        CollectionTreeNode *current = nodes_.back() ;
        if ( current->name_.isEmpty() ) current->name_ = default_name_ ;

        nodes_.pop_back() ;
    }
    else if ( qName == "name" ) {
        CollectionTreeNode *current = nodes_.back() ;

        if ( type == FolderNode ) {
            current->name_ = current_text_ ;
        }
        else if ( type == PlacemarkNode ) {
            placemark_->name_ = current_text_ ;
        }
    }
    else if ( qName == "coordinates" )  {
        placemark_->parseCoordinates(current_text_) ;
    }
    else if ( qName == "time" ) {
        placemark_->time_ = current_text_ ;
    }
    else if (qName == "Placemark") {

        // write

        CollectionTreeNode *current = nodes_.back() ;

        if ( placemark_->type_ == 0 )
        {
            MarkerOverlay *marker = new MarkerOverlay(placemark_->name_) ;

            marker->setPoint(placemark_->geometry_.at(0)) ;

            current->overlay_list_.append(MapOverlayPtr(marker)) ;

        }
        else if ( placemark_->type_ == 1 )
        {
            LinestringOverlay *ovr = new LinestringOverlay(placemark_->name_) ;

            Q_FOREACH(const QPointF &pt, placemark_->geometry_)
                ovr->addPoint(pt) ;

            current->overlay_list_.append(MapOverlayPtr(ovr)) ;
        }

        delete placemark_ ;
    }


    return true;
}

bool KmlHandler::characters(const QString &str)
{
    current_text_ += str;
    return true;
}

bool KmlHandler::fatalError(const QXmlParseException &exception)
{
    qDebug() << "Parse error at line " << exception.lineNumber()
             << ", " << "column " << exception.columnNumber() << ": "
             << qPrintable(exception.message()) ;
    return false;
}



CollectionTreeNode *KMLReader::importKml(QIODevice *data, const QString &defaultName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)
{
    KmlHandler handler(fidx, defaultName) ;
    if ( !handler.parse(data) ) return nullptr ;

    CollectionTreeNode *root = handler.root_node_ ;

    // we add first the tree ( folders and collections )
    fidx->addCollectionTree(root, folder_id) ;
    // and then the actually geometries so that they can be included in a transaction
    fidx->addCollectionTreeOverlays(root) ;

    return root ;
}



KMLReader::KMLReader() {
    OverlayImportManager::instance().registerReader(this);
}

KMLReader KMLReader::instance_ ;
