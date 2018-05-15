#include "shapepointhandlecurve.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapePointHandleCurve::ShapePointHandleCurve(int type, QColor color, int width) {
    setType(type);
    m_curve.clear();
    _moving_points.clear();
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_bAddable = true;
    m_shapeLineWidth = width;
}

ShapePointHandleCurve::~ShapePointHandleCurve() {
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            delete _moving_points.at(i);
        }
    }
    _moving_points.clear();

    for (int i = 0; i < m_curve.count(); i++) {
        if(m_curve.at(i) != nullptr) {
            delete m_curve.at(i);
        }
    }
    m_curve.clear();
}

void ShapePointHandleCurve::setPoints(QList<QPoint> points) {
    m_points.clear();
    for (int i = 0; i < points.size(); ++i) {
        m_points.push_back(points.at(i));
        addArrow(-1);
    }
}

QList<QPoint> ShapePointHandleCurve::getPoints() {
    return m_points;
}

void ShapePointHandleCurve::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeLine();
}

void ShapePointHandleCurve::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeLine();
}

void ShapePointHandleCurve::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_points.size() << endl;
    for (int i = 0; i < m_points.size(); ++i) {
        *stream << m_points[i].x() << " " << m_points[i].y() << endl;
    }
}

void ShapePointHandleCurve::open(QTextStream* stream) {
    BaseShape::open(stream);
    int count = 0;
    *stream >> count;
    int x = 0, y = 0;
    m_points.clear();
    _moving_points.clear();
    for (int i = 0; i < count; ++i) {
        *stream >> x >> y;
        m_points.push_back(QPoint(x, y));
        addArrow(-1);
    }
    addArrow(-1);
    removeArrows();
}

QRectF ShapePointHandleCurve::boundingRect() const {
    return m_boundingRectF;
}

void ShapePointHandleCurve::removeArrows() {
    for (int i = 0 ; i < _moving_points.count(); ++i) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->hide();
        }
    }
    setSelectionState(false);
}

void ShapePointHandleCurve::move(QPointF first, QPointF second) {
    if(m_nMoveType == MOVE_MODE_X) {
        for (int i = 0 ; i < m_points.count(); i++) {
            m_points[i] = moveX(m_points[i], first, second, getAngle());
        }
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        _moving_points[m_nSelectedArrow]->updateView();
    } else if(m_nMoveType == MOVE_MODE_Y) {
        for (int i = 0 ; i < m_points.count(); i++) {
            m_points[i] = moveY(m_points[i], first, second, getAngle());
        }
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
        _moving_points[m_nSelectedArrow]->updateView();
    } else if(m_nMoveType == MOVE_MODE_ROTATE) {
        setCenterPoint(calculateRect(m_points).center());
        float angle = rotate(getCenterPoint(), first, second);
        setAngle(qRadiansToDegrees(angle) + getAngle());
        _moving_points[m_nSelectedArrow]->setAngle(getAngle());
        _moving_points[m_nSelectedArrow]->setCenterPoint(getCenterPoint());
    }
    updateShapeLine();
}

void ShapePointHandleCurve::addArrow(int index) {
    if(index > _moving_points.count()) {
        return;
    }
    if(index < 0 || _moving_points[index] == nullptr) {
        _moving_points.push_back(new MovingArrow(this));
        index = _moving_points.count() - 1;
    }
    _moving_points[index]->setScaleFactor(m_scaleFactor);
    _moving_points[index]->setIsDrawing(getIsDrawing());
    _moving_points[index]->show();
    setSelectionState(true);
    if(index < m_points.count()) {
        _moving_points[index]->setAngle(0);
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[index]->setCenterPoint(m_points[index]);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setIsShowRotation(true);
        _moving_points[index]->setAngle(getAngle());
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[index]->setCenterPoint(getCenterPoint());
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).y() + 5 / m_scaleFactor)));
    }
}

void ShapePointHandleCurve::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapePointHandleCurve::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapePointHandleCurve::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapePointHandleCurve::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

bool ShapePointHandleCurve::checkIsIn(QPoint point) {
    QPolygonF interpolatedPoints;
    interpolatedPoints.clear();
    interpolatedPoints.clear();
    QPolygonF boorNetPoints;
    QVector<qreal> knotVector;

    int middleKnotNumber = m_points.size() - 4;
    knotVector.clear();
    for (int counter = 0; counter < 4; ++counter)
      knotVector.push_back(0.0);
    for (int counter = 1; counter <= middleKnotNumber; ++counter)
      knotVector.push_back(1.0 / (middleKnotNumber + 1) * counter);
    for (int counter = 0; counter < 4; ++counter)
      knotVector.push_back(1.0);

    bezierInterpolator.CalculateBoorNet(m_points, knotVector, boorNetPoints);
    interpolatedPoints.push_back(m_points.first());
    for (int counter = 0; counter < boorNetPoints.size() - 3; counter += 3)
      bezierInterpolator.InterpolateBezier(boorNetPoints[counter],
                                           boorNetPoints[counter + 1],
                                           boorNetPoints[counter + 2],
                                           boorNetPoints[counter + 3],
                                           interpolatedPoints);
    interpolatedPoints.push_back(m_points.last());
    bool bIsIn = false;
    for (QPolygonF::iterator pointIt = interpolatedPoints.begin(),
         pointEnd = interpolatedPoints.end(); pointIt != pointEnd; ++pointIt) {
      if (pointIt != interpolatedPoints.end() - 1) {
          if(bIsIn == false) {
              bIsIn = isNearBy(distancePointLine(point, QPoint((*pointIt).x(), (*pointIt).y()), QPoint((*(pointIt + 1)).x(), (*(pointIt + 1)).y())), m_scaleFactor, "POINT HANDLE CURVE");
              if(bIsIn) {
                  break;
              }
          }
      }
    }
    return bIsIn;
}

void ShapePointHandleCurve::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint origin = rotateDotByAngle(QPoint(m_boundingRectF.center().x(), m_boundingRectF.center().y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        if(checkIsIn(origin)) {
            m_nSelectedArrow = m_points.count();
            emit selectedObject(this, m_points.count());
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapePointHandleCurve::setSelectionState(bool f) {
    if(getIsDrawing()) {
        return;
    }
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        for (int i = 0; i < m_curve.count(); i++) {
            if(m_curve.at(i) != nullptr) {
                m_curve.at(i)->setPen(m_pen);
            }
        }
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        for (int i = 0; i < m_curve.count(); i++) {
            if(m_curve.at(i) != nullptr) {
                m_curve.at(i)->setPen(m_pen);
            }
        }
    }
}

void ShapePointHandleCurve::redraw() {
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->updateView();
        }
    }
}

void ShapePointHandleCurve::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapePointHandleCurve::setIsAddabble(bool f) {
    m_bAddable = f;
}

void ShapePointHandleCurve::addPoint(QPoint data){
    if(!m_bAddable) {
        if(m_points.count() == 1) {
            m_points.push_back(data);
            addArrow(-1);
        } else {
            m_points[m_points.count() - 1] = data;
        }
    } else {
        m_points.push_back(data);
        addArrow(-1);
        m_bAddable = false;
    }
    updateShapeLine();
}

void ShapePointHandleCurve::removeLast() {
    if(m_points.count() == 0) {
        return;
    }
    m_points.removeLast();
    updateShapeLine();
}

void ShapePointHandleCurve::updateShapeLine() {
    setCenterPoint((calculateRect(m_points).center()));
    setBoudingRect(calculateRect(m_points));
    for (int i = 0; i < m_curve.count(); i++) {
        if(m_curve.at(i) != nullptr) {
            delete m_curve.at(i);
        }
    }
    m_curve.clear();
    QPainterPath pth;
    if(m_points.count() < 2) {
        return;
    } else if (m_points.count() == 2) {
        QGraphicsLineItem* item = new QGraphicsLineItem(this);
        item->setLine(QLineF(m_points[0], m_points[1]));
        if(getIsDrawing()) {
            m_pen.setColor(Qt::yellow);
        } else {
            m_pen.setColor(m_shapeLineColor);
        }
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        item->setPen(m_pen);
        m_curve.push_back(item);
    } else {
        QPolygonF interpolatedPoints;
        interpolatedPoints.clear();
        interpolatedPoints.clear();
        QPolygonF boorNetPoints;
        QVector<qreal> knotVector;

        int middleKnotNumber = m_points.size() - 4;
        knotVector.clear();
        for (int counter = 0; counter < 4; ++counter)
          knotVector.push_back(0.0);
        for (int counter = 1; counter <= middleKnotNumber; ++counter)
          knotVector.push_back(1.0 / (middleKnotNumber + 1) * counter);
        for (int counter = 0; counter < 4; ++counter)
          knotVector.push_back(1.0);

        bezierInterpolator.CalculateBoorNet(m_points, knotVector, boorNetPoints);
        interpolatedPoints.push_back(m_points.first());
        for (int counter = 0; counter < boorNetPoints.size() - 3; counter += 3)
          bezierInterpolator.InterpolateBezier(boorNetPoints[counter],
                                               boorNetPoints[counter + 1],
                                               boorNetPoints[counter + 2],
                                               boorNetPoints[counter + 3],
                                               interpolatedPoints);
        interpolatedPoints.push_back(m_points.last());
        for (QPolygonF::iterator pointIt = interpolatedPoints.begin(),
             pointEnd = interpolatedPoints.end(); pointIt != pointEnd; ++pointIt) {
          if (pointIt != interpolatedPoints.end() - 1) {
              QGraphicsLineItem* line = new QGraphicsLineItem(this);
              line->setPen(m_pen);
              line->setLine(QLineF(*pointIt, *(pointIt + 1)));
              if(getIsDrawing()) {
                  m_pen.setColor(Qt::yellow);
              } else {
                  m_pen.setColor(m_shapeLineColor);
              }
              int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
              if(pen_width < 1)
                  pen_width = 1;
              if(pen_width != m_pen.width()){
                  m_pen.setWidth(pen_width);
              }
              line->setPen(m_pen);
              m_curve.push_back(line);
              line->setTransformOriginPoint(getCenterPoint());
              line->setRotation(getAngle());
          }
        }
        if(getIsDrawing()) {
            for (int i = 0; i < m_points.size() - 1; ++i) {
                QGraphicsLineItem* controlLine = new QGraphicsLineItem(this);
                QPen pen = QPen();
                pen.setStyle(Qt::DotLine);
                pen.setColor(Qt::blue);
                int pen_width = (int)(3 / m_scaleFactor);
                if(pen_width < 1)
                    pen_width = 1;
                pen.setWidth(pen_width);
                controlLine->setPen(pen);
                controlLine->setLine(QLineF(m_points[i], m_points[i + 1]));
                m_curve.push_back(controlLine);
            }
        }
    }
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr && _moving_points.at(i)->isVisible()) {
            addArrow(i);
        }
    }
}

void ShapePointHandleCurve::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeLine();
}
