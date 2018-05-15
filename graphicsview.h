#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QScrollBar>
#include <QDebug>
#include <QDate>
#include <QApplication>
#include <QClipboard>
#include <QGraphicsPolygonItem>
#include <QFile>

//#include <QOpenGLWidget>
//#include <QGLWidget>

#include "node.h"
#include "edge.h"
#include "imageitem.h"
#include "baseshape.h"

#include "layerdata.h"
#include "shapeline.h"
#include "shapecornertocornerrectangle.h"
#include "shapecenterpointcircle.h"
#include "shapecentertocornerrectangle.h"
#include "shape3pointcircle.h"
#include "shapecenteroval.h"
#include "shapecenterarc.h"
#include "shape3pointoval.h"
#include "shape3pointarc.h"
#include "shapepolygon.h"
#include "shapepointcurve.h"
#include "shapehandlecurve.h"
#include "shapepointhandlecurve.h"
#include "markeritem.h"
#include "voxelpolygon.h"
#include "qmath.h"

#define EDIT_MODE_SELECT 0
#define EDIT_MODE_LINE 1
#define EDIT_MODE_CORNER_CORNER_TO_REACTANGLE 2
#define EDIT_MODE_CENTER_TO_CORNER_REACTANGLE 3
#define EDIT_MODE_CENTER_CIRCLE 4
#define EDIT_MODE_3_POINT_CIRCLE 5
#define EDIT_MODE_CENTER_OVAL 6
#define EDIT_MODE_3_POINT_OVAL 7
#define EDIT_MODE_CENTER_ARC 8
#define EDIT_MODE_3_POINT_ARC 9
#define EDIT_MODE_POLYGON 10
#define EDIT_MODE_HANDLE_CURVE 11
#define EDIT_MODE_POINT_CURVE 12
#define EDIT_MODE_POINT_HANDLE_CURVE 13

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);
    ~GraphicsView() override;

    // EventHandler from MainWindow
    void    onNew(int gridSize = 30, int spacingSize = 32);
    void    onSave(QString);
    void    onOpen(QString);
    void    onLoadImage(QString);
    void    onClearImage();
    void    onUpdateLineColor(QColor);
    void    onUpdateBackColor(QColor);
    void    onUpdateShapeLineColor(QColor);
    void    setPolygonNumber(int);
    int     getPolygonNumber();
    void    onResetPoint();
    void    onResetAllPoints();
    void    onUndo();
    void    onRedo();
    int     getEditMode();
    bool    isEditMode();
    int     getGridSpacingSize();
    void    setShapeLineWidth(int);
    int     getShapeLineWidth();
    void    selectMarker(Node*, int);
    QColor  getShapeLineColor();

    /// Layer Manager
    void    initLayerProperty();
    void    setCurrentLayerIndex(int index);
    void    showLayer(int layerIndex, int flg = true);
    void    unlockLayer(int layerIndex, int flg = true);
    void    setLayerColor(int layerIndex, QColor color);

    // Set workspace-property
    int     getSpacingSize(){ return m_spacingSize / m_spacingUnit; }
    int     getSpacingUnit(){ return m_spacingUnit; }

    int     updateEditMode(int);
    bool    toggleEditMode();
    void    startSnapObject();
    void    stopSnapObject();
    void    onDelete();
    void    updateMarkPoint(Node*);
    float   getScaleFactor();

    void    onMaintainAspectRatio(bool);
    void    onLockImage(bool);
    void    onShowImage(bool);
    int     getLineWidth();
    bool    getMaintainAspectRatio();
    bool    getLockImage();
    bool    getShowImage();
    Node*   getNode(LayerData* layer, int xIndex, int yIndex);
    Node*   getNode(LayerData* layer, int index);
    Node*   getNode(int xIndex, int yIndex);
    Node*   getNode(int index);

    /// Voxel Feature
    void    addColorVoxel(QColor);
    void    removeColorVoxel();
    void    setVoxelSelectMode(bool f = true);

public slots:
    // From node-signal
    void nodePressed(Node*);
    void nodeChanged(Node*);
    void markersShow(Node*, QPoint);
    void nodeCoordinate(Node* n, QPoint);
    // From MainWindow-signal
    void setReactorNodesFromSelectedVoxel();
    void onReactorGuide(bool);
    void selectedMarker(MarkerItem*);
    void selectedNodeFromMarker(MarkerItem*);
    // From shape
    void shapePressed(BaseShape*, int);
    void imageStartRotate();
    void imageStopRotate();
    void imageStartMove();
    void imageStopMove();

private:
    void    scaleView(qreal);
    void    initGraphicsItems();
    void    drawGraphicsItems();
    void    initMarkerList();
    int     calculateX(int);
    int     calculateY(int);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    float rotateF(QPointF, QPointF, QPointF);

signals:
    // To ReactorPreviewWidget
    void reactorChanged(Node*, Node*, Node*, Node*);
    void markersShowPopup(Node*, QPoint);
    void nodeCoordinatePopup(Node*, QPoint);
    void selectedShape(BaseShape*);
    void changedLayerColor(int layerIndex, QColor color);
    void changedLayerVisibility(int layerIndex, bool flg);
    void changedLayerUnlockState(int layerIndex, bool flg);
    void changedActiveLayer(int layerIndex);
private:
    QWidget* m_Parent;
    // Graphics Properties
    float               m_scaleFactor;
    int                 m_gridSize;
    int                 m_spacingSize;
    int                 m_spacingUnit;
    bool                m_bEditMode;
    bool                m_bMarkerMoveMode;
    QPoint              m_markerMoveStartPoint;
    MarkerItem*         m_selectedMarker;
    int                 m_EditMode;
    QColor              m_lineColor;
    QColor              m_backgroundColor;
    QColor              m_shapeLineColor;
    QList<MarkerItem*>  m_markList;

    // Graphics Items
    QList<LayerData*>   m_lstLayerData;
    int                 m_curLayerIndex;
    int                 m_layerCount;

    ImageItem*          m_imgItem;
    QString             m_imageFileName;
    Node*               m_selectedNode;
    QList<Node*>        m_reactorNodes;

    // Shape Items
    QList<BaseShape*>   m_shapeList;
    BaseShape*          m_currentShape;
    BaseShape*          m_snapShapeObject;
    BaseShape*          m_selectedShape;
    bool                m_bMoveShape;
    bool                m_bSnapShape;
    QPointF             m_moveStartPoint;
    int                 m_polygonNumber;
    int                 m_shapeLineWidth;

    // Image Mode
    bool                m_imageMaintainAspectRatio;
    bool                m_imageLocked;
    bool                m_imageLockedTemp;
    bool                m_imageShown;
    bool                m_bImageRotateMode;
    bool                m_bImageMoveMode;
    QPointF             m_imageMovingPrevPoint;

    // Reactor Guide
    bool                    m_reactorMode;


    // Pan Scrolling
    bool                    m_panScrollMode;
    int                     m_panStartX, m_panStartY;

    // Voxel Select Mode
    bool                    m_voxelSelectMode;
    VoxelPolygon*           m_cursorVoxel;
    VoxelPolygon*           m_selectedVoxel;
    QList<VoxelPolygon*>    m_voxelList;

    // Histories
    QList<QGraphicsItem*>   m_historiesBefore;
    QList<QGraphicsItem*>   m_historiesAfter;
};

#endif // GRAPHICSVIEW_H
