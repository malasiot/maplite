using namespace std ;
#include "pugixml.hpp"
#include "kml_reader.hpp"

#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std ;
using namespace geojson ;

static bool parse_coordinates(const std::string &str, PointList &coords) {

    if ( str.empty() ) return false ;

    string trimmed = str ;

    boost::trim_if(trimmed, boost::is_any_of("\n\r\t ")) ;

    std::vector<string> tokens ;
    boost::split(tokens, trimmed, boost::is_any_of(", \t\n\r"),boost::algorithm::token_compress_on);

    if ( tokens.size() %3 ) return false ;

    try {
        for(int i=0 ; i<tokens.size() ; i+=3)
        {
            float lon = stof(tokens[i]) ;
            float lat = stof(tokens[i+1]) ;
            float ele = stof(tokens[i+2]) ;

            coords.push_back({lon, lat, ele}) ;
        }
    } catch ( std::invalid_argument ) {
        return false ;
    }

    return true ;
}

bool kml_load(const pugi::xml_document &doc, FeatureCollection &col) {

    // we do not parse metadata at the moment since geojson does not support them

    auto plc = doc.select_nodes("descendant::Placemark") ;

    for( auto xpm: plc) {
        pugi::xml_node pm = xpm.node() ;

        if ( pm.empty() ) continue ;

        Feature f ;

        string name = pm.child("name").text().as_string() ;
        string desc = pm.child("description").text().as_string() ;

        f.id_ = pm.attribute("id").as_string() ;
        if ( !name.empty() ) f.properties_.add("name", name) ;
        if ( !desc.empty() ) f.properties_.add("description", desc) ;

        std::shared_ptr<GeometryCollection> gcol(new GeometryCollection) ;

        for ( auto xpi: pm.select_nodes("descendant::Point")) {
            pugi::xml_node node = xpi.node() ;

            if ( !node.empty() ) {  // parse point geometry
                PointList coordinates ;
                if ( parse_coordinates(node.child("coordinates").text().as_string(), coordinates) ) {
                    gcol->geometries_.push_back(std::make_shared<PointGeometry>(coordinates[0])) ;
                }
            }
        }

        for ( auto xpi: pm.select_nodes("descendant::LineString")) {
            pugi::xml_node node = xpi.node() ;

            if ( !node.empty() ) {  // parse line geometry
                PointList coordinates ;
                if ( parse_coordinates(node.child("coordinates").text().as_string(), coordinates) ) {
                    gcol->geometries_.push_back(std::make_shared<LineStringGeometry>(coordinates)) ;
                }
            }
        }

        for ( auto xpi: pm.select_nodes("descendant::Polygon")) {
            pugi::xml_node node = xpi.node() ;

            if ( !node.empty() ) {  // parse exterior/interior rings

                std::shared_ptr<PolygonGeometry> pgeom(new PolygonGeometry) ;
                PointList coordinates ;

                pugi::xml_node ep = node.select_single_node("outerBoundaryIs/LinearRing/coordinates").node() ;
                if ( parse_coordinates(ep.text().as_string(), coordinates) ) {
                    pgeom->coordinates_.push_back(std::move(coordinates)) ;
                }
                else continue ;

                for( auto ipx: node.select_nodes("innerBoundaryIs/LinearRing/coordinates") )
                {
                    PointList coordinates ;
                    pugi::xml_node ip = ipx.node() ;

                    if ( !ip.empty() ) {
                        if ( parse_coordinates(ip.text().as_string(), coordinates) ) {
                                pgeom->coordinates_.push_back(std::move(coordinates)) ;
                        }
                    }
                }

                gcol->geometries_.push_back(pgeom) ;
            }
        }

        if ( gcol->geometries_.size() == 1 )
            f.geometry_ = gcol->geometries_[0] ;
        else
            f.geometry_ = gcol ;

        col.features_.push_back(std::move(f)) ;

    }



    return true ;
}



bool KMLReader::load_from_file(const std::string &file_name, FeatureCollection &col)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());

    if ( !result ) {
        cerr << "GPX file parsed with errors\n";
        cerr << "error parsing file (" <<
                file_name << "): " << result.description() << "\n";
        return false ;
    }
    else
        return kml_load(doc, col) ;

}

bool KMLReader::load_from_string(const std::string &bytes, FeatureCollection &col)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(bytes.c_str());

    if ( !result ) {
        cerr << "GPX content parsed with errors\n";
        cerr << result.description() << "\n";
        return false ;
    }
    else
        return kml_load(doc, col) ;
}


#if 0


bool KmlPlacemark::parseCoordinates(const QString &coords)
{



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

#endif
