#ifndef COORDINATEFORM_H
#define COORDINATEFORM_H

#include <QDialog>
#include "node.h"

namespace Ui {
class CoordinateForm;
}

class CoordinateForm : public QDialog
{
    Q_OBJECT

public:
    explicit CoordinateForm(QWidget *parent = nullptr, int = 0);
    ~CoordinateForm() override;

    void        setCurrentNode(Node* node);
    void        setScaleFactor(float);

protected:
    void        resizeEvent(QResizeEvent*) override;
    void        keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::CoordinateForm *ui;
    int m_gridSize;
    Node* m_currentNode;
    float m_scaleFactor;

public slots:
    void textChangedX();
    void textChangedY();
};

#endif // COORDINATEFORM_H
