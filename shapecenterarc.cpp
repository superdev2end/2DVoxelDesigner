#include "shapecenterarc.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeCenterArc::ShapeCenterArc(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsPathItem(this);
    m_mainLine[0] = new QGraphicsLineItem(this);
    m_mainLine[1] = new QGraphicsLineItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    m_bTwoPointMode = true;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

ShapeCenterArc::~ShapeCenterArc() {
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
    if(m_mainLine[0] != nullptr) {
        delete m_mainLine[0];
    }
    if(m_mainLine[1] != nullptr) {
        delete m_mainLine[1];
       m_mainLine[1] = nullptr;
    }
}

QPoint ShapeCenterArc::getFirstPoint() {
    return m_firstPos;
}

QPoint ShapeCenterArc::getSecondPoint() {
    return m_secondPos;
}

QPoint ShapeCenterArc::getThirdPoint() {
    return m_thirdPos;
}

void ShapeCenterArc::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void ShapeCenterArc::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void ShapeCenterArc::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_firstPos.x() << " " << m_firstPos.y() << " " << m_secondPos.x() << " " << m_secondPos.y() << " " << m_thirdPos.x() << " " << m_thirdPos.y();
    *stream << endl;
}

void ShapeCenterArc::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    *stream >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    m_firstPos = QPoint(x0, y0);
    m_secondPos = QPoint(x1, y1);
    m_thirdPos = QPoint(x2, y2);
}

QRectF ShapeCenterArc::boundingRect() const {
    return m_boundingRectF;
}

void ShapeCenterArc::removeArrows() {
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

void ShapeCenterArc::setTwoPointMode(bool f) {
    m_bTwoPointMode = f;
}

void ShapeCenterArc::addArrow(int index) {
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
    if(index == 0) {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setCenterPoint(m_firstPos);
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_firstPos.x() - 5 / m_scaleFactor, m_firstPos.y() - 5 / m_scaleFactor), QPoint(m_firstPos.x() + 5 / m_scaleFactor, m_firstPos.y() + 5 / m_scaleFactor)));
    } else if(index == 1) {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_secondPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_secondPos.x() - 5 / m_scaleFactor, m_secondPos.y() - 5 / m_scaleFactor), QPoint(m_secondPos.x() + 5 / m_scaleFactor, m_secondPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setAngle(0);
        float length = magnitude(m_firstPos, m_secondPos);
        if(m_thirdPos.x() - m_firstPos.x() == 0 || m_thirdPos.y() - m_firstPos.y() == 0) {
            return;
        }
        float angle = qAtan2(m_thirdPos.y() - m_firstPos.y(), m_thirdPos.x() - m_firstPos.x());
        float x2 = m_firstPos.x() + length * qCos(angle);
        float y2 = m_firstPos.y() + length * qSin(angle);
        QPointF temp = QPointF(x2, y2);
        _moving_points[index]->setCenterPoint(QPoint(x2, y2));
        _moving_points[index]->setBoudingRect(QRectF(QPoint(temp.x() - 5 / m_scaleFactor, temp.y() - 5 / m_scaleFactor), QPoint(temp.x() + 5 / m_scaleFactor, temp.y() + 5 / m_scaleFactor)));
    }
}

void ShapeCenterArc::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeCenterArc::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeCenterArc::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeCenterArc::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapeCenterArc::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint origin = rotateDotByAngle(QPoint(m_firstPos.x(), m_firstPos.y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        QPointF closed = closestPointOnPath(origin, roundRectPath);
        float distance = magnitude(QPoint(closed.x(), closed.y()), QPoint(origin.x(), origin.y()));
        if(isNearBy(distance, m_scaleFactor, "CENTER ARC")) {
            setSelectionState(true);
            m_nSelectedArrow = 0;
            emit selectedObject(this, 0);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeCenterArc::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_mainLine[0]->setPen(m_pen);
        m_mainLine[1]->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_mainLine[0]->setPen(m_pen);
        m_mainLine[1]->setPen(m_pen);
    }
}

void ShapeCenterArc::redraw() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->updateView();
    }
    if(_moving_points[2] != nullptr) {
        _moving_points[2]->updateView();
    }
}

void ShapeCenterArc::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeCenterArc::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_firstPos = data.at(0);
    m_secondPos = data.at(1);
    updateShapeRectangle();
}

void ShapeCenterArc::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 0) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_firstPos = moveX(m_firstPos, first, second, getAngle());
            m_secondPos = moveX(m_secondPos, first, second, getAngle());
            m_thirdPos = moveX(m_thirdPos, first, second, getAngle());
            setCenterPoint(m_firstPos);
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_firstPos = moveY(m_firstPos, first, second, getAngle());
            m_secondPos = moveY(m_secondPos, first, second, getAngle());
            m_thirdPos = moveY(m_thirdPos, first, second, getAngle());
            setCenterPoint(m_firstPos);
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            setCenterPoint(m_firstPos);
            float angle = rotate(m_firstPos, first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        }
    }
    _moving_points[m_nSelectedArrow]->updateView();
    updateShapeRectangle();
}

void ShapeCenterArc::updateShapeRectangle() {
    if(m_bTwoPointMode) {
        m_mainRect->hide();
        m_mainLine[0]->setLine(QLineF(m_firstPos, m_secondPos));
        m_mainLine[1]->setLine(QLineF());
    } else {
        m_mainLine[0]->setLine(QLineF(m_firstPos, m_secondPos));
        float length = magnitude(m_firstPos, m_secondPos);
        if(m_thirdPos.x() - m_firstPos.x() == 0 || m_thirdPos.y() - m_firstPos.y() == 0) {
            return;
        }
        float angle0 = qAtan2(m_secondPos.y() - m_firstPos.y(), m_secondPos.x() - m_firstPos.x());

        float angle = qAtan2(m_thirdPos.y() - m_firstPos.y(), m_thirdPos.x() - m_firstPos.x());
        if(angle < angle0) {
            angle += 2 * M_PI;
        }
        float x2 = m_firstPos.x() + length * qCos(angle);
        float y2 = m_firstPos.y() + length * qSin(angle);
        m_mainLine[1]->setLine(QLineF(m_firstPos, (QPointF(x2, y2))));
        angle0 = qRadiansToDegrees(angle0);
        angle = qRadiansToDegrees(angle);
        roundRectPath = QPainterPath();
        roundRectPath.arcMoveTo(m_boundingRectF, 180 - angle0);
        roundRectPath.arcTo(m_boundingRectF, 180 - angle0, - angle + angle0);
        m_mainRect->setPath(roundRectPath);
        setBoudingRect(QRectF(QPointF(m_firstPos.x() + length, m_firstPos.y() + length), QPointF(m_firstPos.x() - length, m_firstPos.y() - length)));
        m_mainRect->show();
    }
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
        m_mainLine[0]->setPen(m_pen);
        m_mainLine[1]->setPen(m_pen);
    }
    if(getIsDrawing()) {
        m_mainLine[0]->show();
        m_mainLine[1]->show();
    } else {
        m_mainLine[0]->hide();
        m_mainLine[1]->hide();
    }
    m_mainRect->setTransformOriginPoint(getCenterPoint());
    m_mainRect->setRotation(getAngle());
    m_mainLine[0]->setTransformOriginPoint(getCenterPoint());
    m_mainLine[0]->setRotation(getAngle());
    m_mainLine[1]->setTransformOriginPoint(getCenterPoint());
    m_mainLine[1]->setRotation(getAngle());
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

bool ShapeCenterArc::getTwoPointMode() {
    return m_bTwoPointMode;
}

void ShapeCenterArc::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void ShapeCenterArc::setSecondPos(QPoint point) {
    m_secondPos = point;
    updateShapeRectangle();
}

void ShapeCenterArc::setEndPos(QPoint point) {
    m_thirdPos = point;
    updateShapeRectangle();
}
