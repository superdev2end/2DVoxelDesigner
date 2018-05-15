#include "customlayerbutton.h"
#include "ui_customlayerbutton.h"

CustomLayerButton::CustomLayerButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomLayerButton)
{
    ui->setupUi(this);

    connect(ui->btnShow, &QToolButton::released,
            this, [=](){
        emit updatedShowState(isShow());
    });

    connect(ui->btnUnlock, &QToolButton::released,
            this, [=](){
        emit updatedUnlockState(isUnlock());
    });

    connect(ui->btnColor, &QPushButton::released,
            this, [=](){
        emit chooseColor();
    });
}

CustomLayerButton::~CustomLayerButton()
{
    delete ui;
}

void CustomLayerButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void CustomLayerButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    setChecked();
    emit clicked();
}

void CustomLayerButton::setChecked(bool flg)
{
    m_selected = flg;
    ui->widBody->setProperty("selected", flg);
    style()->unpolish(ui->widBody);
    style()->polish(ui->widBody);
}

bool CustomLayerButton::isChecked()
{
    return m_selected;
}

void CustomLayerButton::setLayerName(QString name)
{
    ui->lblLayerName->setText(name);
}

void CustomLayerButton::setLayerColor(QColor color)
{
    m_layerColor = color;
    QString buttonStyle = QString("QPushButton{ \
            background-color: %1; \
            border: 1px solid black; \
            }\
            QPushButton:hover{ \
            background-color: %2; \
            border: 2px solid black; \
            }\
            QPushButton:pressed{ \
            background-color: %3; \
            border: 1px solid black; \
            }").arg(color.name(), color.lighter(115).name(), color.lighter(120).name());
    ui->btnColor->setStyleSheet(buttonStyle);
}

void CustomLayerButton::setShow(bool flg)
{
    ui->btnShow->setChecked(flg);
}

bool CustomLayerButton::isShow()
{
    return ui->btnShow->isChecked();
}

void CustomLayerButton::setUnlock(bool flg)
{
    ui->btnUnlock->setChecked(flg);
}

bool CustomLayerButton::isUnlock()
{
    return ui->btnUnlock->isChecked();
}
