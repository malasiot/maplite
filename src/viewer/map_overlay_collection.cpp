#include "map_overlay_collection.hpp"
#include "map_overlay_manager.hpp"
#include "map_overlay.hpp"

#include <QDomDocument>
#include <QFileInfo>
#include <QFile>

#include "Database.h"

using namespace std ;

MapFeatureCollection::MapFeatureCollection(MapFeatureIndex *idx): feature_index_(idx), storage_id_(-1)
{

}

bool MapFeatureCollection::addFeatures(const QVector<quint64> &features)
{
    SQLite::Session session(feature_index_->db()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        SQLite::Transaction trans(con) ;
        SQLite::Command cmd(con, "INSERT INTO memberships (feature_id, collection_id) VALUES (?, ?)") ;

        for( int i=0 ; i<features.size() ; i++ )
        {
            cmd.bind(1, (long long int)features[i]) ;
            cmd.bind(2, (long long int)storage_id_) ;
            cmd.exec() ;
            cmd.clear();
        }

        trans.commit() ;

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        return false ;
    }

}

bool MapFeatureCollection::write(quint64 folder_id)
{
    SQLite::Session session(feature_index_->db()) ;
    SQLite::Connection &con = session.handle() ;

    try {

        int counter = 0 ;
        SQLite::Query q(con, "SELECT id FROM collections WHERE folder_id = ? AND name = ? LIMIT 1;") ;

        QString name_unique = name_ ;

        while (1)
        {
            q.bind(1, (quint64)folder_id) ;
            q.bind(2, name_unique.toUtf8().data()) ;
            SQLite::QueryResult res = q.exec() ;

            if ( res ) name_unique = name_ + QString(" (%1)").arg(++counter) ;
            else break ;

            q.clear() ;
        }

        string sql = "INSERT INTO collections (folder_id, name, attributes) VALUES (?, ?, ?)" ;

        SQLite::Command cmd(con, sql) ;

        cmd.bind((long long int)folder_id) ;
        cmd.bind(name_unique.toUtf8().data()) ;

        QByteArray ba ;
        QDataStream ds(&ba, QIODevice::WriteOnly) ;

        ds << attributes_ ;

        cmd.bind((const void *)ba.data(), ba.size()) ;
        cmd.exec() ;

        storage_id_ = (qint64)con.last_insert_rowid() ;
        name_ = name_unique ;

        return true ;
    }
    catch ( SQLite::Exception &e)
    {
        return false ;
    }
}


MapFeatureCollection *MapFeatureCollection::importGpx(const QString &fileName, quint64 folder_id, MapFeatureIndex *fidx)
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

    MapFeatureCollection *col = new MapFeatureCollection(fidx) ;

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
    col->path_ = folder_id ;

    // parse waypoints

    QDomNodeList wptList = docElem.elementsByTagName("wpt") ;

    QVector<MapFeature *> featureList ;

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

        MarkerFeature *marker = new MarkerFeature(wptName) ;

        Q_FOREACH(const QString &key, wptMap.keys())
            marker->attributes_.insert(key, wptMap.value(key)) ;

        marker->setPoint(QPointF(lon, lat)) ;

        featureList.append(marker) ;

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

                PolygonFeature *poly = new PolygonFeature(trksegName) ;

                qDebug() << j << trksegName ;
                for(int k=0 ; k<trkptList.count() ; k++ )
                {
                    QDomElement trkpt = trkptList.item(k).toElement() ;
                    if ( trkpt.isNull() ) continue ;

                    double lat = trkpt.attribute("lat").toDouble() ;
                    double lon = trkpt.attribute("lon").toDouble() ;

                    poly->addPoint(QPointF(lon, lat)) ;
                }

                Q_FOREACH(const QString &key, trkMap.keys())
                    poly->attributes_.insert(key, trkMap.value(key)) ;

                featureList.append(poly) ;

            }
        }

    }

    if ( col->write(folder_id) )
    {
        if ( !fidx->write(featureList, col->storage_id_) )
        {
            fidx->deleteCollection(col->storage_id_) ;
            qDeleteAll(featureList) ;
            delete col ;
            return 0 ;
        }
        else {
            qDeleteAll(featureList) ;
            return col ;
        }
    }
    else
    {
         qDeleteAll(featureList) ;
         delete col ;
         return 0 ;
    }
}
