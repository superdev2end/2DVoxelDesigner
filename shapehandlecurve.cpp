#include "shapehandlecurve.h"

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapeHandleCurve::ShapeHandleCurve(int type, QColor color, int width) {
    setType(type);
    m_mainPath = new QGraphicsPathItem(this);
    m_handleLine = new QGraphicsLineItem(this);
    _moving_points.clear();
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_bAddable = true;
    m_shapeLineWidth = width;
}

ShapeHandleCurve::~ShapeHandleCurve() {
    if(m_mainPath != nullptr) {
        delete m_mainPath;
    }
    if(m_handleLine != nullptr) {
        delete m_handleLine;
    }
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            delete _moving_points.at(i);
        }
    }
    _moving_points.clear();
}

void ShapeHandleCurve::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeLine();
}

void ShapeHandleCurve::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeLine();
}

void ShapeHandleCurve::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_points.size() << endl;
    for (int i = 0; i < m_points.size(); ++i) {
        *stream << m_points[i].x() << " " << m_points[i].y() << endl;
    }
}

void ShapeHandleCurve::setPoints(QList<QPoint> points) {
    m_points.clear();
    for (int i = 0; i < points.size(); ++i) {
        m_points.push_back(points.at(i));
        addArrow(-1);
    }
}

QList<QPoint> ShapeHandleCurve::getPoints() {
    return m_points;
}

void ShapeHandleCurve::open(QTextStream* stream) {
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

QRectF ShapeHandleCurve::boundingRect() const {
    return m_boundingRectF;
}

void ShapeHandleCurve::removeArrows() {
    for (int i = 0 ; i < _moving_points.count(); ++i) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->hide();
        }
    }
    setSelectionState(false);
}

void ShapeHandleCurve::move(QPointF first, QPointF second) {
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

void ShapeHandleCurve::addArrow(int index) {
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
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[index]->setAngle(0);
        _moving_points[index]->setCenterPoint(m_points[index]);
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), m_points[index], getAngle()).y() + 5 / m_scaleFactor)));
    } else {
        _moving_points[index]->setIsShowRotation(true);
        setCenterPoint(calculateRect(m_points).center());
        _moving_points[index]->setAngle(getAngle());
        _moving_points[index]->setCenterPoint(getCenterPoint());
        _moving_points[index]->setBoudingRect(QRectF(QPoint(rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).x() - 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).y() - 5 / m_scaleFactor), QPoint(rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).x() + 5 / m_scaleFactor, rotateDotByAngle(getCenterPoint(), getCenterPoint(), getAngle()).y() + 5 / m_scaleFactor)));
    }
}

void ShapeHandleCurve::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapeHandleCurve::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapeHandleCurve::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapeHandleCurve::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapeHandleCurve::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint origin = rotateDotByAngle(QPoint(m_boundingRectF.center().x(), m_boundingRectF.center().y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        QPointF point = closestPointOnPath(origin, m_mainPath->path());
        if(isNearBy(magnitude(QPoint(point.x(), point.y()), QPoint(origin.x(), origin.y())), m_scaleFactor, "HANDLE CURVE")) {
            m_nSelectedArrow = m_points.count();
            emit selectedObject(this, m_points.count());
            setSelectionState(true);

        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapeHandleCurve::setSelectionState(bool f) {
    if(f) {
        m_pen.setColor(Qt::yellow);
        int pen_width = (int)((2 + m_shapeLineWidth) / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainPath->setPen(m_pen);
    } else {
        m_pen.setColor(m_shapeLineColor);
        int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
        if(pen_width < 1)
            pen_width = 1;
        if(pen_width != m_pen.width()){
            m_pen.setWidth(pen_width);
        }
        m_mainPath->setPen(m_pen);
    }
}

void ShapeHandleCurve::redraw() {
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->updateView();
        }
    }
}

void ShapeHandleCurve::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapeHandleCurve::setIsAddabble(bool f) {
    m_bAddable = f;
}

void ShapeHandleCurve::addPoint(QPoint data){
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

void ShapeHandleCurve::removeLast() {
    if(m_points.count() == 0) {
        return;
    }
    m_points.removeLast();
    updateShapeLine();
}

void ShapeHandleCurve::updateShapeLine() {
    setCenterPoint((calculateRect(m_points).center()));
    setBoudingRect(calculateRect(m_points));

    QPainterPath pth;
    if(m_points.count() < 2) {
        return;
    } else if (m_points.count() == 2) {
        pth.moveTo(m_points.at(0));
        pth.lineTo(m_points.at(1));
    } else {
        pth.moveTo(m_points.at(0));
        m_handleLine->hide();
        for (int i = 2; i < m_points.count(); ++i) {
            if(i % 2 == 0) {
                if(i == m_points.count() - 1 && getIsDrawing()) {
                    QPen pen;
                    pen.setStyle(Qt::DashDotLine);
                    pen.setColor(Qt::yellow);
                    pen.setWidth(m_shapeLineWidth / m_scaleFactor);
                    m_handleLine->setPen(pen);
                    m_handleLine->setLine(QLine(m_points.at(i - 1), m_points.at(i)));
                    m_handleLine->show();
                }
                if(i == 2) {
                    pth.cubicTo( m_points.at(i - 1) - (m_points.at(i) - m_points.at(i - 1)), m_points.at(i - 1) - (m_points.at(i) - m_points.at(i - 1)), m_points.at( i - 1 ) );
                }
            } else {
                if(i == m_points.count() - 1) {
                    pth.cubicTo( m_points.at(i - 1), m_points.at(i - 1), m_points.at(i));
                } else {
                    pth.cubicTo( m_points.at(i - 1), m_points.at(i) - (m_points.at(i + 1) - m_points.at(i)), m_points.at(i));
                }
            }
        }
    }
    m_mainPath->setPath(pth);
    int pen_width = (int)(m_shapeLineWidth / m_scaleFactor);
    if(pen_width < 1)
        pen_width = 1;
    if(pen_width != m_pen.width()){
        m_pen.setWidth(pen_width);
        m_mainPath->setPen(m_pen);
    }
    m_mainPath->setTransformOriginPoint(getCenterPoint());
    m_mainPath->setRotation(getAngle());
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr && _moving_points.at(i)->isVisible()) {
            addArrow(i);
        }
    }
}

void ShapeHandleCurve::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeLine();
}

