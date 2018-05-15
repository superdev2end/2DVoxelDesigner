#ifndef MOVINGARROW_H
#define MOVINGARROW_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QDebug>
#include "arrow.h"
#include "movingarc.h"

#define MOVE_MODE_NONE 0
#define MOVE_MODE_Y 1
#define MOVE_MODE_X 2
#define MOVE_MODE_ROTATE 3

class MovingArrow : public QGraphicsItem {
public:
    MovingArrow(QGraphicsItem *parent);
    ~MovingArrow() override;

    void setBoudingRect(QRectF);
    void setIsDrawing(bool);
    bool getIsDrawing();
    void setScaleFactor(float);
    void updateView();
    int setMoveType(QPointF);
    void setAngle(float);
    void setCenterPoint(QPoint);
    float getAngle();
    QPoint getCenterPoint();
    void setIsShowRotation(bool f);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateMovingArrow();

private:
    QRectF m_boundingRectF;
    QGraphicsRectItem* m_mainRect;
    bool m_bDrawing;
    Arrow*  _arrows[2];
    MovingArc* _movingARC;
    float m_scaleFactor;
    int m_nMoveType;
    float m_angle;
    QPoint m_centerPoint;
};

#endif // MOVINGARROW_H
