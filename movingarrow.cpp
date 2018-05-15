#include "movingarrow.h"

MovingArrow::MovingArrow(QGraphicsItem *parent)  {
    setParentItem(parent);
    _arrows[0] = new Arrow(this);
    _arrows[1] = new Arrow(this);
    _movingARC = new MovingArc(this);
    m_mainRect = new QGraphicsRectItem(this);
    m_angle = 0;
}

MovingArrow::~MovingArrow() {
    if(_movingARC != nullptr) {
        delete _movingARC;
        _movingARC = nullptr;
    }
    if(_arrows[0] != nullptr) {
        delete _arrows[0];
        _arrows[0] = nullptr;
    }
    if(_arrows[0] != nullptr) {
        delete _arrows[1];
        _arrows[1] = nullptr;
    }
    delete m_mainRect;
}

QRectF MovingArrow::boundingRect() const {
    return m_boundingRectF;
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void MovingArrow::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void MovingArrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void MovingArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

void MovingArrow::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void MovingArrow::setIsShowRotation(bool f) {
    if(_movingARC == nullptr) {
        return;
    }
    if(f) {
        _movingARC->show();
    } else {
        _movingARC->hide();
    }
}

void MovingArrow::setAngle(float angle) {
    m_angle = angle;
}

void MovingArrow::setCenterPoint(QPoint point) {
    m_centerPoint = point;
}

float MovingArrow::getAngle() {
    return m_angle;
}

QPoint MovingArrow::getCenterPoint() {
    return m_centerPoint;
}

void MovingArrow::setIsDrawing(bool f) {
    m_bDrawing = f;
    if(_movingARC == nullptr) {
        _movingARC = new MovingArc(this);
    }
    _movingARC->setIsDrawing(f);
}
bool MovingArrow::getIsDrawing() {
    return m_bDrawing;
}

void MovingArrow::setScaleFactor(float s){
    m_scaleFactor = s;
    updateMovingArrow();
}

int MovingArrow::setMoveType(QPointF point) {
    QRect yRect = QRect(QPoint(m_boundingRectF.left() - 10 / m_scaleFactor, m_boundingRectF.top() - 90 / m_scaleFactor), QPoint(m_boundingRectF.right() + 10 / m_scaleFactor, m_boundingRectF.top() - 10 / m_scaleFactor));

    QRect xRect = QRect(QPoint(m_boundingRectF.right() + 10 / m_scaleFactor, m_boundingRectF.top() - 10 / m_scaleFactor), QPoint(m_boundingRectF.right() + 90 / m_scaleFactor, m_boundingRectF.bottom() + 10 / m_scaleFactor));
    QTransform transform = QTransform().translate(m_centerPoint.x(), m_centerPoint.y()).rotate(m_angle).translate(-m_centerPoint.x(), -m_centerPoint.y());
    if(_movingARC->isIn(point)) {
        m_nMoveType = MOVE_MODE_ROTATE;
        _arrows[0]->setHovered(false);
        _arrows[1]->setHovered(false);
        _movingARC->setHovered(true);
    } else if((transform.mapToPolygon(yRect)).containsPoint(QPoint(point.x(), point.y()), Qt::OddEvenFill)) {
        m_nMoveType = MOVE_MODE_Y;
        _arrows[0]->setHovered(true);
        _arrows[1]->setHovered(false);
        _movingARC->setHovered(false);
    } else if((transform.mapToPolygon(xRect)).containsPoint(QPoint(point.x(), point.y()), Qt::OddEvenFill)) {
        m_nMoveType = MOVE_MODE_X;
        _arrows[0]->setHovered(false);
        _arrows[1]->setHovered(true);
        _movingARC->setHovered(false);
    } else {
        m_nMoveType = MOVE_MODE_NONE;
        _arrows[0]->setHovered(false);
        _arrows[1]->setHovered(false);
        _movingARC->setHovered(false);
    }
    return m_nMoveType;
}

void MovingArrow::updateMovingArrow() {
    if(m_mainRect == nullptr) {
        m_mainRect = new QGraphicsRectItem(this);
    }
    m_mainRect->setBrush(QBrush(Qt::white));
    m_mainRect->setRect(m_boundingRectF);
    if(m_bDrawing) {
        if(_arrows[0] != nullptr) {
            delete _arrows[0];
            _arrows[0] = nullptr;
        }
        if(_arrows[1] != nullptr) {
            delete _arrows[1];
            _arrows[1] = nullptr;
        }
        if(_movingARC != nullptr) {
            delete _movingARC;
            _movingARC = nullptr;
        }
    } else {
        if(_arrows[0] == nullptr) {
            _arrows[0] = new Arrow(this);
        }
        _arrows[0]->setAngle(m_angle);
        _arrows[0]->setCenterPoint(m_centerPoint);
        _arrows[0]->setScaleFactor(m_scaleFactor);
        _arrows[0]->setBoudingRect(QRectF(QPoint(m_boundingRectF.left() - 10 / m_scaleFactor, m_boundingRectF.top() - 90 / m_scaleFactor), QPoint(m_boundingRectF.right() + 10 / m_scaleFactor, m_boundingRectF.top() - 10 / m_scaleFactor)), 0);
        if(_arrows[1] == nullptr) {
            _arrows[1] = new Arrow(this);
        }
        _arrows[1]->setAngle(m_angle);
        _arrows[1]->setCenterPoint(m_centerPoint);
        _arrows[1]->setScaleFactor(m_scaleFactor);
        _arrows[1]->setBoudingRect(QRectF(QPoint(m_boundingRectF.right() + 10 / m_scaleFactor, m_boundingRectF.top() - 10 / m_scaleFactor), QPoint(m_boundingRectF.right() + 90 / m_scaleFactor, m_boundingRectF.bottom() + 10 / m_scaleFactor)), 1);

        if(_movingARC == nullptr) {
            _movingARC = new MovingArc(this);
        }
        _movingARC->setAngle(m_angle);
        _movingARC->setCenterPoint(m_centerPoint);
        _movingARC->setScaleFactor(m_scaleFactor);
        _movingARC->setBoudingRect(QRectF(QPoint(m_boundingRectF.right() + 90 / m_scaleFactor, m_boundingRectF.top() - 90 / m_scaleFactor), QPoint(m_boundingRectF.left() - 90 / m_scaleFactor, m_boundingRectF.bottom() + 90 / m_scaleFactor)));
    }
    setZValue(1000);
}

void MovingArrow::updateView() {
    if(_movingARC == nullptr) {
        _movingARC = new MovingArc(this);
    }
    _movingARC->setVisible(isVisible());
    _movingARC->updateArc();
}

void MovingArrow::setBoudingRect(QRectF rect) {
    m_boundingRectF = rect;
    updateMovingArrow();
}
