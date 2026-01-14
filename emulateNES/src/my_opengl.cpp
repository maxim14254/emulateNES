#include "my_opengl.h"
#include <QSurfaceFormat>
#include <QDebug>



MyOpenGL::MyOpenGL(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent)
{
    textureId = 0;
    nesFrame = nullptr;
}

MyOpenGL::~MyOpenGL()
{

}

void MyOpenGL::set_frame_buffer(uint32_t* frame_buffer)
{
    nesFrame = frame_buffer;
    update();
}

void MyOpenGL::initializeGL()
{    
    initializeOpenGLFunctions();

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 256, 240, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void MyOpenGL::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void MyOpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT);

    if (!nesFrame || textureId == 0)
        return;

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, 256, 240,
                    GL_RGBA, GL_UNSIGNED_BYTE,
                    nesFrame);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

