#ifndef SELECT_KEY_WGT_H
#define SELECT_KEY_WGT_H

#include <QDialog>
#include <QMap>


class QToolButton;

namespace Ui {
class SelectKeyWgt;
}

class SelectKeyWgt : public QDialog
{
    Q_OBJECT

public:
    explicit SelectKeyWgt(std::map<int, int>& keys, QWidget *parent = nullptr);
    ~SelectKeyWgt();

    void setCurrentButton(QToolButton* _button) { button = _button; }
    void setCurrentButton(QToolButton* _button, int value);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private:
    Ui::SelectKeyWgt *ui;

    QToolButton* button = nullptr;
    std::map<int, int>& keys;

    QMap<Qt::Key, QString> keyMap =
    {
       {Qt::Key_Control, "Ctrl"},
       {Qt::Key_Shift,   "Shift"},
       {Qt::Key_Alt,     "Alt"},
       {Qt::Key_Meta,    "Meta"},
       {Qt::Key_Escape,  "Esc"},
   };

signals:
    void changeParams(QToolButton* button, int key);

};

#endif // SELECT_KEY_WGT_H
