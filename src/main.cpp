#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"
#include <QSurfaceFormat>
#include "my_opengl.h"


int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    MyOpenGL ll;
    ll.resize(800, 600);
    ll.show();

    CPU cpu;
    bool rez = cpu.init_new_cartridge(":/games/Super Mario Bros. (World).nes");

    return a.exec();
}
