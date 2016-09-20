#include "map_widget.hpp"

#include <QFile>
#include <QTextStream>
#include <QDebug>

MapWidget::MapWidget(): QGraphicsWebView() {

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool))) ;

    QWebSettings::globalSettings()->setAttribute(QWebSettings::WebGLEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);

//   QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
//  QWebSettings::globalSettings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);


   const char * html1 = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset='utf-8' />
                <title></title>
                <meta name='viewport' content='initial-scale=1,maximum-scale=1,user-scalable=no' />
                <script src='assets/mapbox-gl.js'></script>
                <link href='assets/mapbox-gl.css' rel='stylesheet' />
                <style>
                    body { margin:0; padding:0; }
                    #map { position:absolute; top:0; bottom:0; width:100%; }
                </style>
            </head>
            <body>
            <div id='map'></div>
           <script>
           mapboxgl.accessToken = 'pk.eyJ1IjoicGV0ZXJuaWs5OSIsImEiOiJjaWxraHZ2aGwwMDBhdWdsenV5NHBvaTJ1In0.gsHJ7w8ndB-gMb4I_k0_Ig';
           var map = new mapboxgl.Map({
               container: 'map', // container id
               style: 'assets/mapbox.json', //stylesheet location
               center: [22.23, 41], // starting position
               zoom: 12 // starting zoom
           });

           map.debug = true ;
           map.collisionDebug = true ;
           </script>
            </body>
            </html>
            )" ;


       setHtml(html1, QUrl("http://127.0.0.1:5000/")) ;



}

void MapWidget::onLoadFinished(bool s) {

}

void MapWidget::setMap(const QString &map)
{

}

void MapWidget::setCenter(qreal x, qreal y, quint32 z)
{

}
