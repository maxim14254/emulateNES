#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_opengl.h"


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
