#include "my_opengl.h"
#include <QSurfaceFormat>
#include <QDebug>



MyOpenGL::MyOpenGL(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent)
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);
}

MyOpenGL::~MyOpenGL()
{

}


void MyOpenGL::initializeGL()
{
    qDebug() << "initializeGL";
    initializeOpenGLFunctions();
    glClearColor(0.f, 0.f, 0.f, 1.f); // Чёрный фон
}

void MyOpenGL::resizeGL(int w, int h)
{
    qDebug() << "resizeGL" << w << h;
    glViewport(0, 0, w, h);
}

void MyOpenGL::paintGL()
{
    qDebug() << "paintGL";
    glClear(GL_COLOR_BUFFER_BIT);
}

