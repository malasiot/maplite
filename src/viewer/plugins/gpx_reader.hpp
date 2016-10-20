#ifndef __GPX_READER_HPP__
#define __GPX_READER_HPP__

#include "overlay_import.hpp"

class GPXReader: public QObject, public OverlayImportInterface {
    Q_OBJECT
    Q_INTERFACES(OverlayImportInterface)

public:

    GPXReader() {}
    ~GPXReader() {}

    virtual struct CollectionTreeNode *import(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx) ;
};

#endif
