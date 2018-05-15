#include "arrow.h"

Arrow::Arrow(QGraphicsItem *parent) {
    setParentItem(parent);
    m_polygon = new QGraphicsPolygonItem(this);
    m_line = new QGraphicsLineItem(this);
    m_axiosIndex = -1;
    m_angle = 0;

    m_bHovered = false;

}

Arrow::~Arrow() {
    delete m_polygon;
}

QRectF Arrow::boundingRect() const {
    return m_boundingRectF;
}

void Arrow::setHovered(bool f) {
    m_bHovered = f;
    updateArrow();
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void Arrow::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Arrow::setAngle(float angle) {
    m_angle = angle;
}

void Arrow::setCenterPoint(QPoint point) {
    m_centerPoint = point;
}

float Arrow::getAngle() {
    return m_angle;
}

QPoint Arrow::getCenterPoint() {
    return m_centerPoint;
}

void Arrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void Arrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

void Arrow::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void Arrow::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateArrow();
}

void Arrow::updateArrow() {
    QPolygonF polygon = QPolygonF();
    if(m_axiosIndex == 0) {
        int arrowSize = m_boundingRectF.height() / 3;
        int arrowWidth = m_boundingRectF.width() / 1.3;
        polygon.push_back(QPoint(m_boundingRectF.center().x(), m_boundingRectF.top()));
        polygon.push_back(QPoint(m_boundingRectF.left() + arrowWidth, m_boundingRectF.top() + arrowSize));
        polygon.push_back(QPoint(m_boundingRectF.right() - arrowWidth, m_boundingRectF.top() + arrowSize));
        polygon.push_back(QPoint(m_boundingRectF.center().x(), m_boundingRectF.top()));
        m_polygon->setPen(Qt::NoPen);
        m_polygon->setBrush(QBrush(m_bHovered ? Qt::yellow : Qt::green));
        QPen pen = QPen();
        pen.setColor(m_bHovered ? Qt::yellow : Qt::green);
        pen.setWidth(4 / m_scaleFactor);
        m_line->setLine(QLineF(QPoint(m_boundingRectF.center().x(), m_boundingRectF.top() + arrowSize), QPoint(m_boundingRectF.center().x(), m_boundingRectF.bottom())));
        m_line->setPen(pen);
    } else {
        int arrowSize = m_boundingRectF.width() / 3;
        int arrowWidth = m_boundingRectF.height() / 1.3;
        polygon.push_back(QPoint(m_boundingRectF.right(), m_boundingRectF.center().y()));
        polygon.push_back(QPoint(m_boundingRectF.right() - arrowSize, m_boundingRectF.top() + arrowWidth));
        polygon.push_back(QPoint(m_boundingRectF.right() - arrowSize, m_boundingRectF.bottom() - arrowWidth));
        polygon.push_back(QPoint(m_boundingRectF.right(), m_boundingRectF.center().y()));
        m_polygon->setPen(Qt::NoPen);
        m_polygon->setBrush(QBrush(m_bHovered ? Qt::yellow : Qt::red));
        QPen pen = QPen();
        pen.setColor(m_bHovered ? Qt::yellow : Qt::red);
        pen.setWidth(4 / m_scaleFactor);
        m_line->setLine(QLineF(QPoint(m_boundingRectF.left(), m_boundingRectF.center().y()), QPoint(m_boundingRectF.right() - arrowWidth, m_boundingRectF.center().y())));
        m_line->setPen(pen);
    }
    m_polygon->setPolygon(polygon);
    m_polygon->setTransformOriginPoint(m_centerPoint);
    m_polygon->setRotation(m_angle);
    m_line->setTransformOriginPoint(m_centerPoint);
    m_line->setRotation(m_angle);
}

void Arrow::setIsDrawing(bool bDrawing) {
    m_bDrawing = bDrawing;

}

bool Arrow::isDrawing() {
    return m_bDrawing;
}

void Arrow::setBoudingRect(QRectF rect, int index) {
    m_boundingRectF = rect;
    m_axiosIndex = index;
    updateArrow();
}
