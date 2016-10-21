#include "popup.hpp"

#include <QTextDocument>
#include <QPainter>
#include <QDebug>

Popup::Popup(const QSize &sz, QWidget *parent): QWidget(parent), sz_(sz) {
    hide() ;
    te_ = new QTextEdit(this) ;
    te_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te_->resize(sz_) ;
    te_->setFrameStyle(QFrame::NoFrame);
}

const int border = 16 ;
const int arrow_border = 20 ;

void Popup::setText(const QString &html)
{
    te_->setText(html) ;
    QTextDocument *td = te_->document() ;
    td->setTextWidth(sz_.width()) ;

    QSizeF sz = td->size() ;

    te_->resize(sz.width(), std::min((int)sz.height(), sz_.height()) ) ;
}

void Popup::show(const QPoint &p)
{
    QRect rect = parentWidget()->rect(), r ;
    QSize tes = te_->size() ;
    int border2 = border + border ;

    anchor_ = Bottom ;
    r = QRect(p.x() - tes.width()/2 - border, p.y() - tes.height() - border2 - arrow_border, tes.width() + border2, tes.height() + border2 + arrow_border) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = Top ;
    r = QRect(p.x() - tes.width()/2 - border, p.y(), tes.width() + border2, tes.height() + border2 + arrow_border) ;

    if ( rect.contains(r) ) {
        te_->move(border, border + arrow_border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = Left ;
    r = QRect(p.x(), p.y() - tes.height()/2 - border, tes.width() + border2 + arrow_border, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border + arrow_border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = Right ;
    r = QRect(p.x() - tes.width() - border2 - arrow_border, p.y() - tes.height()/2 - border, tes.width() + border2 + arrow_border, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = TopLeft ;
    r = QRect(p.x(), p.y(), tes.width() + border2, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = TopRight ;
    r = QRect(p.x() - tes.width() - border2, p.y(), tes.width() + border2, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = BottomLeft ;
    r = QRect(p.x(), p.y() - tes.height() - border2, tes.width() + border2, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }

    anchor_ = BottomRight ;
    r = QRect(p.x() - tes.width() - border2, p.y() - tes.height() - border2, tes.width() + border2, tes.height() + border2) ;

    if ( rect.contains(r) ) {
        te_->move(border, border) ;
        move(r.topLeft()) ;
        resize(r.size()) ;
        QWidget::show() ;
        return ;
    }
}

void Popup::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QRect frameRect = rect() ;

    if ( anchor_ == Bottom ) frameRect.adjust(0, 0, 0, -arrow_border) ;
    else if ( anchor_ == Top ) frameRect.adjust(0, arrow_border, 0, 0) ;
    else if ( anchor_ == Left ) frameRect.adjust(arrow_border, 0, 0, 0) ;
    else if ( anchor_ == Right ) frameRect.adjust(0, 0, -arrow_border, 0) ;

    QPainterPath rect ;
    rect.addRect(frameRect) ;

    QPainterPath triangle ;
    if ( anchor_ == Bottom ) {
        int center = (frameRect.left() + frameRect.right())/2.0 ;
        triangle.moveTo(center - arrow_border, frameRect.bottom() ) ;
        triangle.lineTo(center + arrow_border, frameRect.bottom() ) ;
        triangle.lineTo(center, frameRect.bottom() + arrow_border) ;
        triangle.closeSubpath();
    }
    else if ( anchor_ == Top ) {
        int center = (frameRect.left() + frameRect.right())/2.0 ;
        triangle.moveTo(center - arrow_border, frameRect.top() ) ;
        triangle.lineTo(center + arrow_border, frameRect.top() ) ;
        triangle.lineTo(center, frameRect.top() - arrow_border) ;
        triangle.closeSubpath();
    }
    else if ( anchor_ == Left ) {
        int center = (frameRect.top() + frameRect.bottom())/2.0 ;
        triangle.moveTo(frameRect.left(), center - arrow_border ) ;
        triangle.lineTo(frameRect.left(), center + arrow_border ) ;
        triangle.lineTo(frameRect.left() - arrow_border, center) ;
        triangle.closeSubpath();
    }
    else if ( anchor_ == Right ) {
        int center = (frameRect.top() + frameRect.bottom())/2.0 ;
        triangle.moveTo(frameRect.right(), center - arrow_border ) ;
        triangle.lineTo(frameRect.right(), center + arrow_border ) ;
        triangle.lineTo(frameRect.right() + arrow_border, center) ;
        triangle.closeSubpath();
    }

    painter.fillPath(triangle + rect, QBrush(Qt::white)) ;

    QPoint center(frameRect.right()-8, frameRect.top()+8) ;
    QPainterPath cross ;
    cross.moveTo(center.x()-4, center.y()-4) ;
    cross.lineTo(center.x()+4, center.y()+4) ;
    cross.moveTo(center.x()+4, center.y()-4) ;
    cross.lineTo(center.x()-4, center.y()+4) ;
    painter.strokePath(cross, QPen(Qt::black)) ;

}

void Popup::mousePressEvent(QMouseEvent *e)
{
    QPoint click = e->pos() ;

    QRect frameRect = rect() ;

    if ( anchor_ == Bottom ) frameRect.adjust(0, 0, 0, -arrow_border) ;
    else if ( anchor_ == Top ) frameRect.adjust(0, arrow_border, 0, 0) ;
    else if ( anchor_ == Left ) frameRect.adjust(arrow_border, 0, 0, 0) ;
    else if ( anchor_ == Right ) frameRect.adjust(0, 0, -arrow_border, 0) ;

    QRect close_button(frameRect.right()-8, frameRect.top()+8, 8, 8) ;

    if ( close_button.contains(click) ) {
        hide() ;
        e->accept() ;
    }
}
