#include "coordinateform.h"
#include "ui_coordinateform.h"
#include "qevent.h"

CoordinateForm::CoordinateForm(QWidget *parent, int gridSize) :
    QDialog(parent),
    ui(new Ui::CoordinateForm)
{
    ui->setupUi(this);

    m_currentNode = nullptr;
    m_gridSize = gridSize * 1.5 / 10;

    connect(ui->tEX, SIGNAL(textChanged()), this, SLOT(textChangedX()));
    connect(ui->tEY, SIGNAL(textChanged()), this, SLOT(textChangedY()));
}

CoordinateForm::~CoordinateForm() {
    delete ui;
}

void CoordinateForm::textChangedX() {
    QString textContent = ui->tEX->toPlainText();
    int length = textContent.count();
    if(ui->tEX->toPlainText().toInt() > m_gridSize || ui->tEX->toPlainText().toInt() < -m_gridSize || ui->tEX->toPlainText() != QString::number(ui->tEX->toPlainText().toInt())) {
        if(length > 0 && ui->tEX->toPlainText() != "-") {
            int position = ui->tEX->textCursor().position();
            QTextCursor textCursor = ui->tEX->textCursor();
            textContent.remove(position-1, 1);
            ui->tEX->setText(textContent);
            textCursor.setPosition(position-1);
            ui->tEX->setTextCursor(textCursor);
        }
    }
}

void CoordinateForm::textChangedY() {
    QString textContent = ui->tEY->toPlainText();
    int length = textContent.count();
    if(ui->tEY->toPlainText().toInt() > m_gridSize || ui->tEY->toPlainText().toInt() < -m_gridSize || ui->tEY->toPlainText() != QString::number(ui->tEY->toPlainText().toInt())) {
        if(length > 0 && ui->tEY->toPlainText() != "-") {
            int position = ui->tEY->textCursor().position();
            QTextCursor textCursor = ui->tEY->textCursor();
            textContent.remove(position-1, 1);
            ui->tEY->setText(textContent);
            textCursor.setPosition(position-1);
            ui->tEY->setTextCursor(textCursor);
        }
    }
}

void  CoordinateForm::resizeEvent(QResizeEvent*) {
    this->setMask(QRegion(this->rect()));
}

void CoordinateForm::setCurrentNode(Node* node) {
    m_currentNode = node;
}

void CoordinateForm::setScaleFactor(float scale) {
    m_scaleFactor = scale;
}

void CoordinateForm::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        hide();
        if(m_currentNode != nullptr) {
            m_currentNode->setLocalPos(QPoint(ui->tEX->toPlainText().toInt(), -ui->tEY->toPlainText().toInt()));
            m_currentNode->updateNode();
        }
    }
}
