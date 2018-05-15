#include "cornergrabber.h"
#include <QDebug>

CornerGrabber::CornerGrabber(QGraphicsItem *parent,  int corner) :
    QGraphicsItem(parent),
    mouseDownX(0),
    mouseDownY(0),
    _outterborderColor(Qt::black),
    _outterborderPen(),
    _width(6),
    _height(6),
    _corner(corner),
    _mouseButtonState(kMouseReleased)
{
    setParentItem(parent);
    m_bRotateState = false;
    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(_outterborderColor);

    if(_corner == 4)
        _outterborderColor = Qt::white;

    this->setAcceptHoverEvents(true);
    _scaleFactor = 1;
}

void CornerGrabber::setRotateState(bool f) {
    m_bRotateState = f;
    update(0,0,_width,_height);
}

void CornerGrabber::setMouseState(int s)
{
    _mouseButtonState = s;
}

void CornerGrabber::setScaleFactor(float f) {
    _scaleFactor = f;
}

int CornerGrabber::getMouseState()
{
    return _mouseButtonState;
}

int CornerGrabber::getCorner() {
    return _corner;
}

// we have to implement the mouse events to keep the linker happy,
// but just set accepted to false since are not actually handling them

void CornerGrabber::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(false);
}

void CornerGrabber::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(false);
}

void CornerGrabber::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(false);
}

// change the color on hover events to indicate to the use the object has
// been captured by the mouse

void CornerGrabber::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event) {
    _outterborderColor = Qt::black;
    if(_corner == 4)
        _outterborderColor = Qt::white;

    update(0,0,_width,_height);
}

void CornerGrabber::hoverEnterEvent ( QGraphicsSceneHoverEvent * event) {
    _outterborderColor = Qt::red;
    update(0,0,_width,_height);
}

QRectF CornerGrabber::boundingRect() const {
    return QRectF(0,0,_width,_height);
}

void CornerGrabber::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // fill the box with solid color, use sharp corners
    if(_corner != 4){
        _outterborderPen.setCapStyle(Qt::SquareCap);
        _outterborderPen.setStyle(Qt::SolidLine);
        painter->setPen(_outterborderPen);

        QPointF topLeft (0, 0);
        QPointF bottomRight ( _width, _height);

        QRectF rect (topLeft, bottomRight);

        QBrush brush (Qt::SolidPattern);
        brush.setColor (_outterborderColor);
        painter->fillRect(rect,brush);
    } else {
        int radious = 8 / _scaleFactor < 1 ? 1 : 8 / _scaleFactor;
        QPointF topLeft ( _width / 2 - radious / 2, _height / 2 - radious / 2);
        QPointF bottomRight ( _width / 2 + radious / 2, _height / 2 + radious / 2);

        QRectF rect (topLeft, bottomRight);
        QBrush brush (Qt::SolidPattern);
        brush.setColor (_outterborderColor);
        painter->setBrush(brush);

        QPen pen(m_bRotateState ? Qt::yellow : Qt::red);
        pen.setWidth(radious>2?radious/2:1);

        painter->setPen(pen);

        painter->drawEllipse(rect);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawLine(QLineF(QPointF(_width / 2, _height / 2), QPointF(_width / 2, _height)));
    }
}

int CornerGrabber::getRad() {
    return _corner == 4 ? _width : _width / 2;
}

void CornerGrabber::setRad(int r){
    if(_corner == 4) {
        _width = r;
        _height = r;
    } else {
        _width = 2 * r;
        _height = 2 * r;
    }
}
