#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent) {
    // Set background of drawing-panel
    setStyleSheet("background-color:rgb(210,210,210);");

    //
    /// Layer Manager
    m_layerCount = 3;    

    initLayerProperty();

    /// ///

    // Graphics Properties
    m_scaleFactor = 1;
    m_spacingUnit = 10;
    m_gridSize = 30;                                    // Default : 30 * 30
    m_spacingSize = 32 * m_spacingUnit;                 // Default : 32 * 32
    m_lineColor = Qt::blue;
    m_shapeLineColor = Qt::blue;
    m_backgroundColor = QColor(210, 210, 210);

    m_imgItem = nullptr;

    // Select Options
    m_cursorVoxel = nullptr;
    m_selectedVoxel = nullptr;
    m_voxelSelectMode = false;

    m_EditMode = EDIT_MODE_SELECT;

    // Image Mode
    m_imageMaintainAspectRatio = true;
    m_imageLocked = false;
    m_imageLockedTemp = false;
    m_imageShown = true;
    m_bImageRotateMode = false;
    m_bImageMoveMode = false;

    // Reactor Guide
    m_reactorMode = false;

    // Pan Scrolling
    m_panScrollMode = false;

    // Shape init
    m_selectedMarker = nullptr;
    m_bMarkerMoveMode = false;
    m_currentShape = nullptr;
    m_selectedShape = nullptr;
    m_snapShapeObject = nullptr;
    m_bMoveShape = false;
    m_bSnapShape = false;
    m_polygonNumber = 3;
    m_shapeLineWidth = 2;

    initGraphicsItems();

    /**
     * QOpenGLWidget doesn't support partial updates
     * So zooming and scrolling is super slow........ lagging , lagging ...
     */
//    QOpenGLWidget *gl = new QOpenGLWidget();
//    QSurfaceFormat format;
//    format.setSamples(4);
//    format.setRenderableType(QSurfaceFormat::OpenGL);
//    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
//    gl->setFormat(format);
//    gl->setUpdateBehavior(QOpenGLWidget::PartialUpdate);
//    setViewport(gl);

//    setViewport(new QGLWidget());


    // Create graphics scene
    QGraphicsScene *scene = new QGraphicsScene(this);

    /**
     * QGraphicsScene::BspTreeIndex (DEFAULT) ==>> SUPER FAST TO MOVE NODE ON 100X100 AND 3 VISIBLE LAYERS, BUT MORE LAGGING ZOOMING AND SCROLLING
     * QGraphicsScene::NoIndex ==>> A BIT FAST ZOOMING AND SCROLLING, BUT SUPER SLOW TO MOVE NODE ON 100X100 AND 3 VISIBLE LAYERS
     */
    /*scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);*///QGraphicsScene::NoIndex);//     //// In case of Image Item, it makes wrong
//    scene->setMinimumRenderSize(1);
    setScene(scene);

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);//QGraphicsView::FullViewportUpdate);//MinimalViewportUpdate);//QGraphicsView::BoundingRectViewportUpdate);//
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHint(QPainter::Antialiasing, false);
    setTransformationAnchor(AnchorUnderMouse);
    setOptimizationFlag(QGraphicsView::DontSavePainterState);//DontAdjustForAntialiasing);//QGraphicsView::DontClipPainter);//

    setBackgroundBrush(QBrush(m_backgroundColor));
    // Create new workspace
    onNew();
    // Set initial aspect ratio
    scaleView(0.08);

//  If the view is flickering when you scrool the scene, disabling the scroolbars can fix it:
//    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
//    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

}

GraphicsView::~GraphicsView() {
    /**/
}

void GraphicsView::initLayerProperty()
{
    m_lstLayerData.clear();
    for(int i = 0 ; i < m_layerCount; i++){
        LayerData* layer = new LayerData();
        m_lstLayerData.push_back(layer);
    }

    m_lstLayerData.at(0)->setNodeColor(QColor(0, 102, 255));
    m_lstLayerData.at(1)->setNodeColor(QColor(153, 0, 255));
    m_lstLayerData.at(2)->setNodeColor(QColor(251, 135, 25));

    m_lstLayerData.at(0)->setEdgeColor(QColor(0, 102, 255));
    m_lstLayerData.at(1)->setEdgeColor(QColor(153, 0, 255));
    m_lstLayerData.at(2)->setEdgeColor(QColor(251, 135, 25));

    emit changedLayerColor(0, QColor(0, 102, 255));
    emit changedLayerColor(1, QColor(153, 0, 255));
    emit changedLayerColor(2, QColor(251, 135, 25));

    m_lstLayerData.at(0)->setLayerIndex(0);
    m_lstLayerData.at(1)->setLayerIndex(1);
    m_lstLayerData.at(2)->setLayerIndex(2);

    m_lstLayerData.at(0)->setAllVisible(true);
    m_lstLayerData.at(1)->setAllVisible(false);
    m_lstLayerData.at(2)->setAllVisible(false);

    emit changedLayerVisibility(0, true);
    emit changedLayerVisibility(1, false);
    emit changedLayerVisibility(2, false);

    m_lstLayerData.at(0)->setAllUnlock(true);
    m_lstLayerData.at(1)->setAllUnlock(true);
    m_lstLayerData.at(2)->setAllUnlock(true);

    emit changedLayerUnlockState(0, true);
    emit changedLayerUnlockState(1, true);
    emit changedLayerUnlockState(2, true);

    m_lstLayerData.at(0)->setActivated(true);
    m_lstLayerData.at(1)->setActivated(false);
    m_lstLayerData.at(2)->setActivated(false);

    emit changedActiveLayer(0);

    m_curLayerIndex = 0;
}

void GraphicsView::setCurrentLayerIndex(int index)
{
    m_curLayerIndex = index;

    for(int i = 0 ; i < m_lstLayerData.count(); i++){
        if(m_lstLayerData.at(i)){
            if(i == m_curLayerIndex){
                m_lstLayerData.at(i)->setActivated(true);
            }else{
                m_lstLayerData.at(i)->setActivated(false);
            }
        }
    }

    // SHOW/HIDE VOXEL
    if(m_cursorVoxel){
        m_cursorVoxel->updateVisibility();
        if(!m_cursorVoxel->isChangable())
            m_cursorVoxel->initPoints();
    }

    if(m_selectedVoxel){
        m_selectedVoxel->updateVisibility();
        if(!m_selectedVoxel->isChangable())
            m_selectedVoxel->initPoints();
    }

    // SHOW/HIDE SELECTED NODE
    if(m_selectedNode != nullptr) {
        if(!m_selectedNode->isMovable()){
            m_selectedNode->setSelectionState(false);
            m_selectedNode = nullptr;
        }
    }
}

void GraphicsView::showLayer(int layerIndex, int flg)
{
    if(layerIndex >= 0 && layerIndex < m_lstLayerData.count()){
        if(m_lstLayerData.at(layerIndex)){
            if(flg){
                if(!m_lstLayerData.at(layerIndex)->isAllVisible())
                    scene()->addItem(m_lstLayerData.at(layerIndex));
            }else {
                scene()->removeItem(m_lstLayerData.at(layerIndex));
            }
            m_lstLayerData.at(layerIndex)->setAllVisible(flg);
        }
    }

    // SHOW/HIDE marker like as connected node
    for (int i = 0; i < m_markList.size(); ++i) {
        m_markList[i]->setVisible(m_markList[i]->getNode()->isVisible());
    }

    // SHOW/HIDE VOXEL
    if(m_cursorVoxel){
        m_cursorVoxel->updateVisibility();
        if(!m_cursorVoxel->isChangable())
            m_cursorVoxel->initPoints();
    }

    if(m_selectedVoxel){
        m_selectedVoxel->updateVisibility();
        if(!m_selectedVoxel->isChangable())
            m_selectedVoxel->initPoints();
    }

    foreach(VoxelPolygon* v, m_voxelList){
        if(v){
            v->updateVisibility();
        }
    }
}

void GraphicsView::unlockLayer(int layerIndex, int flg)
{
    if(layerIndex >= 0 && layerIndex < m_lstLayerData.count()){
        if(m_lstLayerData.at(layerIndex)){
            m_lstLayerData.at(layerIndex)->setAllUnlock(flg);
        }
    }
}

void GraphicsView::setLayerColor(int layerIndex, QColor color)
{
    if(layerIndex >= 0 && layerIndex < m_lstLayerData.count()){
        if(m_lstLayerData.at(layerIndex)){
            m_lstLayerData.at(layerIndex)->setNodeColor(color);
            m_lstLayerData.at(layerIndex)->setEdgeColor(color);
        }
    }
}

void GraphicsView::initMarkerList() {
    for (int i = 0; i < m_markList.size(); ++i) {
        if(m_markList[i] != nullptr) {
            delete m_markList[i];
        }
    }
    m_markList.clear();
}

void GraphicsView::initGraphicsItems() {
    // Graphics Items
    foreach(LayerData* layer, m_lstLayerData){
        if(layer)
            layer->clear();
    }

    m_shapeList.clear();
    m_markList.clear();
    m_voxelList.clear();

    // Clear image
    onClearImage();

    m_selectedNode = nullptr;
    m_currentShape = nullptr;
    m_selectedShape = nullptr;
    m_reactorNodes.clear();
    m_bEditMode = false;

    if(m_cursorVoxel != nullptr){
        m_cursorVoxel->deleteLater();
        m_cursorVoxel = nullptr;
    }
    if(m_selectedVoxel != nullptr){
        m_selectedVoxel->deleteLater();
        m_selectedVoxel = nullptr;
    }
}

void GraphicsView::wheelEvent(QWheelEvent* event) {
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        return QGraphicsView::wheelEvent(event);
    } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        return QGraphicsView::wheelEvent(event);
    } else if (event->modifiers().testFlag(Qt::AltModifier)) {
        return QGraphicsView::wheelEvent(event);
    } else {
        // Do a wheel-based zoom about the cursor position
        double angle = event->angleDelta().y();
        double factor = pow(1.0015, angle);

//        /// Qt5 Syntax
//        auto targetViewportPos = event->pos();
//        auto targetScenePos = mapToScene(event->pos());
//        /// Qt6 Syntax
//        auto targetViewportPos = event->globalPosition();
//        auto targetScenePos = mapToScene(event->globalPosition().x(), event->globalPosition().y());

        //scale(factor, factor);
        scaleView(factor);
//        centerOn(targetScenePos);
//        QPointF deltaViewportPos = targetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
//        QPointF viewportCenter = mapFromScene(targetScenePos) - deltaViewportPos;
//        centerOn(mapToScene(viewportCenter.toPoint()));
    }
}

void GraphicsView::setShapeLineWidth(int number) {
    m_shapeLineWidth = number;
    if(m_selectedShape != nullptr) {
        m_selectedShape->setLineWidth(number);
    }
}

int GraphicsView::getShapeLineWidth() {
    return m_shapeLineWidth;
}

int GraphicsView::getPolygonNumber() {
    return m_polygonNumber;
}

void GraphicsView::setPolygonNumber(int number) {
    m_polygonNumber = number;
}

int GraphicsView::calculateX(int x) {
    int xValue = int((horizontalScrollBar()->value() - horizontalScrollBar()->minimum() + x) / m_scaleFactor - ((horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() + viewport()->width()) / m_scaleFactor) / 2);
    return qRound(xValue / 10.0) * 10;

}

int GraphicsView::calculateY(int y) {
    int yValue = int((verticalScrollBar()->value() - verticalScrollBar()->minimum() + y) / m_scaleFactor - ((verticalScrollBar()->maximum() - verticalScrollBar()->minimum() + viewport()->height()) / m_scaleFactor) / 2);
    return qRound(yValue / 10.0) * 10;
}

void GraphicsView::selectedMarker(MarkerItem* item) {
    m_bMarkerMoveMode = true;
    m_selectedMarker = item;
}

void GraphicsView::selectedNodeFromMarker(MarkerItem* item) {
    if(!item->getNode()->isMovable())
        return;

    if(m_selectedNode != nullptr) {
        m_selectedNode->setSelectionState(false);
    }

    m_selectedNode = item->getNode();
    m_selectedNode->setSelectionState(true);
    nodePressed(item->getNode());
    nodeChanged(item->getNode());
}

void GraphicsView::selectMarker(Node* node, int number) {
    if(node != nullptr && number > -1 && number < 10) {
        MarkerItem* item = new MarkerItem();
        item->setNode(node);
        item->setLabel(QString::number(number + 1));
        item->setNode(node);
        item->setPos(node->pos());
        item->setScaleFactor(m_scaleFactor);
        item->setBoundingRect(QRectF(0, 0, 10, 10));
        m_markList.push_back(item);
        QObject::connect(item, SIGNAL(selectedMarker(MarkerItem*)), this, SLOT(selectedMarker(MarkerItem*)));
        QObject::connect(item, SIGNAL(selectedNodeFromMarker(MarkerItem*)), this, SLOT(selectedNodeFromMarker(MarkerItem*)));
        scene()->addItem(item);
    } else if(number == -1) {
        for (int i = 0; i < m_markList.size(); ++i) {
            if(m_markList[i]->getNode() == node) {
               m_markList[i]->setNode(nullptr);
            }
        }
    }
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
    if(m_voxelSelectMode)
        return;

    if(event->button() == Qt::LeftButton) {
        if(m_bEditMode && m_currentShape != nullptr) {
            switch (m_currentShape->getType()) {
                case EDIT_MODE_POINT_HANDLE_CURVE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapePointHandleCurve*)m_currentShape)->removeLast();
                        ((ShapePointHandleCurve*)m_currentShape)->updateShapeLine();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                break;
                case EDIT_MODE_POINT_CURVE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapePointCurve*)m_currentShape)->removeLast();
                        ((ShapePointCurve*)m_currentShape)->updateShapeLine();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_HANDLE_CURVE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapeHandleCurve*)m_currentShape)->removeLast();
                        ((ShapeHandleCurve*)m_currentShape)->updateShapeLine();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if(m_voxelSelectMode)
        return;

    m_markerMoveStartPoint = event->pos();
    if (event->button() == Qt::LeftButton) {
        if(m_bEditMode && (m_imgItem == nullptr || !m_imgItem->isHovered() || getLockImage())) {
            if(m_selectedShape != nullptr) {
                m_bMoveShape = false;
                m_bMoveShape = m_selectedShape->setMoveType(QPointF(calculateX(event->x()), calculateY(event->y())));
                if(m_bMoveShape) {
                    m_moveStartPoint = event->pos();
                    return;
                }
                if(m_EditMode == EDIT_MODE_SELECT) {
                    QGraphicsItem* item;
                    item = itemAt(event->pos());
                    if(item != nullptr){
                        if(!item->flags().testFlag(QGraphicsItem::ItemIsMovable)){
                            m_panScrollMode = true;
                            m_panStartX = event->x();
                            m_panStartY = event->y();
                            setCursor(Qt::ClosedHandCursor);
                        }
                    }else {
                        m_panScrollMode = true;
                        m_panStartX = event->x();
                        m_panStartY = event->y();
                        setCursor(Qt::ClosedHandCursor);
                    }
                }
            } else {
                QList<QPoint> a;
                QGraphicsItem* item;
                switch (m_EditMode) {
                    case EDIT_MODE_SELECT:
                        item = itemAt(event->pos());
                        if(item != nullptr){
                            if(!item->flags().testFlag(QGraphicsItem::ItemIsMovable)){
                                m_panScrollMode = true;
                                m_panStartX = event->x();
                                m_panStartY = event->y();
                                setCursor(Qt::ClosedHandCursor);
                            }
                        }else {
                            m_panScrollMode = true;
                            m_panStartX = event->x();
                            m_panStartY = event->y();
                            setCursor(Qt::ClosedHandCursor);
                        }
                        break;
                    case EDIT_MODE_LINE:
                        m_currentShape = new ShapeLine(EDIT_MODE_LINE, m_shapeLineColor, m_shapeLineWidth);
                        m_currentShape->setSelectable(false);
                        m_currentShape->setIsDrawing(true);
                        m_currentShape->setScaleFactor(m_scaleFactor);
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        ((ShapeLine*)m_currentShape)->setData(a);
                        m_currentShape->addArrow(0);
                        m_currentShape->addArrow(1);
                        scene()->addItem(((ShapeLine*)m_currentShape));
                        m_shapeList.append(m_currentShape);
                        QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        break;
                    case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
                        m_currentShape = new ShapeCornerToCornerRectangle(EDIT_MODE_CORNER_CORNER_TO_REACTANGLE, m_shapeLineColor, m_shapeLineWidth);
                        m_currentShape->setIsDrawing(true);
                        m_currentShape->setSelectable(false);
                        m_currentShape->setScaleFactor(m_scaleFactor);
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        ((ShapeCornerToCornerRectangle*)m_currentShape)->setData(a);
                        m_currentShape->addArrow(0);
                        m_currentShape->addArrow(1);
                        scene()->addItem(((ShapeCornerToCornerRectangle*)m_currentShape));
                        m_shapeList.append(m_currentShape);
                        QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        break;
                    case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
                        m_currentShape = new ShapeCenterToCornerRectangle(EDIT_MODE_CENTER_TO_CORNER_REACTANGLE, m_shapeLineColor, m_shapeLineWidth);
                        m_currentShape->setIsDrawing(true);
                        m_currentShape->setSelectable(false);
                        m_currentShape->setScaleFactor(m_scaleFactor);
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        ((ShapeCenterToCornerRectangle*)m_currentShape)->setData(a);
                        m_currentShape->addArrow(0);
                        scene()->addItem(((ShapeCenterToCornerRectangle*)m_currentShape));
                        m_shapeList.append(m_currentShape);
                        QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        break;
                    case EDIT_MODE_CENTER_CIRCLE:
                        m_currentShape = new ShapeCenterPointCircle(EDIT_MODE_CENTER_CIRCLE, m_shapeLineColor, m_shapeLineWidth);
                        m_currentShape->setIsDrawing(true);
                        m_currentShape->setSelectable(false);
                        m_currentShape->setScaleFactor(m_scaleFactor);
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        ((ShapeCenterPointCircle*)m_currentShape)->setData(a);
                        m_currentShape->addArrow(0);
                        scene()->addItem(((ShapeCenterPointCircle*)m_currentShape));
                        m_shapeList.append(m_currentShape);
                        QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        break;
                    case EDIT_MODE_3_POINT_CIRCLE:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new Shape3PointCircle(EDIT_MODE_3_POINT_CIRCLE, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((Shape3PointCircle*)m_currentShape)->setData(a);
                            m_currentShape->addArrow(0);
                            m_currentShape->addArrow(1);
                            scene()->addItem(((Shape3PointCircle*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(false);
                            m_currentShape->removeArrows();
                            m_currentShape = nullptr;
                        }
                        break;
                    case EDIT_MODE_CENTER_OVAL:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new ShapeCenterOval(EDIT_MODE_CENTER_OVAL, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((ShapeCenterOval*)m_currentShape)->setData(a);
                            m_currentShape->addArrow(0);
                            m_currentShape->addArrow(1);
                            scene()->addItem(((ShapeCenterOval*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(false);
                            ((ShapeCenterOval*)m_currentShape)->updateShapeRectangle();
                            m_currentShape->removeArrows();
                            m_currentShape = nullptr;
                        }
                        break;
                    case EDIT_MODE_3_POINT_OVAL:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new Shape3PointOval(EDIT_MODE_3_POINT_OVAL, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((Shape3PointOval*)m_currentShape)->setData(a);
                            m_currentShape->addArrow(0);
                            m_currentShape->addArrow(1);
                            scene()->addItem(((Shape3PointOval*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(false);
                            ((Shape3PointOval*)m_currentShape)->updateShapeRectangle();
                            m_currentShape->removeArrows();
                            m_currentShape = nullptr;
                        }
                        break;
                    case EDIT_MODE_CENTER_ARC:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new ShapeCenterArc(EDIT_MODE_CENTER_ARC, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((ShapeCenterArc*)m_currentShape)->setData(a);
                            m_currentShape->addArrow(0);
                            m_currentShape->addArrow(1);
                            scene()->addItem(((ShapeCenterArc*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(false);
                            ((ShapeCenterArc*)m_currentShape)->updateShapeRectangle();
                            m_currentShape->removeArrows();
                            m_currentShape = nullptr;
                        }
                        break;
                    case EDIT_MODE_3_POINT_ARC:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new Shape3PointArc(EDIT_MODE_3_POINT_ARC, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((Shape3PointArc*)m_currentShape)->setData(a);
                            m_currentShape->addArrow(0);
                            m_currentShape->addArrow(1);
                            scene()->addItem(((Shape3PointArc*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(false);
                            ((Shape3PointArc*)m_currentShape)->updateShapeRectangle();
                            m_currentShape->removeArrows();
                            m_currentShape = nullptr;
                        }
                        break;
                    case EDIT_MODE_POLYGON:
                        m_currentShape = new ShapePolygon(EDIT_MODE_POLYGON, m_polygonNumber, m_shapeLineColor, m_shapeLineWidth);
                        m_currentShape->setIsDrawing(true);
                        m_currentShape->setSelectable(false);
                        m_currentShape->setScaleFactor(m_scaleFactor);
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        a.push_back(QPoint(calculateX(event->x()), calculateY(event->y())));
                        ((ShapePolygon*)m_currentShape)->setData(a);
                        m_currentShape->addArrow(0);
                        m_currentShape->addArrow(1);
                        scene()->addItem(((ShapePolygon*)m_currentShape));
                        m_shapeList.append(m_currentShape);
                        QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        break;
                    case EDIT_MODE_HANDLE_CURVE:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new ShapeHandleCurve(EDIT_MODE_HANDLE_CURVE, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapeHandleCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapeHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                            scene()->addItem(((ShapeHandleCurve*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setSelectable(false);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapeHandleCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapeHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_POINT_CURVE:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new ShapePointCurve(EDIT_MODE_POINT_CURVE, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapePointCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapePointCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                            scene()->addItem(((ShapePointCurve*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapePointCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapePointCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_POINT_HANDLE_CURVE:
                        if(m_currentShape == nullptr) {
                            m_currentShape = new ShapePointHandleCurve(EDIT_MODE_POINT_HANDLE_CURVE, m_shapeLineColor, m_shapeLineWidth);
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapePointHandleCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapePointHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                            scene()->addItem(((ShapePointHandleCurve*)m_currentShape));
                            m_shapeList.append(m_currentShape);
                            QObject::connect(m_currentShape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        } else {
                            m_currentShape->setIsDrawing(true);
                            m_currentShape->setSelectable(false);
                            m_currentShape->setScaleFactor(m_scaleFactor);
                            ((ShapePointHandleCurve*)m_currentShape)->setIsAddabble(true);
                            ((ShapePointHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    default:
                        break;
                }
            }
        } else {
            if(m_bEditMode){
                if(m_EditMode == EDIT_MODE_SELECT && m_imgItem != nullptr && (m_bImageMoveMode || m_imgItem->isHovered())) {
                    QGraphicsView::mousePressEvent(event);
                    return;
                }
            }

            QGraphicsItem* item = nullptr;
            QList<QGraphicsItem *> selected_items = items(event->pos());
            foreach(QGraphicsItem* _item, selected_items){
                if(_item->flags().testFlag(QGraphicsItem::ItemIsMovable)){
                    item = _item;
                    break;
                }
            }

            if(item == nullptr){
                m_panScrollMode = true;
                m_panStartX = event->x();
                m_panStartY = event->y();
                setCursor(Qt::ClosedHandCursor);
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if(m_voxelSelectMode){
        if(m_cursorVoxel != nullptr){
            if(m_cursorVoxel->polygon().containsPoint(QPoint(calculateX(event->pos().x()), calculateY(event->pos().y())), Qt::OddEvenFill)){
                m_selectedVoxel->setNodesFromVoxelPolygon(m_cursorVoxel);
            }
        }

        if(m_reactorMode)
            setReactorNodesFromSelectedVoxel();
        return;
    }

    m_bMarkerMoveMode = false;
    if(m_bEditMode) {
        if(m_bSnapShape) {
            m_bSnapShape = false;
            if(m_snapShapeObject != nullptr) {
                m_snapShapeObject->setSelectable(true);
                m_snapShapeObject = nullptr;
            }
        }
        if(m_bMoveShape) {
            m_selectedShape->removeMoveType();
            m_bMoveShape = false;
        } else {
            switch (m_EditMode) {
                case EDIT_MODE_SELECT:
                    break;
                case EDIT_MODE_LINE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapeLine*)m_currentShape)->updateShapeLine();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapeCornerToCornerRectangle*)m_currentShape)->updateShapeRectangle();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapeCenterToCornerRectangle*)m_currentShape)->updateShapeRectangle();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_CENTER_CIRCLE:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        ((ShapeCenterPointCircle*)m_currentShape)->updateShapeRectangle();
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_3_POINT_CIRCLE:
                    if(m_currentShape != nullptr && m_currentShape->getType() == EDIT_MODE_3_POINT_CIRCLE) {
                        if(((Shape3PointCircle*)m_currentShape)->getTwoCircleMode()) {
                            ((Shape3PointCircle*)m_currentShape)->setTwoCircleMode(false);
                            ((Shape3PointCircle*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((Shape3PointCircle*)m_currentShape)->addArrow(2);
                        }
                    }
                    break;
                case EDIT_MODE_CENTER_OVAL:
                    if(m_currentShape != nullptr && m_currentShape->getType() == EDIT_MODE_CENTER_OVAL) {
                        if(((ShapeCenterOval*)m_currentShape)->getTwoOvalMode()) {
                            ((ShapeCenterOval*)m_currentShape)->setTwoOvalMode(false);
                            ((ShapeCenterOval*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((ShapeCenterOval*)m_currentShape)->addArrow(2);
                        }
                    }
                    break;
                case EDIT_MODE_3_POINT_OVAL:
                    if(m_currentShape != nullptr && m_currentShape->getType() == EDIT_MODE_3_POINT_OVAL) {
                        if(((Shape3PointOval*)m_currentShape)->getTwoOvalMode()) {
                            ((Shape3PointOval*)m_currentShape)->setTwoOvalMode(false);
                            ((Shape3PointOval*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                    }
                    break;
                case EDIT_MODE_CENTER_ARC:
                    if(m_currentShape != nullptr && m_currentShape->getType() == EDIT_MODE_CENTER_ARC) {
                        if(((ShapeCenterArc*)m_currentShape)->getTwoPointMode()) {
                            ((ShapeCenterArc*)m_currentShape)->setTwoPointMode(false);
                            m_currentShape->addArrow(2);
                            ((ShapeCenterArc*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                    }
                    break;
                case EDIT_MODE_3_POINT_ARC:
                    if(m_currentShape != nullptr && m_currentShape->getType() == EDIT_MODE_3_POINT_ARC) {
                        if(((Shape3PointArc*)m_currentShape)->getTwoCircleMode()) {
                            ((Shape3PointArc*)m_currentShape)->setTwoCircleMode(false);
                            ((Shape3PointArc*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                            ((Shape3PointArc*)m_currentShape)->addArrow(2);
                        }
                    }
                    break;
                case EDIT_MODE_POLYGON:
                    if(m_currentShape != nullptr) {
                        m_currentShape->setIsDrawing(false);
                        m_currentShape->removeArrows();
                        m_currentShape = nullptr;
                    }
                    break;
                case EDIT_MODE_HANDLE_CURVE:
                    break;
                case EDIT_MODE_POINT_CURVE:
                    break;
                case EDIT_MODE_POINT_HANDLE_CURVE:
                    break;
                default:
                    break;
            }
        }
    }

    QGraphicsItem* item = itemAt(event->pos());
    if(item != nullptr){
        if(m_selectedNode != nullptr){
            if(m_selectedNode->isReactor()){
                if(m_reactorNodes.length() == 4) {
                    QPoint pt1 = m_reactorNodes.at(0)->getLocalPos();
                    QPoint pt2 = m_reactorNodes.at(1)->getLocalPos();
                    QPoint pt3 = m_reactorNodes.at(2)->getLocalPos();
                    QPoint pt4 = m_reactorNodes.at(3)->getLocalPos();
                    QString reactor_position = "\"" + QString::number(pt1.x()) + "," + QString::number(-pt1.y())  + ";";
                    reactor_position = reactor_position + QString::number(pt2.x()) + "," + QString::number(-pt2.y()) + ";";
                    reactor_position = reactor_position + QString::number(pt3.x()) + "," + QString::number(-pt3.y()) + ";";
                    reactor_position = reactor_position + QString::number(pt4.x()) + "," + QString::number(-pt4.y()) + "\"";

                    QClipboard *p_Clipboard = QApplication::clipboard();
                    p_Clipboard->setText(reactor_position);
                }
            }
        }
    }

    if (event->button() == Qt::LeftButton && m_panScrollMode) {
        m_panScrollMode = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

float GraphicsView::rotateF(QPointF centerPos, QPointF startPos, QPointF endPos) {
    float angle1 = qAtan2(startPos.y() - centerPos.y(), startPos.x() - centerPos.x());
    float angle2 = qAtan2(endPos.y() - centerPos.y(), endPos.x() - centerPos.x());
    return angle2 - angle1;
}

Node* GraphicsView::getNode(LayerData *layer, int xIndex, int yIndex)
{
    if(!layer)
        return nullptr;

    if( xIndex >= 0 && xIndex <= m_gridSize &&
            yIndex >= 0 && yIndex <= m_gridSize){
        int index = xIndex * (m_gridSize + 1) + yIndex;
        return getNode(layer, index);

    }
    return nullptr;
}

Node* GraphicsView::getNode(LayerData *layer, int index)
{
    if(!layer)
        return nullptr;

    if( index >= 0 && index <= (m_gridSize + 1)*(m_gridSize + 1)){
        if(index >= 0 && index < layer->nodeCount()){
            return layer->nodeAt(index);
        }
    }
    return nullptr;
}

Node* GraphicsView::getNode(int xIndex, int yIndex){
    if( xIndex >= 0 && xIndex <= m_gridSize &&
            yIndex >= 0 && yIndex <= m_gridSize){
        int index = xIndex * (m_gridSize + 1) + yIndex;
        return getNode(index);

    }
    return nullptr;
}

Node* GraphicsView::getNode(int index){
    /// Find a node in the current layer
    if(m_curLayerIndex >= 0 && m_curLayerIndex < m_lstLayerData.count()){
        if(m_lstLayerData.at(m_curLayerIndex)){
            if( index >= 0 && index <= (m_gridSize + 1)*(m_gridSize + 1)){
                if(index >= 0 && index < m_lstLayerData.at(m_curLayerIndex)->nodeCount()){
                    return m_lstLayerData.at(m_curLayerIndex)->nodeAt(index);
                }
            }
        }
    }
    return nullptr;
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if(m_voxelSelectMode){
        if(m_cursorVoxel == nullptr)
            return;

        int xIndex = (calculateX(event->pos().x()) + m_gridSize * m_spacingSize / 2) / m_spacingSize;
        int yIndex = (calculateY(event->pos().y()) + m_gridSize * m_spacingSize / 2) / m_spacingSize;

        for(int x = xIndex - 2; x <= xIndex + 3; x++){
            for(int y = yIndex - 2; y <= yIndex + 3; y++){
                Node* node1 = getNode(x, y);
                Node* node2 = getNode(x + 1, y);
                Node* node3 = getNode(x + 1, y + 1);
                Node* node4 = getNode(x, y + 1);
                if(node1 != nullptr && node1->isVisible() &&
                        node2 != nullptr && node2->isVisible() &&
                        node3 != nullptr && node3->isVisible() &&
                        node4 != nullptr && node4->isVisible()){
                    QPolygon poly(QPolygon(QVector<QPoint>() << node1->pos().toPoint()
                                           << node2->pos().toPoint()
                                           << node3->pos().toPoint()
                                           << node4->pos().toPoint()));
                    if(poly.containsPoint(QPoint(calculateX(event->pos().x()), calculateY(event->pos().y())), Qt::OddEvenFill)){
                        if(!m_cursorVoxel->hasNodes(node1, node2, node3, node4)){
                            m_cursorVoxel->setNodes(node1, node2, node3, node4);
                            return;
                        }
                    }
                }
            }
        }
        return;
    }

    if(m_bImageRotateMode && m_imgItem != nullptr && m_EditMode == EDIT_MODE_SELECT) {
        if(m_imageMovingPrevPoint == QPointF(0, 0)) {
            m_imageMovingPrevPoint = QPointF(calculateX(event->pos().x()), calculateY(event->pos().y()));
        } else {
            QPointF point = QPointF(calculateX(event->pos().x()), calculateY(event->pos().y()));
            float alpha = rotateF(m_imgItem->pos(), m_imageMovingPrevPoint, point);
            m_imgItem->setRotation(m_imgItem->rotation() + qRadiansToDegrees(alpha));
            m_imageMovingPrevPoint = point;
        }
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    if(m_bMarkerMoveMode) {
        if(m_selectedMarker != nullptr) {
            m_selectedMarker->setPosition(m_selectedMarker->getPosition() + (event->pos() - m_markerMoveStartPoint) / m_scaleFactor);
            m_markerMoveStartPoint = event->pos();
        }
    } else {
        if (m_panScrollMode) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - m_panStartX));
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - m_panStartY));
            m_panStartX = event->x();
            m_panStartY = event->y();
        }

        if(m_bEditMode) {
            if(m_selectedShape != nullptr && event->buttons() != Qt::LeftButton && event->buttons() != Qt::RightButton) {
                m_selectedShape->setMoveType(QPointF(calculateX(event->x()), calculateY(event->y())));
            }
            if(m_bSnapShape && m_selectedShape != nullptr && m_selectedShape->getMoveType() != MOVE_MODE_NONE && event->buttons() == Qt::LeftButton) {
                QList<QPoint> a;
                switch (m_selectedShape->getType()) {
                case EDIT_MODE_LINE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeLine(EDIT_MODE_LINE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeLine*)m_selectedShape)->getStartPos());
                        a.push_back(((ShapeLine*)m_selectedShape)->getEndPos());
                        ((ShapeLine*)m_snapShapeObject)->setData(a);
                        scene()->addItem(((ShapeLine*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeCornerToCornerRectangle(EDIT_MODE_CORNER_CORNER_TO_REACTANGLE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeCornerToCornerRectangle*)m_selectedShape)->getStartPos());
                        a.push_back(((ShapeCornerToCornerRectangle*)m_selectedShape)->getEndPos());
                        ((ShapeCornerToCornerRectangle*)m_snapShapeObject)->setData(a);
                        scene()->addItem(((ShapeCornerToCornerRectangle*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeCenterToCornerRectangle(EDIT_MODE_CENTER_TO_CORNER_REACTANGLE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeCenterToCornerRectangle*)m_selectedShape)->getStartPos());
                        a.push_back(((ShapeCenterToCornerRectangle*)m_selectedShape)->getEndPos());
                        ((ShapeCenterToCornerRectangle*)m_snapShapeObject)->setData(a);
                        scene()->addItem(((ShapeCenterToCornerRectangle*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_CENTER_CIRCLE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeCenterPointCircle(EDIT_MODE_CENTER_CIRCLE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeCenterPointCircle*)m_selectedShape)->getStartPos());
                        a.push_back(((ShapeCenterPointCircle*)m_selectedShape)->getEndPos());
                        ((ShapeCenterPointCircle*)m_snapShapeObject)->setData(a);
                        scene()->addItem(((ShapeCenterPointCircle*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_3_POINT_CIRCLE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new Shape3PointCircle(EDIT_MODE_3_POINT_CIRCLE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((Shape3PointCircle*)m_selectedShape)->getFirstPoint());
                        a.push_back(((Shape3PointCircle*)m_selectedShape)->getSecondPoint());
                        ((Shape3PointCircle*)m_snapShapeObject)->setData(a);
                        ((Shape3PointCircle*)m_snapShapeObject)->setEndPos(((Shape3PointCircle*)m_selectedShape)->getThirdPoint());
                        ((Shape3PointCircle*)m_snapShapeObject)->setTwoCircleMode(false);
                        scene()->addItem(((Shape3PointCircle*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->addArrow(3);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_CENTER_OVAL:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeCenterOval(EDIT_MODE_CENTER_OVAL, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeCenterOval*)m_selectedShape)->getFirstPoint());
                        a.push_back(((ShapeCenterOval*)m_selectedShape)->getSecondPoint());
                        ((ShapeCenterOval*)m_snapShapeObject)->setData(a);
                        ((ShapeCenterOval*)m_snapShapeObject)->setEndPos(((ShapeCenterOval*)m_selectedShape)->getThirdPoint());
                        ((ShapeCenterOval*)m_snapShapeObject)->setTwoOvalMode(false);
                        scene()->addItem(((ShapeCenterOval*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_3_POINT_OVAL:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new Shape3PointOval(EDIT_MODE_3_POINT_OVAL, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((Shape3PointOval*)m_selectedShape)->getFirstPoint());
                        a.push_back(((Shape3PointOval*)m_selectedShape)->getSecondPoint());
                        ((Shape3PointOval*)m_snapShapeObject)->setData(a);
                        ((Shape3PointOval*)m_snapShapeObject)->setEndPos(((Shape3PointOval*)m_selectedShape)->getThirdPoint());
                        ((Shape3PointOval*)m_snapShapeObject)->setTwoOvalMode(false);
                        scene()->addItem(((Shape3PointOval*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->addArrow(3);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_CENTER_ARC:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeCenterArc(EDIT_MODE_CENTER_ARC, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapeCenterArc*)m_selectedShape)->getFirstPoint());
                        a.push_back(((ShapeCenterArc*)m_selectedShape)->getSecondPoint());
                        ((ShapeCenterArc*)m_snapShapeObject)->setData(a);
                        ((ShapeCenterArc*)m_snapShapeObject)->setEndPos(((ShapeCenterArc*)m_selectedShape)->getThirdPoint());
                        ((ShapeCenterArc*)m_snapShapeObject)->setTwoPointMode(false);
                        scene()->addItem(((ShapeCenterArc*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_3_POINT_ARC:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new Shape3PointArc(EDIT_MODE_3_POINT_ARC, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((Shape3PointArc*)m_selectedShape)->getFirstPoint());
                        a.push_back(((Shape3PointArc*)m_selectedShape)->getSecondPoint());
                        ((Shape3PointArc*)m_snapShapeObject)->setData(a);
                        ((Shape3PointArc*)m_snapShapeObject)->setEndPos(((Shape3PointArc*)m_selectedShape)->getThirdPoint());
                        ((Shape3PointArc*)m_snapShapeObject)->setTwoCircleMode(false);
                        scene()->addItem(((Shape3PointArc*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->addArrow(2);
                        m_snapShapeObject->addArrow(3);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_POLYGON:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapePolygon(EDIT_MODE_POLYGON, ((ShapePolygon*)m_selectedShape)->getShapeNumber(), m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        a.push_back(((ShapePolygon*)m_selectedShape)->getStartPos());
                        a.push_back(((ShapePolygon*)m_selectedShape)->getEndPos());
                        ((ShapePolygon*)m_snapShapeObject)->setData(a);
                        scene()->addItem(((ShapePolygon*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(0);
                        m_snapShapeObject->addArrow(1);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_HANDLE_CURVE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapeHandleCurve(EDIT_MODE_HANDLE_CURVE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        ((ShapeHandleCurve*)m_snapShapeObject)->setPoints(((ShapeHandleCurve*)m_selectedShape)->getPoints());
                        scene()->addItem(((ShapeHandleCurve*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(-1);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_POINT_CURVE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapePointCurve(EDIT_MODE_POINT_CURVE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        ((ShapePointCurve*)m_snapShapeObject)->setPoints(((ShapePointCurve*)m_selectedShape)->getPoints());
                        scene()->addItem(((ShapePointCurve*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(-1);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                case EDIT_MODE_POINT_HANDLE_CURVE:
                    if(m_snapShapeObject == nullptr) {
                        m_snapShapeObject = new ShapePointHandleCurve(EDIT_MODE_POINT_HANDLE_CURVE, m_selectedShape->getShapeColor(), m_selectedShape->getShapeLineWidth());
                        m_snapShapeObject->setnMoveType(m_selectedShape->getMoveType());
                        m_snapShapeObject->setSelectedArrow(m_selectedShape->getSelectedArrow());
                        m_snapShapeObject->setIsDrawing(false);
                        m_snapShapeObject->setSelectable(false);
                        m_snapShapeObject->setScaleFactor(m_scaleFactor);
                        m_snapShapeObject->setAngle(m_selectedShape->getAngle());
                        ((ShapePointHandleCurve*)m_snapShapeObject)->setPoints(((ShapePointHandleCurve*)m_selectedShape)->getPoints());
                        scene()->addItem(((ShapePointHandleCurve*)m_snapShapeObject));
                        m_shapeList.append(m_snapShapeObject);
                        QObject::connect(m_snapShapeObject, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                        m_snapShapeObject->addArrow(-1);
                        m_snapShapeObject->removeArrows();
                        m_moveStartPoint = event->pos();
                    } else {
                        m_snapShapeObject->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                        m_moveStartPoint = event->pos();
                    }
                    break;
                default:
                    break;
                }
            } else if(m_bMoveShape) {
                m_selectedShape->move(QPointF( calculateX(m_moveStartPoint.x()), calculateY(m_moveStartPoint.y())), QPointF(calculateX(event->x()), calculateY(event->y())));
                m_moveStartPoint = event->pos();
            } else {
                QList<QPoint> a;
                switch (m_EditMode) {
                    case EDIT_MODE_SELECT:
                        break;
                    case EDIT_MODE_LINE:
                        if(m_currentShape != nullptr) {
                            ((ShapeLine*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
                        if(m_currentShape != nullptr) {
                            ((ShapeCornerToCornerRectangle*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                            m_currentShape->addArrow(2);
                        }
                        break;
                    case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
                        if(m_currentShape != nullptr) {
                            ((ShapeCenterToCornerRectangle*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_CENTER_CIRCLE:
                        if(m_currentShape != nullptr) {
                            ((ShapeCenterPointCircle*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_3_POINT_CIRCLE:
                        if(m_currentShape != nullptr) {
                            if(m_currentShape->getType() == EDIT_MODE_3_POINT_CIRCLE) {
                                if(((Shape3PointCircle*)m_currentShape)->getTwoCircleMode()) {
                                    ((Shape3PointCircle*)m_currentShape)->setSecondPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                } else {
                                    ((Shape3PointCircle*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                    ((Shape3PointCircle*)m_currentShape)->addArrow(2);
                                }
                            }
                        }
                        break;
                    case EDIT_MODE_CENTER_OVAL:
                        if(m_currentShape != nullptr) {
                            if(m_currentShape->getType() == EDIT_MODE_CENTER_OVAL) {
                                if(((ShapeCenterOval*)m_currentShape)->getTwoOvalMode()) {
                                    ((ShapeCenterOval*)m_currentShape)->setSecondPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                } else {
                                    ((ShapeCenterOval*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                }
                            }
                        }
                        break;
                    case EDIT_MODE_3_POINT_OVAL:
                        if(m_currentShape != nullptr) {
                            if(m_currentShape->getType() == EDIT_MODE_3_POINT_OVAL) {
                                if(((Shape3PointOval*)m_currentShape)->getTwoOvalMode()) {
                                    ((Shape3PointOval*)m_currentShape)->setSecondPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                } else {
                                    ((Shape3PointOval*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                    m_currentShape->addArrow(2);
                                }
                            }
                        }
                        break;
                    case EDIT_MODE_CENTER_ARC:
                        if(m_currentShape != nullptr) {
                            if(m_currentShape->getType() == EDIT_MODE_CENTER_ARC) {
                                if(((ShapeCenterArc*)m_currentShape)->getTwoPointMode()) {
                                    ((ShapeCenterArc*)m_currentShape)->setSecondPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                } else {
                                    ((ShapeCenterArc*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                }
                            }
                        }
                        break;
                    case EDIT_MODE_3_POINT_ARC:
                        if(m_currentShape != nullptr) {
                            if(m_currentShape->getType() == EDIT_MODE_3_POINT_ARC) {
                                if(((Shape3PointArc*)m_currentShape)->getTwoCircleMode()) {
                                    ((Shape3PointArc*)m_currentShape)->setSecondPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                } else {
                                    ((Shape3PointArc*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                                    ((Shape3PointArc*)m_currentShape)->addArrow(2);
                                }
                            }
                        }
                        break;
                    case EDIT_MODE_POLYGON:
                        if(m_currentShape != nullptr) {
                            ((ShapePolygon*)m_currentShape)->setEndPos(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_HANDLE_CURVE:
                        if(m_currentShape != nullptr) {
                            ((ShapeHandleCurve*)m_currentShape)->setIsAddabble(false);
                            ((ShapeHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_POINT_CURVE:
                        if(m_currentShape != nullptr) {
                            ((ShapePointCurve*)m_currentShape)->setIsAddabble(false);
                            ((ShapePointCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    case EDIT_MODE_POINT_HANDLE_CURVE:
                        if(m_currentShape != nullptr) {
                            ((ShapePointHandleCurve*)m_currentShape)->setIsAddabble(false);
                            ((ShapePointHandleCurve*)m_currentShape)->addPoint(QPoint(calculateX(event->x()), calculateY(event->y())));
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
    QRectF sceneRect = this->sceneRect();

    QRect viewport_rect(0, 0, viewport()->width(), viewport()->height());
    QRectF visible_scene_rect = mapToScene(viewport_rect).boundingRect();

    setBackgroundBrush(QBrush(m_backgroundColor));
//    painter->fillRect(visible_scene_rect,QBrush(m_backgroundColor));

    if(m_scaleFactor > 0.4f) {
        QPen pen(Qt::lightGray);
//        pen.setStyle(Qt::DashDotDotLine);
        painter->setPen(pen);

        for (int x=0; x<=sceneRect.right(); x += m_spacingUnit){
            if(x == 0){
                if(x > visible_scene_rect.left() && x < visible_scene_rect.right())
                    painter->drawLine(x, visible_scene_rect.top(), x, visible_scene_rect.bottom());
            }else{
                if(x > visible_scene_rect.left() && x < visible_scene_rect.right())
                    painter->drawLine(x, visible_scene_rect.top(), x, visible_scene_rect.bottom());
                if(-x > visible_scene_rect.left() && -x < visible_scene_rect.right())
                    painter->drawLine(-x, visible_scene_rect.top(), -x, visible_scene_rect.bottom());
            }
        }

        for (int y=0; y<=sceneRect.bottom(); y += m_spacingUnit){
            if(y == 0){
                if(y > visible_scene_rect.top() && y < visible_scene_rect.bottom())
                    painter->drawLine(visible_scene_rect.left(), y, visible_scene_rect.right(), y);
            }else{
                if(y > visible_scene_rect.top() && y < visible_scene_rect.bottom())
                    painter->drawLine(visible_scene_rect.left(), y, visible_scene_rect.right(), y);
                if(-y > visible_scene_rect.top() && -y < visible_scene_rect.bottom())
                    painter->drawLine(visible_scene_rect.left(), -y, visible_scene_rect.right(), -y);
            }
        }
    }
}

void GraphicsView::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.025 || factor > 4)
        return;

    m_scaleFactor = static_cast<float>(factor);

    // Scale graphics items
    foreach(LayerData* layer, m_lstLayerData){
        if(layer){
            for(int i = 0 ; i < layer->nodeCount(); i++){
                layer->nodeAt(i)->setScaleFactor(m_scaleFactor);
            }
            for(int i = 0 ; i < layer->edgeCount(); i++){
                layer->edgeAt(i)->setScaleFactor(m_scaleFactor);
            }
        }
    }

    if(m_imgItem != nullptr)
        m_imgItem->setScaleFactor(m_scaleFactor);
    for(int i = 0 ; i < m_shapeList.count(); i++){
        m_shapeList.at(i)->setScaleFactor(m_scaleFactor);
        if(m_currentShape != nullptr) {
            m_currentShape->setScaleFactor(m_scaleFactor);
        }
    }
    for (int i = 0; i < m_markList.size(); ++i) {
        if(m_markList[i] != nullptr) {
            m_markList[i]->setScaleFactor(m_scaleFactor);
        }
    }
    scale(scaleFactor, scaleFactor);
}

float GraphicsView::getScaleFactor() {
    return m_scaleFactor;
}

void GraphicsView::startSnapObject() {
    if(!m_bEditMode || m_selectedShape == nullptr) {
        return;
    }
    m_bSnapShape = true;
}

void GraphicsView::stopSnapObject() {
    m_bSnapShape = false;
    if(m_snapShapeObject != nullptr) {
        m_snapShapeObject->setSelectable(true);
        m_snapShapeObject = nullptr;
    }
}

bool GraphicsView::toggleEditMode() {
    m_bImageRotateMode = false;
    m_bImageMoveMode = false;
    m_bEditMode = ! m_bEditMode;

    if(m_imgItem != nullptr) {
        m_imgItem->lockImage(!m_bEditMode || (m_bEditMode && m_imageLocked));
    }

    foreach(LayerData* layer, m_lstLayerData){
        if(layer){
            for(int i = 0 ; i < layer->nodeCount(); i++){
                layer->nodeAt(i)->setMovable(layer->isAllUnlock() &&
                                                    layer->isActivated() &&
                                                    !m_bEditMode);
            }
        }
    }

    if(!m_bEditMode) {
        if(m_currentShape != nullptr) {
            m_currentShape->removeArrows();
            m_currentShape->setSelectable(m_bEditMode);
            m_currentShape = nullptr;
        }
        if(m_selectedShape != nullptr) {
            m_selectedShape->removeArrows();
            m_selectedShape->setSelectable(m_bEditMode);
            m_selectedShape = nullptr;
        }
    } else {
        if(m_selectedNode != nullptr) {
            m_selectedNode->setSelectionState(false);
            m_selectedNode = nullptr;
        }
    }
    for(int i = 0 ; i < m_shapeList.count(); i++){
        m_shapeList.at(i)->setSelectable(m_bEditMode);
    }
    return m_bEditMode;
}

int GraphicsView::getGridSpacingSize() {
    return m_spacingSize;
}

bool GraphicsView::isEditMode() {
    updateEditMode(m_EditMode);
    return m_bEditMode;
}

QColor GraphicsView::getShapeLineColor() {
    return m_shapeLineColor;
}

int GraphicsView::getEditMode() {
    return m_EditMode;
}

int GraphicsView::updateEditMode(int editMode) {
    m_EditMode = editMode;
    m_bImageRotateMode = false;
    m_bImageMoveMode = false;
    if(m_bEditMode && m_imgItem != nullptr) {
        if(m_EditMode == EDIT_MODE_SELECT) {
            m_imageLocked = m_imageLockedTemp;
        } else {
            m_imageLockedTemp = m_imageLocked;
            m_imageLocked = true;
        }
        if(m_imgItem != nullptr)
            m_imgItem->lockImage(m_imageLocked);
    }
    for (int i = 0; i < m_shapeList.count(); i ++) {
        m_shapeList.at(i)->setSelectable(m_EditMode == EDIT_MODE_SELECT);
    }
    if(m_EditMode != EDIT_MODE_SELECT) {
        if(m_currentShape != nullptr) {
            m_currentShape->removeArrows();
            m_currentShape->setSelectable(m_EditMode == EDIT_MODE_SELECT);
        }
    }
    if(m_selectedShape != nullptr) {
        m_selectedShape->removeArrows();
    }
    m_selectedShape = nullptr;
    if(m_currentShape != nullptr) {
        m_shapeList.removeOne(m_currentShape);
        delete m_currentShape;
        m_currentShape = nullptr;
    }
    return m_EditMode;
}

void GraphicsView::onNew(int gridSize, int spacingSize){
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    // Set properties
    m_gridSize = gridSize;
    m_spacingSize = spacingSize * m_spacingUnit;

    // Initialize all graphics variables
    initGraphicsItems();
    // Clear graphcis scene (Removed all graphics items)
    scene()->clear();
    scene()->setSceneRect(-(m_gridSize + 4) * m_spacingSize / 2, -(m_gridSize + 4) * m_spacingSize / 2, (m_gridSize + 4) * m_spacingSize, (m_gridSize + 4) * m_spacingSize);

    initLayerProperty();

    // Setup node data
    for(int i = 0 ; i < m_lstLayerData.count(); i++){
        if(!m_lstLayerData.at(i)){
            continue;
        }

        int nodeIndex = 0;
        for (int x = -m_gridSize/ 2; x <= m_gridSize/ 2; x ++){
            for (int y = -m_gridSize/ 2; y <= m_gridSize/ 2; y ++){
                Node* node = new Node();
                node->setId(nodeIndex);
                int x_pos = x * m_spacingSize;
                int y_pos = y * m_spacingSize;
                node->setLayerIndex(i);
                node->setPos(x_pos,y_pos);
                node->setColor(m_lstLayerData.at(i)->getNodeColor());
                node->setCentralPos(QPoint(x_pos, y_pos));
                node->setSpacingSize(m_spacingSize);
                node->setMovable(m_lstLayerData.at(i)->isAllUnlock() &&
                                  m_lstLayerData.at(i)->isActivated());

                m_lstLayerData.at(i)->addNode(node);
                QObject::connect(node, SIGNAL(nodeCoordinate(Node*, QPoint)), this, SLOT(nodeCoordinate(Node*, QPoint)));
                QObject::connect(node, SIGNAL(nodePressed(Node*)), this, SLOT(nodePressed(Node*)));
                QObject::connect(node, SIGNAL(nodeChanged(Node*)), this, SLOT(nodeChanged(Node*)));
                QObject::connect(node, SIGNAL(markersShow(Node*, QPoint)), this, SLOT(markersShow(Node*, QPoint)));
                nodeIndex ++;
            }
        }
    }

    emit changedActiveLayer(m_curLayerIndex);
    setCurrentLayerIndex(m_curLayerIndex);

    // Add Voxel Items to select a voxel
    if(m_cursorVoxel == nullptr)
        m_cursorVoxel = new VoxelPolygon();
    m_cursorVoxel->setPolygon(QPolygonF(QVector<QPointF>() << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)));
    m_cursorVoxel->setColor(Qt::white);
    m_cursorVoxel->setOpacity(0.3);
    m_cursorVoxel->setZValue(2);

    if(m_selectedVoxel == nullptr)
        m_selectedVoxel = new VoxelPolygon();
    m_selectedVoxel->setSelectionState();
    m_selectedVoxel->setColor(Qt::transparent);
    m_selectedVoxel->setZValue(2);

    m_selectedVoxel->setPolygon(QPolygonF(QVector<QPointF>() << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)
                                        << QPointF(0.0, 0.0)));
    scene()->addItem(m_selectedVoxel);
    scene()->addItem(m_cursorVoxel);
    ////// //////
    drawGraphicsItems();
    m_historiesBefore.clear();
    m_historiesAfter.clear();
    initMarkerList();

    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
}

void GraphicsView::onResetPoint()
{
    if(m_selectedNode != nullptr){
        m_selectedNode->resetPosition();
        updateMarkPoint(m_selectedNode);
        m_historiesBefore.push_back(m_selectedNode);
        m_historiesAfter.clear();
        if(m_historiesBefore.length() > 30)
            m_historiesBefore.pop_front();
    }
}

void GraphicsView::onResetAllPoints() {
    // When you hit reset all point it will reset all point on the selected layer, BUT before it dose it will pop up a message to verify that you want to reset points for that layer.
    if(m_curLayerIndex >= 0 && m_curLayerIndex < m_lstLayerData.count()){
        if(m_lstLayerData.at(m_curLayerIndex)){
            for(int i = 0 ; i < m_lstLayerData.at(m_curLayerIndex)->nodeCount(); i++){
                if(m_lstLayerData.at(m_curLayerIndex)->nodeAt(i) != nullptr){
                    m_lstLayerData.at(m_curLayerIndex)->nodeAt(i)->resetPosition();
                    updateMarkPoint(m_lstLayerData.at(m_curLayerIndex)->nodeAt(i));
                    m_historiesBefore.push_back(m_lstLayerData.at(m_curLayerIndex)->nodeAt(i));
                    m_historiesAfter.clear();
                    if(m_historiesBefore.length() > 30)
                        m_historiesBefore.pop_front();
                }
            }
        }
    }
}

void GraphicsView::updateMarkPoint(Node* node) {
    for (int i = 0; i < m_markList.size(); ++i) {
        if(m_markList[i]->getNode() == node) {
           m_markList[i]->setPos(node->pos());
        }
    }
}

void GraphicsView::onUndo()
{
    if(m_historiesBefore.length() > 0){
        Node* node = (Node*)m_historiesBefore.last();
        if(node != nullptr){
            m_historiesBefore.pop_back();
            m_historiesAfter.push_front(node);
            node->undo();

            if(m_selectedNode != node)
                if(m_selectedNode != nullptr)
                    m_selectedNode->setSelectionState(false);
            m_selectedNode = node;
            node->setSelectionState(true);
        }
    }
}

void GraphicsView::onRedo() {
    if(m_historiesAfter.length() > 0){
        Node* node = (Node*)m_historiesAfter.first();
        if(node != nullptr) {
            m_historiesAfter.pop_front();
            m_historiesBefore.push_back(node);
            node->redo();

            if(m_selectedNode != node)
                if(m_selectedNode != nullptr)
                    m_selectedNode->setSelectionState(false);
            m_selectedNode = node;
            node->setSelectionState(true);
        }
    }

}

void GraphicsView::onDelete() {
    if(m_selectedShape != nullptr) {
        m_shapeList.removeOne(m_selectedShape);
        delete m_selectedShape;
        m_selectedShape = nullptr;
    }
}

void GraphicsView::onSave(QString fn){
    QFile file(fn);
    file.open(QIODevice::WriteOnly);

    if(file.isOpen()) {
        QTextStream outStream(&file);
        int r = -1, g = -1, b = -1, a = -1;
        /// Background
        m_backgroundColor.getRgb(&r, &g, &b, &a);
        outStream << r << " ";
        outStream << g << " ";
        outStream << b << " ";
        outStream << a << endl;

        /// Layer Property
        outStream << m_layerCount << endl;
        for(int i = 0 ; i < m_layerCount; i++){
            outStream << m_lstLayerData.at(i)->getLayerIndex() << endl;
            outStream << m_lstLayerData.at(i)->getNodeColor().red() << " "
                      << m_lstLayerData.at(i)->getNodeColor().green() << " "
                      << m_lstLayerData.at(i)->getNodeColor().blue() << endl;
            outStream << m_lstLayerData.at(i)->getEdgeColor().red() << " "
                      << m_lstLayerData.at(i)->getEdgeColor().green() << " "
                      << m_lstLayerData.at(i)->getEdgeColor().blue() << endl;
            outStream << int(m_lstLayerData.at(i)->isAllVisible()) << " "
                      << int(m_lstLayerData.at(i)->isAllUnlock()) << endl;
        }
        outStream << m_curLayerIndex << endl;

        outStream << m_gridSize << endl;
        outStream << m_spacingSize << endl;

        /// Node Data
        for(int i = 0 ; i < m_layerCount; i++){
            outStream << m_lstLayerData.at(i)->nodeCount() << endl;
            foreach(Node* node, *(m_lstLayerData.at(i)->nodes())){
                node->getColor().getRgb(&r, &g, &b, &a);
                outStream << node->getLocalPos().x() << " "
                          << node->getLocalPos().y() << " "
                          << r << " "
                          << g << " "
                          << b << " "
                          << a << endl;
            }
        }

        /// Voxel Data
        outStream << m_voxelList.count() << endl;
        foreach(VoxelPolygon* voxel, m_voxelList){
            voxel->getColor().getRgb(&r, &g, &b, &a);
            outStream << voxel->getLayerIndex() << " "
                      << voxel->node1()->getId() << " "
                      << voxel->node2()->getId() << " "
                      << voxel->node3()->getId() << " "
                      << voxel->node4()->getId() << " "
                      << r << " "
                      << g << " "
                      << b << endl;
        }

        /// Shape Data
        outStream << m_shapeList.count() << endl;

        foreach(BaseShape* shape, m_shapeList) {
            shape->save(&outStream);
        }

        /// Marker Data
        int markerCount = 0 ;
        foreach(MarkerItem* item, m_markList) {
            if(item->getNode() != nullptr) {
                markerCount ++;
            }
        }

        outStream << markerCount << endl;
        foreach(MarkerItem* item, m_markList) {
            if(item->getNode() != nullptr) {
                item->save(&outStream);
            }
        }

        /// Image Data
        if(m_imgItem != nullptr){
            outStream << m_imageFileName << endl;
            outStream << m_imgItem->getImageX() << " "
                      << m_imgItem->getImageY() << " "
                      << m_imgItem->getImageWith() << " "
                      << m_imgItem->getImageHeight() << " "
                      << m_imgItem->getImageAngle() << endl;
        }
        file.close();
    }
}

void GraphicsView::onOpen(QString fn){
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    QFile file(fn);
    file.open(QIODevice::ReadOnly);

    if(file.isOpen()) {
        initGraphicsItems();
        scene()->clear();
        initLayerProperty();

        int cnt, local_x, local_y;

        QTextStream inStream(&file);
        /// Background
        int r = -1, g = -1, b = -1, a = -1;
        inStream >> r >> g >> b >> a;
        m_backgroundColor.setRgb(r, g, b, a);

        /// Layer Property
        inStream >> m_layerCount;
        for(int i = 0 ; i < m_layerCount; i++){
            int layer_index;
            int is_unlocked, is_visible;

            inStream >> layer_index;
            m_lstLayerData.at(i)->setLayerIndex(layer_index);

            inStream >> r >> g >> b;
            m_lstLayerData.at(i)->setNodeColor(QColor(r, g, b));

            inStream >> r >> g >> b;
            m_lstLayerData.at(i)->setEdgeColor(QColor(r, g, b));

            inStream >> is_visible >> is_unlocked;
            m_lstLayerData.at(i)->setAllVisible(is_visible);
            m_lstLayerData.at(i)->setAllUnlock(is_unlocked);

            emit changedLayerColor(i, QColor(r, g, b));
            emit changedLayerVisibility(i, is_visible);
            emit changedLayerUnlockState(i, is_unlocked);
        }
        inStream >> m_curLayerIndex;
        emit changedActiveLayer(m_curLayerIndex);

        /// ///

        inStream >> m_gridSize;
        inStream >> m_spacingSize;

        for(int i = 0 ; i < m_layerCount; i++){
            inStream >> cnt;
            scene()->setSceneRect(-(m_gridSize + 4) * m_spacingSize / 2, -(m_gridSize + 4) * m_spacingSize / 2, (m_gridSize + 4) * m_spacingSize, (m_gridSize + 4) * m_spacingSize);
            int nodeIndex = 0;
            for (int x = -m_gridSize / 2; x <= m_gridSize / 2; x ++){
                for (int y = -m_gridSize / 2; y <= m_gridSize / 2; y ++){

                    inStream >> local_x >> local_y >> r >> g >> b >> a;

                    Node* node = new Node();
                    node->setColor(QColor(r, g, b, a));
                    int x_pos_origin = x * m_spacingSize;
                    int y_pos_origin = y * m_spacingSize;

                    int x_pos = x * m_spacingSize + local_x * m_spacingUnit;
                    int y_pos = y * m_spacingSize + local_y * m_spacingUnit;

                    node->setLayerIndex(i);
                    node->setPos(x_pos,y_pos);
                    node->setCentralPos(QPoint(x_pos_origin, y_pos_origin));
                    node->setSpacingSize(m_spacingSize);
                    node->setId(nodeIndex);
                    node->setColor(m_lstLayerData.at(i)->getNodeColor());
                    node->setMovable(m_lstLayerData.at(i)->isAllUnlock() &&
                                      m_lstLayerData.at(i)->isActivated());

                    m_lstLayerData.at(i)->addNode(node);
                    QObject::connect(node, SIGNAL(nodeCoordinate(Node*, QPoint)), this, SLOT(nodeCoordinate(Node*, QPoint)));
                    QObject::connect(node, SIGNAL(nodePressed(Node*)), this, SLOT(nodePressed(Node*)));
                    QObject::connect(node, SIGNAL(nodeChanged(Node*)), this, SLOT(nodeChanged(Node*)));
                    QObject::connect(node, SIGNAL(markersShow(Node*, QPoint)), this, SLOT(markersShow(Node*, QPoint)));
                    nodeIndex ++;
                }
            }
        }

        // Add Voxel Items to select a voxel
        if(m_cursorVoxel == nullptr)
            m_cursorVoxel = new VoxelPolygon();
        m_cursorVoxel->setPolygon(QPolygonF(QVector<QPointF>() << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)));
        m_cursorVoxel->setColor(Qt::white);
        m_cursorVoxel->setOpacity(0.3);
        m_cursorVoxel->setZValue(2);

        if(m_selectedVoxel == nullptr)
            m_selectedVoxel = new VoxelPolygon();
        m_selectedVoxel->setSelectionState();
        m_selectedVoxel->setColor(Qt::transparent);
        m_selectedVoxel->setZValue(2);

        m_selectedVoxel->setPolygon(QPolygonF(QVector<QPointF>() << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)
                                            << QPointF(0.0, 0.0)));
        scene()->addItem(m_selectedVoxel);
        scene()->addItem(m_cursorVoxel);
        ////// //////
        drawGraphicsItems();

        /// Read Voxels
        int voxelCount;
        inStream >> voxelCount;
        int layer_index, node1_index, node2_index, node3_index, node4_index;
        for(int i = 0 ; i < voxelCount; i++){
            inStream >> layer_index
                    >> node1_index
                    >> node2_index
                    >> node3_index
                    >> node4_index
                    >> r
                    >> g
                    >> b;
            if(layer_index < 0 || layer_index >= m_layerCount)
                continue;

            Node* node1 = getNode(m_lstLayerData.at(layer_index), node1_index);
            Node* node2 = getNode(m_lstLayerData.at(layer_index), node2_index);
            Node* node3 = getNode(m_lstLayerData.at(layer_index), node3_index);
            Node* node4 = getNode(m_lstLayerData.at(layer_index), node4_index);
            if(node1 != nullptr &&
                    node2 != nullptr &&
                    node3 != nullptr &&
                    node4 != nullptr){
                VoxelPolygon* voxel = new VoxelPolygon();
                voxel->setNodes(node1, node2, node3, node4);
                voxel->setColor(QColor(r, g, b));
                voxel->setOpacity(0.8);

                scene()->addItem(voxel);
                m_voxelList.push_back(voxel);
            }
        }

        /// Read Shapes
        int shapeCount;
        inStream >> shapeCount;
        for (int i = 0; i < shapeCount; ++i) {
            int type = -1;
            inStream >> type;
            BaseShape* shape;
            switch(type) {
                case EDIT_MODE_SELECT:
                    break;
                case EDIT_MODE_LINE:
                    shape = new ShapeLine(EDIT_MODE_LINE, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapeLine*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
                    shape = new ShapeCornerToCornerRectangle(EDIT_MODE_CORNER_CORNER_TO_REACTANGLE, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapeCornerToCornerRectangle*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
                    shape = new ShapeCenterToCornerRectangle(EDIT_MODE_CENTER_TO_CORNER_REACTANGLE, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapeCenterToCornerRectangle*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_CENTER_CIRCLE:
                    shape = new ShapeCenterPointCircle(EDIT_MODE_CENTER_CIRCLE, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapeCenterPointCircle*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_3_POINT_CIRCLE:
                    shape = new Shape3PointCircle(EDIT_MODE_3_POINT_CIRCLE, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((Shape3PointCircle*)shape));
                    ((Shape3PointCircle*)shape)->setTwoCircleMode(false);
                    ((Shape3PointCircle*)shape)->updateShapeRectangle();
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_CENTER_OVAL:
                    shape = new ShapeCenterOval(EDIT_MODE_CENTER_OVAL, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    ((ShapeCenterOval*)shape)->setTwoOvalMode(false);
                    ((ShapeCenterOval*)shape)->updateShapeRectangle();
                    scene()->addItem(((ShapeCenterOval*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_3_POINT_OVAL:
                    shape = new Shape3PointOval(EDIT_MODE_3_POINT_OVAL, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    ((Shape3PointOval*)shape)->setTwoOvalMode(false);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem((Shape3PointOval*)shape);
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_CENTER_ARC:
                    shape = new ShapeCenterArc(EDIT_MODE_CENTER_ARC, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    ((ShapeCenterArc*)shape)->setTwoPointMode(false);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapeCenterArc*)shape));
                    ((ShapeCenterArc*)shape)->updateShapeRectangle();
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_3_POINT_ARC:
                    shape = new Shape3PointArc(EDIT_MODE_3_POINT_ARC, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    ((Shape3PointArc*)shape)->setTwoCircleMode(false);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((Shape3PointArc*)shape));
                    ((Shape3PointArc*)shape)->updateShapeRectangle();
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_POLYGON:
                    shape = new ShapePolygon(EDIT_MODE_POLYGON, m_polygonNumber, m_shapeLineColor, m_shapeLineWidth);
                    shape->open(&inStream);
                    shape->setIsDrawing(false);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    scene()->addItem(((ShapePolygon*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_HANDLE_CURVE:
                    shape = new ShapeHandleCurve(EDIT_MODE_HANDLE_CURVE, m_shapeLineColor, m_shapeLineWidth);
                    shape->setIsDrawing(false);
                    shape->open(&inStream);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    ((ShapeHandleCurve*)shape)->setIsAddabble(false);
                    scene()->addItem(((ShapeHandleCurve*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_POINT_CURVE:
                    shape = new ShapeHandleCurve(EDIT_MODE_HANDLE_CURVE, m_shapeLineColor, m_shapeLineWidth);
                    shape->setIsDrawing(false);
                    shape->open(&inStream);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    ((ShapeHandleCurve*)shape)->setIsAddabble(false);
                    scene()->addItem(((ShapeHandleCurve*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                case EDIT_MODE_POINT_HANDLE_CURVE:
                    shape = new ShapePointHandleCurve(EDIT_MODE_POINT_HANDLE_CURVE, m_shapeLineColor, m_shapeLineWidth);
                    shape->setIsDrawing(false);
                    shape->open(&inStream);
                    shape->setSelectable(false);
                    shape->setScaleFactor(m_scaleFactor);
                    ((ShapePointHandleCurve*)shape)->setIsAddabble(false);
                    scene()->addItem(((ShapePointHandleCurve*)shape));
                    m_shapeList.append(shape);
                    QObject::connect(shape, SIGNAL(selectedObject(BaseShape*, int)), this, SLOT(shapePressed(BaseShape*, int)));
                    break;
                default:
                    break;
            }
        }
        initMarkerList();
        int markerCount;
        inStream >> markerCount;
        for (int i = 0; i < markerCount; ++i) {
            MarkerItem* item = new MarkerItem();
            item->open(&inStream);

            if(item->getLayerIndex() < 0 || item->getLayerIndex() >= m_layerCount)
                continue;

            if(item->getNodeIndex() != -1 && item->getNodeIndex() < m_lstLayerData.at(item->getLayerIndex())->nodeCount()) {
                item->setNode(m_lstLayerData.at(item->getLayerIndex())->nodeAt(item->getNodeIndex()));
                item->setPos(m_lstLayerData.at(item->getLayerIndex())->nodeAt(item->getNodeIndex())->pos());
                item->setScaleFactor(m_scaleFactor);
                item->setBoundingRect(QRectF(0, 0, 10, 10));
                m_markList.push_back(item);
                QObject::connect(item, SIGNAL(selectedMarker(MarkerItem*)), this, SLOT(selectedMarker(MarkerItem*)));
                QObject::connect(item, SIGNAL(selectedNodeFromMarker(MarkerItem*)), this, SLOT(selectedNodeFromMarker(MarkerItem*)));
                scene()->addItem(item);
                m_lstLayerData.at(item->getLayerIndex())->nodeAt(item->getNodeIndex())->addMarker(item->getNumber());
            }
        }
        m_imageFileName = inStream.readLine();

        // Because of "endl"
        if(m_imageFileName.trimmed().isEmpty())
            m_imageFileName = inStream.readLine();

        if(!m_imageFileName.trimmed().isEmpty()){
            if(QFile::exists(m_imageFileName)){
                int img_x, img_y, img_w, img_h, img_a;
                inStream >> img_x >> img_y >> img_w >> img_h >> img_a;

                QPixmap pixmap(m_imageFileName);
                m_imgItem = new ImageItem(pixmap);
                m_imgItem->setPos(img_x,img_y);
                m_imgItem->setImageSize(img_w, img_h);
                m_imgItem->setImageAngle(img_a);
                m_imgItem->setScaleFactor(m_scaleFactor);
                onLockImage(true);

                scene()->addItem(m_imgItem);
            }else{
                QMessageBox::information(this, "Loading a image", "Can't find a file " + m_imageFileName,
                                                QMessageBox::Ok);
            }
        }

        file.close();

        m_historiesBefore.clear();
        m_historiesAfter.clear();

        for(int i = 0 ; i < m_layerCount; i++){
            setLayerColor(i, m_lstLayerData.at(i)->getNodeColor());
            showLayer(i, m_lstLayerData.at(i)->isAllVisible());
            unlockLayer(i, m_lstLayerData.at(i)->isAllUnlock());
        }

        setCurrentLayerIndex(m_curLayerIndex);
    }

    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
}

void GraphicsView::onLoadImage(QString fn){
    onClearImage();

    QPixmap pixmap(fn);
    m_imgItem = new ImageItem(pixmap);
    m_imgItem->setPos(0,0);
    m_imgItem->setScaleFactor(m_scaleFactor);
    m_imgItem->lockImage(m_imageLocked);
    m_imgItem->maintainAspectRatio(m_imageMaintainAspectRatio);
    m_imgItem->setVisible(m_imageShown);
    scene()->addItem(m_imgItem);
    QObject::connect(m_imgItem, SIGNAL(imageStartRotate()), this, SLOT(imageStartRotate()));
    QObject::connect(m_imgItem, SIGNAL(imageStopRotate()), this, SLOT(imageStopRotate()));
    QObject::connect(m_imgItem, SIGNAL(imageStartMove()), this, SLOT(imageStartMove()));
    QObject::connect(m_imgItem, SIGNAL(imageStopMove()), this, SLOT(imageStopMove()));
    m_imageFileName = fn;
}

void GraphicsView::onClearImage() {
    if(m_imgItem != nullptr)
        delete[] m_imgItem;
    m_imgItem = nullptr;
    m_imageFileName = "";
}

void GraphicsView::drawGraphicsItems(){
    foreach(LayerData* layer, m_lstLayerData){
        int index = 0;
        for(int i = 0 ; i < layer->nodeCount(); i++){
            // Right node
            index = i + 1;
            if(index >= 0 && index < layer->nodeCount() && i % (m_gridSize + 1) != m_gridSize){
                // Create edge-item
                Edge* e = new Edge(layer->nodeAt(i), layer->nodeAt(index));
                e->setScaleFactor(m_scaleFactor);
                e->setColor(layer->getEdgeColor());
                layer->addEdge(e);
            }

            // Bottom node
            index = i + m_gridSize + 1;
            if(index >= 0 && index < layer->nodeCount()){
                // Create edge-item
                Edge* e = new Edge(layer->nodeAt(i), layer->nodeAt(index));
                e->setScaleFactor(m_scaleFactor);
                e->setColor(layer->getEdgeColor());
                layer->addEdge(e);
            }

            // Add node-item in scene
            layer->nodeAt(i)->setScaleFactor(m_scaleFactor);
        }

        scene()->removeItem(layer);
        if(layer->isAllVisible())
            scene()->addItem(layer);
    }
}

void GraphicsView::onUpdateBackColor(QColor color) {
    m_backgroundColor = color;
    setBackgroundBrush(QBrush(m_backgroundColor));
    this->scene()->update(this->sceneRect());
}

void GraphicsView::onUpdateShapeLineColor(QColor color) {
    m_shapeLineColor = color;
    if(m_selectedShape != nullptr) {
        m_selectedShape->setLineColor(color);
    }
}

void GraphicsView::onUpdateLineColor(QColor color){
    m_lineColor = color;

    foreach(LayerData* layer, m_lstLayerData){
        if(layer){
            for(int i = 0 ; i < layer->edgeCount(); i++){
                layer->edgeAt(i)->setColor(layer->getEdgeColor());
            }
        }
    }
}

void GraphicsView::onMaintainAspectRatio(bool f){
    m_imageMaintainAspectRatio = f;
    if(m_imgItem != nullptr)
        m_imgItem->maintainAspectRatio(m_imageMaintainAspectRatio);
}

bool GraphicsView::getMaintainAspectRatio() {
    return m_imageMaintainAspectRatio;
}

void GraphicsView::onLockImage(bool f)
{
    m_imageLocked = f;
    m_imageLockedTemp = f;
    if(m_imgItem != nullptr)
        m_imgItem->lockImage(m_imageLocked);
}

bool GraphicsView::getLockImage() {
    return m_imageLocked;
}

void GraphicsView::onShowImage(bool f)
{
    m_imageShown = f;
    if(m_imgItem != nullptr)
        m_imgItem->setVisible(m_imageShown);
}

bool GraphicsView::getShowImage() {
    return m_imageShown;
}

void GraphicsView::imageStartRotate() {
    if(m_imgItem != nullptr && m_bEditMode && m_EditMode == EDIT_MODE_SELECT) {
        m_bImageRotateMode = true;
        m_imageMovingPrevPoint = QPointF(0, 0);
    }
}

void GraphicsView::imageStopRotate() {
    m_bImageRotateMode = false;
}

void GraphicsView::imageStartMove() {
    if(m_imgItem != nullptr && m_bEditMode && m_EditMode == EDIT_MODE_SELECT) {
        m_bImageMoveMode = true;
    }
}

void GraphicsView::imageStopMove() {
    m_bImageMoveMode = false;
}

void GraphicsView::shapePressed(BaseShape* shape, int index) {
    if(!m_bEditMode) {
        return;
    }
    if(m_selectedShape != nullptr) {
        for (int i = 0; i < m_shapeList.count(); ++i) {
            m_shapeList.at(i)->removeArrows();
        }
    }
    m_selectedShape = shape;
    m_selectedShape->addArrow(index);
    emit selectedShape(m_selectedShape);
}

void GraphicsView::markersShow(Node *n, QPoint point) {
    if(m_bEditMode) {
        return;
    }
    emit markersShowPopup(n, point);
}

void GraphicsView::nodeCoordinate(Node* n, QPoint point) {
    emit nodeCoordinatePopup(n, point);
}

int GraphicsView::getLineWidth() {
    return m_shapeLineWidth;
}

void GraphicsView::nodePressed(Node *n)
{
    if(m_reactorMode){
        // Reactor Mode
        if(m_reactorNodes.length() < 4){
            if(m_reactorNodes.length() > 0){
                if(m_reactorNodes.last() != nullptr){
                    if(!m_reactorNodes.last()->isLinkedNode(n)){
                        for(int i = 0 ; i < m_reactorNodes.length(); i++){
                            if(m_reactorNodes.at(i) != nullptr)
                                m_reactorNodes.at(i)->setReactor(false);
                        }
                        m_reactorNodes.clear();
                    }
                }
            }
            // Reactor Node
            m_reactorNodes << n;
            n->setReactor(true);
        }
    }

    if(m_selectedNode != n)
        if(m_selectedNode != nullptr)
            m_selectedNode->setSelectionState(false);

    m_selectedNode = n;

    /////////Add Item to History List
    if(n != nullptr){
        m_historiesBefore.push_back(n);
        m_historiesAfter.clear();
        if(m_historiesBefore.length() > 30)
            m_historiesBefore.pop_front();
    }
    /////////
}

void GraphicsView::nodeChanged(Node *n) {
    if(n != nullptr){
        if(n->isReactor()){
            if(m_reactorNodes.length() == 4)
            {
                emit reactorChanged(m_reactorNodes.at(0), m_reactorNodes.at(1), m_reactorNodes.at(2), m_reactorNodes.at(3));
            }
        }
        updateMarkPoint(n);
    }
}

void GraphicsView::addColorVoxel(QColor color) {
    if(m_selectedVoxel == nullptr)
        return;

    if(m_selectedVoxel->hasNullNode())
        return;

    /// Check if there is a selected voxel
    foreach(VoxelPolygon* voxel, m_voxelList){
        if(voxel != nullptr){
            if(m_selectedVoxel->hasSameNodes(voxel)){
                /// Update Voxel Color
                voxel->setColor(color);
                return;
            }
        }
    }

    /// Create New Voxel
    VoxelPolygon* voxel = new VoxelPolygon();
    voxel->setNodesFromVoxelPolygon(m_selectedVoxel);
    voxel->setColor(color);
    voxel->setOpacity(0.8);

    /// Add Voxel to List and Scene
    scene()->addItem(voxel);
    m_voxelList.push_back(voxel);
}

void GraphicsView::removeColorVoxel()
{
    if(m_selectedVoxel == nullptr)
        return;

    if(m_selectedVoxel->hasNullNode())
        return;

    /// Check if there is a selected voxel
    foreach(VoxelPolygon* voxel, m_voxelList){
        if(voxel != nullptr){
            if(m_selectedVoxel->hasSameNodes(voxel)){
                /// Update Voxel Color
                scene()->removeItem(voxel);
                m_voxelList.removeAll(voxel);
                return;
            }
        }
    }
}

void GraphicsView::setVoxelSelectMode(bool f)
{
    m_voxelSelectMode = f;
    if(!m_voxelSelectMode){
        if(m_cursorVoxel != nullptr)
            m_cursorVoxel->initPoints();
        if(m_selectedVoxel != nullptr)
            m_selectedVoxel->initPoints();
    }
}

void GraphicsView::setReactorNodesFromSelectedVoxel()
{
    if(m_voxelSelectMode && m_reactorMode){
        if(m_selectedVoxel != nullptr){
            if(!m_selectedVoxel->hasNullNode()){
                for(int i = 0 ; i < m_reactorNodes.length() ; i++){
                    if(m_reactorNodes.at(i) != nullptr)
                        m_reactorNodes.at(i)->setReactor(false);
                }
                m_reactorNodes.clear();

                m_reactorNodes << m_selectedVoxel->node1()
                               << m_selectedVoxel->node2()
                               << m_selectedVoxel->node3()
                               << m_selectedVoxel->node4();
                m_reactorNodes.at(0)->setReactor(true);
                m_reactorNodes.at(1)->setReactor(true);
                m_reactorNodes.at(2)->setReactor(true);
                m_reactorNodes.at(3)->setReactor(true);

                m_reactorNodes.at(0)->setSelectionState(false);
                m_reactorNodes.at(1)->setSelectionState(false);
                m_reactorNodes.at(2)->setSelectionState(false);
                m_reactorNodes.at(3)->setSelectionState(false);

                emit reactorChanged(m_reactorNodes.at(0), m_reactorNodes.at(1), m_reactorNodes.at(2), m_reactorNodes.at(3));

                QPoint pt1 = m_reactorNodes.at(0)->getLocalPos();
                QPoint pt2 = m_reactorNodes.at(1)->getLocalPos();
                QPoint pt3 = m_reactorNodes.at(2)->getLocalPos();
                QPoint pt4 = m_reactorNodes.at(3)->getLocalPos();
                QString reactor_position = "\"" + QString::number(pt1.x()) + "," + QString::number(-pt1.y())  + ";";
                reactor_position = reactor_position + QString::number(pt2.x()) + "," + QString::number(-pt2.y()) + ";";
                reactor_position = reactor_position + QString::number(pt3.x()) + "," + QString::number(-pt3.y()) + ";";
                reactor_position = reactor_position + QString::number(pt4.x()) + "," + QString::number(-pt4.y()) + "\"";

                QClipboard *p_Clipboard = QApplication::clipboard();
                p_Clipboard->setText(reactor_position);
            }
        }
    }
}

void GraphicsView::onReactorGuide(bool f) {
    for(int i = 0 ; i < m_reactorNodes.length() ; i++){
        if(m_reactorNodes.at(i) != nullptr)
            m_reactorNodes.at(i)->setReactor(false);
    }
    m_reactorNodes.clear();
    m_reactorMode = f;

    if(m_selectedNode != nullptr)
        m_selectedNode->setSelectionState(false);

    if(m_reactorMode){
        setReactorNodesFromSelectedVoxel();
    }
}

