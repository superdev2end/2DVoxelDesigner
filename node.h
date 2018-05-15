#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

#include <QDebug>

class Node :  public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    Node();
    ~Node() override;

    void            setId(int);
    int             getId(){ return m_nodeIndex; }
    void            setLayerIndex(int index){ m_layerIndex = index; }
    int             getLayerIndex(){ return m_layerIndex; }
    QColor          getColor();
    void            setColor(QColor);
    void            setCentralPos(QPoint pt);
    QPoint          getCentralPos();
    QPoint          getLocalPos();
    void            setLocalPos(QPoint pt);
    void            setScaleFactor(float s);
    void            setSpacingSize(float s);
    void            setSelectionState(bool f);
    void            setTextVisible(bool f);
    void            setReactor(bool f);
    bool            isReactor();
    bool            isLinkedNode(Node*);
    void            resetPosition();

    void            undo();
    void            redo();
    void            setMovable(bool);
    bool            isMovable();

    void            updateNode();
    void            addMarker(int);
    void            clearMarker();
    QList<int>      getMarkerList();

protected:
    QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    void            mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void            mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void            mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void            initMarkerList();

public:
    int             m_nodeIndex;

private:
    bool                m_layerIndex;

    QPoint              m_centralPos;
    float               m_scaleFactor;
    int                 m_spacingSize;
    int                 m_spacingUnit;
    QGraphicsTextItem * m_TextItem;
    float               m_movableRangeFactor;
    QColor              m_color;
    bool                m_isReactorNode;

    QList<QPoint>       m_historyPositionBefore;
    QList<QPoint>       m_historyPositionAfter;
    QList<int>          m_markerList;

signals:
    void nodeChanged(Node*);
    void nodePressed(Node*);
    void markersShow(Node*, QPoint);
    void nodeCoordinate(Node*, QPoint);

};

#endif // NODE_H
