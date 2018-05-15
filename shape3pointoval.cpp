#include "shape3pointoval.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

Shape3PointOval::Shape3PointOval(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsEllipseItem(this);
    m_mainLine = new QGraphicsLineItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    _moving_points[3] = nullptr;
    m_bTwoOvalMode = true;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

Shape3PointOval::~Shape3PointOval() {
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
    if(m_mainLine != nullptr) {
        delete m_mainLine;
       m_mainLine = nullptr;
    }
}

QPoint Shape3PointOval::getFirstPoint() {
    return m_firstPos;
}

QPoint Shape3PointOval::getSecondPoint() {
    return m_secondPos;
}

QPoint Shape3PointOval::getThirdPoint() {
    return m_thirdPos;
}

void Shape3PointOval::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void Shape3PointOval::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void Shape3PointOval::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_firstPos.x() << " " << m_firstPos.y() << " " << m_secondPos.x() << " " << m_secondPos.y() << " " << m_thirdPos.x() << " " << m_thirdPos.y();
    *stream << endl;
}

void Shape3PointOval::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    *stream >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    m_firstPos = QPoint(x0, y0);
    m_secondPos = QPoint(x1, y1);
    m_thirdPos = QPoint(x2, y2);
}

QRectF Shape3PointOval::boundingRect() const {
    return m_boundingRectF;
}

void Shape3PointOval::removeArrows() {
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

void Shape3PointOval::setTwoOvalMode(bool f) {
    m_bTwoOvalMode = f;
}

void Shape3PointOval::addArrow(int index) {
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
        _moving_points[index]->setCenterPoint(m_secondPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_secondPos.x() - 5 / m_scaleFactor, m_secondPos.y() - 5 / m_scaleFactor), QPoint(m_secondPos.x() + 5 / m_scaleFactor, m_secondPos.y() + 5 / m_scaleFactor)));
    } else if(index == 2){
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_thirdPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_thirdPos.x() - 5 / m_scaleFactor, m_thirdPos.y() - 5 / m_scaleFactor), QPoint(m_thirdPos.x() + 5 / m_scaleFactor, m_thirdPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setAngle(getAngle());
        QPoint centerPos = (m_firstPos + m_secondPos) / 2;
        _moving_points[index]->setCenterPoint(centerPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(centerPos.x() - 5 / m_scaleFactor, centerPos.y() - 5 / m_scaleFactor), QPoint(centerPos.x() + 5 / m_scaleFactor, centerPos.y() + 5 / m_scaleFactor)));
    }
    _moving_points[index]->update();
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void Shape3PointOval::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Shape3PointOval::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void Shape3PointOval::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool Shape3PointOval::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

bool Shape3PointOval::isOvalSelectable(QPoint point) {
    QPoint centerPos = (m_firstPos + m_secondPos) / 2;
    float angle = qAtan2((m_secondPos.y() - centerPos.y()), (m_secondPos.x() - centerPos.x()));
    float angle1 = qAtan2((m_thirdPos.y() - centerPos.y()), (m_thirdPos.x() - centerPos.x()));
    float length = magnitude(centerPos, m_thirdPos);
    QPointF temp = QPointF(centerPos.x() + length * qCos(angle - angle1), centerPos.y() + length * qSin(angle - angle1));
    float a = magnitude(centerPos, m_secondPos);
    float b = qSqrt((a * a * (temp.y() - centerPos.y()) * (temp.y() - centerPos.y())) / ((a * a - (temp.x() - centerPos.x()) * (temp.x() - centerPos.x()))));
    float c = (centerPos.x() - point.x()) * (centerPos.x() - point.x()) / (a * a) + (centerPos.y() - point.y()) * (centerPos.y() - point.y()) / (b * b);
    return qAbs(1 - c) < 0.1;
}

void Shape3PointOval::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint centerPos = (m_firstPos + m_secondPos) / 2;
        float angle = qAtan2((m_secondPos.y() - centerPos.y()), (m_secondPos.x() - centerPos.x()));
        QPoint origin = rotateDotByAngle(QPoint(centerPos.x(), centerPos.y()), QPoint(event->pos().x(), event->pos().y()), - qRadiansToDegrees(angle));

        if(isOvalSelectable(origin)) {
            setSelectionState(true);
            m_nSelectedArrow = 3;
            emit selectedObject(this, 3);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void Shape3PointOval::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_mainLine->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainRect->setPen(m_pen);
        m_mainLine->setPen(m_pen);
    }
}

void Shape3PointOval::redraw() {
    if(_moving_points[0] != nullptr && _moving_points[0]->isVisible()) {
        _moving_points[0]->updateView();
    }
    if(_moving_points[1] != nullptr && _moving_points[1]->isVisible()) {
        _moving_points[1]->updateView();
    }
    if(_moving_points[2] != nullptr && _moving_points[2]->isVisible()) {
        _moving_points[2]->updateView();
    }
    if(_moving_points[3] != nullptr && _moving_points[3]->isVisible()) {
        _moving_points[3]->updateView();
    }
}

void Shape3PointOval::setSelectable(bool f){
    m_bSelectable = f;
}

void Shape3PointOval::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_firstPos = data.at(0);
    m_secondPos = data.at(1);
    updateShapeRectangle();
}

void Shape3PointOval::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 3) {
        setCenterPoint((m_firstPos + m_secondPos) / 2);
        _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        if(m_nMoveType == MOVE_MODE_X) {
            m_firstPos = moveX(m_firstPos, first, second, getAngle());
            m_secondPos = moveX(m_secondPos, first, second, getAngle());
            m_thirdPos = moveX(m_thirdPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_firstPos = moveY(m_firstPos, first, second, getAngle());
            m_secondPos = moveY(m_secondPos, first, second, getAngle());
            m_thirdPos = moveY(m_thirdPos, first, second, getAngle());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            float angle = rotate(getCenterPoint(), first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
            m_firstPos = rotateDotByAngle(getCenterPoint(), m_firstPos, qRadiansToDegrees(angle));
            m_secondPos = rotateDotByAngle(getCenterPoint(), m_secondPos, qRadiansToDegrees(angle));
            m_thirdPos = rotateDotByAngle(getCenterPoint(), m_thirdPos, qRadiansToDegrees(angle));
        }
        _moving_points[m_nSelectedArrow]->updateView();
    }
    updateShapeRectangle();
}

void Shape3PointOval::updateShapeRectangle() {
    if(m_bTwoOvalMode) {
        m_mainRect->setRect(QRectF());
        m_mainLine->setLine(QLineF(m_firstPos, m_secondPos));
    } else {
        m_mainLine->setLine(QLineF());
        QPoint centerPos = (m_firstPos + m_secondPos) / 2;
        float angle = qAtan2((m_secondPos.y() - centerPos.y()), (m_secondPos.x() - centerPos.x()));
        float angle1 = qAtan2((m_thirdPos.y() - centerPos.y()), (m_thirdPos.x() - centerPos.x()));
        float length = magnitude(centerPos, m_thirdPos);
        QPointF temp = QPointF(centerPos.x() + length * qCos(angle - angle1), centerPos.y() + length * qSin(angle - angle1));
        float a = magnitude(centerPos, m_secondPos);
        float b = qSqrt((a * a * (temp.y() - centerPos.y()) * (temp.y() - centerPos.y())) / ((a * a - (temp.x() - centerPos.x()) * (temp.x() - centerPos.x()))));
        m_mainRect->setRect(QRectF(QPointF(centerPos.x() - a, centerPos.y() - b), QPointF(centerPos.x() + a, centerPos.y() + b)));
        m_mainRect->setTransformOriginPoint(centerPos);
        m_mainRect->setRotation(angle * 180 / M_PI);
        float distance = a > b ? a : b;
        setBoudingRect(QRectF(centerPos - QPointF(distance, distance), centerPos + QPoint(distance, distance)));

    }
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()) {
        m_pen.setWidth(pen_width);
        m_mainRect->setPen(m_pen);
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
    if(_moving_points[3] != nullptr && _moving_points[3]->isVisible()) {
        addArrow(3);
    }
}

bool Shape3PointOval::getTwoOvalMode() {
    return m_bTwoOvalMode;
}

void Shape3PointOval::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void Shape3PointOval::setSecondPos(QPoint point) {
    m_secondPos = point;
    updateShapeRectangle();
}

void Shape3PointOval::setEndPos(QPoint point) {
    m_thirdPos = point;
    updateShapeRectangle();
}
