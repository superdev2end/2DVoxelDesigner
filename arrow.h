#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QDebug>

class Arrow : public QGraphicsItem {
public:
    Arrow(QGraphicsItem *parent);
    ~Arrow() override;

    void setBoudingRect(QRectF, int);
    void setScaleFactor(float);
    void setIsDrawing(bool);
    bool isDrawing();
    void setAngle(float);
    void setCenterPoint(QPoint);
    float getAngle();
    QPoint getCenterPoint();
    void setHovered(bool);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateArrow();

private:
    QRectF m_boundingRectF;
    int m_axiosIndex;
    QGraphicsPolygonItem* m_polygon;
    QGraphicsLineItem* m_line;
    float m_scaleFactor;
    bool m_bDrawing;
    float m_angle;
    QPoint m_centerPoint;
    bool m_bHovered;
};

#endif // ARROW_H
