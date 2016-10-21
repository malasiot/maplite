#include "gps_overlays.hpp"
#include "map_widget.hpp"

#include <qplugin.h>

void GPSTrackOverlay::draw(QPainter &p, MapWidget *w)
{
    LinestringOverlay::draw(p, w) ;

    if ( selected_ )
        drawArrows(p, w) ;
}

void GPSTrackOverlay::drawArrows(QPainter &painter, MapWidget *view)
{

    painter.setBrush(Qt::red) ;
    painter.setPen(Qt::white) ;

    const float arrow_size = 10 ;
    const float arrow_gap = 100 ;

    int n = poly_.size() ;
    QPoint p0 = view->coordsToDisplay(poly_.at(0)) ;
    float x0, y0, x1 = p0.x(), y1 = p0.y() ;
    float sl = 0, tl = 0, ptl = 0, sp = 0 ;
    x0 = x1 ; y0 = y1 ;

    int j = 1 ;
    while ( j < n )
    {
        double dx, dy ;

        if ( sp > tl ) {
            x0 = x1 ; y0 = y1 ;

            const QPointF &cp = poly_.at(j) ;
            QPoint cps = view->coordsToDisplay(cp) ;

            x1 = cps.x() ;
            y1 = cps.y() ;

            dx = x1 - x0 ;
            dy = y1 - y0 ;

            sl = sqrt( dx * dx + dy * dy ) ; // segment length
            ptl = tl ;
            tl += sl ;

            ++j ;
        }
        else  {
            float h = (sp - ptl)/sl ;

            float x = ( 1 - h ) * x0 + h * x1 ;
            float y = ( 1 - h ) * y0 + h * y1 ;
            float angle = atan2(dy, dx);

            QPointF dir( cos(angle), sin(angle) ) ;
            double a2 = arrow_size/2 ;

            QPainterPath arrow ;
            arrow.moveTo(x, y) ;
            arrow.lineTo(x - a2*dir.y(), y + a2*dir.x()) ;
            arrow.lineTo(x + arrow_size * dir.x(), y + arrow_size * dir.y()) ;
            arrow.lineTo(x + a2*dir.y(), y - a2*dir.x()) ;
            arrow.closeSubpath() ;

            painter.drawPath(arrow) ;

            sp += arrow_gap ;
        }

    }
}

MapOverlayPtr GPSTrackOverlay::clone() const
{
    return MapOverlayPtr(new GPSTrackOverlay(*this)) ;
}

GPSOverlaysFactory GPSOverlaysFactory::instance_ ;
