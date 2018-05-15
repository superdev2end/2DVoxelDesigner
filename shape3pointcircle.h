#ifndef SHAPE3POINTCIRCLE_H
#define SHAPE3POINTCIRCLE_H

#include "baseshape.h"
#include <QGraphicsItem>
#include <QPainter>
#include "movingarrow.h"
#include <QDebug>

class Shape3PointCircle : public BaseShape, public QGraphicsItem {
public:
    Shape3PointCircle(int, QColor, int);
    ~Shape3PointCircle() override;

    void setData(QList<QPoint>);
    void setEndPos(QPoint);
    void setSecondPos(QPoint);
    void setSelectable(bool) override;
    void addArrow(int) override;
    void removeArrows() override;
    void setScaleFactor(float) override;
    void updateShapeRectangle();
    void setTwoCircleMode(bool);
    bool getTwoCircleMode();
    bool setMoveType(QPointF) override;
    QPoint getFirstPoint();
    QPoint getSecondPoint();
    QPoint getThirdPoint();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void redraw() override;
    void move(QPointF first, QPointF second) override;
    void save(QTextStream* stream) override;
    void open(QTextStream* stream) override;
    void setLineWidth(float) override;
    void setLineColor(QColor) override;

private:
    void setSelectionState(bool);

private:
    QGraphicsEllipseItem* m_mainRect;
    MovingArrow*  _moving_points[4];
    QPoint  m_firstPos;
    QPoint  m_secondPos;
    QPoint  m_thirdPos;
    bool    m_bTwoCircleMode;
};

#endif // SHAPE3POINTCIRCLE_H
