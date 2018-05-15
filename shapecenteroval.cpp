#include "shapecenteroval.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeCenterOval::ShapeCenterOval(int type, QColor color, int width) {
    setType(type);
    m_mainRect = new QGraphicsEllipseItem(this);
    m_mainLine = new QGraphicsLineItem(this);
    _moving_points[0] = nullptr;
    _moving_points[1] = nullptr;
    _moving_points[2] = nullptr;
    m_bTwoOvalMode = true;
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_shapeLineWidth = width;
}

ShapeCenterOval::~ShapeCenterOval() {
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
    if(m_mainLine != nullptr) {
        delete m_mainLine;
       m_mainLine = nullptr;
    }
}

QPoint ShapeCenterOval::getFirstPoint() {
    return m_firstPos;
}

QPoint ShapeCenterOval::getSecondPoint() {
    return m_secondPos;
}

QPoint ShapeCenterOval::getThirdPoint() {
    return m_thirdPos;
}

void ShapeCenterOval::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeRectangle();
}

void ShapeCenterOval::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeRectangle();
}

void ShapeCenterOval::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_firstPos.x() << " " << m_firstPos.y() << " " << m_secondPos.x() << " " << m_secondPos.y() << " " << m_thirdPos.x() << " " << m_thirdPos.y();
    *stream << endl;
}

void ShapeCenterOval::open(QTextStream* stream) {
    BaseShape::open(stream);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    *stream >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    m_firstPos = QPoint(x0, y0);
    m_secondPos = QPoint(x1, y1);
    m_thirdPos = QPoint(x2, y2);
}

QRectF ShapeCenterOval::boundingRect() const {
    return m_boundingRectF;
}

void ShapeCenterOval::removeArrows() {
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

void ShapeCenterOval::setTwoOvalMode(bool f) {
    m_bTwoOvalMode = f;
}

void ShapeCenterOval::addArrow(int index) {
    if(index < 0 || index > 2) {
        return;
    }
    if(_moving_points[index] == nullptr) {
        _moving_points[index] = new MovingArrow(this);
    }
    setSelectionState(true);
    _moving_points[index]->setScaleFactor(m_scaleFactor);
    _moving_points[index]->setIsDrawing(getIsDrawing());
    _moving_points[index]->show();
    setCenterPoint(m_firstPos);
    if(index == 0) {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setCenterPoint(m_firstPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_firstPos.x() - 5 / m_scaleFactor, m_firstPos.y() - 5 / m_scaleFactor), QPoint(m_firstPos.x() + 5 / m_scaleFactor, m_firstPos.y() + 5 / m_scaleFactor)));
    } else if(index == 1) {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_secondPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_secondPos.x() - 5 / m_scaleFactor, m_secondPos.y() - 5 / m_scaleFactor), QPoint(m_secondPos.x() + 5 / m_scaleFactor, m_secondPos.y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_thirdPos);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(m_thirdPos.x() - 5 / m_scaleFactor, m_thirdPos.y() - 5 / m_scaleFactor), QPoint(m_thirdPos.x() + 5 / m_scaleFactor, m_thirdPos.y() + 5 / m_scaleFactor)));
    }
}

void ShapeCenterOval::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeCenterOval::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeCenterOval::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeCenterOval::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

bool ShapeCenterOval::isOvalSelectable(QPoint point) {
    float angle = qAtan2((m_secondPos.y() - m_firstPos.y()), (m_secondPos.x() - m_firstPos.x()));
    float angle1 = qAtan2((m_thirdPos.y() - m_firstPos.y()), (m_thirdPos.x() - m_firstPos.x()));
    float length = magnitude(m_firstPos, m_thirdPos);
    QPointF temp = QPointF(m_firstPos.x() + length * qCos(angle - angle1), m_firstPos.y() + length * qSin(angle - angle1));
    float a = magnitude(m_firstPos, m_secondPos);
    float b = qSqrt((a * a * (temp.y() - m_firstPos.y()) * (temp.y() - m_firstPos.y())) / ((a * a - (temp.x() - m_firstPos.x()) * (temp.x() - m_firstPos.x()))));
    float c = (m_firstPos.x() - point.x()) * (m_firstPos.x() - point.x()) / (a * a) + (m_firstPos.y() - point.y()) * (m_firstPos.y() - point.y()) / (b * b);
    return qAbs(1 - c) < 0.1;
}

void ShapeCenterOval::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        float angle = qAtan2((m_secondPos.y() - m_firstPos.y()), (m_secondPos.x() - m_firstPos.x()));
        QPoint origin = rotateDotByAngle(QPoint(m_firstPos.x(), m_firstPos.y()), QPoint(event->pos().x(), event->pos().y()), - qRadiansToDegrees(angle));
        if(isOvalSelectable(origin)) {
            setSelectionState(true);
            m_nSelectedArrow = 0;
            emit selectedObject(this, 0);
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeCenterOval::setSelectionState(bool f) {
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

void ShapeCenterOval::redraw() {
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

void ShapeCenterOval::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeCenterOval::setData(QList<QPoint> data){
    if(data.length() != 2)
        return;
    m_firstPos = data.at(0);
    m_secondPos = data.at(1);
    updateShapeRectangle();
}

void ShapeCenterOval::move(QPointF first, QPointF second) {
    if(m_nSelectedArrow == 0) {

        if(m_nMoveType == MOVE_MODE_X) {
            m_firstPos = moveX(m_firstPos, first, second, getAngle());
            m_secondPos = moveX(m_secondPos, first, second, getAngle());
            m_thirdPos = moveX(m_thirdPos, first, second, getAngle());
            setCenterPoint(m_firstPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_Y) {
            m_firstPos = moveY(m_firstPos, first, second, getAngle());
            m_secondPos = moveY(m_secondPos, first, second, getAngle());
            m_thirdPos = moveY(m_thirdPos, first, second, getAngle());
            setCenterPoint(m_firstPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        } else if(m_nMoveType == MOVE_MODE_ROTATE) {
            setCenterPoint(m_firstPos);
            _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
            float angle = rotate(getCenterPoint(), first, second);
            setAngle(qRadiansToDegrees(angle) + getAngle());
            _moving_points[m_nSelectedArrow]->setAngle(getAngle());
            m_secondPos = rotateDotByAngle(getCenterPoint(), m_secondPos, qRadiansToDegrees(angle));
            m_thirdPos = rotateDotByAngle(getCenterPoint(), m_thirdPos, qRadiansToDegrees(angle));
        }
        _moving_points[m_nSelectedArrow]->updateView();
    }
    updateShapeRectangle();
}

void ShapeCenterOval::updateShapeRectangle() {
    if(m_bTwoOvalMode) {
        setBoudingRect(QRectF(m_firstPos, m_secondPos));
        m_mainRect->setRect(QRectF());
        m_mainLine->setLine(QLineF(m_firstPos, m_secondPos));
    } else {
        m_mainLine->setLine(QLineF());
        float angle = qAtan2((m_secondPos.y() - m_firstPos.y()), (m_secondPos.x() - m_firstPos.x()));
        float angle1 = qAtan2((m_thirdPos.y() - m_firstPos.y()), (m_thirdPos.x() - m_firstPos.x()));
        float length = magnitude(m_firstPos, m_thirdPos);
        QPointF temp = QPointF(m_firstPos.x() + length * qCos(angle - angle1), m_firstPos.y() + length * qSin(angle - angle1));
        float a = magnitude(m_firstPos, m_secondPos);
        float b = qSqrt((a * a * (temp.y() - m_firstPos.y()) * (temp.y() - m_firstPos.y())) / ((a * a - (temp.x() - m_firstPos.x()) * (temp.x() - m_firstPos.x()))));
        m_mainRect->setRect(QRectF(QPointF(m_firstPos.x() - a, m_firstPos.y() - b), QPointF(m_firstPos.x() + a, m_firstPos.y() + b)));
        m_mainRect->setTransformOriginPoint(m_firstPos);
        m_mainRect->setRotation(angle * 180 / M_PI);
        float distance = a > b ? a : b;
        setBoudingRect(QRectF(m_firstPos - QPointF(distance, distance), m_firstPos + QPoint(distance, distance)));
    }
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
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
}

bool ShapeCenterOval::getTwoOvalMode() {
    return m_bTwoOvalMode;
}

void ShapeCenterOval::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeRectangle();
}

void ShapeCenterOval::setSecondPos(QPoint point) {
    m_secondPos = point;
    updateShapeRectangle();
}

void ShapeCenterOval::setEndPos(QPoint point) {
    m_thirdPos = point;
    updateShapeRectangle();
}
