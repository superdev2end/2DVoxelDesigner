#include "shapeline.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeLine::ShapeLine(int type, QColor color, int width) {
    setType(type);
    m_mainLine = new QGraphicsLineItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    m_shapeLineColor = color;
    m_shapeLineWidth = width;
    m_pen.setColor(m_shapeLineColor);
}

ShapeLine::~ShapeLine() {
    if(m_mainLine != nullptr) {
        delete m_mainLine;
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

void ShapeLine::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeLine();
}

QPoint ShapeLine::getStartPos() {
    return m_startPos;
}

QPoint ShapeLine::getEndPos() {
    return m_endPos;
}

void ShapeLine::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeLine();
}



void ShapeLine::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_startPos.x() << " " << m_startPos.y() << " " << m_endPos.x() << " " << m_endPos.y();
    *stream << endl;
}

void ShapeLine::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    *stream >> x0 >> y0 >> x1 >> y1;
    m_startPos = QPoint(x0, y0);
    m_endPos = QPoint(x1, y1);
}

QRectF ShapeLine::boundingRect() const {
    return m_boundingRectF;
}

void ShapeLine::removeArrows() {
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

void ShapeLine::move(QPointF first, QPointF second) {
    switch (m_nSelectedArrow) {
        case 1:
            if(m_nMoveType == MOVE_MODE_X) {
                m_endPos = moveX(m_endPos, first, second, 0);
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_Y) {
                m_endPos = moveY(m_endPos, first, second, 0);
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_ROTATE) {

            }
            break;
        case 0:
            if(m_nMoveType == MOVE_MODE_X) {
                m_startPos = moveX(m_startPos, first, second, 0);
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_Y) {
                m_startPos = moveY(m_startPos, first, second, 0);
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            }
            break;
        case 2:
            if(m_nMoveType == MOVE_MODE_X) {
                m_startPos = moveX(m_startPos, first, second, getAngle());
                m_endPos = moveX(m_endPos, first, second, getAngle());
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_Y) {
                m_startPos = moveY(m_startPos, first, second, getAngle());
                m_endPos = moveY(m_endPos, first, second, getAngle());
                setCenterPoint((m_startPos + m_endPos) / 2);
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
            } else if(m_nMoveType == MOVE_MODE_ROTATE) {
                setCenterPoint((m_startPos + m_endPos) / 2);
                float angle = rotate(getCenterPoint(), first, second);
                setAngle(qRadiansToDegrees(angle) + getAngle());
                _moving_points[m_nSelectedArrow]->setAngle(getAngle());
                _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
                _moving_points[m_nSelectedArrow]->updateView();
                m_startPos = rotateDotByAngle(getCenterPoint(), m_startPos, qRadiansToDegrees(angle));
                m_endPos = rotateDotByAngle(getCenterPoint(), m_endPos, qRadiansToDegrees(angle));
            }
            break;
        default:
            break;
    }
    updateShapeLine();
}

void ShapeLine::addArrow(int index) {
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
        _moving_points[index]->setAngle(0);
        setCenterPoint(m_startPos);
        _moving_points[index]->setCenterPoint(m_startPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), m_startPos, getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_startPos, getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), m_startPos, getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_startPos, getAngle()).y() + 5 / m_scaleFactor)));
        _moving_points[index]->setIsShowRotation(false);
    } else if(index == 1) {
        _moving_points[index]->setAngle(0);
        setCenterPoint(m_endPos);
        _moving_points[index]->setCenterPoint(m_endPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), m_endPos, getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_endPos, getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), m_endPos, getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_endPos, getAngle()).y() + 5 / m_scaleFactor)));
        _moving_points[index]->setIsShowRotation(false);
    } else if(index == 2) {
        _moving_points[index]->setIsShowRotation(true);
        setCenterPoint(((m_startPos + m_endPos) / 2));
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setCenterPoint(((m_startPos + m_endPos) / 2));
        _moving_points[index]->setBoudingRect(QRectF(QPoint(getCenterPoint().x() - 5 / m_scaleFactor, getCenterPoint().y() - 5 / m_scaleFactor), QPoint(getCenterPoint().x() + 5 / m_scaleFactor, getCenterPoint().y() + 5 / m_scaleFactor)));
    }
}

void ShapeLine::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeLine::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapeLine::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        setCenterPoint((m_startPos + m_endPos) / 2);
        if(isNearBy(magnitude(m_startPos, m_endPos), m_scaleFactor, "LINE")) {
            m_nSelectedArrow = 1;
            emit selectedObject(this, 1);
        } else {
            if(isNearBy(distancePointLine(QPoint(event->pos().x(), event->pos().y()), m_startPos, m_endPos), m_scaleFactor, "LINE")) {
                setSelectionState(true);
                if(magnitude(m_startPos, QPoint(event->pos().x(), event->pos().y())) < (magnitude(m_startPos, m_endPos) / 4.0)) {
                    m_nSelectedArrow = 0;
                    emit selectedObject(this, 0);
                } else if(magnitude(m_endPos, QPoint(event->pos().x(), event->pos().y())) < (magnitude(m_startPos, m_endPos) / 4.0)) {
                    m_nSelectedArrow = 1;
                    emit selectedObject(this, 1);
                } else {
                    m_nSelectedArrow = 2;
                    emit selectedObject(this, 2);
                }
            }
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeLine::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainLine->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainLine->setPen(m_pen);
    }
}

void ShapeLine::redraw() {
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

void ShapeLine::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeLine::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_startPos = data.at(0);
    m_endPos = data.at(1);
    updateShapeLine();
}

void ShapeLine::updateShapeLine() {
    setCenterPoint((m_startPos + m_endPos) / 2);
    float length = magnitude(getCenterPoint(), m_startPos) * 1.2;
    setBoudingRect(QRectF(getCenterPoint() - QPointF(length, length), getCenterPoint() + QPointF(length, length)));
    m_mainLine->setLine(m_startPos.x(), m_startPos.y(), m_endPos.x(), m_endPos.y());
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainLine->setPen(m_pen);
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
}

void ShapeLine::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeLine();
}

void ShapeLine::setEndPos(QPoint point) {
    m_endPos = point;
    updateShapeLine();
}
