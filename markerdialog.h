#ifndef MARKERDIALOG_H
#define MARKERDIALOG_H

#include <QDialog>
#include "node.h"
#include "markeritem.h"

namespace Ui {
class MarkerDialog;
}

class MarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerDialog(QWidget *parent = nullptr);
    ~MarkerDialog() override;

    void setCurrentNode(Node*);
    void setCurrentMarkers(QList<int>);

protected:
    void  resizeEvent(QResizeEvent*) override;

public:
signals:
    void selectMarker(Node*, int);

private:
signals:
    void setMarker_triggered(int);

private slots:
    void setMarker(int);

private:
    Ui::MarkerDialog *ui;

    Node* m_currentNode;
    QList<int> m_markerList;
};

#endif // MARKERDIALOG_H
