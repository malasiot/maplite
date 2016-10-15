#include "DataImport.h"
#include "MapFeature.h"
#include "MapFeatureIndex.h"

#include <QStringList>
#include <QDebug>
#include <QDomDocument>
#include <QXmlDefaultHandler>
#include <QFile>
#include <QBuffer>
#include <QStack>

#include <minizip/unzip.h>

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

CollectionTreeNode *importKmz(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx)
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
            res = importKml(&buffer, folder_id, fidx) ;
        }
    }

    unzClose( zipfile );

    return res ;
}

CollectionTreeNode *importKml(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx)
{
    QFile file(fileName) ;
    if ( !file.open(QIODevice::ReadOnly) ) return 0 ;

    return importKml(&file, folder_id, fidx) ;
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
    KmlHandler(MapFeatureIndex *index): index_(index) {
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
    QString current_text_;

    QStack<CollectionTreeNode *> nodes_ ;
    QStack<NodeType> types_ ;
    MapFeatureIndex *index_ ;
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
            MarkerFeature *marker = new MarkerFeature(placemark_->name_) ;

            marker->setPoint(placemark_->geometry_.at(0)) ;

            current->feature_list_.append(MapFeaturePtr(marker)) ;

        }
        else if ( placemark_->type_ == 1 )
        {
            PolygonFeature *marker = new PolygonFeature(placemark_->name_) ;

            Q_FOREACH(const QPointF &pt, placemark_->geometry_)
                marker->addPoint(pt) ;

            current->feature_list_.append(MapFeaturePtr(marker)) ;
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


static void createFoldersRecursive(CollectionTreeNode *node, quint64 parent_id, MapFeatureIndex *fidx)
{
    QString unique_name ;
    quint64 item_id ;

    fidx->addNewFolder(node->name_, parent_id, unique_name, item_id) ;

    node->name_ = unique_name ;
    node->folder_id_ = item_id ;

    if ( !node->feature_list_.isEmpty() )
    {
        CollectionData *col = new CollectionData ;

        node->collection_ = col ;

        QString unique_col_name ;
        quint64 collection_id ;
        QMap<QString, QVariant> attr ;

        fidx->addNewCollection(node->name_, node->folder_id_, attr, unique_col_name, collection_id) ;

        col->name_ = unique_col_name ;
        col->id_ = collection_id ;
        col->folder_ = item_id ;

        fidx->write(node->feature_list_, col->id_ )  ;
    }

    Q_FOREACH(CollectionTreeNode *child, node->children_)
        createFoldersRecursive(child, item_id, fidx) ;
}

CollectionTreeNode *importKml(QIODevice *data, quint64 folder_id, MapFeatureIndex *fidx)
{
    KmlHandler handler(fidx) ;
    if ( !handler.parse(data) ) return 0 ;

    CollectionTreeNode *root = handler.root_node_ ;

    if ( root ) createFoldersRecursive(root, folder_id, fidx) ;

    return root ;
}
