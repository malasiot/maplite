#ifndef __MAP_WIDGET_H__
#define __MAP_WIDGET_H__

#include <QGraphicsWebView>

class MapWidget : public QGraphicsWebView
{
    Q_OBJECT

public:
    MapWidget();
    void setMap(const QString &map) ;
    void setCenter(qreal x, qreal y, quint32 z) ;

private slots:
    void onLoadFinished(bool success) ;

};

#endif
