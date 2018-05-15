#ifndef VOXELPOLYGON_H
#define VOXELPOLYGON_H

#include <QObject>
#include <QGraphicsPolygonItem>

#include "node.h"

class VoxelPolygon : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    VoxelPolygon();
    ~VoxelPolygon() override;

    void    initPoints();
    void    setNodesFromVoxelPolygon(VoxelPolygon* voxel);
    void    setNodes(Node* node1, Node* node2, Node* node3, Node* node4);
    void    setColor(QColor color);
    QColor  getColor(){ return m_fillColor; }
    void    setSelectionState(bool flg = true);
    bool    hasSameNodes(VoxelPolygon* voxel);
    bool    hasNodes(Node* node1, Node* node2, Node* node3, Node* node4);
    bool    hasNullNode();
    void    updateVisibility();
    bool    isChangable();

    Node*   node1(){ return m_node1; }
    Node*   node2(){ return m_node2; }
    Node*   node3(){ return m_node3; }
    Node*   node4(){ return m_node4; }

    int     getLayerIndex(){ if(m_node1) return m_node1->getLayerIndex(); else return -1; }

public slots:
    void    updatePolygon();
private:
    Node*       m_node1;
    Node*       m_node2;
    Node*       m_node3;
    Node*       m_node4;

    QColor      m_fillColor;
};

#endif // VOXELPOLYGON_H
