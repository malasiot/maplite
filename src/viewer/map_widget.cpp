#include "map_widget.hpp"
#include "map_tool.hpp"
#include "map_overlay.hpp"
#include "map_overlay_manager.hpp"
#include "main_window.hpp"


#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QTime>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QVector2D>
#include <math.h>
#include <cassert>

using namespace std ;


static qreal deg2rad(qreal x)
{
    return x * (M_PI/180.0);
}

static qreal rad2deg(qreal x)
{
    return x * (180/M_PI);
}

// spherical mercator projection

QPoint ll2px(float lon, float lat, unsigned int zoomLevel, unsigned int tile_size_)
{
    qreal numberOfTiles = pow(2.0, zoomLevel) ;

    qreal x = (lon + 180) * (numberOfTiles*tile_size_)/360. ;
    qreal y = (1-(log(tan(M_PI/4 + deg2rad(lat)/2)) /M_PI)) /2 * (numberOfTiles * tile_size_);

    return QPoint(int(x), int(y));
}

QPointF px2ll(const QPoint &coord, unsigned int zoomLevel, unsigned int tile_size_)
{
    qreal numberOfTiles = pow(2.0, zoomLevel) ;

    qreal longitude = (coord.x()*(360/(numberOfTiles*tile_size_)))-180;
    qreal latitude = rad2deg(atan(sinh((1-coord.y()*(2/(numberOfTiles*tile_size_)))*M_PI)));

    return QPointF(longitude, latitude);
}


MapWidget::MapWidget(QWidget *parent): QWidget(parent), overlay_cache_(10000), cache_tiles_(false) {

    zoom_ = 10 ;
    zoom_mode_ = Mouse ;

    setAttribute(Qt::WA_OpaquePaintEvent) ;
    fetch_canceled_ = false ;
    show_tile_boundaries_ = true ;
    current_tool_ = 0 ;

    undo_stack_ = new QUndoStack(this) ;

    popup_ = new Popup(QSize(300, 300), this) ;
}

void MapWidget::setBasemap(const std::shared_ptr<TileProvider> &tiles)
{
    base_map_ = tiles ;

    if ( base_map_->isAsync() )
        connect(base_map_.get(), SIGNAL(tileReady(QString,QImage)), this, SLOT(onFetchFinished(QString,QImage)) ) ;
}


void MapWidget::selectOverlays(const QVector<quint64> &ids)
{
    selected_.clear() ;

    Q_FOREACH(quint64 id, ids)
        selected_.insert(id) ;

    update() ;
}

void MapWidget::setCenter(float lon, float lat, int zoom)
{
    assert(base_map_) ;

    zoom_ = zoom ;
    center_ = ll2px(lon, lat, zoom_, base_map_->tileSize()) ;
}

QPointF MapWidget::getCenter() const
{
    return px2ll(center_, zoom_, base_map_->tileSize()) ;
}

void MapWidget::setCurrentOverlay(const MapOverlayPtr &o)
{
 //   selected_.clear() ;
    current_overlay_ = o ;
    if ( o && o->id() > 0 ) overlay_cache_.remove(o->id()) ;
}


void MapWidget::saveOverlay(const MapOverlayPtr &o, bool update)
{
    undo_stack_->setActive() ;

    if ( update )
    {
        overlay_manager_->update(o) ;
    }
    else
    {
     //   overlay_cache_.insert(o->id(), o, o->cost()) ;
        emit newOverlay(o) ;
    }

}
void MapWidget::setCacheDir(const QString &dir)
{
    cache_dir_.setPath(dir) ;

}

void MapWidget::paintEvent(QPaintEvent* evnt)
{
    QWidget::paintEvent(evnt);
    QPainter painter(this);

    const QRegion &region = evnt->region () ;
    painter.setClipRegion(region) ;

    // base map
    drawBaseMap(painter, region) ;

    painter.save() ;

    QSize sz = size() ;
    QPoint m(sz.width()/2, sz.height()/2) ;

    painter.translate(m - center_) ;

    // draw overlays
    drawOverlays(painter, region);

    // tool
    current_tool_->paint(painter) ;

    // popup



    painter.restore() ;
}


void MapWidget::mousePressEvent(QMouseEvent* evnt)
{
    if ( current_tool_ )
        current_tool_->mousePressed(evnt);
}

void MapWidget::scroll(const QPoint &offset)
{
    center_ += offset ;
  //  QWidget::scroll(-offset.x(), -offset.y()) ;
    update() ;
}

QPointF MapWidget::displayToCoords(const QPoint &p)
{
    return px2ll(p, zoom_, base_map_->tileSize()) ;
}

QPoint MapWidget::coordsToDisplay(const QPointF &p)
{
    return ll2px(p.x(), p.y(), zoom_, base_map_->tileSize() ) ;
}

QRect MapWidget::coordsToWindow(const QRectF &coords)
{
    QPoint p1 = displayToPosition(coordsToDisplay(coords.topLeft())) ;
    QPoint p2 = displayToPosition(coordsToDisplay(coords.bottomRight())) ;

    return QRect(p1, p2).normalized() ;
}

QRectF MapWidget::windowToCoords(const QRect &rect)
{
    QPointF p1 = displayToCoords(positionToDisplay(rect.topLeft())) ;
    QPointF p2 = displayToCoords(positionToDisplay(rect.bottomRight())) ;

    return QRectF(p1, p2).normalized() ;
}

QPoint MapWidget::positionToDisplay(const QPoint &pos)
{
    QSize sz = size() ;
    QPoint middle(sz.width()/2, sz.height()/2) ;
    return pos - middle + center_ ;
}

QPoint MapWidget::displayToPosition(const QPoint &coord)
{
    QSize sz = size() ;
    QPoint middle(sz.width()/2, sz.height()/2) ;
    return coord + middle - center_ ;
}

void MapWidget::mouseMoveEvent(QMouseEvent* evnt)
{
    QPoint pos = evnt->pos() ;

    QPointF coords = displayToCoords(positionToDisplay(pos)) ;
    MainWindow::instance()->displayCoords(coords) ;

    current_tool_->mouseMoved(evnt);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* evnt)
{
    current_tool_->mouseReleased(evnt);
 }

void MapWidget::wheelEvent(QWheelEvent *e)
{
    zoom(e->delta() > 0) ;

    current_tool_->wheelEvent(e) ;

}

void MapWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
   current_tool_->mouseDoubleClicked(e) ;
}

bool MapWidget::ensureVisible(const QPoint &pos)
{
    if ( !rect().contains(pos) )
    {
        QSize sz = size() ;

        int ofx = 0, ofy = 0 ;
        if ( pos.x() >= sz.width() ) ofx = pos.x() - (sz.width() - 1) ;
        if ( pos.x() < 0 ) ofx = pos.x() ;
        if ( pos.y() >= sz.height() ) ofy = pos.y() - (sz.height() - 1) ;
        if ( pos.y() < 0 ) ofy = pos.y() ;

        scroll(QPoint(ofx, ofy)) ;
        return true ;
     }
    else return false ;
}

void MapWidget::zoom(int delta)
{
    assert(base_map_) ;

    // generate an offscreen image of the current basemap

    fetch_canceled_ = true ;
    QThreadPool::globalInstance()->waitForDone() ;
    pending_.clear() ;
    fetch_canceled_ = false ;

    QImage im(size(), QImage::Format_ARGB32) ;
    QPainter painter(&im) ;

    QSize sz = size() ;
    QPoint sc(sz.width()/2, sz.height()/2) ;

    // screen coordinates of current mouse position

    QPoint current_ = mapFromGlobal(QCursor::pos());
    QPoint offset = current_ - sc ;

    QPoint cp = ( zoom_mode_ == Mouse ) ? current_ : sc ;

    double scale ;
    if ( delta > 0 ) scale = 2 ;
    else scale = 0.5 ;

    painter.translate(cp) ;
    painter.scale(scale, scale) ;
    painter.translate(-cp) ;

    base_map_image_ = im ;
    drawBaseMap(painter, QRegion(im.rect())) ;

    if ( delta > 0 )
        zoom_ ++ ;
    else
    {
        zoom_ -- ;
        if ( zoom_ < 0 ) zoom_ = 0 ;
    }



    if ( zoom_mode_ == Mouse )
        center_ = (center_ + offset)*scale - offset  ;
    else
        center_ = center_*scale ;

    update() ;
}

void MapWidget::zoom(const QRect &extents)
{
    center_ = extents.center() ;
    int exw = qAbs(extents.width()), exh = qAbs(extents.height()) ;
    QSize sz = size() ;

    while ( 2*exw < sz.width() && 2*exh < sz.height() )
    {
        zoom_ ++ ;
        exw *= 2 ;
        exh *= 2 ;
        center_ *= 2 ;
    }

    while ( exw > sz.width() || exh > sz.height() )
    {
        zoom_ -- ;
        exw /= 2 ;
        exh /= 2 ;
        center_ /= 2 ;
    }


    update() ;
}

void MapWidget::zoomIn(const QPoint &center)
{
    center_ = center ;
    center_ *= 2 ;
    zoom_ ++ ;
    update() ;
}

void MapWidget::zoomOut(const QPoint &center)
{
    center_ = center ;
    center_ /= 2 ;
    zoom_ -- ;
    update() ;
}


void MapWidget::drawBaseMap(QPainter &painter, const QRegion &region)
{
    if ( base_map_image_.isNull() )
    {
        vector<Tile> tiles ;
        QPoint offset ;

        int tile_size = base_map_->tileSize() ;

        computeTiles(tiles, offset, true) ;

        QList<Tile> missing ;

        for(int i=0 ; i<tiles.size() ; i++ )
        {
            const Tile &t = tiles[i] ;
            int x = t.x_, y = t.y_, z = t.z_ ;

            QRect r( x*tile_size + offset.x(),
                 y*tile_size + offset.y(),
                 tile_size,
                 tile_size) ;

            if ( region.intersects(r) )
            {
                QString key = base_map_->getKey(x, y, z) ;

                if ( QPixmap *px = tile_cache_.find(key) )
                {
                    painter.drawPixmap(r, *px) ;
                }
                else {
                    painter.fillRect(r, Qt::black) ;

                    missing.append(t) ;
                }

                if ( show_tile_boundaries_ )
                {

                    painter.setPen(QPen(QColor(255, 255, 0), 2, Qt::SolidLine)) ;

                    painter.drawRect(r) ;

                    QString label = QString("%1 %2").arg(t.x_).arg(t.y_) ;

                    painter.setPen(Qt::black) ;

                    painter.drawText(r, label, Qt::AlignHCenter | Qt::AlignCenter) ;
                }

            }
        }

        fetchMissing(missing) ;
    }
    else {
        painter.drawImage(rect(), base_map_image_) ;
        base_map_image_ = QImage() ;
        update() ;
    }
}

void MapWidget::updateOverlay(const MapOverlayPtr &ovr)
{
    QRect rect = ovr->displayRect(this) ;
    update(rect) ;
}

void MapWidget::showPopup(const QString &txt, const QPoint &click)
{
    popup_->setText(txt) ;
    popup_->show(click) ;
}

void MapWidget::hidePopup() {
    popup_->hide() ;
}

void MapWidget::zoomToRect(const QRectF &coords)
{
    QPoint p1 = coordsToDisplay(coords.topLeft()) ;
    QPoint p2 = coordsToDisplay(coords.bottomRight()) ;

    zoom(QRect(p1, p2)) ;
}

void MapWidget::invalidateOverlay()
{
    overlay_cache_.clear() ;
    update() ;
}


void MapWidget::drawOverlays(QPainter &painter, const QRegion &region)
{
    QVector<QRectF> bboxes ;
    QVector<QRect> rects = region.rects() ;

    Q_FOREACH( QRect rect, rects) {
        bboxes.append(windowToCoords(rect)) ;
    }

    // query spatialindex for objects that are within the repaint area
    QVector<quint64> ovr ;
    overlay_manager_->query(ovr, bboxes) ;

    QVector<MapOverlayPtr> features_to_draw ;

    // search the cache for already available objects otherwise load them from storage


    Q_FOREACH( quint64 id, ovr )
    {
        MapOverlayPtr obj ;

        if ( current_overlay_ && current_overlay_->id() == id ) continue ;

        if ( overlay_cache_.contains(id) )
        {
            obj = MapOverlayPtr(overlay_cache_.object(id)) ;
//            qDebug() << "loading cached object" << obj->id() << obj->name() ;
        }
        else  {
            obj = overlay_manager_->load(id) ;
//            qDebug() << "loading stored object" << obj->id() << obj->name() ;
        }

        if ( obj == 0 )
            continue ;
        if ( !obj->isVisible() ) continue ;

        obj->setSelected(selected_.contains(id)) ;


        features_to_draw.append(obj) ;

        obj->draw(painter, this) ;
    }

    // after we have drawn add them to the cache
    // TODO: Crashes
/*
    Q_FOREACH( MapFeature *o, features_to_draw )
    {
       //  if ( !overlay_cache_.contains(o->id())) ;
            overlay_cache_.insert(o->id(), o, o->cost()) ;

    }
*/
    // draw currently active object if any

    if ( current_overlay_ )
    {
        QRect ovr_box = current_overlay_->displayRect(this) ;

        Q_FOREACH( QRect r, rects )
        {
            if ( r.intersects(ovr_box) )
            {
                current_overlay_->draw(painter, this) ;
            }
        }
    }


}


void MapWidget::computeTiles(vector<Tile> &tiles, QPoint &offset, bool prefetch)
{
    // viewport dimensions

    QSize sz = size() ;
    unsigned int sx2 = sz.width()/2 ;
    unsigned int sy2 = sz.height()/2 ;

    // offset of map center from nearest tile
    unsigned int tile_size = base_map_->tileSize() ;
    unsigned int cx = center_.x() % tile_size ;
    unsigned int cy = center_.y() % tile_size ;

    // number of tiles surrounding center tile

    int space_on_left = sx2 - cx ;
    int tiles_on_left = (space_on_left + tile_size)/tile_size ;

    int space_on_top = sy2 - cy ;
    int tiles_on_top = (space_on_top + tile_size)/tile_size ;

    int space_on_right = sx2 - ( tile_size - cx )  ;
    int tiles_on_right = (space_on_right + tile_size)/tile_size ;

    int space_on_bottom = sy2 - ( tile_size - cy )  ;
    int tiles_on_bottom = (space_on_bottom + tile_size)/tile_size ;

    // center tile coordinates

    int tx = center_.x()/tile_size ;
    int ty = center_.y()/tile_size ;

    int min_tx = tx - tiles_on_left ;
    int max_tx = tx + tiles_on_right ;
    int min_ty = ty - tiles_on_top ;
    int max_ty = ty + tiles_on_bottom ;

    offset = QPoint(sx2 - cx - tx * tile_size,
                    sy2 - cy - ty * tile_size) ;

    for( int y=min_ty ; y<=max_ty ; y++ )
        for( int x=min_tx ; x<=max_tx ; x++ )
            tiles.push_back(Tile(x, y, zoom_)) ;

    if ( prefetch )
    {

        for ( int x = min_tx-1; x <= max_tx+1; x++ )
            tiles.push_back(Tile(x, min_ty-1, zoom_)) ;

        for ( int x = min_tx-1; x <= max_tx+1; x++ )
            tiles.push_back(Tile(x, max_ty + 1, zoom_)) ;

        for ( int y = min_ty; y <= max_ty; y++ )
            tiles.push_back(Tile(min_tx-1, y, zoom_)) ;

        for ( int y = min_ty; y <= max_ty; y++ )
            tiles.push_back(Tile(max_tx+1, y, zoom_)) ;
    }

}

TileFetcher::TileFetcher(MapWidget *w, int x, int y, int z):
    instance_(w), x_(x), y_(y), z_(z) {
}

void TileFetcher::run()
{
    if ( instance_->fetch_canceled_ ) return ;

    QString key = instance_->base_map_->getKey(x_, y_, z_) ;

    QImage cached_tile = instance_->fetchSavedTile(x_, y_, z_) ;

    if ( !cached_tile.isNull() )
        emit tileFetchFinished(key, cached_tile) ;
    else
    {
        QImage tile = instance_->base_map_->getTile(x_, y_, z_) ;

    //    instance_->saveTile(tile, x_, y_, z_) ;

        emit tileFetchFinished(key, tile) ;
    }
}

void MapWidget::fetchMissing(const QList<Tile> &missing)
{
    QListIterator<Tile> it(missing) ;

    while( it.hasNext())
    {
        const Tile &t = it.next() ;

        QString key = base_map_->getKey(t.x_, t.y_, t.z_) ;

        {
            QMutexLocker locker(&mutex_) ;
            if ( pending_.contains(key) ) continue ;
            pending_.insert(key) ;
        }

        if ( base_map_->isAsync() )
        {
            QImage cached_tile = fetchSavedTile(t.x_, t.y_, t.z_) ;

            if ( !cached_tile.isNull() )
                onFetchFinished(key, cached_tile) ;
            else
                base_map_->getTile(t.x_, t.y_, t.z_) ;
        }
        else {

            TileFetcher *fetcher = new TileFetcher(this, t.x_, t.y_, t.z_) ;
            QObject::connect(fetcher, SIGNAL(tileFetchFinished(QString,QImage)), this, SLOT(onFetchFinished(QString,QImage))) ;

            QThreadPool::globalInstance()->start(fetcher);
        }

    }

}

QImage MapWidget::fetchSavedTile(int x, int y, int z)
{
    if ( cache_tiles_ )
    {
        QString filePath = cache_dir_.absolutePath() + "/" + base_map_->name() + QString("/%1/%2/%3.png").arg(z).arg(x).arg(y);

        QImage tile ;
        tile.load(filePath) ;

        return tile ;
    }
    else return QImage() ;
}

void MapWidget::saveTile(const QImage &tile_, int x, int y, int z)
{
    if ( cache_tiles_ )
    {
        QString filePath = cache_dir_.absolutePath() + "/" + base_map_->name() + QString("/%1/%2/").arg(z).arg(x) ;

        QDir(filePath).mkpath(".") ;

        tile_.save(filePath + QString("%1.png").arg(y)) ;
    }

}

void MapWidget::onFetchFinished(const QString &key, const QImage &tile)
{
    if ( !tile.isNull() )
    {
        QPixmap px = QPixmap::fromImage(tile) ;

        tile_cache_.insert(key, px) ;

        if ( base_map_->isAsync() )
        {
            int x, y, z ;
            base_map_->coordsFromKey(key, x, y, z) ;
            saveTile(tile, x, y, z) ;
        }
    }
    else
    {
        unsigned int  tile_size = base_map_->tileSize() ;

        QPixmap pic(tile_size, tile_size) ;
        QPainter painter(&pic) ;

        QBrush brush(Qt::white, Qt::DiagCrossPattern) ;

        painter.fillRect(pic.rect(), Qt::red) ;
        painter.fillRect(pic.rect(), brush) ;

        tile_cache_.insert(key, pic) ;
    }

    {
        QMutexLocker locker(&mutex_) ;

        pending_.remove(key) ;
    }

    update() ;
}


void MapWidget::setTool(MapTool *tool)
{
    if ( current_tool_ )
        current_tool_->deinit() ;
    current_tool_ = tool ;
    current_tool_->init(this) ;
}

