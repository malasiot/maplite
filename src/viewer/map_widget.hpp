#ifndef __MAP_WIDGET_H__
#define __MAP_WIDGET_H__

#include <QWidget>
#include <QSet>
#include <QMap>
#include <QMutex>
#include <QRunnable>
#include <QThreadPool>
#include <QDir>
#include <QCache>
#include <QUndoGroup>

#include "tile_provider.hpp"
#include "map_overlay.hpp"
#include "popup.hpp"

#include <memory>

class MapTool ;
class MapOverlayManager ;

class MapWidget: public QWidget {
    Q_OBJECT

public:

    enum ZoomMode { Center, Mouse } ;

    MapWidget(QWidget *parent = 0) ;

    void setBasemap(const std::shared_ptr<TileProvider> &tiles) ;

    // Set map center
    void setCenter(float lon, float lat, int zoom) ;

    void setCacheDir(const QString &dir) ;

    void setTool(MapTool *tool) ;

    void scroll(const QPoint &q);

    // get display coordinates from widget coordinates
    QPoint positionToDisplay(const QPoint &pos) ;

    // get window position from display coordinates
    QPoint displayToPosition(const QPoint &coord);

    // get lat/lot coordinates from display coordinates
    QPointF displayToCoords(const QPoint &p) ;
    // get display coordinates from lat/lon coordinates
    QPoint coordsToDisplay(const QPointF &q) ;

    QRect coordsToWindow(const QRectF &coords) ;
    QRectF windowToCoords(const QRect &rect);

    // scroll view so that mouse position becomes visible. Returns true if scrolling took place
    bool ensureVisible(const QPoint &pos);

    // zoom and center screen so that regions is maximized
    void zoom(const QRect &extents) ;

    void zoomIn(const QPoint &center) ;
    void zoomOut(const QPoint &center) ;

    // get map center in lat lon
    QPointF getCenter() const ;
    // get map center in mercator
    QPoint getMercCenter() const { return center_ ; }

    int getZoom() const { return zoom_ ; }

    void setCurrentOverlay(const MapOverlayPtr &o) ;

    void saveOverlay(const MapOverlayPtr &o, bool updateOnly=false);

    void updateOverlay(const MapOverlayPtr &o) ;

    QSharedPointer<MapOverlayManager> getOverlayManager() const { return overlay_manager_ ; }

    quint64 currentCollection() const {
        return current_collection_ ;
    }

    void showPopup(const QString &txt, const QPoint &click) ;
    void hidePopup() ;

    void invalidateMap() ;

public Q_SLOTS:
    void zoomToRect(const QRectF &rect) ;
    void invalidateOverlay() ;
protected:

    friend class MainWindow ;
    friend class PolygonTool ;
    friend class FeatureEditTool ;

    void paintEvent(QPaintEvent *) ;
    void mousePressEvent(QMouseEvent *) ;
    void mouseMoveEvent(QMouseEvent *) ;
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

    void zoom(int delta) ;

    void drawBaseMap(QPainter &painter, const QRegion &region) ;
    void drawOverlays(QPainter &painter, const QRegion &region) ;

    void setOverlayManager(QSharedPointer<MapOverlayManager> mgr) {
        overlay_manager_ = mgr ;
    }

    friend class TileFetcher ;

    struct Tile {
        Tile(int x, int y, int z): x_(x), y_(y), z_(z) {}
        int x_, y_, z_ ;
    };

    void computeTiles(std::vector<Tile> &tiles, QPoint &offset, bool prefetch = false) ;
    void fetchMissing(const QList<Tile> &tiles) ;
    QImage fetchSavedTile(int x, int y, int z) ;
    void saveTile(const QImage &tile_, int x, int y, int z) ;


    QPoint center_ ;           // mercator coordinates of point at the center of screen
    unsigned int zoom_ ;        // zoom level of the map

    QPoint start_point_, current_ ;
    ZoomMode zoom_mode_ ;
    bool panning_, fetch_canceled_, show_tile_boundaries_, cache_tiles_ ;
    std::shared_ptr<TileProvider> base_map_ ;
    QPixmapCache tile_cache_ ;
    QSet<QString> pending_ ;
    QMutex mutex_ ;
    QDir cache_dir_ ;
    QImage base_map_image_ ;
    MapTool *current_tool_ ;
    QCache<quint64, MapOverlay> overlay_cache_ ; // cached overlays
    QSharedPointer<MapOverlayManager> overlay_manager_ ;
    MapOverlayPtr current_overlay_ ;
    QSet<quint64> selected_ ;
    quint64 current_collection_ ;
    QUndoStack *undo_stack_ ;
    Popup *popup_ ;


public Q_SLOTS:

    void onFetchFinished(const QString &, const QImage &img) ;
    void selectOverlays(const QVector<quint64> &ids);

Q_SIGNALS:

    void newOverlay(MapOverlayPtr) ;
};

class TileFetcher: public QObject, public QRunnable {
    Q_OBJECT

public:

    TileFetcher(MapWidget *w, int x, int y, int z) ;

    void run() ;

Q_SIGNALS:
    void tileFetchFinished(QString, QImage) ;

private:

    int x_, y_, z_ ;
    MapWidget *instance_ ;
};

// spherical mercator projection

QPoint ll2px(float lon, float lat, unsigned int zoomLevel, unsigned int tile_size_) ;
QPointF px2ll(const QPoint &coord, unsigned int zoomLevel, unsigned int tile_size_) ;

#endif
