#include "movingarc.h"
#include <QtMath>
#include <QGraphicsSceneHoverEvent>

MovingArc::MovingArc(QGraphicsItem *parent)  {
    setParentItem(parent);
    m_arc = new QGraphicsPathItem(this);
    m_small = new QGraphicsEllipseItem(this);
    m_line = new QGraphicsLineItem(this);
    this->setAcceptHoverEvents(false);
    m_angle = 0;
    m_bHovered = false;
}

MovingArc::~MovingArc() {
    if(m_arc != nullptr) {
        delete m_arc;
        m_arc = nullptr;
    }
    if(m_small == nullptr) {
        delete m_small;
        m_small = nullptr;
    }
    if(m_line == nullptr) {
        delete m_line;
        m_line = nullptr;
    }
}

void MovingArc::setHovered(bool f) {
    m_bHovered = f;
    updateArc();
}

QRectF MovingArc::boundingRect() const {
    return m_boundingRectF;
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void MovingArc::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void MovingArc::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void MovingArc::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

void MovingArc::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void MovingArc::setIsDrawing(bool f) {
    m_bDrawing = f;
}
bool MovingArc::getIsDrawing() {
    return m_bDrawing;
}

void MovingArc::setAngle(float angle) {
    m_angle = angle;
}

void MovingArc::setCenterPoint(QPoint point) {
    m_centerPoint = point;
}

float MovingArc::getAngle() {
    return m_angle;
}

QPoint MovingArc::getCenterPoint() {
    return m_centerPoint;
}

void MovingArc::setScaleFactor(float s){
    m_scaleFactor = s;
}

void MovingArc::setBoudingRect(QRectF rect) {
    m_boundingRectF = rect;
    updateArc();
}

bool MovingArc::isIn(QPointF point) {
    QRectF rect = QRectF(QPointF(m_boundingRectF.center().x() + m_boundingRectF.width() / 4 / 1.4 - 10 / m_scaleFactor, m_boundingRectF.center().y() + m_boundingRectF.height() / 4 / 1.4 - 10 / m_scaleFactor), QPointF(m_boundingRectF.center().x() + m_boundingRectF.width() / 4 / 1.4 + 10 / m_scaleFactor, m_boundingRectF.center().y() + m_boundingRectF.height() / 4 / 1.4 + 10 / m_scaleFactor));
    float length = qSqrt(qPow(rect.center().x() - m_centerPoint.x(), 2) + qPow(rect.center().y() - m_centerPoint.y(), 2));
    float angle = qAtan2(rect.center().y() - m_centerPoint.y(), rect.center().x() - m_centerPoint.x());
    QPointF temp = QPointF(m_centerPoint.x() + length * qCos(angle + qDegreesToRadians(getAngle())), m_centerPoint.y() + length * qSin(angle + qDegreesToRadians(getAngle())));
    return ((point.x() - temp.x()) * (point.x() - temp.x()) + (point.y() - temp.y()) * (point.y() - temp.y())) < 2 * (qPow(10 / m_scaleFactor, 2));
}

void MovingArc::updateArc() {
    if(m_bDrawing) {
        m_arc->setPath(QPainterPath());
        m_small->hide();
        m_arc->hide();
        m_line->hide();
    } else {
        if(isVisible()) {
            QPen pen;
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(m_bHovered ? Qt::yellow : Qt::white);
            m_small->setPen(pen);
            m_small->setBrush(brush);
            m_small->setRect(QRectF(QPointF(m_boundingRectF.center().x() + m_boundingRectF.width() / 4 / 1.4 - 5 / m_scaleFactor, m_boundingRectF.center().y() + m_boundingRectF.height() / 4 / 1.4 - 5 / m_scaleFactor), QPointF(m_boundingRectF.center().x() + m_boundingRectF.width() / 4 / 1.4 + 5 / m_scaleFactor, m_boundingRectF.center().y() + m_boundingRectF.height() / 4 / 1.4 + 5 / m_scaleFactor)));

            pen.setCapStyle(Qt::SquareCap);
            pen.setStyle(Qt::SolidLine);
            pen.setWidth(4 / m_scaleFactor);
            pen.setColor(Qt::blue);
            m_arc->setPen(pen);
            QPainterPath roundRectPath;
            roundRectPath.arcMoveTo(m_boundingRectF, 0);
            roundRectPath.arcTo(m_boundingRectF, 0, -90);
            m_arc->setPath(roundRectPath);
            pen.setStyle(Qt::DashDotLine);
            pen.setColor(Qt::white);
            pen.setWidth(2 / m_scaleFactor);
            m_line->setPen(pen);
            m_line->setLine(QLineF(m_boundingRectF.center(), QPointF(m_boundingRectF.center().x() + m_boundingRectF.width() / 4 / 1.4, m_boundingRectF.center().y() + m_boundingRectF.height() / 4 / 1.4)));
            m_small->show();
            m_arc->show();
            m_line->show();
            m_small->setTransformOriginPoint(m_centerPoint);
            m_small->setRotation(m_angle);
            m_arc->setTransformOriginPoint(m_centerPoint);
            m_arc->setRotation(m_angle);
            m_line->setTransformOriginPoint(m_centerPoint);
            m_line->setRotation(m_angle);
        } else {
            m_small->hide();
            m_arc->hide();
            m_line->hide();
        }
    }
}
