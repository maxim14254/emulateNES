#ifndef MY_OPENGL_H
#define MY_OPENGL_H

#include <QtOpenGL/QtOpenGL>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>


class MyOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    explicit MyOpenGL(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~MyOpenGL();


protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

};

#endif // MY_OPENGL_H
