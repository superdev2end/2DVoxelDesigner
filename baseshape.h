#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include <QVector2D>
#include <QTextStream>
#include <QPainterPath>

#define MOVE_MODE_NONE 0
#define MOVE_MODE_Y 1
#define MOVE_MODE_X 2
#define MOVE_MODE_ROTATE 3

class BaseShape : public QObject
{
    Q_OBJECT
public:
    explicit BaseShape(QObject *parent = nullptr);

    virtual void save(QTextStream* stream);
    virtual void open(QTextStream* stream);

    virtual void setLineWidth(float);
    virtual void setLineColor(QColor);
    int getShapeLineWidth();
    QColor getShapeColor();
    void setCenterPoint(QPoint);
    void setAngle(float);
    QPoint getCenterPoint();
    float getAngle();
    void setBoudingRect(QRectF);
    int  getType();
    void setType(int);
    int getMoveType();
    void setSelectedArrow(int);
    int getSelectedArrow();
    void setIsDrawing(bool);
    bool getIsDrawing();
    void removeMoveType();
    void setnMoveType(int);
    bool isNearBy(float, float, QString);
    float distancePointLine( const QPoint &p, const QPoint &l1,
                                          const QPoint &l2 );
    QPointF closestPointOnPath(const QPointF &point, const QPainterPath &path);
    float magnitude( const QPoint &p1, const QPoint &p2 );
    float magnitude( const QPointF &p1, const QPointF &p2 );
    float magnitude( const QPoint &p1, const QPointF &p2 );
    float magnitude( const QPointF &p1, const QPoint &p2 );
    float rotateF(QPointF, QPointF, QPointF);
    float rotate(QPointF, QPointF, QPointF);
    QPoint rotateDotByAngle(QPoint, QPoint, float);
    QPoint moveX(QPointF, QPointF, QPointF, float);
    QPoint moveY(QPointF, QPointF, QPointF, float);
    QPoint calculate3PointCircle(QPoint, QPoint, QPoint);
    QRect calculateRect(QList<QPoint>);

    QPair<QPointF, QPointF> controlPoints(QPointF const& p0, QPointF const& p1, QPointF const& p2, qreal t);
    virtual void move(QPointF first, QPointF second) = 0;
    virtual void setSelectable(bool) = 0;
    virtual void addArrow(int) = 0;
    virtual void removeArrows() = 0;
    virtual void setScaleFactor(float) = 0;
    virtual void redraw() = 0;
    virtual bool setMoveType(QPointF) = 0;

private:
    int m_type;
    bool m_bDrawing;
    QPoint m_centerPoint;
    float m_angle;
public:
    QRectF m_boundingRectF;
    bool m_bSelectable;
    QPen m_pen;
    QColor m_shapeLineColor;
    float m_scaleFactor;
    int m_nMoveType;
    int m_nSelectedArrow;
    int m_shapeLineWidth;

signals:
    void selectedObject(BaseShape*, int);

};

#endif // SHAPE_H
