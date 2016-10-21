#include "map_overlay.hpp"
#include "map_widget.hpp"

#include <QPainter>
#include <QVector2D>
#include <QDebug>

#include <math.h>
#include <float.h>


MapOverlayPtr MapOverlay::create(const std::string &type_name, const QString &name)
{
    if ( type_name == "polygon" )
        return MapOverlayPtr(new PolygonOverlay(name)) ;
    if ( type_name == "linestring" )
        return MapOverlayPtr(new LinestringOverlay(name)) ;
    else if ( type_name == "marker" )
        return MapOverlayPtr(new MarkerOverlay(name)) ;
    else
        return MapOverlayFactoryManager::instance().createFromRegister(type_name, name) ;
}


PolylineOverlay::PolylineOverlay(const QString &name, bool closed): MapOverlay(name), is_closed_(closed)
{
    pen_.setColor(QColor(0x8b, 0x8b, 0xc2)) ;
    pen_.setWidth(3) ;
    pen_.setJoinStyle(Qt::RoundJoin);

    selected_pen_.setColor(QColor(0x3d, 0x3d, 0x99)) ;
    selected_pen_.setWidth(3) ;
    selected_pen_.setJoinStyle(Qt::RoundJoin);

    edit_pen_.setColor(QColor(0xff, 0x3d, 0x99)) ;
    edit_pen_.setWidth(4) ;
    edit_pen_.setJoinStyle(Qt::RoundJoin);

    fill_brush_.setStyle(Qt::SolidPattern) ;
    fill_brush_.setColor(QColor(0x8b, 0x8b, 0xc2, 128)) ;
}

void PolylineOverlay::draw(QPainter &painter, MapWidget *view)
{
    painter.save() ;

    if ( selected_ )
        drawSelected(painter, view) ;
    else if ( active_ )
        drawActive(painter, view) ;
    else
        drawSimple(painter, view) ;

    painter.restore() ;
}

void PolylineOverlay::drawSimple(QPainter &painter, MapWidget *view)
{
    QPolygonF dpoly ;

    for(int i=0 ; i<poly_.size() ; i++)
    {
        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;
        dpoly.append(s1) ;
    }

    if ( is_closed_ ) {
        QPainterPath p ;
        p.addPolygon(dpoly);
        p.closeSubpath();
        painter.fillPath(p, fill_brush_) ;
        painter.strokePath(p, pen_) ;
    }
    else {
        painter.setPen(pen_) ;
        painter.drawPolyline(dpoly) ;
    }

}

void PolylineOverlay::drawSelected(QPainter &painter, MapWidget *view)
{
    QPolygonF dpoly ;

    for(int i=0 ; i<poly_.size() ; i++)
    {
        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;
        dpoly.append(s1) ;
    }

    if ( is_closed_ ) {
        QPainterPath p ;
        p.addPolygon(dpoly);
        p.closeSubpath();
        painter.fillPath(p, fill_brush_) ;
        painter.strokePath(p, selected_pen_) ;
    }
    else {
        painter.setPen(selected_pen_) ;
        painter.drawPolyline(dpoly) ;
    }
}

void PolylineOverlay::drawActive(QPainter &painter, MapWidget *view)
{
    QPolygonF dpoly ;

    for(int i=0 ; i<poly_.size() ; i++)
    {
        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;
        dpoly.append(s1) ;
    }

    if ( is_closed_ ) {
        QPainterPath p ;
        p.addPolygon(dpoly);
        p.closeSubpath();
        painter.fillPath(p, fill_brush_) ;
        painter.strokePath(p, edit_pen_) ;
    }
    else {
        painter.setPen(edit_pen_) ;
        painter.drawPolyline(dpoly) ;
    }

    // draw handles

    painter.setBrush(Qt::white) ;
    painter.setPen(Qt::red) ;

    for(int i=0 ; i<poly_.size() ; i++)
    {
        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;

        painter.drawEllipse(s1, 3, 3) ;
    }
}



QRect PolylineOverlay::displayRect(MapWidget *view) const
{
    return view->coordsToWindow(poly_.boundingRect()) ;
}

double PolylineOverlay::distanceToPt(const QPoint &coords, int &segment, MapWidget *view) const
{
    double min_dist = DBL_MAX ;

    for(int i=0 ; i<poly_.size() - !is_closed_ ; i++)
    {
        QPoint pa = view->coordsToDisplay(poly_.at(i)) ;
        QPoint pb ;

        if ( is_closed_ && i == poly_.size() -1 ) pb = view->coordsToDisplay(poly_.at(0)) ;
        else pb = view->coordsToDisplay(poly_.at(i+1)) ;

        QVector2D dir(pb - pa), v1(coords - pa), v2(pb - coords) ;

        double dist ;
        double d = QVector2D::dotProduct( dir, v1 ) ;
        double len = dir.lengthSquared() ;

        if ( len == 0 ) dist = v1.length() ;
        else {
                d /= dir.lengthSquared() ;

            if ( d < 0 )
                dist = v1.length() ;
            else if ( d > 1 )
                dist = v2.length() ;
            else
            {
                QPointF pp(pa.x() + d * dir.x(), pa.y() + d * dir.y()) ;
                dist = QVector2D(pp - coords).length() ;
            }
        }

        if ( dist < min_dist )
        {
            min_dist = dist ;
            segment = i ;
        }

    }

    return min_dist ;

}

int PolylineOverlay::touches(const QPoint &coords, MapWidget *view, int &node) const
{
    for(int i=0 ; i<poly_.size() ; i++)
    {
        QPoint p = view->coordsToDisplay(poly_.at(i)) ;

        double dist = QVector2D(coords - p).length() ;

        if ( dist < 10 ) {
            node = i ;

            if ( is_closed_ ) return TOUCH_NODE ;

            if ( i == 0 )
                return ( TOUCH_NODE | TOUCH_BEGIN_POINT ) ;
            else if (  i == poly_.size() - 1 )
                return ( TOUCH_NODE | TOUCH_END_POINT ) ;
            else
                return TOUCH_NODE ;
        }
    }

    if ( distanceToPt(coords, node, view) < 10 ) return TOUCH_EDGE ;
    else return TOUCH_NOTHING ;
}

void PolylineOverlay::moveNode(int node, const QPointF &delta)
{
    poly_[node] += delta ;
}

void PolylineOverlay::moveEdge(const QPointF &delta)
{
    poly_.translate(delta) ;
}


void PolylineOverlay::deleteNode(int node)
{
    poly_.remove(node);
}

void PolylineOverlay::insertNode(int nodeAfter, const QPointF &pt)
{
    const QPointF &p1 = poly_.at(nodeAfter) ;
    const QPointF &p2 = ( is_closed_ && nodeAfter == poly_.size() - 1 ) ? poly_.at(0) : poly_.at(nodeAfter+1) ;

    QVector2D v1(p2 - p1) ;
    QVector2D v2(pt - p1) ;
    float t = QVector2D::dotProduct(v1, v2)/v1.lengthSquared() ;

    poly_.insert(nodeAfter + 1, p1 + (t * v1).toPointF()) ;
}

////////////////////////////////////////////////////////////////////////////////////


MarkerOverlay::MarkerOverlay(const QString &name): MapOverlay(name)
{
    pixmap_ = QImage(":/images/flag.png") ;
    font_ = QFont("Arial", 10) ;
    updateDisplayRect();
}

MarkerOverlay::~MarkerOverlay()
{

}


void MarkerOverlay::draw(QPainter &painter, MapWidget *view)
{
    // draw pixmap

    QPoint p0 = view->coordsToDisplay(point_) ;
    QSize pxs = pixmap_.size() ;

    QRect r(p0 + QPoint(0, -pxs.height()), pxs) ;
    QRect ra = r.adjusted(-2, -2, 2, 2) ;

    if ( selected_ )
    {
        painter.setPen(Qt::NoPen) ;
        painter.setBrush(QColor(0xa0, 0xa0, 0xa0, 0xa0)) ;
        painter.drawRoundedRect(ra, 2, 2) ;
    }
    else if ( active_ )
    {
        painter.setPen(Qt::NoPen) ;
        painter.setBrush(QColor(0xa0, 0xa0, 0xa0, 0xf0)) ;
        painter.drawRoundedRect(ra, 2, 2) ;
    }

    painter.drawImage(r, pixmap_) ;

    painter.setPen(Qt::white) ;
    painter.setFont(font_) ;

    QRect tr = painter.boundingRect(QRect(), Qt::AlignCenter|Qt::TextSingleLine, name_) ;
    tr.moveTo(p0 + QPoint(pxs.width() - 2, -pxs.height() - tr.height() + 2)) ;

    QRectF tra = tr.adjusted(-2, -2, 2, 2) ;

    if ( selected_ )
    {
        painter.setPen(Qt::NoPen) ;
        painter.setBrush(QColor(0x80, 0x80, 0xff)) ;
        painter.drawRoundedRect(tra, 2, 2) ;

        painter.setPen(Qt::white) ;
        painter.drawText(tr, name_) ;
    }
    else if ( active_ )
    {
        painter.setPen(Qt::NoPen) ;
        painter.setBrush(QColor(0xff, 0x0, 0xff)) ;
        painter.drawRoundedRect(tra, 2, 2) ;

        painter.setPen(Qt::white) ;
        painter.drawText(tr, name_) ;
    }
    else
    {
        painter.setPen(Qt::NoPen) ;
        painter.setBrush(Qt::white) ;
        painter.drawRoundedRect(tra, 2, 2) ;

        painter.setPen(Qt::black) ;
        painter.drawText(tr, name_) ;
    }
}

QRect MarkerOverlay::displayRect(MapWidget *view) const
{
    QPoint p0 = view->displayToPosition(view->coordsToDisplay(point_)) ;

    QRect r(p0 + rect_.topLeft(), rect_.size()) ;

    return r ;
}

void MarkerOverlay::updateDisplayRect()
{
    QSize pxs = pixmap_.size() ;

    QRect r(QPoint(0, -pxs.height()), pxs) ;
    QRect ra = r.adjusted(-2, -2, 2, 2) ;

    QRect tr = QFontMetrics(font_).boundingRect(QRect(), Qt::AlignCenter|Qt::TextSingleLine, name_) ;
    tr.moveTo(QPoint(pxs.width() - 2, -pxs.height() - tr.height() + 2)) ;

    QRect tra = tr.adjusted(-2, -2, 2, 2) ;

    rect_ = ra.united(tra) ;
}

double MarkerOverlay::distanceToPt(const QPoint &coords, int &seg, MapWidget *view) const
{
    QSize pxs = pixmap_.size() ;

    QRect r(QPoint(0, -pxs.height()), pxs) ;

    seg = 0 ;
    return QVector2D(coords - view->coordsToDisplay(point_) - r.center()).length() ;
}

int MarkerOverlay::touches(const QPoint &coords, MapWidget *widget, int &node) const
{
    if ( distanceToPt(coords, node, widget) < 5 ) {
        node = 0 ;
        return TOUCH_NODE ;
    }
    else return TOUCH_NOTHING ;
}

void MarkerOverlay::moveNode(int node, const QPointF &coords)
{
    point_ += coords ;
}

void MarkerOverlay::moveEdge(const QPointF &delta)
{
    point_ += delta ;
}

MapOverlayPtr MarkerOverlay::clone() const
{
    return MapOverlayPtr(new MarkerOverlay(*this)) ;
}

void MarkerOverlay::deleteNode(int node) {

}

PolygonOverlay::PolygonOverlay(const QString &name): PolylineOverlay(name, true) {

}


LinestringOverlay::LinestringOverlay(const QString &name): PolylineOverlay(name, false) {

}


MapOverlayPtr PolygonOverlay::clone() const {
    return MapOverlayPtr(new PolygonOverlay(*this)) ;
}

MapOverlayPtr LinestringOverlay::clone() const {
    return MapOverlayPtr(new LinestringOverlay(*this)) ;
}
