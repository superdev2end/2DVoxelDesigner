#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QBrush>
#include <QLinearGradient>
#include <QtMath>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <math.h>
#include "cornergrabber.h"

class ImageItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    ImageItem(QPixmap pix);
    void    setScaleFactor(float f);
    void    setImageSize(int w, int h);
    void    setImageAngle(int alpha);
    int     getImageWith();
    int     getImageHeight();
    int     getImageX();
    int     getImageY();
    int     getImageAngle();
    void    lockImage(bool f);
    void    maintainAspectRatio(bool);
    bool    isHovered();
    void    setMoveable(bool);
    bool    isLocked();
    QPointF getCenterPos();

protected:
signals:
    void imageStartRotate();
    void imageStopRotate();
    void imageStartMove();
    void imageStopMove();
private:
    virtual QRectF boundingRect() const override;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)  override;
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )  override;
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )  override;

    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event )  override;

    void setCornerPositions();

    qreal   _width;
    qreal   _height;

    CornerGrabber*  _corners[5];// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box, 4- center->rotate

    QPixmap _pixmap;
    float   _cornerRad;
    float   _scaleFactor;

    bool    _locked;
    bool    _maintainAspectRatio;
    qreal   _aspectRatio;
    bool    m_hovered;
    QPointF m_moveStartPoint;
};

#endif // IMAGEITEM_H
