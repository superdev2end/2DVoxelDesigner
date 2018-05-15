#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QColorDialog>
#include <QDate>
#include <QSpinBox>

#include "graphicsview.h"
#include "newscenedlg.h"
#include "customlayerbutton.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void updateEditMode(int editMode);

private:
    void updateLineColor();
    void updateShapeLineColor();
    void initImageItemProperty();
    void updateBackColor();
    void updateLineWidth();
    void tabSelected();

private slots:
    // Button event's handler
    void onNew_triggered();
    void onEditMode_triggered();
    void onSave_triggered();
    void onOpen_triggered();
    void onImportImage_triggered();
    void onClearImage_triggered();
    void onChooseLineColor_triggered();
    void onChooseVoxelColor_triggered();
    void onSetVoxelColor_triggered();
    void onRemoveVoxelColor_triggered();
    void onChooseBackColor_triggered();
    void onChooseShapeLineColor_triggered();
    void onReactorGuide_triggered(bool);
    void onResetPoint_triggered();
    void onClearPoints_triggered();
    void onUpdateEditMode_triggered(int);
    void onPolygonChange_triggered(int);
    void onLineWidthChange_triggered(int);
    void markerShow(Node* node, QPoint point);
    void nodeCoordinatePopup(Node*, QPoint);
    void selectedShape(BaseShape* shape);
    void onMaintainAspectRatio(bool);
    void onLockImage(bool);
    void onShowImage(bool);

    /// Layer Manager
    void onSelectedLayer();
    void onUpdateShowState(bool flg);
    void onUpdateUnlockState(bool flg);
    void onChooseLayerColor();

public slots:
    void onSelectMarker_triggered(Node*, int);

protected:
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *ui;
    QColor          m_lineColor;
    QColor          m_shapeLineColor;
    int             m_lineWidth;
    QColor          m_backgroundColor;
    QColor          m_voxelColor;

    QList<CustomLayerButton*>       m_lstLayerButtons;
};

#endif // MAINWINDOW_H
