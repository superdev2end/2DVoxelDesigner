#include "shape3pointarc.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

Shape3PointArc::Shape3PointArc(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsPathItem(this);
    m_line = new QGraphicsLineItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    _moving_points[3] = nullptr;
    m_bTwoCircleMode = true;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

Shape3PointArc::~Shape3PointArc() {
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
    if(_moving_points[3] != nullptr) {
        delete _moving_points[3];
        _moving_points[3] = nullptr;
    }
    if(m_line != nullptr) {
        delete m_line;
    }
}

QPoint Shape3PointArc::getFirstPoint() {
    return m_firstPos;
}

QPoint Shape3PointArc::getSecondPoint() {
    return m_secondPos;
}

QPoint Shape3PointArc::getThirdPoint() {
    return m_thirdPos;
}

void Shape3PointArc::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void Shape3PointArc::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void Shape3PointArc::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_firstPos.x() << " " << m_firstPos.y() << " " << m_secondPos.x() << " " << m_secondPos.y() << " " << m_thirdPos.x() << " " << m_thirdPos.y();
    *stream << endl;
}

void Shape3PointArc::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    *stream >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    m_firstPos = QPoint(x0, y0);
    m_secondPos = QPoint(x1, y1);
    m_thirdPos = QPoint(x2, y2);
}

QRectF Shape3PointArc::boundingRect() const {
    return m_boundingRectF;
}

void Shape3PointArc::removeArrows() {
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

void Shape3PointArc::setTwoCircleMode(bool f) {
    m_bTwoCircleMode = f;
}

void Shape3PointArc::addArrow(int index) {
    if(index < 0 || index > 3) {
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
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_firstPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_firstPos.x() - 5 / m_scaleFactor, m_firstPos.y() - 5 / m_scaleFactor), QPoint(m_firstPos.x() + 5 / m_scaleFactor, m_firstPos.y() + 5 / m_scaleFactor)));
    } else if(index == 1) {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_secondPos.x() - 5 / m_scaleFactor, m_secondPos.y() - 5 / m_scaleFactor), QPoint(m_secondPos.x() + 5 / m_scaleFactor, m_secondPos.y() + 5 / m_scaleFactor)));
    } else if(index == 2){
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_thirdPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_thirdPos.x() - 5 / m_scaleFactor, m_thirdPos.y() - 5 / m_scaleFactor), QPoint(m_thirdPos.x() + 5 / m_scaleFactor, m_thirdPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setAngle(getAngle());
        QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        _moving_points[index]->setCenterPoint(centerPoint);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(centerPoint.x() - 5 / m_scaleFactor, centerPoint.y() - 5 / m_scaleFactor), QPoint(centerPoint.x() + 5 / m_scaleFactor, centerPoint.y() + 5 / m_scaleFactor)));
    }
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void Shape3PointArc::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Shape3PointArc::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void Shape3PointArc::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

void Shape3PointArc::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        QPoint origin = rotateDotByAngle(QPoint(centerPoint.x(), centerPoint.y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        QPointF closed = closestPointOnPath(origin, roundRectPath);
        float distance = magnitude(QPoint(closed.x(), closed.y()), QPoint(origin.x(), origin.y()));
        if(isNearBy(distance, m_scaleFactor, "3 POINT ARC")) {
            setSelectionState(true);
            m_nSelectedArrow = 3;
            emit selectedObject(this, 3);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void Shape3PointArc::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_line->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_line->setPen(m_pen);
    }
}

void Shape3PointArc::redraw() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->updateView();
    }
    if(_moving_points[2] != nullptr) {
        _moving_points[2]->updateView();
    }
    if(_moving_points[3] != nullptr) {
        _moving_points[3]->updateView();
    }
}

void Shape3PointArc::setSelectable(bool f){
    //setFlag(ItemIsSelectable, f);
    m_bSelectable = f;
}

void Shape3PointArc::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_firstPos = data.at(0);
    m_secondPos = data.at(1);
    updateShapeRectangle();
}

bool Shape3PointArc::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void Shape3PointArc::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 3) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_firstPos = moveX(m_firstPos, first, second, getAngle());
            m_secondPos = moveX(m_secondPos, first, second, getAngle());
            m_thirdPos = moveX(m_thirdPos, first, second, getAngle());
            QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
            setCenterPoint(centerPoint);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_firstPos = moveY(m_firstPos, first, second, getAngle());
            m_secondPos = moveY(m_secondPos, first, second, getAngle());
            m_thirdPos = moveY(m_thirdPos, first, second, getAngle());
            QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
            setCenterPoint(centerPoint);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            QPoint centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
            setCenterPoint(centerPoint);
            float angle = rotate(centerPoint, first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        }
    }
    _moving_points[m_nSelectedArrow]->updateView();

    updateShapeRectangle();
}

void Shape3PointArc::updateShapeRectangle() {
    if(m_bTwoCircleMode) {
        m_mainRect->hide();
        m_line->setLine(QLineF(m_firstPos, m_secondPos));
        m_line->show();
    } else {
        if(m_firstPos == m_secondPos ||
                m_firstPos == m_thirdPos ||
                m_secondPos == m_thirdPos)
            return;

        m_line->hide();
        QPointF centerPoint = calculate3PointCircle(m_firstPos, m_secondPos, m_thirdPos);
        float length = magnitude(centerPoint, m_secondPos);
        if(float(m_firstPos.x()) - centerPoint.x() == 0 || float(m_firstPos.y()) - centerPoint.y() == 0) {
            return;
        }
        float angle0 = qAtan2(float(m_firstPos.y()) - centerPoint.y(), float(m_firstPos.x()) - centerPoint.x());
        float angle = qAtan2(float(m_secondPos.y()) - centerPoint.y(), float(m_secondPos.x()) - centerPoint.x());
        float angle1 = qAtan2(float(m_thirdPos.y()) - centerPoint.y(), float(m_thirdPos.x()) - centerPoint.x());
        if(angle0 < 0) {
            angle0 += 2 * M_PI;
        }
        if(angle < 0) {
            angle += 2 * M_PI;
        }
        if(angle1 < 0) {
            angle1 += 2 * M_PI;
        }
        float temp = 0;
        float angle_temp = 0;

        if(angle0 >= angle && angle0 >= angle1){
            if(angle1 > angle){
                angle_temp = angle0;
                temp = angle0 - angle;
            }else{
                angle_temp = angle;
                temp = 2 * M_PI - (angle0 - angle);
            }
        }else if(angle >= angle0 && angle >= angle1){
            if(angle1 > angle0){
                angle_temp = angle;
                temp = angle - angle0;
            }else{
                angle_temp = angle0;
                temp = 2 * M_PI - (angle - angle0);
            }
        }else if(angle1 >= angle0 && angle1 >= angle){
            if(angle > angle0){
                angle_temp = angle0;
                temp = 2 * M_PI - (angle - angle0);
            }else{
                angle_temp = angle;
                temp = 2 * M_PI - (angle0 - angle);
            }
        }

        angle_temp = qRadiansToDegrees(angle_temp);
        roundRectPath = QPainterPath();
        roundRectPath.arcMoveTo(m_boundingRectF, 180.0f - angle_temp);
        roundRectPath.arcTo(m_boundingRectF, 180.0f - angle_temp, qRadiansToDegrees(temp));
        m_mainRect->setPath(roundRectPath);
        setBoudingRect(QRectF(QPointF(centerPoint.x() + length, centerPoint.y() + length), QPointF(centerPoint.x() - length, centerPoint.y() - length)));
        m_mainRect->show();
        m_line->hide();

    }
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
        m_line->setPen(m_pen);
    }
    m_mainRect->setTransformOriginPoint(getCenterPoint());
    m_mainRect->setRotation(getAngle());
    m_line->setTransformOriginPoint(getCenterPoint());
    m_line->setRotation(getAngle());
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

bool Shape3PointArc::getTwoCircleMode() {
    return m_bTwoCircleMode;
}

void Shape3PointArc::setScaleFactor(float s) {
    m_scaleFactor = s;

    updateShapeRectangle();
}

void Shape3PointArc::setSecondPos(QPoint point) {
    m_secondPos = point;

    updateShapeRectangle();
}

void Shape3PointArc::setEndPos(QPoint point) {
    m_thirdPos = point;

    updateShapeRectangle();
    updateShapeRectangle(); // some bugging if only called once
}
