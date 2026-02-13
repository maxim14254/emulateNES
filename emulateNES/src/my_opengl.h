#ifndef MY_OPENGL_H
#define MY_OPENGL_H

#include <QtOpenGL/QtOpenGL>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include "ppu.h"


class MyOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    explicit MyOpenGL(GLsizei _width, GLsizei _height, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~MyOpenGL();

    void set_frame_buffer(std::vector<uint32_t>& frame_buffer);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    GLuint textureId;
    std::vector<uint32_t> nesFrame;
    GLsizei width1, height1;

    QRect m_viewport;
    qreal m_dpr = 1.0;

    MainWindow* window;

};

#endif // MY_OPENGL_H
