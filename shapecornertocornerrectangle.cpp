#include "shapecornertocornerrectangle.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeCornerToCornerRectangle::ShapeCornerToCornerRectangle(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsRectItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

ShapeCornerToCornerRectangle::~ShapeCornerToCornerRectangle() {
    if(m_mainRect != nullptr) {
        delete m_mainRect;
    }
    if(_moving_points[0] != nullptr) {
        delete _moving_points[0];
        _moving_points[0] = nullptr;
    }
    if(_moving_points[1] != nullptr) {
        delete _moving_points[1];
        _moving_points[1] = nullptr;
    }
    if(_moving_points[2] != nullptr) {
        delete _moving_points[2];
        _moving_points[2] = nullptr;
    }
}

QPoint ShapeCornerToCornerRectangle::getStartPos() {
    return m_startPos;
}

QPoint ShapeCornerToCornerRectangle::getEndPos() {
    return m_endPos;
}

void ShapeCornerToCornerRectangle::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void ShapeCornerToCornerRectangle::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void ShapeCornerToCornerRectangle::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_startPos.x() << " " << m_startPos.y() << " " << m_endPos.x() << " " << m_endPos.y();
    *stream << endl;
}

void ShapeCornerToCornerRectangle::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    *stream >> x0 >> y0 >> x1 >> y1;
    m_startPos = QPoint(x0, y0);
    m_endPos = QPoint(x1, y1);
}

QRectF ShapeCornerToCornerRectangle::boundingRect() const {
    return m_boundingRectF;
}

void ShapeCornerToCornerRectangle::removeArrows() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->hide();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->hide();
    }
    if(_moving_points[2] != nullptr) {
        _moving_points[2]->hide();
    }
    setSelectionState(false);
}

void ShapeCornerToCornerRectangle::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 2) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_startPos = moveX(m_startPos, first, second, getAngle());
            m_endPos = moveX(m_endPos, first, second, getAngle());
            setCenterPoint((m_startPos + m_endPos) / 2);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_startPos = moveY(m_startPos, first, second, getAngle());
            m_endPos = moveY(m_endPos, first, second, getAngle());
            setCenterPoint((m_startPos + m_endPos) / 2);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            setCenterPoint((m_startPos + m_endPos) / 2);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
            float angle = rotate(getCenterPoint(), first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
        }
        _moving_points[m_nSelectedArrow]->updateView();
    }
    updateShapeRectangle();
}

void ShapeCornerToCornerRectangle::addArrow(int index) {
    if(index < 0 || index > 2) {
        return;
    }
    if(_moving_points[index] == nullptr) {
        _moving_points[index] = new MovingArrow(this);
    }
    _moving_points[index]->setScaleFactor(m_scaleFactor);
    _moving_points[index]->setIsDrawing(getIsDrawing());
    _moving_points[index]->show();
    setSelectionState(true);
    setCenterPoint((m_startPos + m_endPos) / 2);
    _moving_points[index]->setCenterPoint((m_startPos + m_endPos) / 2);
    if(index == 0) {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_startPos.x() - 5 / m_scaleFactor, m_startPos.y() - 5 / m_scaleFactor), QPoint(m_startPos.x() + 5 / m_scaleFactor, m_startPos.y() + 5 / m_scaleFactor)));
    } else if(index == 1) {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_endPos.x() - 5 / m_scaleFactor, m_endPos.y() - 5 / m_scaleFactor), QPoint(m_endPos.x() + 5 / m_scaleFactor, m_endPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setBoudingRect(QRectF(QPoint(getCenterPoint().x() - 5 / m_scaleFactor, getCenterPoint().y() - 5 / m_scaleFactor), QPoint(getCenterPoint().x() + 5 / m_scaleFactor, getCenterPoint().y() + 5 / m_scaleFactor)));
    }
    _moving_points[index]->update();
}

void ShapeCornerToCornerRectangle::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeCornerToCornerRectangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeCornerToCornerRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeCornerToCornerRectangle::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapeCornerToCornerRectangle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        int width  = qAbs(m_startPos.x() - m_endPos.x());
        int height  = qAbs(m_startPos.y() - m_endPos.y());
        QPoint centerPoint = (m_startPos + m_endPos) / 2;
        QPoint point1 = centerPoint  + QPoint( - width / 2,  - height / 2);
        QPoint point2 = centerPoint  + QPoint(width / 2,  - height / 2);
        QPoint point3 = centerPoint  + QPoint(width / 2, height / 2);
        QPoint point4 = centerPoint  + QPoint(- width / 2, height / 2);
        point1 = rotateDotByAngle(centerPoint, point1, getAngle());
        point2 = rotateDotByAngle(centerPoint, point2, getAngle());
        point3 = rotateDotByAngle(centerPoint, point3, getAngle());
        point4 = rotateDotByAngle(centerPoint, point4, getAngle());
        float distance1 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point1, point2);
        float distance2 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point2, point3);
        float distance3 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point3, point4);
        float distance4 = distancePointLine(QPoint(event->pos().x(), event->pos().y()), point4, point1);
        if(isNearBy(distance1, m_scaleFactor, "CORNER TO CORNER RECTANGLE") || isNearBy(distance2, m_scaleFactor, "CORNER TO CORNER RECTANGLE") || isNearBy(distance3, m_scaleFactor, "CORNER TO CORNER RECTANGLE") || isNearBy(distance4, m_scaleFactor, "CORNER TO CORNER RECTANGLE") ) {
            setSelectionState(true);
            m_nSelectedArrow = 2;
            emit selectedObject(this, 2);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeCornerToCornerRectangle::setSelectionState(bool f) {
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

void ShapeCornerToCornerRectangle::redraw() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->updateView();
    }
}

void ShapeCornerToCornerRectangle::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeCornerToCornerRectangle::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_startPos = data.at(0);
    m_endPos = data.at(1);
    updateShapeRectangle();
}

void ShapeCornerToCornerRectangle::updateShapeRectangle() {
    float radious = magnitude(m_startPos, (m_startPos + m_endPos) / 2);
    setBoudingRect(QRectF((m_startPos + m_endPos) / 2 - QPointF(radious, radious), (m_startPos + m_endPos) / 2 + QPointF(radious, radious)));
    m_mainRect->setRect(QRect(m_startPos, m_endPos));
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
    }
    m_mainRect->setTransformOriginPoint(getCenterPoint());
    m_mainRect->setRotation(getAngle());
    if(_moving_points[0] != nullptr && _moving_points[0]->isVisible()) {
        addArrow(0);
    }
    if(_moving_points[1] != nullptr && _moving_points[1]->isVisible()) {
        addArrow(1);
    }
    if(_moving_points[2] != nullptr && _moving_points[2]->isVisible()) {
        addArrow(2);
    }
}

void ShapeCornerToCornerRectangle::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void ShapeCornerToCornerRectangle::setEndPos(QPoint point) {
    m_endPos = point;
    updateShapeRectangle();
}
