#include "select_key_wgt.h"
#include "ui_select_key_wgt.h"
#include <QToolButton>
#include <QKeyEvent>


SelectKeyWgt::SelectKeyWgt(std::map<int, int>& _keys, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectKeyWgt),
    keys(_keys)
{
    ui->setupUi(this);
}

SelectKeyWgt::~SelectKeyWgt()
{
    delete ui;
}

void SelectKeyWgt::setCurrentButton(QToolButton *_button, int value)
{
    QString value1;

    if (keyMap.contains((Qt::Key)value))
        value1 = keyMap.value((Qt::Key)value);
    else
        value1 = QKeySequence(value).toString(QKeySequence::NativeText);

    _button->setText(value1);
}

void SelectKeyWgt::keyPressEvent(QKeyEvent *e)
{
    setCurrentButton(button, e->key());

    changeParams(button, e->key());

    setVisible(false);
    button = nullptr;
}

