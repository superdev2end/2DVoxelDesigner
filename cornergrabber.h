#ifndef CORNERGRABBER_H
#define CORNERGRABBER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>

class CornerGrabber : public QGraphicsItem
{

public:
    explicit CornerGrabber(QGraphicsItem *parent = nullptr,  int corner=0);

    int  getCorner(); ///< allows the owner to find out which coner this is
    void setMouseState(int); ///< allows the owner to record the current mouse state
    int  getMouseState(); ///< allows the owner to get the current mouse state
    void setRad(int r);
    int getRad();
    qreal mouseDownX;
    qreal mouseDownY;
    void setScaleFactor(float);
    void setRotateState(bool);

     enum {kMouseReleased=0, kMouseDown, kMouseMoving}; ///< define the mouse states

private:

    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) override;
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) override;

    // once the hover event handlers are implemented in this class,
    // the mouse events must allow be implemented because of
    // some linkage issue - apparrently there is some connection
    // between the hover events and mouseMove/Press/Release
    // events which triggers a vtable issue
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event ) override;
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event ) override;

    QColor _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen _outterborderPen; ///< the pen is used to paint the red/black border

    qreal   _width;
    qreal   _height;

    int _corner;// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box
    int _mouseButtonState;
    float _scaleFactor;
    bool m_bRotateState;
};

#endif // CORNERGRABBER_H
