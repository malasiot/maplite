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

class MapOverlay ;
typedef QSharedPointer<MapOverlay> MapOverlayPtr ;

// This is an overlay object drawn above the base map
// The map may contains several such objects
// It consists of geometry, attributes and style information

class MapOverlay
{

public:

    MapOverlay(const QString &name): name_(name), storage_id_(-1), active_(false), selected_(false), visible_(true) {}

    virtual void draw(QPainter &p, MapWidget *view) = 0;
    virtual QRectF boundingBox() const = 0;
    virtual QRect displayRect(MapWidget *) const = 0;

    // serialize attributes and geometry

    virtual void serialize(QDataStream &ds) const {
        serializeAttributes(ds);
    }
        ;
    virtual void deserialize(QDataStream &ds) {
        deserializeAttributes(ds) ;
    }

    QByteArray serialize() const {
        QByteArray ba ;
        QDataStream ds(&ba, QIODevice::WriteOnly );
        serialize(ds) ;
        return ba ;
    }
        ;
    void deserialize(QByteArray &ba) {
        QDataStream ds(&ba, QIODevice::ReadOnly );
        deserialize(ds);
    }

    // a unique type identifier for the overlay class
    virtual QByteArray type() const = 0 ;

    // computes distance of geometry to point
    virtual double distanceToPt(const QPoint &coords, int &seg, MapWidget *widget) const = 0 ;

    // determine if the geometry is touched by the cursor. we assume that the geometry may be controlled by a set of handles or nodes
    // (e.g. polygon vertices or rectangle vertices).
    virtual int touches(const QPoint &coords, MapWidget *widget, int &node) const = 0 ;

    virtual void moveNode(int node, const QPointF &coords) = 0 ;
    virtual void moveEdge(const QPointF &delta) = 0 ;
    virtual void deleteNode(int node) = 0 ;
    virtual void insertNode(int nodeAfter, const QPointF &pt) = 0 ;
    virtual int numNodes() const = 0 ;

    // override to provide custom description that appears in a popup window when an overlay is clicked
    virtual QString description() const { return QString("<p>%1</p>").arg(name_) ; }

    // type of geometry of the overlay
    enum MapOverlayGeometryType { PointGeometry = 0, LinestringGeometry = 1, PolygonGeometry = 2 } ;

    virtual MapOverlayGeometryType geomType() const = 0 ;

    // used to determine cost of storage in cache memory
    virtual int cost() const { return 1 ; }

    // the overlay id stored in the database
    qint64 id() const { return storage_id_; }

    // the name of the overlay that is displayed in the list view
    QString name() const { return name_ ; }
    void setName(const QString &name) { name_ = name ; }

    // overlays may be selected/highlight and active/edited
    bool isSelected() const { return selected_ ; }
    void setSelected(bool selected) { selected_ = selected ; }

    bool isActive() const { return active_ ; }
    void setActive(bool active) { active_ = active ; }

    bool isVisible() const { return visible_ ; }

    // overlay factory that creates an instance of the overlay from the type name
    static MapOverlayPtr create(const std::string &type_name, const QString &name) ; // factory

    virtual MapOverlayPtr clone() const = 0;

    // list of all attributes

    QMap<QString, QVariant> attributes_ ;
    QString name_ ;

protected:

    void serializeAttributes(QDataStream &ds) const {
        ds << attributes_ ;
    }

    void deserializeAttributes(QDataStream &ds) {
        ds >> attributes_ ;
    }

    friend class MapOverlayManager ;
    friend class MapOverlayCollection ;

    qint64 storage_id_ ;
    bool selected_, active_, visible_ ;
};

Q_DECLARE_INTERFACE(MapOverlay, "maplite.MapOverlay")

// overlays with geometry associated with polygon

class PolygonOverlay: public MapOverlay
{
public:

    PolygonOverlay(const QString &name) ;

    virtual void draw(QPainter &p, MapWidget *w) ;

    virtual MapOverlayGeometryType geomType() const { return PolygonGeometry ; }

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

    MapOverlayPtr clone() const ;

    void serialize(QDataStream &ds) const {
        MapOverlay::serialize(ds) ;
        ds << poly_ ;
    }

    void deserialize(QDataStream &ds) {
        MapOverlay::deserialize(ds) ;
        ds >> poly_ ;
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
    void drawActive(QPainter &painter, MapWidget *parent);

};

// displays an icon with a label over a point

class MarkerOverlay: public MapOverlay
{

public:

    MarkerOverlay(const QString &name) ;
    ~MarkerOverlay();

    virtual void draw(QPainter &p, MapWidget *parent) ;

    virtual MapOverlayGeometryType geomType() const { return PointGeometry ; }

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

    MapOverlayPtr clone() const ;

    void serialize(QDataStream &ds) const {
        MapOverlay::serialize(ds) ;
        ds << point_ ;
    }

    void deserialize(QDataStream &ds) {
        MapOverlay::deserialize(ds) ;
        ds >> point_ ;
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
