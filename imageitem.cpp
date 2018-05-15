#include "imageitem.h"

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  */

ImageItem::ImageItem(QPixmap pix):
        _pixmap(pix),
        _cornerRad(3)
{
    m_hovered = false;
    setFlags(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);

    _width = _pixmap.width();
    _height = _pixmap.height();

    _scaleFactor = 1;
    _locked = false;
    _maintainAspectRatio = true;

    if(_height != 0)
        _aspectRatio = _width / _height;
    else
        _aspectRatio = 0;

    setZValue(-1);

    _corners[0] = new CornerGrabber(this,0);
    _corners[1] = new CornerGrabber(this,1);
    _corners[2] = new CornerGrabber(this,2);
    _corners[3] = new CornerGrabber(this,3);
    _corners[4] = new CornerGrabber(this,4);

    _corners[0]->setFlag(ItemIsMovable);
    _corners[1]->setFlag(ItemIsMovable);
    _corners[2]->setFlag(ItemIsMovable);
    _corners[3]->setFlag(ItemIsMovable);
    _corners[4]->setFlag(ItemIsMovable);

    _corners[0]->setVisible(false);
    _corners[1]->setVisible(false);
    _corners[2]->setVisible(false);
    _corners[3]->setVisible(false);
    _corners[4]->setVisible(false);
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool ImageItem::sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) {
    if(_locked)
        return true;

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL)
        return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }

    switch (event->type() )
    {
            // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress:
            {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();
                if(corner->getCorner() == 4) {
                    emit imageStartRotate();
                    corner->setRotateState(true);
                } else {
                    emit imageStartMove();
                }
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                corner->setMouseState(CornerGrabber::kMouseReleased);
                if(corner->getCorner() == 4) {
                    emit imageStopRotate();
                    corner->setRotateState(false);
                } else {
                    emit imageStopMove();
                }
            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
    }

    if ( corner->getMouseState() == CornerGrabber::kMouseMoving ) {
        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;
        case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;

        case 2:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;

        case 3:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;
        case 4:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;
        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        if(corner->getCorner() != 4){

            int newWidth = _width + ( XaxisSign * xMoved);
            if ( newWidth < 40 ) newWidth  = 40;

            int newHeight = _height + (YaxisSign * yMoved) ;
            if ( newHeight < 40 ) newHeight = 40;

            if(_maintainAspectRatio){
                if(newWidth > newHeight * _aspectRatio){
                    newWidth = newHeight * _aspectRatio;
                }else {
                    newHeight = newWidth / _aspectRatio;
                }
            }

            _width = newWidth;
            _height = newHeight;

            setCornerPositions();

        } else {
            /*float alpha = rotateF(QPointF(0, 0), QPointF(0, -_height/2 - _corners[4]->getRad() / 2), QPointF(0 - xMoved, -_height/2 - _corners[4]->getRad() / 2 - yMoved));
            item->setRect(QRectF(QPointF(0, -_height/2 - _corners[4]->getRad() / 2) - QPointF(10, 10), QPointF(0, -_height/2 - _corners[4]->getRad() / 2) + QPointF(10, 10)));
            item->setRect(QRectF(QPointF(0 - xMoved, -_height/2 - _corners[4]->getRad() / 2 - yMoved) - QPointF(10, 10), QPointF(0 - xMoved, -_height/2 - _corners[4]->getRad() / 2 - yMoved) + QPointF(10, 10)));
            int alpha = int((xMoved + yMoved) / 10 * _scaleFactor);
            setRotation(((int(rotation())) + alpha));
            float alpha = rotateF(QPointF(0, 0), QPointF(0, -_height/2 - _corners[4]->getRad() / 2), QPointF(0 - xMoved, -_height/2 - _corners[4]->getRad() / 2 - yMoved));
                        setRotation(rotation() + (int)qRadiansToDegrees(alpha) % 360);*/
        }
    }

    return true;
}

// hide the corner grabbers
void ImageItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ) {
    m_hovered = false;
    if(_locked)
        return;

    _corners[0]->setVisible(false);
    _corners[1]->setVisible(false);
    _corners[2]->setVisible(false);
    _corners[3]->setVisible(false);
    _corners[4]->setVisible(false);

    prepareGeometryChange();
}


// show the corner grabbers
void ImageItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * ) {
    m_hovered = true;
    if(_locked)
        return;

    _corners[0]->setVisible(true);
    _corners[1]->setVisible(true);
    _corners[2]->setVisible(true);
    _corners[3]->setVisible(true);
    _corners[4]->setVisible(true);

    _corners[0]->installSceneEventFilter(this);
    _corners[1]->installSceneEventFilter(this);
    _corners[2]->installSceneEventFilter(this);
    _corners[3]->installSceneEventFilter(this);
    _corners[4]->installSceneEventFilter(this);

    setScaleFactor(_scaleFactor);

    prepareGeometryChange();
}

void ImageItem::setMoveable(bool f) {
    setFlag(ItemIsMovable, f && _locked);
}

bool ImageItem::isHovered() {
    return m_hovered;
}

void ImageItem::setCornerPositions() {
    _corners[0]->setPos(-_width / 2, -_height / 2);
    _corners[1]->setPos(_width / 2 - _cornerRad * 2,  -_height / 2);
    _corners[2]->setPos(_width / 2 - _cornerRad * 2 , _height / 2 - _cornerRad * 2);
    _corners[3]->setPos(-_width / 2, _height / 2 - _cornerRad * 2);
    _corners[4]->setRad(_width < _height? _width/2: _height / 2);
    _corners[4]->setPos(0 - _corners[4]->getRad() / 2, -_height / 2 - _corners[4]->getRad());
}

void ImageItem::setScaleFactor(float f){
    _scaleFactor = f;
    _cornerRad = (4 / f);
    if(_cornerRad < 1)
        _cornerRad = 1;

    if(_corners[0] != nullptr)
        _corners[0]->setRad(_cornerRad);
    if(_corners[1] != nullptr)
        _corners[1]->setRad(_cornerRad);
    if(_corners[2] != nullptr)
        _corners[2]->setRad(_cornerRad);
    if(_corners[3] != nullptr)
        _corners[3]->setRad(_cornerRad);
    if(_corners[4] != nullptr) {
        _corners[4]->setScaleFactor(f);
        _corners[4]->setRad(_width < _height? _width/2: _height/2);
    }
    if(_corners[0] != nullptr && _corners[1] != nullptr && _corners[2] != nullptr && _corners[3] != nullptr && _corners[4] != nullptr)
        setCornerPositions();
}

QRectF ImageItem::boundingRect() const {
    return QRectF(-_width / 2, -_height / 2, _width, _height);
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes
void ImageItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawPixmap(-_width/2, -_height/2, _width, _height, _pixmap);
    if(_corners[0]->isVisible() && _corners[1]->isVisible() && _corners[2]->isVisible() && _corners[3]->isVisible() && _corners[4]->isVisible())
    {
        QPen pen(Qt::DashDotLine);
        int w = _cornerRad > 2 ? (int)(_cornerRad/2): 1;
        pen.setWidth(w);
        painter->setPen(pen);

        painter->drawRect(boundingRect());
    }
}

QPointF ImageItem::getCenterPos() {
    return boundingRect().center();
}

void ImageItem::setImageSize(int w, int h) {
    _width = w;
    _height = h;
    if(_corners[4] != nullptr)
        _corners[4]->setRad(_width < _height? _width/2: _height/2);
}

void ImageItem::setImageAngle(int alpha) {
    setRotation(alpha);
}

int ImageItem::getImageWith(){
    return int(_width);
}

int ImageItem::getImageHeight(){
    return (int)_height;
}

int ImageItem::getImageX(){
    return pos().x();
}

int ImageItem::getImageY(){
    return pos().y();
}

int ImageItem::getImageAngle(){
    return rotation();
}

bool ImageItem::isLocked() {
    return _locked;
}

void ImageItem::lockImage(bool f){
    _locked = f;
    this->setFlag(ItemIsMovable, !_locked);
}

void ImageItem::maintainAspectRatio(bool f){
    _maintainAspectRatio = f;
    if(_height != 0)
        _aspectRatio = _width / _height;
    else
        _aspectRatio = 0;
}
