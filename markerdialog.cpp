#include "markerdialog.h"
#include "ui_markerdialog.h"

MarkerDialog::MarkerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarkerDialog)
{
    m_currentNode = nullptr;
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(setMarker_triggered(int)), this, SLOT(setMarker(int)));
    QObject::connect(ui->btnMaker1, &QPushButton::released, [=]() {emit setMarker_triggered(0);});
    QObject::connect(ui->btnMaker2, &QPushButton::released, [=]() {emit setMarker_triggered(1);});
    QObject::connect(ui->btnMaker3, &QPushButton::released, [=]() {emit setMarker_triggered(2);});
    QObject::connect(ui->btnMaker4, &QPushButton::released, [=]() {emit setMarker_triggered(3);});
    QObject::connect(ui->btnMaker5, &QPushButton::released, [=]() {emit setMarker_triggered(4);});
    QObject::connect(ui->btnMaker6, &QPushButton::released, [=]() {emit setMarker_triggered(5);});
    QObject::connect(ui->btnMaker7, &QPushButton::released, [=]() {emit setMarker_triggered(6);});
    QObject::connect(ui->btnMaker8, &QPushButton::released, [=]() {emit setMarker_triggered(7);});
    QObject::connect(ui->btnMaker9, &QPushButton::released, [=]() {emit setMarker_triggered(8);});
    QObject::connect(ui->btnMaker10, &QPushButton::released, [=]() {emit setMarker_triggered(9);});
    QObject::connect(ui->btnClearMarker, &QPushButton::released, [=]() {emit setMarker_triggered(-1);});
}

MarkerDialog::~MarkerDialog()
{
    delete ui;
}

void  MarkerDialog::resizeEvent(QResizeEvent*) {
    this->setMask(QRegion(this->rect()));
}

void MarkerDialog::setCurrentMarkers(QList<int> markers) {
    QList<QPushButton*> list;
    list.clear();
    list.push_back(ui->btnMaker1);
    list.push_back(ui->btnMaker2);
    list.push_back(ui->btnMaker3);
    list.push_back(ui->btnMaker4);
    list.push_back(ui->btnMaker5);
    list.push_back(ui->btnMaker6);
    list.push_back(ui->btnMaker7);
    list.push_back(ui->btnMaker8);
    list.push_back(ui->btnMaker8);
    list.push_back(ui->btnMaker10);

    m_markerList = markers;
    int count = 0;
    for (int i = 0; i < markers.size(); ++i) {
        if(markers.at(i) != -1) {
            list.at(markers.at(i))->setFlat(true);
            list.at(markers.at(i))->setDisabled(true);
            count ++;
        }
    }
    if(count == 0) {
        ui->btnClearMarker->setFlat(true);
        ui->btnClearMarker->setDisabled(true);
    } else {
        ui->btnClearMarker->setFlat(false);
        ui->btnClearMarker->setDisabled(false);
    }
}

void MarkerDialog::setCurrentNode(Node* node) {
    m_currentNode = node;
}

void MarkerDialog::setMarker(int number) {
    if(m_currentNode == nullptr) {
        return;
    }
    if(number == -1) {
        m_currentNode->clearMarker();
    } else {
        m_currentNode->addMarker(number);
    }
    emit selectMarker(m_currentNode, number);
    this->close();
}
