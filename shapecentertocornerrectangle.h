#ifndef SHAPECENTERTOCORNERRECTANGLE_H
#define SHAPECENTERTOCORNERRECTANGLE_H

#include "baseshape.h"
#include <QGraphicsItem>
#include <QPainter>
#include "movingarrow.h"
#include <QDebug>
#include <QTextStream>

class ShapeCenterToCornerRectangle : public BaseShape, public QGraphicsItem {
public:
    ShapeCenterToCornerRectangle(int, QColor, int);
    ~ShapeCenterToCornerRectangle() override;

    void setData(QList<QPoint>);
    void setEndPos(QPoint);
    void setSelectable(bool) override;
    void addArrow(int) override;
    void removeArrows() override;
    void setScaleFactor(float) override;
    void updateShapeRectangle();
    bool setMoveType(QPointF) override;
    QPoint getStartPos();
    QPoint getEndPos();

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
    QGraphicsRectItem* m_mainRect;
    MovingArrow*  _moving_point;
    QPoint  m_centerPos;
    QPoint  m_endPos;
};

#endif // SHAPECENTERTOCORNERRECTANGLE_H
