#include "gpx_reader.hpp"

#include "map_overlay_collection.hpp"
#include "map_overlay_manager.hpp"
#include "map_overlay.hpp"

#include "overlays/gps_overlays.hpp"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

#include <qplugin.h>


CollectionTreeNode *GPXReader::import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)
{
    // try to load the document from file

    QDomDocument doc("gpx");
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) return 0 ;

    if (!doc.setContent(&file)) {
       file.close();
       return 0 ;
    }
    file.close();

    // check if this is a GPX file

    QDomElement docElem = doc.documentElement();
    if ( docElem.tagName() != "gpx" ) return 0 ;

    // create and populate the collection

    CollectionData *col = new CollectionData ;

    // parse metadata

    QString collectionName ;
    QMap<QString, QString> metaMap ;

    QDomElement metadata = docElem.firstChildElement("metadata") ;

    QDomNodeList children = metadata.childNodes() ;

    for(int j=0 ; j<children.count() ; j++ )
    {
        QDomElement childElem = children.item(j).toElement() ;

        if ( childElem.isNull() ) continue ;

        QString tagName = childElem.tagName() ;
        if ( tagName == "extensions") continue ;
        if ( tagName == "name" ) collectionName = childElem.text() ;

        metaMap[tagName] = childElem.text() ;

    }

    Q_FOREACH(const QString &key, metaMap.keys())
        col->attributes_.insert(key, metaMap.value(key)) ;

    if ( collectionName.isEmpty() )
        collectionName = QFileInfo(fileName).baseName() ;

    col->name_ = collectionName ;
    col->folder_ = folder_id ;

    // parse waypoints

    QDomNodeList wptList = docElem.elementsByTagName("wpt") ;

    QVector<MapOverlayPtr> featureList ;

    for(int i=0 ; i<wptList.count() ; i++ )
    {
        QDomElement elem = wptList.item(i).toElement() ;
        if ( elem.isNull() ) continue ;

        double lat = elem.attribute("lat").toDouble() ;
        double lon = elem.attribute("lon").toDouble() ;

        QDomNodeList children = elem.childNodes() ;

        // read attributes

        QMap<QString, QString> wptMap ;
        QString wptName ;

        for(int j=0 ; j<children.count() ; j++ )
        {
            QDomElement childElem = children.item(j).toElement() ;

            if ( childElem.isNull() ) continue ;

            QString tagName = childElem.tagName() ;
            if ( tagName == "name" )
                wptName = childElem.text() ;

            wptMap[tagName] = childElem.text() ;
        }

        if ( wptName.isEmpty() ) continue ;

        // create marker feature

        MarkerOverlay *marker = new MarkerOverlay(wptName) ;

        Q_FOREACH(const QString &key, wptMap.keys())
            marker->attributes_.insert(key, wptMap.value(key)) ;

        marker->setPoint(QPointF(lon, lat)) ;

        featureList.append(MapOverlayPtr(marker)) ;

    }


    // write tracks

    int nseg = 0;
    QDomNodeList trkList = docElem.elementsByTagName("trk") ;

    for(int i=0 ; i<trkList.count() ; i++ )
    {

        QDomElement elem = trkList.item(i).toElement() ;
        if ( elem.isNull() ) continue ;

        QDomNodeList children = elem.childNodes() ;

        // parse track attributes

        QMap<QString, QString> trkMap ;
        QString trkName ;

        for(int j = 0 ; j<children.count() ; j++ )
        {
            QDomElement childElem = children.item(j).toElement() ;

            if ( childElem.isNull() ) continue ;

            if ( childElem.tagName() == "trkseg" ) continue ;
            if ( childElem.tagName() == "extensions" ) continue ;

            QString tagName = childElem.tagName() ;

            if ( tagName == "name" ) trkName = childElem.text() ;

            trkMap[tagName] = childElem.text() ;
        }

        QDomNodeList trkSegList = elem.elementsByTagName("trkseg") ;

        int cseg = 0 ;

        for(int j=0 ; j<trkSegList.size() ; j++ )
        {

            QDomElement childElem = trkSegList.item(j).toElement() ;

            if ( childElem.isNull() ) continue ;

            if ( childElem.tagName() == "trkseg")
            {

                QDomNodeList trkptList = childElem.elementsByTagName("trkpt") ;
                if ( trkptList.count() == 0 ) continue ;

                nseg ++ ;
                cseg ++ ;

                QString trksegName = trkName ;

                if ( trkName.isEmpty() ) {
                    trksegName = "Track" ;
                    trksegName += QString(" Segment %1").arg(nseg) ;
                }
                else trksegName += QString(" Segment %1").arg(cseg) ;

                GPSTrackOverlay *poly = new GPSTrackOverlay(trksegName) ;

                for(int k=0 ; k<trkptList.count() ; k++ )
                {
                    QDomElement trkpt = trkptList.item(k).toElement() ;
                    if ( trkpt.isNull() ) continue ;

                    double lat = trkpt.attribute("lat").toDouble() ;
                    double lon = trkpt.attribute("lon").toDouble() ;

                    double ele = 0 ;
                    bool ok ;
                    QDomElement e = trkpt.firstChildElement("ele") ;
                    if ( !e.isNull() ) ele = e.text().toFloat(&ok) ;

                    poly->addPoint(QPointF(lon, lat)) ;
                    poly->addElevation(ele) ;
                }

                Q_FOREACH(const QString &key, trkMap.keys())
                    poly->attributes_.insert(key, trkMap.value(key)) ;

                featureList.append(MapOverlayPtr(poly)) ;

            }
        }

    }

    CollectionTreeNode *node = new CollectionTreeNode ;
    node->collection_ = col ;
    col->name_ = fidx->uniqueCollectionName(col->name_, col->folder_) ;

    if ( fidx->addNewCollection(col->name_, col->folder_, col->attributes_,  col->id_ ) )
    {
        if ( !fidx->write(featureList, col->id_) )
        {
            fidx->deleteCollection(col->id_) ;

            delete node ;
            return nullptr ;
        }
        else {
            return node ;
        }
    }
    else
    {
         delete node ;
         return nullptr ;
    }
}

GPXReader::GPXReader() {
    OverlayImportManager::instance().registerReader(this);
}

GPXReader GPXReader::instance_ ;
