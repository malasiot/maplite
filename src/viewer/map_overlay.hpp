#ifndef __MAP_OVERLAY_H__
#define __MAP_OVERLAY_H__

#include <QRect>
#include <QPolygon>
#include <QPainter>
#include <QPen>
#include <QVariant>
#include <QSharedPointer>
#include <QDebug>

class MapWidget ;

enum Touch { TOUCH_NOTHING = 0x1, TOUCH_EDGE = 0x2, TOUCH_NODE = 0x4, TOUCH_END_POINT = 0x8, TOUCH_BEGIN_POINT = 0x10 } ;

class MapFeature ;
typedef QSharedPointer<MapFeature> MapFeaturePtr ;

// This is an overlay object drawn above the base map
// The map may contains several such objects
// It consists of geometry, attributes and style information

class MapFeature
{

public:

    MapFeature(const QString &name): name_(name), storage_id_(-1), active_(false), selected_(false), visible_(true) {}

    virtual void draw(QPainter &p, MapWidget *view) = 0;
    virtual QRectF boundingBox() const = 0;
    virtual QRect displayRect(MapWidget *) const = 0;

    virtual QByteArray serialize() const = 0 ;
    virtual void deserialize(QByteArray &data) = 0 ;

    virtual QByteArray type() const = 0 ;
    virtual double distanceToPt(const QPoint &coords, int &seg, MapWidget *widget) const = 0 ;
    virtual int touches(const QPoint &coords, MapWidget *widget, int &node) const = 0 ;

    virtual void moveNode(int node, const QPointF &coords) = 0 ;
    virtual void moveEdge(const QPointF &delta) = 0 ;
    virtual void deleteNode(int node) = 0 ;
    virtual void insertNode(int nodeAfter, const QPointF &pt) = 0 ;
    virtual int numNodes() const = 0 ;

    virtual int cost() const { return 1 ; }

    qint64 id() const { return storage_id_; }

    QString name() const { return name_ ; }
    void setName(const QString &name) { name_ = name ; }

    bool isSelected() const { return selected_ ; }
    void setSelected(bool selected) { selected_ = selected ; }

    bool isActive() const { return active_ ; }
    void setActive(bool active) { active_ = active ; }

    bool isVisible() const { return visible_ ; }

    static MapFeaturePtr create(const std::string &type_name, const QString &name) ; // factory

    virtual MapFeaturePtr clone() const = 0;

    QMap<QString, QVariant> attributes_ ;
    QString name_ ;

protected:

    void serializeAttributes(QDataStream &ds) const {
        ds << attributes_ ;
    }

    void deserializeAttributes(QDataStream &ds) {
        ds >> attributes_ ;
    }

    friend class MapFeatureIndex ;
    friend class MapFeatureCollection ;

    qint64 storage_id_ ;
    bool selected_, active_, visible_ ;
};



class PolygonFeature: public MapFeature
{
public:

    PolygonFeature(const QString &name) ;

    virtual void draw(QPainter &p, MapWidget *w) ;

    QRectF boundingBox() const {
        return poly_.boundingRect() ;
    }

    QRect displayRect(MapWidget *w) const;

    QPolygonF getPolygon() const {
        return poly_ ;
    }

    void setPolygon(const QPolygonF &poly)  {
        poly_ = poly ;
    }

    void addPoint(const QPointF &pt) {
        poly_.append(pt)  ;
     }

    void prepend(const QPointF &pt) {
        poly_.push_front(pt) ;
    }

    void popBack() {
        if ( !poly_.empty() ) poly_.pop_back() ;
    }

    void popFront() {
        if ( !poly_.empty() ) poly_.pop_front() ;
    }

    double distanceToPt(const QPoint &coords, int &seg, MapWidget *widget) const ;
    int touches(const QPoint &coords, MapWidget *widget, int &node) const ;

    void moveNode(int node, const QPointF &coords)  ;
    void moveEdge(const QPointF &delta) ;
    void deleteNode(int node) ;
    void insertNode(int nodeAfter, const QPointF &pt) ;
    int numNodes() const { return poly_.size() ; }

    MapFeaturePtr clone() const ;

    QByteArray serialize() const {
        QByteArray ba ;
        QDataStream ds(&ba, QIODevice::WriteOnly) ;
        ds << poly_ ;
        serializeAttributes(ds) ;
        return ba ;
    }

    void deserialize(QByteArray &data) {
        QDataStream ds(data) ;
        ds >> poly_ ;
        deserializeAttributes(ds) ;
    }

    virtual QByteArray type() const {
        return "polygon" ;
    }

    virtual int cost() const {
        return poly_.size() * sizeof(QPointF) + name_.size()*sizeof(ushort) ;
    }

protected:

    QPolygonF poly_ ;

    QPen pen_, selected_pen_, edit_pen_ ;


private:

    void drawSelected(QPainter &painter, MapWidget *parent);
    void drawSimple(QPainter &painter, MapWidget *parent);
    void drawArrows(QPainter &painter, MapWidget *parent);
    void drawActive(QPainter &painter, MapWidget *parent);

};

// displays an icon with a label over a point

class MarkerFeature: public MapFeature
{

public:

    MarkerFeature(const QString &name) ;
    ~MarkerFeature();

    virtual void draw(QPainter &p, MapWidget *parent) ;

    virtual QByteArray type() const {
        return "marker" ;
    }

    QRectF boundingBox() const {
        return QRectF(point_.x() - 0.001,
               point_.y() - 0.001,
               0.002,
               0.002 ) ;
    }

    QRect displayRect(MapWidget *parent) const ;

    QPointF getPosition() const {
        return point_ ;
    }

    void setPoint(const QPointF &p)  {
        point_ = p ;
    }

    double distanceToPt(const QPoint &coords, int &seg, MapWidget *widget) const ;
    int touches(const QPoint &coords, MapWidget *widget, int &node) const ;

    void moveNode(int node, const QPointF &coords) ;
    void moveEdge(const QPointF &delta) ;
    void deleteNode(int node) ;
    void insertNode(int nodeAfter, const QPointF &pt) {}
    int numNodes() const { return 1 ; }

    MapFeaturePtr clone() const ;

    QByteArray serialize() const {
        QByteArray ba ;
        QDataStream ds(&ba, QIODevice::WriteOnly) ;
        ds << point_ ;
        serializeAttributes(ds) ;
        return ba ;
    }

    void deserialize(QByteArray &data) {
        QDataStream ds(data) ;
        ds >> point_ ;
        deserializeAttributes(ds) ;
    }

    virtual int cost() const {
        return sizeof(QPointF) /*+ name_.size() * sizeof(ushort) */;
    }

protected:

    QPointF point_ ;
    QImage pixmap_ ;
    QFont font_ ;
    QRect rect_ ;

private:

    void updateDisplayRect();
} ;



#endif
