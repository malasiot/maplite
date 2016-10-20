#ifndef __GPX_READER_HPP__
#define __GPX_READER_HPP__

#include "overlay_import.hpp"

class GPXReader: public OverlayReader {

public:

    GPXReader() ;

    QString filters() const { return "*.gpx" ; }
    QString description() const { return "GPS Exchange Format" ; }

    CollectionTreeNode *import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) ;

    static GPXReader instance_ ;
};

#endif
