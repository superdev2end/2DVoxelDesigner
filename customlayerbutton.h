#ifndef CUSTOMLAYERBUTTON_H
#define CUSTOMLAYERBUTTON_H

#include <QWidget>
#include <QStyle>

namespace Ui {
class CustomLayerButton;
}

class CustomLayerButton : public QWidget
{
    Q_OBJECT

public:
    explicit CustomLayerButton(QWidget *parent = nullptr);
    ~CustomLayerButton();

    void    setLayerColor(QColor color);
    QColor  getLayerColor(){return m_layerColor; }
    void    setChecked(bool flg = true);
    void    setLayerName(QString name);
    void    setShow(bool flg = true);
    void    setUnlock(bool flg = true);

    bool    isChecked();
    bool    isShow();
    bool    isUnlock();
signals:
    void    clicked();
    void    updatedShowState(bool flg);
    void    updatedUnlockState(bool flg);
    void    chooseColor();

protected:
    void    mousePressEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::CustomLayerButton *ui;

    bool    m_selected;
    QColor  m_layerColor;
};

#endif // CUSTOMLAYERBUTTON_H
