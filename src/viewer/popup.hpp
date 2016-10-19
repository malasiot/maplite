#ifndef __POPUP_HPP__
#define __POPUP_HPP__

#include <QWidget>
#include <QSize>
#include <QPoint>
#include <QTextEdit>
#include <QMouseEvent>

class Popup: public QWidget {
    Q_OBJECT
public:

    Popup(const QSize &sz, QWidget *parent) ;

    void setText(const QString &html) ;
    void show(const QPoint &p) ;

    void paintEvent(QPaintEvent *e) ;
    void mousePressEvent(QMouseEvent *e) ;


private:

    enum Anchor { Top, Bottom, Left, Right, TopLeft, TopRight, BottomLeft, BottomRight } ;
    QTextEdit *te_ ;
    QSize sz_ ;
    Anchor anchor_ ;
};


#endif
