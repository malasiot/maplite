#ifndef __MAP_TOOL_H__
#define __MAP_TOOL_H__

#include <QObject>
#include <QMouseEvent>
#include <QPolygon>
#include <QPainter>
#include <QUndoCommand>

#include "map_overlay.hpp"

// abstract class for handling interaction operations on map canvas

class MapWidget ;


class MapTool: public QObject
{
    Q_OBJECT

  public:

    MapTool(QObject *p =NULL) ;
    virtual ~MapTool();

    virtual void show(bool show = true) {}

protected:
    friend class MapWidget ;

    // should be overriden to perform per view initialization
    virtual void init(MapWidget *v) {
        view_ = v ;
    }

    virtual void deinit() {}

    // handles mouse-pressed event
    virtual void mousePressed(QMouseEvent *) {}

    // handles mouse-released event
    virtual void mouseReleased(QMouseEvent *) {}

    // handles mouse-move event
    virtual void mouseMoved(QMouseEvent *) {}

    // handles wheel event
    virtual void wheelEvent(QWheelEvent *e) {}

    // handles mouse double click
    virtual void mouseDoubleClicked(QMouseEvent *e) {}

    // passes the paint event of the underlying widget so that the tool can provide any visual feedback
    virtual void paint(QPainter &) {}

protected:

    friend class FeatureEditUndoCommand ;

    MapWidget *view_ ;
    MapFeaturePtr current_object_ ;
} ;

// Map panning
class PanTool: public MapTool
{
    Q_OBJECT

public:

    PanTool(QObject *p) ;
    virtual ~PanTool() ;

  protected:

    friend class MapWidget ;

    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void init(MapWidget *v) ;

private:

    QPoint start_point_, current_ ;
    bool panning_ ;

} ;

// Tool that enables map zooming
// By dragging a rectangle the view is centered and zoomed as appropriate
// A single left click recenters the map and zooms in
// A single right click recenters the map and zooms out

class ZoomTool: public MapTool
{
    Q_OBJECT

public:

    ZoomTool(QObject *p) ;
    virtual ~ZoomTool() ;

protected:

    friend class MapWidget ;

    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void paint(QPainter &) ;
    virtual void init(MapWidget *v);

private:

    QPoint start_, current_ ;  // map coordinates of drag start and current position
    QPoint click_ ; // initial click position
    bool show_ ; // show rubberband rectangle ;
    bool is_dragging_ ;
    bool is_panning_ ;
} ;

// Draw a polygon

class PolygonTool: public MapTool
{
    Q_OBJECT

public:

    PolygonTool(QObject *p) ;
    virtual ~PolygonTool() ;

protected:

    friend class MapWidget ;
    friend class MainWindow ;


    virtual void mouseDoubleClicked(QMouseEvent *pevent) ;
    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void wheelEvent(QWheelEvent *e) ;
    virtual void paint(QPainter &) ;
    virtual void init(MapWidget *v);
    virtual void deinit() ;

private:

    QPoint start_, current_ ;  // map coordinates of drag start and current position
    QPoint click_ ; // initial click position
    bool is_dragging_, is_editing_, is_panning_ ;

    int track_counter_ ;
    QUndoStack *undo_stack_ ;
} ;

// Draw a point

class PointTool: public MapTool
{
    Q_OBJECT

public:

    PointTool(QObject *p) ;
    virtual ~PointTool() ;

protected:

    friend class MapWidget ;

    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void init(MapWidget *v);
    virtual void deinit() ;

private:

    int point_counter_ ;
    QPoint click_ ; // initial click position
    bool is_panning_ ;
} ;

// Edit a feature
class FeatureSelectTool: public MapTool
{
    Q_OBJECT

public:

    FeatureSelectTool(QObject *p) ;
    virtual ~FeatureSelectTool() ;

protected:

    friend class MapWidget ;
    friend class MainWindow ;

    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void wheelEvent(QWheelEvent *e) ;
    virtual void paint(QPainter &) ;
    virtual void init(MapWidget *v);
    virtual void deinit() ;

private:

    QPoint start_, current_ ;  // map coordinates of drag start and current position
    QPoint click_ ; // initial click position
    bool is_panning_ ;

 Q_SIGNALS:

    void featureClicked(quint64 id) ;


} ;



class FeatureEditUndoCommand ;

// Edit a feature
class FeatureEditTool: public MapTool
{
    Q_OBJECT

public:

    FeatureEditTool(QObject *p) ;
    virtual ~FeatureEditTool() ;

protected:

    friend class MapWidget ;
    friend class MainWindow ;

    virtual void mousePressed(QMouseEvent *pevent) ;
    virtual void mouseReleased(QMouseEvent *pevent) ;
    virtual void mouseMoved(QMouseEvent *pevent) ;
    virtual void wheelEvent(QWheelEvent *e) ;
    virtual void mouseDoubleClicked(QMouseEvent *pevent) ;
    virtual void paint(QPainter &) ;
    virtual void init(MapWidget *v);
    virtual void deinit() ;

private:

    QPoint start_, current_ ;  // map coordinates of drag start and current position
    QPoint start_point_ ;
    QPoint click_ ; // initial click position
    bool is_dragging_, is_editing_, is_panning_ ;
    int is_extending_ ;

    int current_touch_ ;
    int current_node_ ;
    bool object_modified_ ;
    QUndoStack *undo_stack_ ;

} ;


enum FeatureEditCommand { FEATURE_APPEND_POINT_CMD, FEATURE_MOVE_NODE_CMD, FEATURE_MOVE_EDGE_CMD, FEATURE_UPDATE_VIEW_CMD,
                          FEATURE_DELETE_NODE_CMD, FEATURE_INSERT_NODE_CMD, FEATURE_PREPEND_POINT_CMD } ;

class FeatureEditUndoCommand: public QUndoCommand {
public:

    FeatureEditUndoCommand(MapWidget *view, MapTool *tool, FeatureEditCommand cmd, QUndoCommand *parent = 0): view_(view), cmd_(cmd), tool_(tool), QUndoCommand(parent) {}

    virtual void undo() ;
    virtual void redo() ;
    virtual int id() const { return cmd_ ; }

    FeatureEditCommand cmd_ ;
    QPointF pt_, new_pt_ ;
    MapFeaturePtr feature_, old_feature_ ;
    MapWidget *view_ ;
    MapTool *tool_ ;
    int node_ ;
};



#endif
