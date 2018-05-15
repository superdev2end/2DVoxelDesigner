#ifndef SHAPEPOLYGON_H
#define SHAPEPOLYGON_H

#include "baseshape.h"
#include <QGraphicsItem>
#include <QPainter>
#include "movingarrow.h"
#include <QDebug>
#include <QTextStream>

class ShapePolygon : public BaseShape, public QGraphicsItem {
public:
    ShapePolygon(int, int, QColor, int);
    ~ShapePolygon() override;

    void setData(QList<QPoint>);
    void setEndPos(QPoint);
    void setSelectable(bool) override;
    void addArrow(int) override;
    void removeArrows() override;
    void setScaleFactor(float) override;
    void updateShapePolygon();
    bool setMoveType(QPointF) override;
    QPoint getStartPos();
    QPoint getEndPos();
    int getShapeNumber();

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
    QGraphicsPolygonItem*   m_mainPolygon;
    MovingArrow*            _moving_points[2];
    QPoint                  m_startPos;
    QPoint                  m_endPos;
    int                     m_number;
};

#endif // SHAPEPOLYGON_H
