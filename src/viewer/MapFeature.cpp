#include "MapFeature.h"
#include "map_widget.hpp"

#include <QPainter>
#include <QVector2D>
#include <QDebug>

#include <math.h>
#include <float.h>

MapFeaturePtr MapFeature::create(const std::string &type_name, const QString &name)
{
    if ( type_name == "polygon" )
        return MapFeaturePtr(new PolygonFeature(name)) ;
    else if ( type_name == "marker" )
        return MapFeaturePtr(new MarkerFeature(name)) ;
    else return MapFeaturePtr() ;
}


PolygonFeature::PolygonFeature(const QString &name): MapFeature(name)
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
}

void PolygonFeature::draw(QPainter &painter, MapWidget *view)
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

void PolygonFeature::drawSimple(QPainter &painter, MapWidget *view)
{
    QPoint cp ;

    painter.setPen(pen_) ;

    for(int i=0 ; i<poly_.size() ; i++)
    {

        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;

        if ( i > 0 )
            painter.drawLine(cp, s1) ;

       cp = s1 ;
    }
}

void PolygonFeature::drawSelected(QPainter &painter, MapWidget *view)
{
    // draw polyline

    QPoint cp ;

    painter.setPen(selected_pen_) ;

    for(int i=0 ; i<poly_.size() ; i++)
    {

        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;

        if ( i > 0 )
            painter.drawLine(cp, s1) ;

       cp = s1 ;
    }

    // draw handles
/*
    painter.setBrush(Qt::white) ;
    painter.setPen(Qt::black) ;

    for(int i=0 ; i<poly_.size() ; i++)
    {
        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;

        painter.drawEllipse(s1, 2, 2) ;
    }
*/

    // draw arrows

    drawArrows(painter, view) ;
}

void PolygonFeature::drawActive(QPainter &painter, MapWidget *view)
{
    // draw polyline

    QPoint cp ;

    painter.setPen(edit_pen_) ;

    for(int i=0 ; i<poly_.size() ; i++)
    {

        const QPointF &p1 = poly_.at(i) ;
        QPoint s1 = view->coordsToDisplay(p1) ;

        if ( i > 0 )
            painter.drawLine(cp, s1) ;

       cp = s1 ;
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


    // draw arrows

    drawArrows(painter, view) ;
}

void PolygonFeature::drawArrows(QPainter &painter, MapWidget *view)
{
    painter.setBrush(Qt::red) ;
    painter.setPen(Qt::white) ;

    const float arrow_size = 24 ;
    const float arrow_gap = 300 ;

    int n = poly_.size() ;
    QPoint p0 = view->coordsToDisplay(poly_.at(0)) ;
    float x0, y0, x1 = p0.x(), y1 = p0.y() ;
    float sl = 0, tl = 0, ptl = 0, cl = arrow_gap ;

    for( int j = 1 ; j < n  ; j++ )
    {
        x0 = x1 ; y0 = y1 ;

        const QPointF &cp = poly_.at(j) ;
        QPoint cps = view->coordsToDisplay(cp) ;

        x1 = cps.x() ;
        y1 = cps.y() ;

        double dx = x1 - x0, dy = y1 - y0 ;

        sl = sqrt( dx * dx + dy * dy ) ;
        ptl = tl ;
        tl += sl ;

        if ( tl > cl )
        {
            float h = std::max(cl - ptl, arrow_size)/(tl - ptl) ;

            float x = ( 1 - h ) * x0 + h * x1 ;
            float y = ( 1 - h ) * y0 + h * y1 ;
            float angle = atan2(dy, dx) + M_PI;

            QPointF p(x, y) ;

            QPointF arrowP1 = p + QPointF(cos(angle + M_PI / 20) * arrow_size,  sin(angle  + M_PI / 20) * arrow_size);
            QPointF arrowP2 = p + QPointF(cos(angle - M_PI / 20) * arrow_size, sin(angle  - M_PI / 20) * arrow_size);

            QPolygonF arrow ;

            painter.drawPolygon(arrow);

            cl += arrow_gap ;
        }
    }
}

QRect PolygonFeature::displayRect(MapWidget *view) const
{
    return view->coordsToWindow(poly_.boundingRect()) ;
}

double PolygonFeature::distanceToPt(const QPoint &coords, int &segment, MapWidget *view) const
{
    double min_dist = DBL_MAX ;

    for(int i=0 ; i<poly_.size() - 1 ; i++)
    {
        const QPoint pa = view->coordsToDisplay(poly_.at(i)) ;
        const QPoint pb = view->coordsToDisplay(poly_.at(i+1)) ;

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

int PolygonFeature::touches(const QPoint &coords, MapWidget *view, int &node) const
{
    for(int i=0 ; i<poly_.size() ; i++)
    {
        QPoint p = view->coordsToDisplay(poly_.at(i)) ;

        double dist = QVector2D(coords - p).length() ;

        if ( dist < 10 ) {
            node = i ;

            if ( i == 0 )
                return ( TOUCH_NODE | TOUCH_BEGIN_POINT ) ;
            else if (  i == poly_.size() - 1 )
                return ( TOUCH_NODE | TOUCH_END_POINT ) ;
            else
                return TOUCH_NODE ;
        }
    }

    if ( distanceToPt(coords, node, view) < 10 )
        return TOUCH_EDGE ;
    else return TOUCH_NOTHING ;
}

void PolygonFeature::moveNode(int node, const QPointF &delta)
{
    poly_[node] += delta ;
}

void PolygonFeature::moveEdge(const QPointF &delta)
{
    poly_.translate(delta) ;
}

MapFeaturePtr PolygonFeature::clone() const
{
    return MapFeaturePtr(new PolygonFeature(*this)) ;
}

void PolygonFeature::deleteNode(int node)
{
    poly_.remove(node);
}

void PolygonFeature::insertNode(int nodeAfter, const QPointF &pt)
{
    const QPointF &p1 = poly_.at(nodeAfter) ;
    const QPointF &p2 = poly_.at(nodeAfter+1) ;

    QVector2D v1(p2 - p1) ;
    QVector2D v2(pt - p1) ;
    float t = QVector2D::dotProduct(v1, v2)/v1.lengthSquared() ;

    poly_.insert(nodeAfter + 1, p1 + (t * v1).toPointF()) ;
}

////////////////////////////////////////////////////////////////////////////////////


MarkerFeature::MarkerFeature(const QString &name): MapFeature(name)
{
    pixmap_ = QImage(":/images/flag.png") ;
    font_ = QFont("Arial", 10) ;
    updateDisplayRect();
}

MarkerFeature::~MarkerFeature()
{

}


void MarkerFeature::draw(QPainter &painter, MapWidget *view)
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

QRect MarkerFeature::displayRect(MapWidget *view) const
{
    QPoint p0 = view->displayToPosition(view->coordsToDisplay(point_)) ;

    QRect r(p0 + rect_.topLeft(), rect_.size()) ;

    return r ;
}

void MarkerFeature::updateDisplayRect()
{
    QSize pxs = pixmap_.size() ;

    QRect r(QPoint(0, -pxs.height()), pxs) ;
    QRect ra = r.adjusted(-2, -2, 2, 2) ;

    QRect tr = QFontMetrics(font_).boundingRect(QRect(), Qt::AlignCenter|Qt::TextSingleLine, name_) ;
    tr.moveTo(QPoint(pxs.width() - 2, -pxs.height() - tr.height() + 2)) ;

    QRect tra = tr.adjusted(-2, -2, 2, 2) ;

    rect_ = ra.united(tra) ;
}

double MarkerFeature::distanceToPt(const QPoint &coords, int &seg, MapWidget *view) const
{
    QSize pxs = pixmap_.size() ;

    QRect r(QPoint(0, -pxs.height()), pxs) ;

    seg = 0 ;
    return QVector2D(coords - view->coordsToDisplay(point_) - r.center()).length() ;
}

int MarkerFeature::touches(const QPoint &coords, MapWidget *widget, int &node) const
{
    if ( distanceToPt(coords, node, widget) < 5 ) {
        node = 0 ;
        return TOUCH_NODE ;
    }
    else return TOUCH_NOTHING ;
}

void MarkerFeature::moveNode(int node, const QPointF &coords)
{
    point_ += coords ;
}

void MarkerFeature::moveEdge(const QPointF &delta)
{
    point_ += delta ;
}

MapFeaturePtr MarkerFeature::clone() const
{
    return MapFeaturePtr(new MarkerFeature(*this)) ;
}

void MarkerFeature::deleteNode(int node) {

}
