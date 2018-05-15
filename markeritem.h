#ifndef MAKERITEM_H
#define MAKERITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <node.h>
#include <QTextStream>

class MarkerItem :  public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    MarkerItem();
    ~MarkerItem() override;

    void setPosition(QPoint);
    QPoint getPosition();
    void setLabel(QString);
    QString getLabel();
    void setNode(Node*);
    Node* getNode();
    void setBoundingRect(QRectF);
    void setScaleFactor(float);
    void updateMarker();
    void save(QTextStream* stream);
    void open(QTextStream* stream);
    int getNodeIndex();
    int getNumber();
    int  getLayerIndex();
public:
signals:
    void selectedMarker(MarkerItem*);
    void selectedNodeFromMarker(MarkerItem*);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;    

private:
    QPoint m_position;
    QString m_label;
    Node* m_node;
    QRectF m_boundingRectF;
    QGraphicsLineItem* m_mainLine;
    QGraphicsTextItem* m_TextItem;
    QGraphicsRectItem* m_rect;
    float m_scaleFactor;
    int m_nodeIndex;
    int m_number;
    int m_layerIndex;
};

#endif // MAKERITEM_H
