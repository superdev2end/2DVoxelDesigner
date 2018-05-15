#include "shapecentertocornerrectangle.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeCenterToCornerRectangle::ShapeCenterToCornerRectangle(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsRectItem(this);
    _moving_point = nullptr;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

ShapeCenterToCornerRectangle::~ShapeCenterToCornerRectangle() {
    if(m_mainRect != nullptr) {
        delete m_mainRect;
    }
    if(_moving_point != nullptr) {
        delete _moving_point;
        _moving_point = nullptr;
    }
}

void ShapeCenterToCornerRectangle::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void ShapeCenterToCornerRectangle::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void ShapeCenterToCornerRectangle::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_centerPos.x() << " " << m_centerPos.y() << " " << m_endPos.x() << " " << m_endPos.y();
    *stream << endl;
}

void ShapeCenterToCornerRectangle::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    *stream >> x0 >> y0 >> x1 >> y1;
    m_centerPos = QPoint(x0, y0);
    m_endPos = QPoint(x1, y1);
}

QRectF ShapeCenterToCornerRectangle::boundingRect() const {
    return m_boundingRectF;
}

void ShapeCenterToCornerRectangle::removeArrows() {
    if(_moving_point != nullptr) {
        _moving_point->hide();
    }
    setSelectionState(false);
}

void ShapeCenterToCornerRectangle::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 0) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_centerPos = moveX(m_centerPos, first, second, getAngle());
            m_endPos = moveX(m_endPos, first, second, getAngle());
            setCenterPoint(m_centerPos);
            _moving_point->setCenterPoint(getCenterPoint());

        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_centerPos = moveY(m_centerPos, first, second, getAngle());
            m_endPos = moveY(m_endPos, first, second, getAngle());
            setCenterPoint(m_centerPos);
            _moving_point->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            setCenterPoint(m_centerPos);
            _moving_point->setCenterPoint(getCenterPoint());
            float angle = rotate(getCenterPoint(), first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_point->setAngle(getAngle());
        }
        _moving_point->updateView();
    }
    updateShapeRectangle();
}

void ShapeCenterToCornerRectangle::addArrow(int index) {
    if(index < 0 || index > 0) {
        return;
    }
    if(_moving_point == nullptr) {
        _moving_point = new MovingArrow(this);
    }
    _moving_point->setIsShowRotation(true);
    _moving_point->setCenterPoint(m_centerPos);
    _moving_point->setAngle(getAngle());
    _moving_point->show();
    setSelectionState(true);
    _moving_point->setScaleFactor(m_scaleFactor);
    _moving_point->setIsDrawing(getIsDrawing());
    _moving_point->setBoudingRect(QRectF(QPoint(m_centerPos.x() - 5 / m_scaleFactor, m_centerPos.y() - 5 / m_scaleFactor), QPoint(m_centerPos.x() + 5 / m_scaleFactor, m_centerPos.y() + 5 / m_scaleFactor)));
    _moving_point->update();
}

void ShapeCenterToCornerRectangle::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeCenterToCornerRectangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeCenterToCornerRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeCenterToCornerRectangle::setMoveType(QPointF point) {
    if(_moving_point != nullptr) {
        m_nMoveType = _moving_point->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

QPoint ShapeCenterToCornerRectangle::getStartPos() {
    return m_centerPos;
}

QPoint ShapeCenterToCornerRectangle::getEndPos() {
    return m_endPos;
}

void ShapeCenterToCornerRectangle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        int width  = 2 * qAbs(m_centerPos.x() - m_endPos.x());
        int height  = 2 * qAbs(m_centerPos.y() - m_endPos.y());
        QPoint point1 = m_centerPos  + QPoint( - width / 2,  - height / 2);
        QPoint point2 = m_centerPos  + QPoint(width / 2,  - height / 2);
        QPoint point3 = m_centerPos  + QPoint(width / 2, height / 2);
        QPoint point4 = m_centerPos  + QPoint(- width / 2, height / 2);
        point1 = rotateDotByAngle(m_centerPos, point1, getAngle());
        point2 = rotateDotByAngle(m_centerPos, point2, getAngle());
        point3 = rotateDotByAngle(m_centerPos, point3, getAngle());
        point4 = rotateDotByAngle(m_centerPos, point4, getAngle());
        float distance1 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point1, point2);
        float distance2 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point2, point3);
        float distance3 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point3, point4);
        float distance4 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point4, point1);
        if(isNearBy(distance1, m_scaleFactor, "CENTER TO CORNER RECTANGLE") || isNearBy(distance2, m_scaleFactor, "CENTER TO CORNER RECTANGLE") || isNearBy(distance3, m_scaleFactor, "CENTER TO CORNER RECTANGLE") || isNearBy(distance4, m_scaleFactor, "CENTER TO CORNER RECTANGLE") ) {
            setSelectionState(true);
            m_nSelectedArrow = 0;
            emit selectedObject(this, 0);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeCenterToCornerRectangle::setSelectionState(bool f) {
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

void ShapeCenterToCornerRectangle::redraw() {
    if(_moving_point != nullptr && _moving_point->isVisible()) {
        _moving_point->updateView();
    }
}

void ShapeCenterToCornerRectangle::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeCenterToCornerRectangle::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_centerPos = data.at(0);
    m_endPos = data.at(1);
    updateShapeRectangle();
}

void ShapeCenterToCornerRectangle::updateShapeRectangle() {
    int width = qAbs(m_endPos.x() - m_centerPos.x());
    int height = qAbs(m_endPos.y() - m_centerPos.y());
    float radious = magnitude(m_endPos, m_centerPos);
    setBoudingRect(QRectF(m_centerPos - QPointF(radious, radious), m_centerPos + QPointF(radious, radious)));
    m_mainRect->setRect(QRectF(m_centerPos - QPoint(width, height), m_centerPos + QPoint(width, height)));
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
    }
    m_mainRect->setTransformOriginPoint(getCenterPoint());
    m_mainRect->setRotation(getAngle());
    if(_moving_point != nullptr && _moving_point->isVisible()) {
        addArrow(0);
    }
}

void ShapeCenterToCornerRectangle::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void ShapeCenterToCornerRectangle::setEndPos(QPoint point) {
    m_endPos = point;
    updateShapeRectangle();
}
