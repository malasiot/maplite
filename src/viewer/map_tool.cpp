#include "map_widget.hpp"
#include "map_tool.hpp"
#include "map_overlay.hpp"
#include "map_overlay_manager.hpp"
#include "main_window.hpp"

#include <QDebug>
#include <QRubberBand>
#include <QPainter>


MapTool::~MapTool() {}
MapTool::MapTool(QObject *p): QObject(p), view_(0) {}

////////////////////////////////////////////////////////////////////////////////////

PanTool::PanTool(QObject *p): MapTool(p)
{
    panning_ = false ;
}

void PanTool::mousePressed(QMouseEvent *evnt)
{
    start_point_ = QPoint(evnt->x(), evnt->y());

    view_->hidePopup();

    if ( current_object_ ) {
        view_->showPopup(current_object_->description(), start_point_) ;
        qDebug() << current_object_->id() ;
    }
    else {
        panning_ = true ;
        view_->setCursor(QCursor(Qt::ClosedHandCursor)) ;
    }

}

void PanTool::mouseMoved(QMouseEvent *evnt)
{
    current_ = QPoint(evnt->x(), evnt->y()) ;

    if ( panning_ )
    {
        QPoint offset = start_point_ - current_ ;

        view_->scroll(offset) ;

        start_point_ = current_ ;
    }
    else {

        QPoint p = view_->positionToDisplay(current_) ;

        QSharedPointer<MapOverlayManager> mgr = view_->getOverlayManager() ;

        current_object_ = mgr->findNearest("*", p, view_, 10) ;

        if ( current_object_ ) {
            view_->setCursor(QCursor(Qt::ArrowCursor)) ;
            current_object_->setSelected(true) ;
            view_->setCurrentOverlay(current_object_) ;
        }
        else {
            view_->setCurrentOverlay(MapOverlayPtr()) ;
            view_->setCursor(QCursor(Qt::OpenHandCursor)) ;
        }

        view_->update() ;
    }

}

void PanTool::mouseReleased(QMouseEvent *mouseEvent)
{
    panning_ = false ;
    view_->setCursor(QCursor(Qt::OpenHandCursor)) ;
}

PanTool::~PanTool() {}

void PanTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    panning_ = false ;
    view_->setMouseTracking(true) ;

}

///////////////////////////////////////////////////////////////////////////////////
static const int CLICK_THRESHOLD = 5 ;

ZoomTool::ZoomTool(QObject *p): MapTool(p), is_dragging_(false), is_panning_(false) {
    show_ = false ;
}

ZoomTool::~ZoomTool()
{

}

void ZoomTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    is_dragging_ = false ;
    is_panning_ = false ;
}

void ZoomTool::mouseMoved(QMouseEvent *mouseEvent)
{
    QPoint pos = mouseEvent->pos() ;

    if ( mouseEvent->buttons() & Qt::LeftButton && !is_dragging_ )
    {
        if ( abs(pos.x() - click_.x()) < CLICK_THRESHOLD &&
             abs(pos.y() - click_.y()) < CLICK_THRESHOLD )
            is_dragging_ = false ;
        else is_dragging_ = true ;
    }
    else if ( mouseEvent->buttons() & Qt::RightButton && is_panning_ )
    {
        QPoint offset = click_ - pos ;
        view_->scroll(offset) ;
        click_ = pos ;
    }

    if ( is_dragging_ )
    {
        int x = pos.x(), y = pos.y() ;

        QSize sz = view_->size() ;

        if ( x >= sz.width() ) x = sz.width() - 1 ;
        else if ( x < 0 ) x = 0 ;
        if ( y >= sz.height() ) y = sz.height() - 1 ;
        else if ( y < 0 ) y = 0 ;

        current_ = view_->positionToDisplay(QPoint(x, y)) ;

        view_->update() ;
    }
}

void ZoomTool::mousePressed(QMouseEvent *mouseEvent)
{
    click_ = mouseEvent->pos() ;
    start_ = view_->positionToDisplay(click_) ;
    current_ = start_ ;

    if ( mouseEvent->buttons() & Qt::RightButton )
        is_panning_ = true ;
    else {
        show_ = true ;
        is_dragging_ = false ;
    }

}


void ZoomTool::paint(QPainter &painter)
{
    if ( !show_) return ;

    QRect rect(start_, current_) ;

    painter.fillRect(rect, QBrush(QColor(0, 0, 100, 50))) ;
    painter.setPen(QPen(Qt::red)) ;
    painter.drawRect(rect) ;

}

void ZoomTool::mouseReleased(QMouseEvent *mouseEvent)
{
    if ( is_dragging_ )
    {
        show_ = false ;
        view_->zoom(QRect(start_, current_)) ;
        is_dragging_ = false ;
    }
    else if ( is_panning_ )
        is_panning_ = false ;
    else
    {
        if ( mouseEvent->button() == Qt::LeftButton  ) {
            if ( mouseEvent->modifiers() & Qt::ControlModifier )
                view_->zoomOut(QPoint( view_->positionToDisplay(mouseEvent->pos()))) ;
            else
                view_->zoomIn(QPoint( view_->positionToDisplay(mouseEvent->pos()))) ;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////


PolygonTool::PolygonTool(QObject *p): MapTool(p), is_dragging_(false), is_panning_(false), track_counter_(1) {
    undo_stack_ = new QUndoStack(this) ;
}

PolygonTool::~PolygonTool()
{

}

void PolygonTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    is_dragging_ = false ;
    is_editing_ = false ;
    is_panning_ = false ;
    view_->setMouseTracking(true) ;

}

void PolygonTool::deinit()
{
    view_->setMouseTracking(false) ;
}

void PolygonTool::mouseMoved(QMouseEvent *mouseEvent)
{
    QPoint pos = mouseEvent->pos() ;

    if ( !is_editing_ && !is_panning_ ) return ;

    if ( mouseEvent->buttons() & Qt::LeftButton && !is_dragging_ )
    {
        if ( abs(pos.x() - click_.x()) < CLICK_THRESHOLD &&
             abs(pos.y() - click_.y()) < CLICK_THRESHOLD )
            is_dragging_ = false ;
        else is_dragging_ = true ;
    }
    else if ( ( mouseEvent->buttons() & Qt::RightButton ) && is_panning_ )
    {
        QPoint offset = click_ - pos ;
        view_->scroll(offset) ;
        click_ = pos ;
    }

    if ( is_dragging_ )
    {
        if ( !view_->ensureVisible(pos) )
            view_->update() ;

        current_ = view_->positionToDisplay(pos) ;

    }
    else
    {
        current_ = view_->positionToDisplay(pos) ;

        view_->update() ;
    }


}

void PolygonTool::wheelEvent(QWheelEvent *e)
{
    if ( is_editing_ )
        current_ = view_->positionToDisplay(e->pos()) ;
}

void PolygonTool::mousePressed(QMouseEvent *mouseEvent)
{
    click_ = mouseEvent->pos() ;
    start_ = view_->positionToDisplay(click_) ;
    current_ = start_ ;

    if ( mouseEvent->buttons() & Qt::RightButton )
        is_panning_ = true ;
    else {
        is_dragging_ = false ;
        is_editing_ = true ;

        undo_stack_->setActive() ;
    }
}


void PolygonTool::paint(QPainter &painter)
{
    if ( is_editing_ && current_object_ )
    {
        painter.save() ;

        painter.setPen(QPen(Qt::red)) ;

        PolygonOverlay *co = dynamic_cast<PolygonOverlay *>(current_object_.data()) ;

        const QPolygonF &poly_ = co->getPolygon() ;

        if ( !poly_.isEmpty() )
        {
            const QPointF &p1 = poly_.back() ;
            QPoint s1 = view_->coordsToDisplay(p1) ;

            painter.drawLine(s1, current_) ;
        }

        painter.restore() ;
    }

}

void PolygonTool::mouseReleased(QMouseEvent *mouseEvent)
{
    if ( mouseEvent->button() == Qt::LeftButton && is_editing_ )
    {
        if ( current_object_ == 0  )
        {
            QSharedPointer<MapOverlayManager> mgr = view_->getOverlayManager() ;
            QString name = mgr->uniqueOverlayName("Track %1", view_->currentCollection(), track_counter_) ;

            current_object_ = MapOverlayPtr(new PolygonOverlay(name)) ;
            current_object_->setSelected(true) ;

            view_->setCurrentOverlay(current_object_) ;

            undo_stack_->clear() ;
        }


        QPointF pt = view_->displayToCoords(current_) ;

        FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_APPEND_POINT_CMD) ;
        ucmd->feature_ = current_object_->clone() ;
        ucmd->pt_ = pt ;
        undo_stack_->push(ucmd);

    }

    is_dragging_ = is_panning_ = false ;
}


void PolygonTool::mouseDoubleClicked(QMouseEvent *mouseEvent)
{
    is_editing_ = false ;

    view_->saveOverlay(current_object_) ;
    current_object_->setSelected(false) ;
    view_->setCurrentOverlay(MapOverlayPtr()) ;
    view_->update() ;
    current_object_.clear() ;

    is_dragging_ = false ;
}

///////////////////////////////////////////////////////////////////////////


PointTool::PointTool(QObject *p): MapTool(p), is_panning_(false), point_counter_(1) {

}

PointTool::~PointTool()
{

}

void PointTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    current_object_.clear() ;
    is_panning_ = false ;
}

void PointTool::deinit()
{
    if ( current_object_ )
    {
        current_object_->setSelected(false) ;
        view_->updateOverlay(current_object_) ;
        current_object_.clear() ;
    }
}

void PointTool::mousePressed(QMouseEvent *mouseEvent)
{
    click_ = mouseEvent->pos() ;

    if ( mouseEvent->buttons() & Qt::RightButton )
        is_panning_ = true ;
}

void PointTool::mouseReleased(QMouseEvent *mouseEvent)
{
    QPoint p = view_->positionToDisplay(mouseEvent->pos()) ;

    if ( mouseEvent->button() == Qt::LeftButton )
    {
        if ( current_object_ )
        {
            current_object_->setSelected(false) ;
            view_->updateOverlay(current_object_) ;
        }

        QSharedPointer<MapOverlayManager> mgr = view_->getOverlayManager() ;
        QString name = mgr->uniqueOverlayName("%1", view_->currentCollection(), point_counter_) ;

        MarkerOverlay *mf = new MarkerOverlay(name) ;
        mf->setPoint(view_->displayToCoords(p)) ;

        current_object_ = MapOverlayPtr(mf) ;
        current_object_->setSelected(true) ;

        view_->saveOverlay(current_object_) ;
        view_->setCurrentOverlay(current_object_);
        view_->updateOverlay(current_object_) ;

    }
    else if ( mouseEvent->button() == Qt::RightButton )
        is_panning_ = false ;

}

void PointTool::mouseMoved(QMouseEvent *mouseEvent)
{
    QPoint pos = mouseEvent->pos() ;

    if ( ( mouseEvent->buttons() & Qt::RightButton ) && is_panning_ )
    {
        QPoint offset = click_ - pos ;
        view_->scroll(offset) ;
        click_ = pos ;
    }

}

////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////


FeatureSelectTool::FeatureSelectTool(QObject *p): MapTool(p) {
    is_panning_ = false ;
}

FeatureSelectTool::~FeatureSelectTool()
{

}

void FeatureSelectTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    view_->setMouseTracking(true) ;

}

void FeatureSelectTool::deinit()
{
    view_->setMouseTracking(false) ;

}

void FeatureSelectTool::mouseMoved(QMouseEvent *mouseEvent)
{
    QPoint pos = mouseEvent->pos() ;

    if ( is_panning_  ) {
        QPoint offset = click_ - pos ;
        view_->scroll(offset) ;
        click_ = pos ;
    }

}

void FeatureSelectTool::wheelEvent(QWheelEvent *e)
{
    current_ = view_->positionToDisplay(e->pos()) ;

}

void FeatureSelectTool::mousePressed(QMouseEvent *mouseEvent)
{
    click_ = mouseEvent->pos() ;
    start_ = view_->positionToDisplay(click_) ;
    current_ = start_ ;

    if ( mouseEvent->buttons() & Qt::RightButton )
        is_panning_ = true ;
}


void FeatureSelectTool::paint(QPainter &painter)
{


}

void FeatureSelectTool::mouseReleased(QMouseEvent *mouseEvent)
{
    if ( is_panning_ )
        is_panning_ = false ;
    else {
        QSharedPointer<MapOverlayManager> mgr = view_->getOverlayManager() ;

        MapOverlayPtr obj = mgr->findNearest("marker", start_, view_, 10) ;
        if ( !obj ) obj = mgr->findNearest("polygon", start_, view_, 10) ;

        if ( obj ) {
            emit featureClicked(obj->id()) ;
        }
    }

}


///////////////////////////////////////////////////////////////////////////////////


FeatureEditTool::FeatureEditTool(QObject *p): MapTool(p), is_dragging_(false) {
    undo_stack_ = new QUndoStack(this) ;
}

FeatureEditTool::~FeatureEditTool()
{

}

void FeatureEditTool::init(MapWidget *v)
{
    MapTool::init(v) ;
    is_dragging_ = false ;
    is_editing_ = false ;
    is_panning_ = false ;
    is_extending_ = 0 ;
    view_->setMouseTracking(true) ;

}

void FeatureEditTool::deinit()
{
    view_->setMouseTracking(false) ;

}

void FeatureEditTool::mouseMoved(QMouseEvent *mouseEvent)
{
    if ( !current_object_ ) return ;

    QPoint pos = mouseEvent->pos() ;

    if ( mouseEvent->buttons() & Qt::LeftButton )
    {
        if ( !view_->ensureVisible(pos) )
            view_->update() ;

        current_ = view_->positionToDisplay(pos) ;

        if ( is_extending_ ) {

            start_ = current_ ;
        }
        else {

        if ( current_touch_ & TOUCH_NODE )
        {
            QPointF start_coords = view_->displayToCoords(start_) ;
            QPointF new_coords = view_->displayToCoords(current_) ;

            FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_MOVE_NODE_CMD) ;
            ucmd->feature_ = current_object_->clone() ;
            ucmd->pt_ = start_coords ;
            ucmd->new_pt_ = new_coords ;
            ucmd->node_ = current_node_ ;

            undo_stack_->push(ucmd);

            start_ = current_ ;
            object_modified_ = true ;
        }
        else if ( current_touch_ == TOUCH_EDGE )
        {
            QPointF start_coords = view_->displayToCoords(start_) ;
            QPointF new_coords = view_->displayToCoords(current_) ;

            FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_MOVE_EDGE_CMD) ;
            ucmd->feature_ = current_object_->clone() ;
            ucmd->pt_ = start_coords ;
            ucmd->new_pt_ = new_coords ;
            ucmd->node_ = current_node_ ;

            undo_stack_->push(ucmd);

            start_ = current_ ;
            object_modified_ = true ;

        }
        }

    }
    else if ( mouseEvent->buttons() & Qt::RightButton && is_panning_ )
    {
        QPoint offset = start_point_ - pos ;

        view_->scroll(offset) ;

        start_point_ = pos ;
    }
    else if ( !is_extending_ )
    {

        current_ = view_->positionToDisplay(pos) ;

        current_touch_ = current_object_->touches(current_, view_, current_node_) ;

        if ( current_touch_ == TOUCH_NOTHING )
            view_->setCursor(QCursor(Qt::ArrowCursor)) ;
        else if ( current_touch_ & TOUCH_NODE ) {
            if ( mouseEvent->modifiers() & Qt::ControlModifier)
                view_->setCursor(QCursor(QPixmap(":/images/delete-vertex.png"))) ;
            else if ( ( mouseEvent->modifiers() & Qt::ShiftModifier ) && ( current_touch_ == ( TOUCH_NODE | TOUCH_END_POINT) ) )
                view_->setCursor(QCursor(QPixmap(":/images/append-node.png"))) ;
            else if ( ( mouseEvent->modifiers() & Qt::ShiftModifier ) && ( current_touch_ == ( TOUCH_NODE | TOUCH_BEGIN_POINT)  ) )
                view_->setCursor(QCursor(QPixmap(":/images/prepend-node.png"))) ;
            else
                view_->setCursor(QCursor(Qt::SizeAllCursor)) ;
        }
        else if ( current_touch_ == TOUCH_EDGE )
        {
            if ( mouseEvent->modifiers() & Qt::ShiftModifier)
                view_->setCursor(QCursor(QPixmap(":/images/insert-node.png"))) ;
            else
                view_->setCursor(QCursor(Qt::SizeAllCursor)) ;
        }
    }


}

void FeatureEditTool::wheelEvent(QWheelEvent *e)
{
    if ( is_editing_ )
        current_ = view_->positionToDisplay(e->pos()) ;
}

void FeatureEditTool::mousePressed(QMouseEvent *mouseEvent)
{
    click_ = mouseEvent->pos() ;
    start_ = view_->positionToDisplay(click_) ;
    current_ = start_ ;

    undo_stack_->setActive() ;
    undo_stack_->beginMacro("command");

    if ( mouseEvent->buttons() & Qt::RightButton ) {
        is_panning_ = true;
        start_point_ = click_;
    }
    else if ( is_editing_ && ( mouseEvent->buttons() & Qt::LeftButton ) && current_touch_ == TOUCH_NOTHING )
        is_editing_ = false ;
    else  if ( is_editing_ && ( mouseEvent->buttons() & Qt::LeftButton ) && ( current_touch_ & TOUCH_NODE ) &&
               ( mouseEvent->modifiers() & Qt::ControlModifier))
    {
        FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_DELETE_NODE_CMD) ;
        ucmd->feature_ = current_object_->clone() ;
        ucmd->node_ = current_node_ ;

        undo_stack_->push(ucmd);
        object_modified_ = true ;
    }
    else  if ( is_editing_ && ( mouseEvent->buttons() & Qt::LeftButton ) && current_touch_ == TOUCH_EDGE &&
               ( mouseEvent->modifiers() & Qt::ShiftModifier))
    {
        QPointF new_coords = view_->displayToCoords(current_) ;

        FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_INSERT_NODE_CMD) ;
        ucmd->feature_ = current_object_->clone() ;
        ucmd->node_ = current_node_ ;
        ucmd->new_pt_ = new_coords ;
        undo_stack_->push(ucmd);
        object_modified_ = true ;
        current_touch_ = TOUCH_NODE ;
        current_node_ = current_node_ + 1 ;
    }
    else if ( is_editing_ && ( mouseEvent->buttons() & Qt::LeftButton ) &&
               ( mouseEvent->modifiers() & Qt::ShiftModifier) && !is_extending_)
    {
        if ( current_touch_ == (TOUCH_NODE | TOUCH_END_POINT) )
            is_extending_ = 1 ;
        else if ( current_touch_ == (TOUCH_NODE | TOUCH_BEGIN_POINT) )
            is_extending_ = 2 ;
        else
            is_extending_ = 0 ;

    }
    else if ( is_extending_ )
    {
        QPointF pt = view_->displayToCoords(current_) ;

        if ( is_extending_ == 1 )
        {
            FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_APPEND_POINT_CMD) ;
            ucmd->feature_ = current_object_->clone() ;
            ucmd->pt_ = pt ;
            undo_stack_->push(ucmd);

            current_node_ = current_object_->numNodes() - 1  ;
            object_modified_ = true ;
        }
        else {
            FeatureEditUndoCommand *ucmd = new FeatureEditUndoCommand(view_, this, FEATURE_PREPEND_POINT_CMD) ;
            ucmd->feature_ = current_object_->clone() ;
            ucmd->pt_ = pt ;
            undo_stack_->push(ucmd);

            current_node_ = 0  ;
            object_modified_ = true ;
        }

    }
}



void FeatureEditTool::mouseDoubleClicked(QMouseEvent *mouseEvent)
{
    is_extending_ = false ;
}

void FeatureEditTool::paint(QPainter &painter)
{


}

void FeatureEditTool::mouseReleased(QMouseEvent *mouseEvent)
{

    if ( !is_editing_ )
    {
        QSharedPointer<MapOverlayManager> mgr = view_->getOverlayManager() ;
        QPointF coords = view_->displayToCoords(start_) ;

        if ( current_object_ != 0 )
        {
            if ( object_modified_ )
            {
                undo_stack_->clear() ;
                view_->saveOverlay(current_object_, true) ;
            }

            current_object_->setActive(false) ;
            view_->update();

            current_object_.clear() ;
            view_->setCurrentOverlay(current_object_) ;
        }

        MapOverlayPtr obj = mgr->findNearest("marker", start_, view_, 10) ;
        if ( !obj ) obj = mgr->findNearest("polygon", start_, view_, 10) ;

        if ( obj ) {
            current_object_ = obj ;
            current_object_->setActive(true) ;
            view_->setCurrentOverlay(current_object_) ;
            view_->update() ;
            is_editing_ = true ;
            object_modified_ = true ;
        }

    }
    else
    {

        view_->setCursor(QCursor(Qt::ArrowCursor)) ;
    }

    undo_stack_->endMacro() ;
    is_panning_ = false ;

}


///////////////////////////////////////////////////////////////////////////////////////////////

void FeatureEditUndoCommand::undo()
{
    switch ( cmd_ )
    {
    case FEATURE_APPEND_POINT_CMD:
    {
        PolygonOverlay *feature = static_cast<PolygonOverlay *>(feature_.data()) ;
        feature->popBack() ;
        view_->update() ;
        break ;
    }
    case FEATURE_PREPEND_POINT_CMD:
    {
        PolygonOverlay *feature = static_cast<PolygonOverlay *>(feature_.data()) ;
        feature->popFront() ;
        view_->update() ;
        break ;
    }
    case FEATURE_MOVE_NODE_CMD:
    {
        feature_->moveNode(node_, pt_ - new_pt_) ;
        view_->update() ;
        break ;
    }
    case FEATURE_MOVE_EDGE_CMD:
    {
        feature_->moveEdge(pt_ - new_pt_) ;
        view_->update() ;
        break ;
    }
    case FEATURE_DELETE_NODE_CMD:
    {
        feature_ = old_feature_->clone() ;
        view_->update();
        break ;
    }
    case FEATURE_INSERT_NODE_CMD:
    {
        feature_->deleteNode(node_) ;
        node_ -- ;
        view_->update() ;
        break ;
    }

    }

    tool_->current_object_ = feature_ ;
    view_->setCurrentOverlay(feature_);
}

void FeatureEditUndoCommand::redo()
{
    switch ( cmd_ )
    {
    case FEATURE_APPEND_POINT_CMD:
    {
        PolygonOverlay *feature = static_cast<PolygonOverlay *>(feature_.data()) ;
        feature->addPoint(pt_) ;
        view_->update() ;
        break ;
    }
    case FEATURE_PREPEND_POINT_CMD:
    {
        PolygonOverlay *feature = static_cast<PolygonOverlay *>(feature_.data()) ;
        feature->prepend(pt_) ;;
        view_->update() ;
        break ;
    }
    case FEATURE_MOVE_NODE_CMD:
    {
        feature_->moveNode(node_, new_pt_ - pt_) ;
        view_->update();
        break ;
    }
    case FEATURE_MOVE_EDGE_CMD:
    {
        feature_->moveEdge(new_pt_ - pt_) ;
        view_->update();
        break ;
    }
    case FEATURE_DELETE_NODE_CMD:
    {
        old_feature_ = feature_->clone() ;
        feature_->deleteNode(node_) ;
        view_->update();
        break ;
    }
    case FEATURE_INSERT_NODE_CMD:
    {
        old_feature_ = feature_->clone() ;
        feature_->insertNode(node_, new_pt_) ;
        node_ ++ ;
        view_->update();
        break ;
    }

    }

    tool_->current_object_ = feature_ ;
    view_->setCurrentOverlay(feature_);
}

