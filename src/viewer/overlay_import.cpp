#include "overlay_import.hpp"


CollectionTreeNode *OverlayImportManager::read(const QString &fileName, quint64 folder_id, QSharedPointer<MapOverlayManager> fidx)
{
    for( OverlayReader *reader: readers_ ) {
        CollectionTreeNode *data = reader->import(fileName, folder_id, fidx) ;
        if ( data ) return data ;
    }
}

QString OverlayImportManager::filter() const {
    QMap<QString, QString> filter_map ;

    QString res = "All supported formats (" ;

    for( OverlayReader *reader: readers_ ) {
        QString desc = reader->description() ;
        QString filters = reader->filters() ;

        filters.replace(";", " ") ;
        res += filters + " " ;
        if ( filter_map.contains(desc) ) filter_map[desc] += " " + filters ;
        else filter_map[desc] = filters ;
    }
    res.chop(1);

    res += ")" ;

    foreach ( const QString &d, filter_map.keys() ) {
        if ( !res.isEmpty() ) res += ";;" ;
        res += d ;
        res += " (" ;
        res += filter_map[d] ;
        res += ")" ;
    }

    return res ;

}
