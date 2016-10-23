#ifndef __KML_READER_HPP__
#define __KML_READER_HPP__

#include "overlay_import.hpp"

class KMLReader: public OverlayReader {

public:

    KMLReader() ;

    CollectionTreeNode *import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) ;
    QString filters() const { return "*.kml;*.kmz" ; }
    QString description() const { return "Google Keyhole Markup Language" ; }

private:

    CollectionTreeNode *importKml(QIODevice *data, const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) ;
    CollectionTreeNode *importKmz(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) ;

    static KMLReader instance_ ;
    QString default_name_ ;
};

#endif
