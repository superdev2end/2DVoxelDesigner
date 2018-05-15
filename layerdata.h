#ifndef LAYERDATA_H
#define LAYERDATA_H

#include <QObject>

#include "node.h"
#include "edge.h"

class LayerData : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    explicit    LayerData();

    void        setLayerIndex(int index);
    int         getLayerIndex(){ return m_layerIndex; }

    void        setActivated(bool flg);
    void        setAllVisible(bool flg);
    void        setAllUnlock(bool flg);

    void        clear();

    bool        isAllVisible();
    bool        isAllUnlock(){ return m_unlocked; }
    bool        isActivated(){ return m_activated; }

    void        setNodeColor(QColor c);
    void        setEdgeColor(QColor c);
    QColor      getNodeColor(){ return m_nodeColor; }
    QColor      getEdgeColor(){ return m_edgeColor; }

    QList<Node*>*   nodes();
    QList<Edge*>*   edges();
    Node*           nodeAt(int index);
    Edge*           edgeAt(int index);
    int             nodeCount();
    int             edgeCount();
    void            addNode(Node* n);
    void            addEdge(Edge* e);
signals:

public slots:

public:
    int                 m_layerIndex;

    bool                m_visibility;
    bool                m_activated;
    bool                m_unlocked;

    QColor              m_nodeColor;
    QColor              m_edgeColor;

private:
    QList<Node*>        m_nodeList;
    QList<Edge*>        m_edgeList;
};

#endif // LAYERDATA_H
