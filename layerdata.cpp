#include "layerdata.h"

LayerData::LayerData()
{
//    setCacheMode(DeviceCoordinateCache);
    setHandlesChildEvents(false);
    m_visibility = false;
}

void LayerData::clear()
{
//    foreach(Edge* edge, m_edgeList){
//        removeFromGroup(edge);
////        edge->deleteLater();
//    }

//    foreach(Node* node, m_nodeList){
//        removeFromGroup(node);
////        node->deleteLater();
//    }

    m_nodeList.clear();
    m_edgeList.clear();
}

void LayerData::setLayerIndex(int index)
{
    m_layerIndex = index;
    setZValue((index + 1) * 10);
    foreach(Node* node, m_nodeList){
        if(node){
            node->setLayerIndex(index);
        }
    }
}

void LayerData::setAllVisible(bool flg)
{
//    setVisible(flg);
    m_visibility = flg;
}

bool LayerData::isAllVisible()
{
    return m_visibility;
//    return isVisible();
}

void LayerData::setActivated(bool flg)
{
    m_activated = flg;
    foreach(Node* node, m_nodeList){
        if(node){
            node->setMovable(m_activated && m_unlocked);
        }
    }
}

void LayerData::setAllUnlock(bool flg)
{
    m_unlocked = flg;

    foreach(Node* node, m_nodeList){
        if(node){
            node->setMovable(m_activated && m_unlocked);
        }
    }
}

void LayerData::setNodeColor(QColor c)
{
    m_nodeColor = c;
    foreach(Node* node, m_nodeList){
        if(node){
            node->setColor(m_nodeColor);
        }
    }
}

void LayerData::setEdgeColor(QColor c)
{
    m_edgeColor = c;

    foreach(Edge* edge, m_edgeList){
        if(edge){
            edge->setColor(m_edgeColor);
        }
    }
}

QList<Node*>* LayerData::nodes()
{
    return &m_nodeList;
}

Node* LayerData::nodeAt(int index)
{
    if(index < m_nodeList.count() && index >= 0)
        return m_nodeList.at(index);
    else {
        return nullptr;
    }
}

Edge* LayerData::edgeAt(int index)
{
    if(index < m_edgeList.count() && index >= 0)
        return m_edgeList.at(index);
    else {
        return nullptr;
    }
}

int LayerData::nodeCount()
{
    return m_nodeList.count();
}

int LayerData::edgeCount()
{
    return m_edgeList.count();
}

QList<Edge*>* LayerData::edges()
{
    return &m_edgeList;
}

void LayerData::addNode(Node *n)
{
    addToGroup(n);
    m_nodeList.push_back(n);
}

void LayerData::addEdge(Edge *e)
{
    addToGroup(e);
    m_edgeList.push_back(e);
}
