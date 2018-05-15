#include "baseshape.h"
#include <QtDebug>
#include <QtMath>

BaseShape::BaseShape(QObject *parent) : QObject(parent) {
    m_bSelectable = false;
    m_nMoveType = MOVE_MODE_NONE;
    m_nSelectedArrow = 0;
    m_angle = 0;
}

bool BaseShape::isNearBy(float f, float scale, QString str) {
    return f < 8 / scale;
}

void BaseShape::save(QTextStream *stream) {
    *stream << m_type << endl;
    *stream << m_centerPoint.x() << " ";
    *stream << m_centerPoint.y() << " ";
    *stream << m_angle << " ";
    int r = -1, g = -1, b = -1, a = -1;
    m_shapeLineColor.getRgb(&r, &g, &b, &a);
    *stream << r << " ";
    *stream << g << " ";
    *stream << b << " ";
    *stream << a << " ";
    *stream << m_shapeLineWidth << endl;
}

void BaseShape::setSelectedArrow(int nSelected) {
    m_nSelectedArrow = nSelected;
}

int BaseShape::getSelectedArrow() {
    return m_nSelectedArrow;
}

void BaseShape::open(QTextStream *stream) {
    int x = 0, y = 0;
    int r = -1, g = -1, b = -1, a = -1;
    *stream >> x >> y >> m_angle >> r >> g >> b >> a >> m_shapeLineWidth;
    setCenterPoint(QPoint(x, y));
    m_shapeLineColor.setRgb(r, g, b, a);
}

void BaseShape::setLineWidth(float width) {
    m_shapeLineWidth = width;
}

void BaseShape::setLineColor(QColor color) {
    m_shapeLineColor = color;
}

void BaseShape::setAngle(float angle) {
    m_angle = angle;
}

void BaseShape::setnMoveType(int type) {
    m_nMoveType = type;
}

void BaseShape::setCenterPoint(QPoint point) {
    m_centerPoint = point;
}

float BaseShape::getAngle() {
    return m_angle;
}

int BaseShape::getShapeLineWidth() {
    return m_shapeLineWidth;
}

QColor BaseShape::getShapeColor() {
    return m_shapeLineColor;
}

QPoint BaseShape::getCenterPoint() {
    return m_centerPoint;
}

void BaseShape::removeMoveType() {
    m_nMoveType = MOVE_MODE_NONE;
}

QPair<QPointF, QPointF> BaseShape::controlPoints(QPointF const& p0, QPointF const& p1, QPointF const& p2, qreal t) {
    QPair<QPointF, QPointF> pair;
    qreal d01 = qSqrt( ( p1.x() - p0.x() ) * ( p1.x() - p0.x() ) + ( p1.y() - p0.y() ) * ( p1.y() - p0.y() ) );
    qreal d12 = qSqrt( ( p2.x() - p1.x() ) * ( p2.x() - p1.x() ) + ( p2.y() - p1.y() ) * ( p2.y() - p1.y() ) );

    qreal fa = t * d01 / ( d01 + d12 );
    qreal fb = t * d12 / ( d01 + d12 );

    qreal c1x = p1.x() - fa * ( p2.x() - p0.x() );
    qreal c1y = p1.y() - fa * ( p2.y() - p0.y() );
    qreal c2x = p1.x() + fb * ( p2.x() - p0.x() );
    qreal c2y = p1.y() + fb * ( p2.y() - p0.y() );

    pair.first = QPointF( c1x, c1y );
    pair.second = QPointF( c2x, c2y );

    return pair;
}

QRect BaseShape::calculateRect(QList<QPoint> points) {
    if(points.count() < 2) {
        return QRect();
    }
    int left = points.at(0).x();
    int top = points.at(0).y();
    int right = points.at(0).x();
    int bottom = points.at(0).y();
    for (int i = 1; i < points.count(); i++) {
        if(left > points.at(i).x()) {
            left = points.at(i).x();
        }
        if(top > points.at(i).y()) {
            top = points.at(i).y();
        }
        if(right < points.at(i).x()) {
            right = points.at(i).x();
        }
        if(bottom < points.at(i).y()) {
            bottom = points.at(i).y();
        }
    }
    QPoint center = QRect(QPoint(left, top), QPoint(right, bottom)).center();
    float distance = 0;
    for (int i = 0; i < points.count(); ++i) {
        if(distance < magnitude(center, points.at(i))) {
            distance = magnitude(center, points.at(i));
        }
    }
    distance *= 10;
    return QRect(center - QPoint(distance, distance), center + QPoint(distance, distance));
}

QPointF BaseShape::closestPointOnPath(const QPointF &point, const QPainterPath &path) {
    if (path.isEmpty())
        return point;

    auto vec = QVector2D(point);
    auto poly = path.toFillPolygon();
    float d, minDist = FLT_MAX;
    QVector2D p, q, v, u, minVec;

    for (int k=0; k < poly.count() - 1; k++) {
        p = QVector2D(poly.at(k));
        if (k == poly.count() - 1)
            k = -1;
        q = QVector2D(poly.at(k+1));
        v = q - p;
        u = v.normalized();
        d = QVector2D::dotProduct(u, vec - p);

        if (d < 0.0f) {
            d = (vec - p).lengthSquared();

            if (d < minDist)
            {
                minDist = d;
                minVec = p;
            }
        }
        else if (d*d > v.lengthSquared()) {
            d = (vec - q).lengthSquared();

            if (d < minDist)
            {
                minDist = d;
                minVec = q;
            }
        }
        else {
            u *= d;
            u += p;
            d = (vec - u).lengthSquared();

            if (d < minDist) {
                minDist = d;
                minVec = u;
            }
        }
    }

    if (minDist >= FLT_MAX)
        return point;

    return minVec.toPointF();
}

int BaseShape::getMoveType() {
    return m_nMoveType;
}

QPoint BaseShape::calculate3PointCircle(QPoint first, QPoint second, QPoint third) {
    float D = (second.x() - first.x()) * (third.y() - second.y()) - (second.y() - first.y()) * (third.x() - second.x());
    float D1 = (0.5 * (second.x() * second.x() + second.y() * second.y() - first.x() * first.x() - first.y() * first.y()) * (third.y() - second.y())) -
            (0.5 * (third.x() * third.x() + third.y() * third.y() - second.x() * second.x() - second.y() * second.y()) * (second.y() - first.y()));
    float D2 = ((second.x() - first.x()) * 0.5 * (third.x() * third.x() + third.y() * third.y() - second.x() * second.x() - second.y() * second.y())) -
            (0.5 * (third.x() - second.x()) * (second.x() * second.x() + second.y() * second.y() - first.x() * first.x() - first.y() * first.y()));
    float x = D1 / D;
    float y = D2 / D;
    return QPoint(x, y);
}

float BaseShape::rotateF(QPointF centerPos, QPointF startPos, QPointF endPos) {
    if(endPos.x() == centerPos.x()) {
        return 0;
    }
    float angle1 = qAtan2(startPos.y() - centerPos.y(), startPos.x() - centerPos.x());
    float angle2 = qAtan2(endPos.y() - centerPos.y(), endPos.x() - centerPos.x());
    return angle2 - angle1;
}

float BaseShape::rotate(QPointF centerPos, QPointF startPos, QPointF endPos) {
    if(startPos.x() == centerPos.x() || endPos.x() == centerPos.x()) {
        return 0;
    }
    float angle1 = qAtan2(startPos.y() - centerPos.y(), startPos.x() - centerPos.x());
    float angle2 = qAtan2(endPos.y() - centerPos.y(), endPos.x() - centerPos.x());
    return angle2 - angle1;
}

QPoint BaseShape::rotateDotByAngle(QPoint center, QPoint origin, float angle) {
    QTransform transform = QTransform().translate(center.x(), center.y()).rotate(angle).translate(-center.x(), -center.y());
    return transform.map(origin);
}

QPoint BaseShape::moveX(QPointF originPos, QPointF startPos, QPointF endPos, float angle) {
    if(endPos.x() == startPos.x()) {
        return QPoint(originPos.x(), originPos.y());
    }
    float angle1 = qAtan2(endPos.y() - startPos.y(), endPos.x() - startPos.x());
    float length1 = qSqrt(qPow(endPos.y() - startPos.y(), 2) + qPow(endPos.x() - startPos.x(), 2)) * qCos(angle1 - qDegreesToRadians(angle));
    return QPoint(originPos.x() + length1 * qCos(qDegreesToRadians(angle)), originPos.y() + length1 * qSin(qDegreesToRadians(angle)));
}

QPoint BaseShape::moveY(QPointF originPos, QPointF startPos, QPointF endPos, float angle) {
    float angle1 = qAtan2(endPos.y() - startPos.y(), endPos.x() - startPos.x());
    float length1 = qSqrt(qPow(endPos.y() - startPos.y(), 2) + qPow(endPos.x() - startPos.x(), 2)) * qSin(angle1 - qDegreesToRadians(angle));
    return QPoint(originPos.x() + length1 * qCos(M_PI_2 + qDegreesToRadians(angle)), originPos.y() + length1 * qSin(M_PI_2 + qDegreesToRadians(angle)));
}

void BaseShape::setBoudingRect(QRectF rect) {
    m_boundingRectF = rect;
}

int BaseShape::getType() {
    return m_type;
}

void BaseShape::setType(int type) {
    m_type = type;
}

void BaseShape::setIsDrawing(bool f) {
    m_bDrawing = f;
}
bool BaseShape::getIsDrawing() {
    return m_bDrawing;
}

float BaseShape::distancePointLine( const QPoint &p, const QPoint &l1,
                                      const QPoint &l2 ) {
    float lineMag;
    float u;
    QPoint intersectPnt;

    lineMag = magnitude( l2, l1 );

    u = ( ( ( p.x() - l1.x() ) * ( l2.x() - l1.x() ) ) +
          ( ( p.y() - l1.y() ) * ( l2.y() - l1.y() ) ) ) /
        ( lineMag * lineMag );

    if ( u < 0.0f || u > 1.0f )
        return 100;   // closest point does not fall within the line segment

    intersectPnt.setX( int( l1.x() + u * ( l2.x() - l1.x() ) ) );
    intersectPnt.setY( int( l1.y() + u * ( l2.y() - l1.y() ) ) );

    return magnitude( p, intersectPnt );
}

float BaseShape::magnitude( const QPoint &p1, const QPoint &p2 ) {
    float vx, vy;

    vx = p2.x() - p1.x();
    vy = p2.y() - p1.y();

    return sqrt( vx * vx + vy * vy );
}

float BaseShape::magnitude( const QPointF &p1, const QPointF &p2 ) {
    float vx, vy;

    vx = p2.x() - p1.x();
    vy = p2.y() - p1.y();

    return sqrt( vx * vx + vy * vy );
}

float BaseShape::magnitude( const QPoint &p1, const QPointF &p2 ) {
    float vx, vy;

    vx = p2.x() - p1.x();
    vy = p2.y() - p1.y();

    return sqrt( vx * vx + vy * vy );
}

float BaseShape::magnitude( const QPointF &p1, const QPoint &p2 ) {
    float vx, vy;

    vx = p2.x() - p1.x();
    vy = p2.y() - p1.y();

    return sqrt( vx * vx + vy * vy );
}
