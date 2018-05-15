#include "shapepointcurve.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ShapePointCurve::ShapePointCurve(int type, QColor color, int width) {
    setType(type);
    m_mainPath = new QGraphicsPathItem(this);
    _moving_points.clear();
    m_shapeLineColor = color;
    m_pen.setColor(m_shapeLineColor);
    m_bAddable = true;
    m_shapeLineWidth = width;
}

ShapePointCurve::~ShapePointCurve() {
    if(m_mainPath != nullptr) {
        delete m_mainPath;
    }
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            delete _moving_points.at(i);
        }
    }
    _moving_points.clear();
}

void ShapePointCurve::setLineWidth(float width) {
    BaseShape::setLineWidth(width);
    updateShapeLine();
}

void ShapePointCurve::setLineColor(QColor color) {
    BaseShape::setLineColor(color);
    updateShapeLine();
}

void ShapePointCurve::save(QTextStream* stream) {
    BaseShape::save(stream);
    *stream << m_points.size() << endl;
    for (int i = 0; i < m_points.size(); ++i) {
        *stream << m_points[i].x() << " " << m_points[i].y() << endl;
    }
}

void ShapePointCurve::open(QTextStream* stream) {
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

QRectF ShapePointCurve::boundingRect() const {
    return m_boundingRectF;
}

void ShapePointCurve::removeArrows() {
    for (int i = 0 ; i < _moving_points.count(); ++i) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->hide();
        }
    }
    setSelectionState(false);
}

void ShapePointCurve::move(QPointF first, QPointF second) {
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

void ShapePointCurve::addArrow(int index) {
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

void ShapePointCurve::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
}

void ShapePointCurve::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent((event));
}

void ShapePointCurve::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent((event));
}

bool ShapePointCurve::setMoveType(QPointF point) {
    if(_moving_points[m_nSelectedArrow] != nullptr) {
        m_nMoveType = _moving_points[m_nSelectedArrow]->setMoveType(point);
    }
    return m_nMoveType != MOVE_MODE_NONE;
}

void ShapePointCurve::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(m_bSelectable) {
        QPoint origin = rotateDotByAngle(QPoint(m_boundingRectF.center().x(), m_boundingRectF.center().y()), QPoint(event->pos().x(), event->pos().y()), - getAngle());
        QPointF point = closestPointOnPath(origin, m_mainPath->path());
        if(isNearBy(magnitude(QPoint(point.x(), point.y()), QPoint(origin.x(), origin.y())), m_scaleFactor, "POINT CURVE")) {
            m_nSelectedArrow = m_points.count();
            emit selectedObject(this, m_points.count());
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void ShapePointCurve::setSelectionState(bool f) {
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

void ShapePointCurve::redraw() {
    for (int i = 0; i < _moving_points.count(); i++) {
        if(_moving_points.at(i) != nullptr) {
            _moving_points.at(i)->updateView();
        }
    }
}

void ShapePointCurve::setPoints(QList<QPoint> points) {
    m_points.clear();
    for (int i = 0; i < points.size(); ++i) {
        m_points.push_back(points.at(i));
        addArrow(-1);
    }
}

QList<QPoint> ShapePointCurve::getPoints() {
    return m_points;
}

void ShapePointCurve::setSelectable(bool f){
    m_bSelectable = f;
}

void ShapePointCurve::setIsAddabble(bool f) {
    m_bAddable = f;
}

void ShapePointCurve::addPoint(QPoint data){
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

void ShapePointCurve::removeLast() {
    if(m_points.count() == 0) {
        return;
    }
    m_points.removeLast();
    updateShapeLine();
}

void ShapePointCurve::updateShapeLine() {
    setCenterPoint((calculateRect(m_points).center()));
    setBoudingRect(calculateRect(m_points));

    QPainterPath pth;
    if(m_points.count() < 2) {
        return;
    } else if (m_points.count() == 2) {
        pth.moveTo(m_points.at(0));
        pth.lineTo(m_points.at(1));
    } else {
        QPair<QPointF, QPointF> pair = controlPoints(m_points.at(0), m_points.at(1), m_points.at(2), 0.55);
        QPointF p0 = pair.second;
        pth.moveTo(m_points.at(0));
        pth.cubicTo(m_points.at(0), pair.first, m_points[1]);
        for (int i = 2; i < m_points.count() - 1; ++i) {
            QPair<QPointF, QPointF> pair = controlPoints( m_points.at(i - 1), m_points.at(i), m_points.at(i + 1), 0.55);
            pth.cubicTo( p0, pair.first, m_points.at( i ) );
            p0 = pair.second;
        }
        pair = controlPoints( m_points.at(m_points.count() - 2), m_points.at(m_points.count() - 1), m_points.at(m_points.count() - 2), 0.55);
        pth.cubicTo( p0, pair.first, m_points.at( m_points.count() - 1 ) );
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

void ShapePointCurve::setScaleFactor(float s) {
    m_scaleFactor = s;
    updateShapeLine();
}

