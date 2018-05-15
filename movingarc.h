#ifndef MOVINGARC_H
#define MOVINGARC_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QDebug>

class MovingArc : public QGraphicsItem {
public:
    MovingArc(QGraphicsItem *parent);
    ~MovingArc() override;

    void setBoudingRect(QRectF);
    void setIsDrawing(bool);
    bool getIsDrawing();
    void setScaleFactor(float);
    bool isIn(QPointF);
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

public:
    void updateArc();

private:
    QRectF m_boundingRectF;
    bool m_bDrawing;
    float m_scaleFactor;
    QGraphicsPathItem* m_arc;
    QGraphicsEllipseItem* m_small;
    QGraphicsLineItem* m_line;
    float m_angle;
    QPoint m_centerPoint;
    bool m_bHovered;
};

#endif // MOVINGARC_H
