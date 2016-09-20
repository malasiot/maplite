#ifndef __MAP_VIEW_H__
#define __MAP_VIEW_H__

#include <QNetworkReply>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QDebug>

class QAction;
class MapWidget ;

class MapView : public QGraphicsView
{
    Q_OBJECT

public:
    MapView(QWidget *parent): QGraphicsView(parent), main_widget_(0) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void setMainWidget(QGraphicsWidget* widget)
    {
        QRectF rect(QRect(QPoint(0, 0), size()));
        widget->setGeometry(rect);
        main_widget_ = widget;
    }

    void resizeEvent(QResizeEvent* event)
    {
        QGraphicsView::resizeEvent(event);
        if ( !main_widget_ ) return;
        QRectF rect(QPoint(0, 0), event->size());

        main_widget_->setGeometry(rect);

    }

private:
    QGraphicsWidget *main_widget_ ;
};

#endif

