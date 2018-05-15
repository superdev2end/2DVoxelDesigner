#include "markeritem.h"
#include <QGraphicsSceneMouseEvent>

MarkerItem::MarkerItem() {
    setFlag(ItemSendsGeometryChanges);    

    m_mainLine = new QGraphicsLineItem(this);
    m_rect = new QGraphicsRectItem(this);
    m_TextItem = new QGraphicsTextItem(this);

    m_node = nullptr;
    m_label = "";
    m_nodeIndex = -1;
    m_layerIndex = -1;
}

MarkerItem::~MarkerItem() {
    if(m_TextItem != nullptr) {
        delete m_TextItem;
    }
    if(m_mainLine != nullptr) {
        delete m_mainLine;
    }
    if(m_rect != nullptr) {
        delete m_rect;
    }
}

int MarkerItem::getNodeIndex() {
    return m_nodeIndex;
}

void MarkerItem::save(QTextStream* stream) {
    *stream << getLayerIndex() << " "
            << m_position.x() << " "
            << m_position.y() << " "
            << m_nodeIndex << " "
            << m_label << " "
            << m_number;
    *stream << endl;
}

void MarkerItem::open(QTextStream* stream) {
    int x = 0, y = 0;
    *stream >> m_layerIndex >> x >> y >> m_nodeIndex >> m_label >> m_number;
    setLabel(m_label);
    m_position = QPoint(x, y);
    updateMarker();
}

void MarkerItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void MarkerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
}

void MarkerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

void MarkerItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit selectedMarker(this);
}

void MarkerItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    emit selectedNodeFromMarker(this);
}

QRectF MarkerItem::boundingRect() const {
    return m_boundingRectF;
}

void MarkerItem::setScaleFactor(float scale) {
    m_scaleFactor = scale;
    m_TextItem->setScale(1.5/ scale);
    updateMarker();
}

void MarkerItem::setBoundingRect(QRectF rect) {
    updateMarker();
}

void MarkerItem::updateMarker() {
    if(m_node == nullptr) {
        hide();
        return;
    }

    setVisible(m_node->isVisible());

    m_TextItem->setPos(QPointF(m_position.x(), m_position.y()));
    m_boundingRectF = QRectF(QPointF(m_position.x(), m_position.y()), QPointF(m_position.x() + 60 / m_scaleFactor, m_position.y() + 15 * 2/ m_scaleFactor));

    QLineF line(QPointF(0, 0), m_boundingRectF.center());
    float line_len = line.length();
    float node_size = 1.5 * m_node->scale() * m_node->rect().width() / 2;// * 100;

    if(line_len <= node_size)
        m_mainLine->setLine(QLineF(m_boundingRectF.center(), m_boundingRectF.center()));
    else{
        float line_x1 = line.x2() * (node_size / line_len);
        float line_y1 = line.y2() * (node_size / line_len);
        m_mainLine->setLine(QLineF(QPointF(line_x1, line_y1), m_boundingRectF.center()));
    }

    QPen pen = QPen(Qt::red);
    pen.setWidth(2 / m_scaleFactor);
    m_mainLine->setPen(pen);
    pen.setColor(Qt::yellow);
    m_rect->setPen(pen);
    m_rect->setBrush(QBrush(Qt::yellow));
    m_rect->setRect(m_boundingRectF);
    setZValue(999);
}

void MarkerItem::setPosition(QPoint point) {
    m_position = point;
    updateMarker();
}

QPoint MarkerItem::getPosition() {
    return m_position;
}

int MarkerItem::getNumber() {
    return m_number;
}

int MarkerItem::getLayerIndex()
{
    if(m_node)
        m_layerIndex = m_node->getLayerIndex();

    return m_layerIndex;
}
void MarkerItem::setLabel(QString str) {
    m_number = str.toInt();
    m_label = str.contains("M") ? str : "M" + str;
    m_TextItem->setHtml("<div style='z-index: 999;background-color:yellow;border-color:black;text-align:center;'>" + m_label + "</div>");
    m_TextItem->setTextWidth(40);
}

QString MarkerItem::getLabel() {
    return m_label;
}

void MarkerItem::setNode(Node * node) {
    m_node = node;
    if(m_node == nullptr) {
        hide();
        m_nodeIndex = -1;
    } else {
        show();
        m_nodeIndex = node->m_nodeIndex;
    }
}

Node* MarkerItem::getNode() {
    return m_node;
}




