#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_opengl.h"
#include <QSurface>
#include <qmessagebox.h>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/nintendoNES.ico"));

    ui->verticalLayout->addWidget(new MyOpenGL(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_show_error_message()
{
    QMessageBox message(QMessageBox::Icon::Critical, "Error", "Ошибка эмуляции KIL", QMessageBox::StandardButton::Ok);
    message.exec();
}
