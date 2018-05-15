#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSignalMapper>
#include "markerdialog.h"
#include "coordinateform.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //// Init Layer Buttons
    ui->widButtonLayer1->setLayerName("LAYER 1");
    ui->widButtonLayer1->setChecked(true);
    ui->widButtonLayer1->setShow(true);
    ui->widButtonLayer1->setUnlock(true);
    ui->widButtonLayer1->setLayerColor(QColor(0, 102, 255));
    connect(ui->widButtonLayer1, &CustomLayerButton::clicked, this, &MainWindow::onSelectedLayer);
    connect(ui->widButtonLayer1, &CustomLayerButton::updatedShowState, this, &MainWindow::onUpdateShowState);
    connect(ui->widButtonLayer1, &CustomLayerButton::updatedUnlockState, this, &MainWindow::onUpdateUnlockState);
    connect(ui->widButtonLayer1, &CustomLayerButton::chooseColor, this, &MainWindow::onChooseLayerColor);

    ui->widButtonLayer2->setLayerName("LAYER 2");
    ui->widButtonLayer2->setChecked(false);
    ui->widButtonLayer2->setShow(false);
    ui->widButtonLayer2->setUnlock(true);
    ui->widButtonLayer2->setLayerColor(QColor(153, 0, 255));
    connect(ui->widButtonLayer2, &CustomLayerButton::clicked, this, &MainWindow::onSelectedLayer);
    connect(ui->widButtonLayer2, &CustomLayerButton::updatedShowState, this, &MainWindow::onUpdateShowState);
    connect(ui->widButtonLayer2, &CustomLayerButton::updatedUnlockState, this, &MainWindow::onUpdateUnlockState);
    connect(ui->widButtonLayer2, &CustomLayerButton::chooseColor, this, &MainWindow::onChooseLayerColor);

    ui->widButtonLayer3->setLayerName("LAYER 3");
    ui->widButtonLayer3->setChecked(false);
    ui->widButtonLayer3->setShow(false);
    ui->widButtonLayer3->setUnlock(true);
    ui->widButtonLayer3->setLayerColor(QColor(251, 135, 25));
    connect(ui->widButtonLayer3, &CustomLayerButton::clicked, this, &MainWindow::onSelectedLayer);
    connect(ui->widButtonLayer3, &CustomLayerButton::updatedShowState, this, &MainWindow::onUpdateShowState);
    connect(ui->widButtonLayer3, &CustomLayerButton::updatedUnlockState, this, &MainWindow::onUpdateUnlockState);
    connect(ui->widButtonLayer3, &CustomLayerButton::chooseColor, this, &MainWindow::onChooseLayerColor);

    m_lstLayerButtons.push_back(ui->widButtonLayer1);
    m_lstLayerButtons.push_back(ui->widButtonLayer2);
    m_lstLayerButtons.push_back(ui->widButtonLayer3);

    connect(ui->graphicsView, &GraphicsView::changedLayerColor, this, [=](int layerIndex, QColor color){
        if(layerIndex >= 0 && layerIndex < m_lstLayerButtons.count()){
            m_lstLayerButtons.at(layerIndex)->setLayerColor(color);
        }
    });
    connect(ui->graphicsView, &GraphicsView::changedLayerVisibility, this, [=](int layerIndex, bool flg){
        if(layerIndex >= 0 && layerIndex < m_lstLayerButtons.count()){
            m_lstLayerButtons.at(layerIndex)->setShow(flg);
        }
    });
    connect(ui->graphicsView, &GraphicsView::changedLayerUnlockState, this, [=](int layerIndex, bool flg){
        if(layerIndex >= 0 && layerIndex < m_lstLayerButtons.count()){
            m_lstLayerButtons.at(layerIndex)->setUnlock(flg);
        }
    });
    connect(ui->graphicsView, &GraphicsView::changedActiveLayer, this, [=](int layerIndex){
        if(layerIndex >= 0 && layerIndex < m_lstLayerButtons.count()){
            for(int i = 0 ; i < m_lstLayerButtons.count(); i++){
                m_lstLayerButtons.at(i)->setChecked(i == layerIndex);
            }
        }
    });
    /// ///
    initImageItemProperty();
    ui->widReactorPreview->setSpacingSize(ui->graphicsView->getSpacingSize());
    ui->widReactorPreview->setSpacingUnit(ui->graphicsView->getSpacingUnit());

    // Setup Line-Color from one node to another node
    m_lineColor = Qt::blue;
    m_shapeLineColor = Qt::blue;
    m_lineWidth = 2;
    m_backgroundColor = QColor(210, 210, 210);

    /// Voxel Color Initialize
    m_voxelColor = QColor(80, 250, 80);
    QString voxelColorButtonStyle = QString("QPushButton{ \
            background-color: %1; \
            border:none; \
            }\
            QPushButton:disabled{ \
            background-color: rgb(50, 50, 50); \
            border:none; \
            }\
            QPushButton:hover{ \
            background-color: %2; \
            border:none; \
            } \
            QPushButton:pressed{ \
            background-color: %3; \
            border:none; \
            }").arg(m_voxelColor.name(), m_voxelColor.lighter(115).name(), m_voxelColor.lighter(120).name());
    ui->btnColorVoxel->setStyleSheet(voxelColorButtonStyle);

    updateLineColor();
    updateShapeLineColor();
    updateBackColor();
    tabSelected();
    ui->sBPolygon->setMinimum(3);
    ui->sBPolygon->setValue(3);
    ui->sBLineWidth->setMinimum(1);
    ui->sBLineWidth->setValue(m_lineWidth);
    ui->sBPolygon->hide();

    ui->chkLockImage->setChecked(false);
    ui->graphicsView->onLockImage(false);
    // Button's event-handler
    QObject::connect(ui->btnNew, SIGNAL(released()), this, SLOT(onNew_triggered()));
    QObject::connect(ui->btnOpen, SIGNAL(released()), this, SLOT(onOpen_triggered()));
    QObject::connect(ui->btnSave, SIGNAL(released()), this, SLOT(onSave_triggered()));
    QObject::connect(ui->btnModifyImage, SIGNAL(released()), this, SLOT(onImportImage_triggered()));
    QObject::connect(ui->btnClearImage, SIGNAL(released()), this, SLOT(onClearImage_triggered()));
    QObject::connect(ui->btnLineColor, SIGNAL(released()), this, SLOT(onChooseLineColor_triggered()));
    QObject::connect(ui->btnColorVoxel, SIGNAL(released()), this, SLOT(onChooseVoxelColor_triggered()));
    QObject::connect(ui->btnRemoveVoxelColor, SIGNAL(released()), this, SLOT(onRemoveVoxelColor_triggered()));
    QObject::connect(ui->btnSetVoxelColor, SIGNAL(released()), this, SLOT(onSetVoxelColor_triggered()));
    QObject::connect(ui->btnBackgroundColor, SIGNAL(released()), this, SLOT(onChooseBackColor_triggered()));
    QObject::connect(ui->chkAspectRatio, SIGNAL(toggled(bool)), this, SLOT(onMaintainAspectRatio(bool)));
    QObject::connect(ui->chkLockImage, SIGNAL(toggled(bool)), this, SLOT(onLockImage(bool)));
    QObject::connect(ui->chkShowImage, SIGNAL(toggled(bool)), this, SLOT(onShowImage(bool)));
    QObject::connect(ui->btnPointSelect, &QPushButton::released, this, [=](){
        ui->graphicsView->setVoxelSelectMode(false);
        ui->btnPointSelect->setChecked(true);
        ui->btnVoxelSelect->setChecked(false);

        ///
        ui->btnSetVoxelColor->setEnabled(false);
        ui->btnColorVoxel->setEnabled(false);
        ui->btnRemoveVoxelColor->setEnabled(false);
    });
    QObject::connect(ui->btnVoxelSelect, &QPushButton::released, this, [=](){
        ui->graphicsView->setVoxelSelectMode(true);
        ui->btnPointSelect->setChecked(false);
        ui->btnVoxelSelect->setChecked(true);

        ///
        ui->btnSetVoxelColor->setEnabled(true);
        ui->btnColorVoxel->setEnabled(true);
        ui->btnRemoveVoxelColor->setEnabled(true);
    });
    ui->btnPointSelect->released();

    // Draw Reactor-Preview
    QObject::connect(ui->graphicsView, SIGNAL(reactorChanged(Node*, Node*, Node*, Node*)), ui->widReactorPreview, SLOT(drawReactorPreview(Node*, Node*, Node*, Node*)));
    QObject::connect(ui->graphicsView, SIGNAL(markersShowPopup(Node*, QPoint)), this, SLOT(markerShow(Node*, QPoint)));
    QObject::connect(ui->graphicsView, SIGNAL(nodeCoordinatePopup(Node*, QPoint)), this, SLOT(nodeCoordinatePopup(Node*, QPoint)));
    QObject::connect(ui->graphicsView, SIGNAL(selectedShape(BaseShape*)), this, SLOT(selectedShape(BaseShape*)));
    QObject::connect(ui->btnReactorGuide, SIGNAL(toggled(bool)), this, SLOT(onReactorGuide_triggered(bool)));
    QObject::connect(ui->btnResetPoint, SIGNAL(released()), this, SLOT(onResetPoint_triggered()));
    QObject::connect(ui->btnEditMode, SIGNAL(released()), this, SLOT(onEditMode_triggered()));
    QObject::connect(ui->btnLeaveEditMode, SIGNAL(released()), this, SLOT(onEditMode_triggered()));
    QObject::connect(ui->btnResetAllPoint, SIGNAL(released()), this, SLOT(onClearPoints_triggered()));

    QObject::connect(this, SIGNAL(updateEditMode(int)), this, SLOT(onUpdateEditMode_triggered(int)));

    // Draw Button Signals

    QObject::connect(ui->btnShapeLineColor, SIGNAL(released()), this, SLOT(onChooseShapeLineColor_triggered()));
    QObject::connect(ui->btnSelectObject, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_SELECT);});
    QObject::connect(ui->btnLine, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_LINE);});
    QObject::connect(ui->btnCornerToCornerReactangle, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_CORNER_CORNER_TO_REACTANGLE);});
    QObject::connect(ui->btnCenterToCornerRectangle, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_CENTER_TO_CORNER_REACTANGLE);});
    QObject::connect(ui->btnCenterCircle, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_CENTER_CIRCLE);});
    QObject::connect(ui->btn3PointCircle, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_3_POINT_CIRCLE);});
    QObject::connect(ui->btnCenterOval, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_CENTER_OVAL);});
    QObject::connect(ui->btn3PointOval, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_3_POINT_OVAL);});
    QObject::connect(ui->btnCenterArc, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_CENTER_ARC);});
    QObject::connect(ui->btn3PointArc, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_3_POINT_ARC);});
    QObject::connect(ui->btnPolygon, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_POLYGON);});
    QObject::connect(ui->btnHandleCurve, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_HANDLE_CURVE);});
    QObject::connect(ui->btnPointCurve, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_POINT_CURVE);});
    QObject::connect(ui->btnPointHandleCurve, &QPushButton::released, [=]() {emit updateEditMode(EDIT_MODE_POINT_HANDLE_CURVE);});
    QObject::connect(ui->sBPolygon, SIGNAL(valueChanged(int)), this, SLOT(onPolygonChange_triggered(int)));
    QObject::connect(ui->sBLineWidth, SIGNAL(valueChanged(int)), this, SLOT(onLineWidthChange_triggered(int)));
    ui->stackedWidget->setCurrentIndex(0);

    ui->btnOffset->setVisible(false);
    ui->chkHideShowBtn->setVisible(false);
    ui->widSpace_2->setVisible(false);
    ui->widLineColor->setVisible(false);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onMaintainAspectRatio(bool f) {
    ui->graphicsView->onMaintainAspectRatio(f);
}

void MainWindow::onLockImage(bool f) {
    ui->graphicsView->onLockImage(f);
}

void MainWindow::onShowImage(bool f) {
    ui->graphicsView->onShowImage(f);
}

void MainWindow::tabSelected(){
    onUpdateEditMode_triggered(ui->graphicsView->getEditMode());
    onLineWidthChange_triggered(ui->graphicsView->getShapeLineWidth());
    onPolygonChange_triggered(ui->graphicsView->getPolygonNumber());
    ui->sBPolygon->setValue(ui->graphicsView->getPolygonNumber());
    ui->sBLineWidth->setValue(ui->graphicsView->getLineWidth());
    m_shapeLineColor = ui->graphicsView->getShapeLineColor();
    ui->chkAspectRatio->setChecked(ui->graphicsView->getMaintainAspectRatio());
    ui->chkLockImage->setChecked(ui->graphicsView->getLockImage());
    ui->chkShowImage->setChecked(ui->graphicsView->getShowImage());
    updateLineWidth();
    updateShapeLineColor();
    if ( ui->graphicsView->isEditMode() ) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::onUpdateEditMode_triggered(int editMode) {
    editMode = ui->graphicsView->updateEditMode(editMode);
    ui->btnSelectObject->setChecked(false);
    ui->btnLine->setChecked(false);
    ui->btnCornerToCornerReactangle->setChecked(false);
    ui->btnCenterToCornerRectangle->setChecked(false);
    ui->btnCenterCircle->setChecked(false);
    ui->btn3PointCircle->setChecked(false);
    ui->btnCenterOval->setChecked(false);
    ui->btn3PointOval->setChecked(false);
    ui->btnCenterArc->setChecked(false);
    ui->btn3PointArc->setChecked(false);
    ui->btnPolygon->setChecked(false);
    ui->btnHandleCurve->setChecked(false);
    ui->btnPointCurve->setChecked(false);
    ui->btnPointHandleCurve->setChecked(false);
    ui->sBPolygon->hide();
    switch (editMode) {
        case EDIT_MODE_SELECT:
            ui->btnSelectObject->setChecked(true);
            break;
        case EDIT_MODE_LINE:
            ui->btnLine->setChecked(true);
            break;
        case EDIT_MODE_CORNER_CORNER_TO_REACTANGLE:
            ui->btnCornerToCornerReactangle->setChecked(true);
            break;
        case EDIT_MODE_CENTER_TO_CORNER_REACTANGLE:
            ui->btnCenterToCornerRectangle->setChecked(true);
            break;
        case EDIT_MODE_CENTER_CIRCLE:
            ui->btnCenterCircle->setChecked(true);
            break;
        case EDIT_MODE_3_POINT_CIRCLE:
            ui->btn3PointCircle->setChecked(true);
            break;
        case EDIT_MODE_CENTER_OVAL:
            ui->btnCenterOval->setChecked(true);
            break;
        case EDIT_MODE_3_POINT_OVAL:
            ui->btn3PointOval->setChecked(true);
            break;
        case EDIT_MODE_CENTER_ARC:
            ui->btnCenterArc->setChecked(true);
            break;
        case EDIT_MODE_3_POINT_ARC:
            ui->btn3PointArc->setChecked(true);
            break;
        case EDIT_MODE_POLYGON:
            ui->btnPolygon->setChecked(true);
            ui->sBPolygon->show();
            break;
        case EDIT_MODE_HANDLE_CURVE:
            ui->btnHandleCurve->setChecked(true);
            break;
        case EDIT_MODE_POINT_CURVE:
            ui->btnPointCurve->setChecked(true);
            break;
        case EDIT_MODE_POINT_HANDLE_CURVE:
            ui->btnPointHandleCurve->setChecked(true);
            break;
        default:
            break;
    }
}

void MainWindow::onLineWidthChange_triggered(int number) {
    m_lineWidth = number;
    updateLineWidth();
}

void MainWindow::onPolygonChange_triggered(int number) {
    ui->graphicsView->setPolygonNumber(number);
}

void MainWindow::initImageItemProperty()
{
    // LOCK/UNLOCK IMAGE
    ui->chkLockImage->setChecked(true);
    ui->graphicsView->onLockImage(true);

    // MAINTAIN ASPECT RATIO
    ui->chkAspectRatio->setChecked(true);
    ui->graphicsView->onMaintainAspectRatio(true);

    // SHOW/HIDE IMAGE
    ui->chkShowImage->setChecked(true);
    ui->graphicsView->onShowImage(true);

}

void MainWindow::onEditMode_triggered() {
    if ( ui->graphicsView->toggleEditMode()) {
        ui->btnPointSelect->released();
        ui->stackedWidget->setCurrentIndex(1);

        ui->widButtonLayer1->setEnabled(false);
        ui->widButtonLayer2->setEnabled(false);
        ui->widButtonLayer3->setEnabled(false);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
        ui->widButtonLayer1->setEnabled(true);
        ui->widButtonLayer2->setEnabled(true);
        ui->widButtonLayer3->setEnabled(true);
    }
}

void MainWindow::selectedShape(BaseShape* shape) {
    m_lineWidth = int(shape->getShapeLineWidth());
    m_shapeLineColor = shape->getShapeColor();
    updateShapeLineColor();
    ui->sBLineWidth->setValue(m_lineWidth);
}

void MainWindow::nodeCoordinatePopup(Node* node, QPoint point) {
    if(ui->graphicsView->isEditMode()) {
        return;
    }
    CoordinateForm dlg(this, ui->graphicsView->getGridSpacingSize());
    dlg.setCurrentNode(node);
    dlg.setScaleFactor(ui->graphicsView->getScaleFactor());
    dlg.setWindowFlags(Qt::Popup);
    dlg.setMaximumHeight(40);
    if((point.x() + dlg.width()) > mapToGlobal(this->rect().topRight()).x()) {
        point.setX(point.x() - dlg.width());
    }
    if((point.y() + dlg.rect().height()) > mapToGlobal(this->rect().bottomLeft()).y()) {
        point.setY(point.y() - dlg.rect().height() - 25);
    }
    dlg.move(point);

    dlg.exec();
    node->setEnabled(true);
}

void MainWindow::markerShow(Node* node, QPoint point) {
    if(ui->graphicsView->isEditMode()) {
        return;
    }
    MarkerDialog dlg(this);
    dlg.setCurrentNode(node);
    dlg.setCurrentMarkers(node->getMarkerList());
    dlg.setWindowFlags(Qt::Popup);
    if((point.x() + dlg.width()) > mapToGlobal(this->rect().topRight()).x()) {
        point.setX(point.x() - dlg.width());
    }
    if((point.y() + dlg.rect().height()) > mapToGlobal(this->rect().bottomLeft()).y()) {
        point.setY(point.y() - dlg.rect().height() - 25);
    }
    dlg.move(point);
    QObject::connect(&dlg, SIGNAL(selectMarker(Node*, int)), this, SLOT(onSelectMarker_triggered(Node*, int)));
    dlg.exec();

    node->setEnabled(true);
}

void MainWindow::onSelectMarker_triggered(Node* node, int number) {
    ui->graphicsView->selectMarker(node, number);
}

void MainWindow::onChooseLayerColor()
{
    CustomLayerButton* layerbutton = static_cast<CustomLayerButton*>(sender());
    if(!layerbutton)
        return;

    for(int i = 0 ; i < m_lstLayerButtons.count(); i++){
        if(layerbutton == m_lstLayerButtons.at(i)){
            QColor color = QColorDialog::getColor(layerbutton->getLayerColor(), parentWidget());

            if(color.isValid()){
                layerbutton->setLayerColor(color);
                ui->graphicsView->setLayerColor(i, color);
            }
            break;
        }
    }
}

void MainWindow::onSelectedLayer()
{
    CustomLayerButton* layerbutton = static_cast<CustomLayerButton*>(sender());
    if(!layerbutton)
        return;

    ui->widButtonLayer1->setChecked(false);
    ui->widButtonLayer2->setChecked(false);
    ui->widButtonLayer3->setChecked(false);
    layerbutton->setChecked(true);

    for(int i = 0 ; i < m_lstLayerButtons.count(); i++){
        if(layerbutton == m_lstLayerButtons.at(i)){
            ui->graphicsView->setCurrentLayerIndex(i);
            break;
        }
    }
}

void MainWindow::onUpdateShowState(bool flg)
{
    CustomLayerButton* layerbutton = static_cast<CustomLayerButton*>(sender());
    if(!layerbutton)
        return;

    for(int i = 0 ; i < m_lstLayerButtons.count(); i++){
        if(layerbutton == m_lstLayerButtons.at(i)){
            ui->graphicsView->showLayer(i, flg);
            break;
        }
    }
}

void MainWindow::onUpdateUnlockState(bool flg)
{
    CustomLayerButton* layerbutton = static_cast<CustomLayerButton*>(sender());
    if(!layerbutton)
        return;

    for(int i = 0 ; i < m_lstLayerButtons.count(); i++){
        if(layerbutton == m_lstLayerButtons.at(i)){
            ui->graphicsView->unlockLayer(i, flg);
            break;
        }
    }
}

void MainWindow::onNew_triggered(){
    // Open New-Dialog
    NewSceneDlg dlg(this);
    if(dlg.exec() == QDialog::Accepted){
        initImageItemProperty();

        ui->chkLockImage->setChecked(false);
        ui->graphicsView->onLockImage(false);

        // REACTOR MODE
        ui->btnReactorGuide->setChecked(false);
        ui->graphicsView->onReactorGuide(false);
        ui->widReactorPreview->drawReactorPreview();

        // Create new workspace with grid-size and spacing-size
        ui->graphicsView->onNew(dlg.getGridSize(), dlg.getSpacingSize());

        ui->widReactorPreview->setSpacingSize(ui->graphicsView->getSpacingSize());
        ui->widReactorPreview->setSpacingUnit(ui->graphicsView->getSpacingUnit());

    }
}

void MainWindow::onSave_triggered(){
    // Open Save-Dialog
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save File"),
                nullptr,
                tr("Image Tracer Files (*.itr)"),
                nullptr,
                nullptr);

    if (!fileName.isEmpty())
        ui->graphicsView->onSave(fileName);
}

void MainWindow::onOpen_triggered(){
    // Open Open-Dialog
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                nullptr,
                tr("Image Tracer Files (*.itr)"),
                nullptr,
                nullptr);


    if (!fileName.isEmpty()){
        initImageItemProperty();

        // REACTOR MODE
        ui->btnReactorGuide->setChecked(false);
        ui->graphicsView->onReactorGuide(false);
        ui->widReactorPreview->drawReactorPreview();

        ui->graphicsView->onOpen(fileName);

        ui->widReactorPreview->setSpacingSize(ui->graphicsView->getSpacingSize());
        ui->widReactorPreview->setSpacingUnit(ui->graphicsView->getSpacingUnit());

    }
}

void MainWindow::onImportImage_triggered(){
    // Open Open-Dialog
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Import an image"),
                nullptr,
                tr("Image Files (*.png *.jpg)"),
                nullptr,
                nullptr);

    if (!fileName.isEmpty()){
        initImageItemProperty();

        ui->graphicsView->onLoadImage(fileName);

        // Set Image Property
        ui->chkLockImage->setChecked(false);
        ui->graphicsView->onLockImage(ui->chkLockImage->isChecked());
        ui->graphicsView->onMaintainAspectRatio(ui->chkAspectRatio->isChecked());
        ui->graphicsView->onShowImage(ui->chkShowImage->isChecked());
    }
}


void MainWindow::onClearImage_triggered(){
    // Remove Image
    ui->graphicsView->onClearImage();
}

void MainWindow::updateLineWidth() {
    ui->graphicsView->setShapeLineWidth(m_lineWidth);
}

void MainWindow::onChooseShapeLineColor_triggered() {
    m_shapeLineColor = QColorDialog::getColor(m_shapeLineColor,parentWidget());

    if(m_shapeLineColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_shapeLineColor.name(), m_shapeLineColor.lighter(115).name(), m_shapeLineColor.lighter(120).name());
        ui->btnShapeLineColor->setStyleSheet(buttonStyle);
        updateShapeLineColor();
    }
}

void MainWindow::updateShapeLineColor() {
    if(m_shapeLineColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_shapeLineColor.name(), m_shapeLineColor.lighter(115).name(), m_shapeLineColor.lighter(120).name());
        ui->btnShapeLineColor->setStyleSheet(buttonStyle);
        ui->graphicsView->onUpdateShapeLineColor(m_shapeLineColor);
    }
}

void MainWindow::onChooseBackColor_triggered() {
    m_backgroundColor = QColorDialog::getColor(m_backgroundColor, parentWidget());

    if(m_backgroundColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_backgroundColor.name(), m_backgroundColor.lighter(115).name(), m_backgroundColor.lighter(120).name());
        ui->btnBackgroundColor->setStyleSheet(buttonStyle);
        updateBackColor();
    }
}

void MainWindow::updateBackColor(){
    if(m_backgroundColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_backgroundColor.name(), m_backgroundColor.lighter(115).name(), m_backgroundColor.lighter(120).name());
        ui->btnBackgroundColor->setStyleSheet(buttonStyle);
        ui->graphicsView->onUpdateBackColor(m_backgroundColor);
    }
}

void MainWindow::onSetVoxelColor_triggered() {
    if(m_voxelColor.isValid()){
        ui->graphicsView->addColorVoxel(m_voxelColor);
    }
}

void MainWindow::onRemoveVoxelColor_triggered() {
    ui->graphicsView->removeColorVoxel();
}

void MainWindow::onChooseVoxelColor_triggered() {
    QColor color = QColorDialog::getColor(m_voxelColor, parentWidget());

    if(color.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:disabled{ \
                background-color: rgb(50, 50, 50); \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(color.name(), color.lighter(115).name(), color.lighter(120).name());
        ui->btnColorVoxel->setStyleSheet(buttonStyle);
        m_voxelColor = color;
    }
}

void MainWindow::onChooseLineColor_triggered() {
    m_lineColor = QColorDialog::getColor(m_lineColor,parentWidget());

    if(m_lineColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_lineColor.name(), m_lineColor.lighter(115).name(), m_lineColor.lighter(120).name());
        ui->btnLineColor->setStyleSheet(buttonStyle);
        updateLineColor();
    }
}

void MainWindow::updateLineColor(){
    if(m_lineColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                background-color: %1; \
                border:none; \
                }\
                QPushButton:hover{ \
                background-color: %2; \
                border:none; \
                } \
                QPushButton:pressed{ \
                background-color: %3; \
                border:none; \
                }").arg(m_lineColor.name(), m_lineColor.lighter(115).name(), m_lineColor.lighter(120).name());
        ui->btnLineColor->setStyleSheet(buttonStyle);
        ui->graphicsView->onUpdateLineColor(m_lineColor);
    }
}

void MainWindow::onReactorGuide_triggered(bool f)
{
    // REACTOR GUIDE
    ui->widReactorPreview->drawReactorPreview();
    ui->graphicsView->onReactorGuide(f);
    ui->widReactorPreview->setSpacingSize(ui->graphicsView->getSpacingSize());
    ui->widReactorPreview->setSpacingUnit(ui->graphicsView->getSpacingUnit());

}

void MainWindow::onResetPoint_triggered() {
    ui->graphicsView->onResetPoint();
}

void MainWindow::onClearPoints_triggered() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Information", "Are you sure clear all nodes in current layer?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->graphicsView->onResetAllPoints();

        ui->btnReactorGuide->setChecked(false);
        ui->graphicsView->onReactorGuide(false);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Shift) {
        //ui->graphicsView->startSnapObject();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if(event->matches(QKeySequence::Undo)) {
        // Ctrl + Z
        ui->graphicsView->onUndo();
    } else if(event->matches(QKeySequence::Redo) || (event->key() == Qt::Key_R && event->modifiers() == Qt::ControlModifier)) {
        // Shift + Ctrl + Z, Ctrl + R
        ui->graphicsView->onRedo();
    } else if(event->matches(QKeySequence::Delete)) {
        ui->graphicsView->onDelete();
    } else if (event->key() == Qt::Key_Enter) {
    } else if (event->key() == Qt::Key_Shift) {
        ui->graphicsView->stopSnapObject();
    }
}
