#include "shape3pointcircle.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

Shape3PointCircle::Shape3PointCircle(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsEllipseItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    _moving_points[3] = nullptr;
    m_bTwoCircleMode = true;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

Shape3PointCircle::~Shape3PointCircle() {
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
        delete _moving_points[1];
        _moving_points[2] = nullptr;
    }
    if(_moving_points[3] != nullptr) {
        delete _moving_points[1];
        _moving_points[3] = nullptr;
    }
}

void Shape3PointCircle::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void Shape3PointCircle::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void Shape3PointCircle::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_firstPos.x() << " " << m_firstPos.y() << " " << m_secondPos.x() << " " << m_secondPos.y() << " " << m_thirdPos.x() << " " << m_thirdPos.y();
    *stream << endl;
}

void Shape3PointCircle::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    *stream >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    m_firstPos = QPoint(x0, y0);
    m_secondPos = QPoint(x1, y1);
    m_thirdPos = QPoint(x2, y2);
}

QRectF Shape3PointCircle::boundingRect() const {
    return m_boundingRectF;
}

void Shape3PointCircle::removeArrows() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->hide();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->hide();
    }
    if(_moving_points[2] != nullptr) {
        _moving_points[2]->hide();
    }
    if(_moving_points[3] != nullptr) {
        _moving_points[3]->hide();
    }
    setSelectionState(false);
}

QPoint Shape3PointCircle::getFirstPoint() {
    return m_firstPos;
}

QPoint Shape3PointCircle::getSecondPoint() {
    return m_secondPos;
}

QPoint Shape3PointCircle::getThirdPoint() {
    return m_thirdPos;
}

void Shape3PointCircle::setTwoCircleMode(bool f) {
    m_bTwoCircleMode = f;
}

void Shape3PointCircle::addArrow(int index) {
    if(index < 0 || index > 3) {
        return;
    }
    if(_moving_points[index] == nullptr) {
        _moving_points[index] = new MovingArrow(this);
    }
    _moving_points[index]->setScaleFactor(m_scaleFactor);
    _moving_points[index]->setIsDrawing(getIsDrawing());
    _moving_points[index]->setIsShowRotation(false);
    _moving_points[index]->show();
    setSelectionState(true);
    if(index == 0) {
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_firstPos.x() - 5 / m_scaleFactor, m_firstPos.y() - 5 / m_scaleFactor), QPoint(m_firstPos.x() + 5 / m_scaleFactor, m_firstPos.y() + 5 / m_scaleFactor)));
    } else if (index == 1) {
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_secondPos.x() - 5 / m_scaleFactor, m_secondPos.y() - 5 / m_scaleFactor), QPoint(m_secondPos.x() + 5 / m_scaleFactor, m_secondPos.y() + 5 / m_scaleFactor)));
    } else if(index == 2){
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_thirdPos.x() - 5 / m_scaleFactor, m_thirdPos.y() - 5 / m_scaleFactor), QPoint(m_thirdPos.x() + 5 / m_scaleFactor, m_thirdPos.y() + 5 / m_scaleFactor)));
    } else {
        QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(centerPoint.x() - 5 / m_scaleFactor, centerPoint.y() - 5 / m_scaleFactor), QPoint(centerPoint.x() + 5 / m_scaleFactor, centerPoint.y() + 5 / m_scaleFactor)));
    }
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void Shape3PointCircle::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Shape3PointCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void Shape3PointCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool Shape3PointCircle::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void Shape3PointCircle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        float distance1 = magnitude(centerPoint, m_firstPos);
        float distance2 = magnitude(centerPoint, QPoint(event->pos().x(), event->pos().y()));
        if(isNearBy(qAbs(distance1 - distance2), m_scaleFactor, "3 POINT CIRCLE")) {
            setSelectionState(true);
            m_nSelectedArrow = 3;
            emit selectedObject(this, 3);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void Shape3PointCircle::setSelectionState(bool f) {
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

void Shape3PointCircle::redraw() {
    if(_moving_points[0] != nullptr && _moving_points[0]->isVisible()) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr && _moving_points[0]->isVisible()) {
        _moving_points[1]->updateView();
    }
    if(_moving_points[2] != nullptr && _moving_points[0]->isVisible()) {
        _moving_points[2]->updateView();
    }
    if(_moving_points[3] != nullptr && _moving_points[3]->isVisible()) {
        _moving_points[3]->updateView();
    }
}

void Shape3PointCircle::setSelectable(bool f){
    //setFlag(ItemIsSelectable, f);
    m_bSelectable = f;
}

void Shape3PointCircle::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_firstPos = data.at(0);
    m_secondPos = data.at(1);
    updateShapeRectangle();
}

void Shape3PointCircle::updateShapeRectangle() {
    if(m_bTwoCircleMode) {
        float distance = magnitude(m_firstPos, m_secondPos) / 2;
        QPoint centerPoint = (m_firstPos + m_secondPos) / 2;
        setBoudingRect(QRectF(QPoint(centerPoint.x() - distance, centerPoint.y() - distance), QPoint(centerPoint.x() + distance, centerPoint.y() + distance)));
    } else {
        QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        float distance = magnitude(centerPoint, m_firstPos);
        setBoudingRect(QRectF(QPoint(centerPoint.x() - distance, centerPoint.y() - distance), QPoint(centerPoint.x() + distance, centerPoint.y() + distance)));
    }
    m_mainRect->setRect(m_boundingRectF);
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
    }
    if(_moving_points[0] != nullptr && _moving_points[0]->isVisible()) {
        addArrow(0);
    }
    if(_moving_points[1] != nullptr && _moving_points[1]->isVisible()) {
        addArrow(1);
    }
    if(_moving_points[2] != nullptr && _moving_points[2]->isVisible()) {
        addArrow(2);
    }
    if(_moving_points[3] != nullptr && _moving_points[3]->isVisible()) {
        addArrow(3);
    }
}

void Shape3PointCircle::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 3) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_firstPos = moveX(m_firstPos, first, second, getAngle());
            m_secondPos = moveX(m_secondPos, first, second, getAngle());
            m_thirdPos = moveX(m_thirdPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_firstPos = moveY(m_firstPos, first, second, getAngle());
            m_secondPos = moveY(m_secondPos, first, second, getAngle());
            m_thirdPos = moveY(m_thirdPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
        }
    }
    updateShapeRectangle();
}

bool Shape3PointCircle::getTwoCircleMode() {
    return m_bTwoCircleMode;
}

void Shape3PointCircle::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void Shape3PointCircle::setSecondPos(QPoint point) {
    m_secondPos = point;
    updateShapeRectangle();
}

void Shape3PointCircle::setEndPos(QPoint point) {
    m_thirdPos = point;
    updateShapeRectangle();
}

