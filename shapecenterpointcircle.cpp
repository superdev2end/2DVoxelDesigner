#include "shapecenterpointcircle.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeCenterPointCircle::ShapeCenterPointCircle(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsEllipseItem(this);
    _moving_point = nullptr;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

ShapeCenterPointCircle::~ShapeCenterPointCircle() {
    if(m_mainRect != nullptr) {
        delete m_mainRect;
    }
    if(_moving_point != nullptr) {
        delete _moving_point;
        _moving_point = nullptr;
    }
}

void ShapeCenterPointCircle::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void ShapeCenterPointCircle::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void ShapeCenterPointCircle::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_centerPos.x() << " " << m_centerPos.y() << " " << m_endPos.x() << " " << m_endPos.y();
    *stream << endl;
}

void ShapeCenterPointCircle::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    *stream >> x0 >> y0 >> x1 >> y1;
    m_centerPos = QPoint(x0, y0);
    m_endPos = QPoint(x1, y1);
}

QRectF ShapeCenterPointCircle::boundingRect() const {
    return m_boundingRectF;
}

void ShapeCenterPointCircle::removeArrows() {
    if(_moving_point != nullptr) {
        _moving_point->hide();
    }
    setSelectionState(false);
}

void ShapeCenterPointCircle::addArrow(int index) {
    if(index < 0 || index > 0) {
        return;
    }
    if(_moving_point == nullptr) {
        _moving_point = new MovingArrow(this);
    }
    _moving_point->setIsShowRotation(false);
    _moving_point->show();
    setSelectionState(true);
    _moving_point->setScaleFactor(m_scaleFactor);
    _moving_point->setIsDrawing(getIsDrawing());
    _moving_point->setBoudingRect(QRectF(QPoint(m_centerPos.x() - 5 / m_scaleFactor, m_centerPos.y() - 5 / m_scaleFactor), QPoint(m_centerPos.x() + 5 / m_scaleFactor, m_centerPos.y() + 5 / m_scaleFactor)));
    _moving_point->update();
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void ShapeCenterPointCircle::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeCenterPointCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeCenterPointCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeCenterPointCircle::setMoveType(QPointF point) {
    if(_moving_point != nullptr) {
        m_nMoveType = _moving_point->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

QPoint ShapeCenterPointCircle::getStartPos() {
    return m_centerPos;
}

QPoint ShapeCenterPointCircle::getEndPos() {
    return m_endPos;
}

void ShapeCenterPointCircle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        float distance1 = magnitude(m_centerPos, m_endPos);
        float distance2 = magnitude(m_centerPos, QPoint(event->pos().x(), event->pos().y()));
        if(isNearBy(qAbs(distance1 - distance2), m_scaleFactor, "CENTER POINT CIRCLE")) {
            setSelectionState(true);
            m_nSelectedArrow = 0;
            emit selectedObject(this, 0);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeCenterPointCircle::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
    }
}

void ShapeCenterPointCircle::redraw() {
    if(_moving_point != nullptr) {
        _moving_point->updateView();
    }
}

void ShapeCenterPointCircle::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeCenterPointCircle::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_centerPos = data.at(0);
    m_endPos = data.at(1);
    updateShapeRectangle();
}

void ShapeCenterPointCircle::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 0) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_centerPos = moveX(m_centerPos, first, second, getAngle());
            m_endPos = moveX(m_endPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_centerPos = moveY(m_centerPos, first, second, getAngle());
            m_endPos = moveY(m_endPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
        }
    }
    updateShapeRectangle();
}

void ShapeCenterPointCircle::updateShapeRectangle() {
    float distance = magnitude(m_centerPos, m_endPos);
    setBoudingRect(QRectF(QPoint(m_centerPos.x() - distance, m_centerPos.y() - distance), QPoint(m_centerPos.x() + distance, m_centerPos.y() + distance)));
    m_mainRect->setRect(m_boundingRectF);
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
    }
    if(_moving_point != nullptr && _moving_point->isVisible()) {
        addArrow(0);
    }
}

void ShapeCenterPointCircle::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void ShapeCenterPointCircle::setEndPos(QPoint point) {
    m_endPos = point;
    updateShapeRectangle();
}
