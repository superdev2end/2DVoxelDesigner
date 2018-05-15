#include "shapepolygon.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapePolygon::ShapePolygon(int type, int number, QColor color, int width) {
    setType(type);
    m_mainPolygon = new QGraphicsPolygonItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_number = number;
    m_shapeLineWidth = width;
}

ShapePolygon::~ShapePolygon() {
    if(m_mainPolygon != nullptr) {
        delete m_mainPolygon;
    }
    if(_moving_points[0] != nullptr) {
        delete _moving_points[0];
        _moving_points[0] = nullptr;
    }
    if(_moving_points[1] != nullptr) {
        delete _moving_points[1];
        _moving_points[1] = nullptr;
    }
}

QPoint ShapePolygon::getStartPos() {
    return m_startPos;
}

int ShapePolygon::getShapeNumber() {
    return m_number;
}

QPoint ShapePolygon::getEndPos() {
    return m_endPos;
}

void ShapePolygon::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapePolygon();
}

void ShapePolygon::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapePolygon();
}

void ShapePolygon::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_number << " "
            << m_startPos.x() << " "
            << m_startPos.y() << " "
            << m_endPos.x() << " "
            << m_endPos.y();
    *stream << endl;
}

void ShapePolygon::open(QTextStream* stream) {
    BaseShape::open(stream);
    int n, x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    *stream >> n >> x0 >> y0 >> x1 >> y1;

    m_number = n;
    m_startPos = QPoint(x0, y0);
    m_endPos = QPoint(x1, y1);
}

QRectF ShapePolygon::boundingRect() const {
    return m_boundingRectF;
}

void ShapePolygon::removeArrows() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->hide();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->hide();
    }
    setSelectionState(false);
}

void ShapePolygon::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 0) {
        if(m_nMoveType == MOVE_MODE_X) {
            m_startPos = moveX(m_startPos, first, second, getAngle());
            m_endPos = moveX(m_endPos, first, second, getAngle());
            setCenterPoint(m_startPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_startPos = moveY(m_startPos, first, second, getAngle());
            m_endPos = moveY(m_endPos, first, second, getAngle());
            setCenterPoint(m_startPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            setCenterPoint(m_startPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
            float angle = rotate(getCenterPoint(), first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
        }
        _moving_points[m_nSelectedArrow]->updateView();
    }
    updateShapePolygon();
}

void ShapePolygon::addArrow(int index) {
    if(index < 0 || index > 2) {
        return;
    }
    if(_moving_points[index] == nullptr) {
        _moving_points[index] = new MovingArrow(this);
    }
    _moving_points[index]->setScaleFactor(m_scaleFactor);
    _moving_points[index]->setIsDrawing(getIsDrawing());
    _moving_points[index]->show();
    setCenterPoint(m_startPos);
    setSelectionState(true);
    if(index == 0) {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setCenterPoint(getCenterPoint());
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_startPos.x() - 5 / m_scaleFactor, m_startPos.y() - 5 / m_scaleFactor), QPoint(m_startPos.x() + 5 / m_scaleFactor, m_startPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setCenterPoint(m_endPos);
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_endPos.x() - 5 / m_scaleFactor, m_endPos.y() - 5 / m_scaleFactor), QPoint(m_endPos.x() + 5 / m_scaleFactor, m_endPos.y() + 5 / m_scaleFactor)));
    }
}

void ShapePolygon::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapePolygon::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapePolygon::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapePolygon::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapePolygon::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint origin = rotateDotByAngle(QPoint(m_startPos.x(), m_startPos.y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        float initAngle = qAtan2(m_endPos.y() - m_startPos.y(), m_endPos.x() - m_startPos.x());
        float length = magnitude(m_startPos, m_endPos);
        QPointF tempPoint = QPointF(m_startPos.x() + length * qCos(initAngle), m_startPos.y() + length * qSin(initAngle));
        bool isIn = false;
        for (int i = 1; i < m_number; ++i) {
            float angle = (2.0 * M_PI / m_number * i) + initAngle;
            if(isIn == false) {
                isIn = isNearBy(distancePointLine(QPoint(origin.x(), origin.y()), QPoint(tempPoint.x(), tempPoint.y()), QPoint(QPointF(m_startPos.x() + length * qCos(angle), m_startPos.y() + length * qSin(angle)).x() ,QPointF(m_startPos.x() + length * qCos(angle), m_startPos.y() + length * qSin(angle)).y())), m_scaleFactor, "POLYGON");
                if(isIn) {
                    break;
                }
            }
            tempPoint = QPointF(m_startPos.x() + length * qCos(angle), m_startPos.y() + length * qSin(angle));
        }
        if(!isIn) {
            isIn = isNearBy(distancePointLine(QPoint(origin.x(), origin.y()), QPoint(tempPoint.x(), tempPoint.y()), QPoint(QPointF(m_startPos.x() + length * qCos(initAngle), m_startPos.y() + length * qSin(initAngle)).x(), QPointF(m_startPos.x() + length * qCos(initAngle), m_startPos.y() + length * qSin(initAngle)).y())), m_scaleFactor, "POLYGON");
        }
        if(isIn) {
            m_nSelectedArrow = 0;
            emit selectedObject(this, 0);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapePolygon::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainPolygon->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainPolygon->setPen(m_pen);
    }
}

void ShapePolygon::redraw() {
    if(_moving_points[0] != nullptr) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr) {
        _moving_points[1]->updateView();
    }

}

void ShapePolygon::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapePolygon::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_startPos = data.at(0);
    m_endPos = data.at(1);
    updateShapePolygon();
}

void ShapePolygon::updateShapePolygon() {
    if(m_number < 3) {
        return;
    }
    setBoudingRect(QRectF(m_startPos - QPointF(magnitude(m_startPos, m_endPos), magnitude(m_startPos, m_endPos)), m_startPos + QPointF(magnitude(m_startPos, m_endPos), magnitude(m_startPos, m_endPos))));
    QPolygonF polygon = QPolygonF();
    float initAngle = qAtan2(m_endPos.y() - m_startPos.y(), m_endPos.x() - m_startPos.x());
    float length = magnitude(m_startPos, m_endPos);
    polygon.push_back(QPointF(m_startPos.x() + length * qCos(initAngle), m_startPos.y() + length * qSin(initAngle)));
    for (int i = 1; i < m_number; ++i) {

        float angle = (2.0 * M_PI / m_number * i) + initAngle;
        polygon.push_back(QPointF(m_startPos.x() + length * qCos(angle), m_startPos.y() + length * qSin(angle)));
    }
    polygon.push_back(QPointF(m_startPos.x() + length * qCos(initAngle), m_startPos.y() + length * qSin(initAngle)));
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainPolygon->setPen(m_pen);
    }
    m_mainPolygon->setPolygon(polygon);
    m_mainPolygon->setTransformOriginPoint(getCenterPoint());
    m_mainPolygon->setRotation(getAngle());
    if(_moving_points[0] != nullptr && _moving_points[0]->isVisible()) {
        addArrow(0);
    }
    if(_moving_points[1] != nullptr && _moving_points[1]->isVisible()) {
        addArrow(1);
    }
}

void ShapePolygon::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapePolygon();
}

void ShapePolygon::setEndPos(QPoint point) {
    m_endPos = point;
    updateShapePolygon();
}
