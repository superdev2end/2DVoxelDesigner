#include "voxelpolygon.h"

VoxelPolygon::VoxelPolygon()
{
    m_node1 = m_node2 = m_node3 = m_node4 = nullptr;
    initPoints();
    m_fillColor = Qt::white;
    setPen(Qt::NoPen);
}

VoxelPolygon::~VoxelPolygon()
{

}

void VoxelPolygon::initPoints()
{
    if(!hasNullNode()){
        disconnect(m_node1, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node2, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node3, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node4, SIGNAL(nodeChanged(Node*)));
    }

    m_node1 = m_node2 = m_node3 = m_node4 = nullptr;
    QPolygon poly(QPolygon(QVector<QPoint>() << QPoint(0, 0)
                           << QPoint(0, 0)
                           << QPoint(0, 0)
                           << QPoint(0, 0)));

    setPolygon(poly);
}

void VoxelPolygon::updateVisibility()
{
    if(hasNullNode()){
        initPoints();
        setVisible(true);
    }else
        setVisible(m_node1->isVisible());
}

bool VoxelPolygon::isChangable()
{
    if(hasNullNode()){
        initPoints();
        return false;
    }else{
        return m_node1->isVisible() && m_node1->flags().testFlag(QGraphicsItem::ItemIsMovable);
    }
}

void VoxelPolygon::updatePolygon()
{
    if(m_node1 == nullptr || m_node2 == nullptr || m_node3 == nullptr || m_node4 == nullptr)
        return;

    QPolygon poly(QPolygon(QVector<QPoint>() << m_node1->pos().toPoint()
                           << m_node2->pos().toPoint()
                           << m_node3->pos().toPoint()
                           << m_node4->pos().toPoint()));

    setBrush(QBrush(m_fillColor));
    setPolygon(poly);
}

void VoxelPolygon::setNodesFromVoxelPolygon(VoxelPolygon* voxel){

    if(voxel == nullptr)
        return;

    if(voxel->hasNullNode())
        return;

    setNodes(voxel->node1(),
             voxel->node2(),
             voxel->node3(),
             voxel->node4());
}

void VoxelPolygon::setNodes(Node *node1, Node *node2, Node *node3, Node *node4)
{
    if(node1 == nullptr || node2 == nullptr || node3 == nullptr || node4 == nullptr)
        return;

    if(!hasNullNode()){
        disconnect(m_node1, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node2, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node3, SIGNAL(nodeChanged(Node*)));
        disconnect(m_node4, SIGNAL(nodeChanged(Node*)));
    }

    m_node1 = node1;
    m_node2 = node2;
    m_node3 = node3;
    m_node4 = node4;

    connect(m_node1, SIGNAL(nodeChanged(Node*)), this, SLOT(updatePolygon()));
    connect(m_node2, SIGNAL(nodeChanged(Node*)), this, SLOT(updatePolygon()));
    connect(m_node3, SIGNAL(nodeChanged(Node*)), this, SLOT(updatePolygon()));
    connect(m_node4, SIGNAL(nodeChanged(Node*)), this, SLOT(updatePolygon()));
    updatePolygon();
}

void VoxelPolygon::setColor(QColor color)
{
    m_fillColor = color;
    updatePolygon();
}

void VoxelPolygon::setSelectionState(bool flg)
{
    if(flg){
        QPen pen(Qt::black);
        pen.setWidth(6);
        setPen(pen);
    }else{
        setPen(Qt::NoPen);
    }
    updatePolygon();
}

bool VoxelPolygon::hasSameNodes(VoxelPolygon *voxel)
{
    if(voxel == nullptr)
        return false;
    if(m_node1 == nullptr || m_node2 == nullptr || m_node3 == nullptr || m_node4 == nullptr)
        return false;

    if(m_node1 == voxel->node1() &&
            m_node2 == voxel->node2() &&
            m_node3 == voxel->node3() &&
            m_node4 == voxel->node4())
        return true;

    return false;
}

bool VoxelPolygon::hasNodes(Node *node1, Node *node2, Node *node3, Node *node4)
{
    if(m_node1 == nullptr || m_node2 == nullptr || m_node3 == nullptr || m_node4 == nullptr)
        return false;

    if(m_node1 == node1 &&
            m_node2 == node2 &&
            m_node3 == node3 &&
            m_node4 == node4)
        return true;
    return false;
}

bool VoxelPolygon::hasNullNode()
{
    if(m_node1 == nullptr || m_node2 == nullptr || m_node3 == nullptr || m_node4 == nullptr)
        return true;
    else {
        return false;
    }
}
