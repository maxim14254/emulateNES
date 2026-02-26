#include "my_opengl.h"
#include <QSurfaceFormat>
#include <QDebug>
#include "mainwindow.h"
#include "global.h"



MyOpenGL::MyOpenGL(GLsizei _width, GLsizei _height, QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent)
{
    textureId = 0;
    nesFrame.resize(256 * 240);

    width1 = _width;
    height1 = _height;

    window = qobject_cast<MainWindow*>(parent);

    QTimer* timerFPS = new QTimer();
    timerFPS->setTimerType(Qt::CoarseTimer);

    connect(timerFPS, &QTimer::timeout, this, [&]()
    {
        update();
    });

    timerFPS->start(1000.0 / 60.0988);
}

MyOpenGL::~MyOpenGL()
{

}

void MyOpenGL::set_frame_buffer(std::vector<uint32_t>& frame_buffer)
{
    if(width1 == 256 && height1 == 240) // условие для отсевания дебажных экранов от главного
    {
        std::lock_guard lock(mutex_lock_frame_buffer);
        nesFrame.swap(frame_buffer);
    }
    else
        nesFrame.swap(frame_buffer);
}

void MyOpenGL::initializeGL()
{    
    initializeOpenGLFunctions();

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width1, height1, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void MyOpenGL::resizeGL(int w, int h)
{
    float targetAspect = 8.0 / 7.0;

    float widgetAspect = (h > 0) ? (float(w) / float(h)) : targetAspect;

    int vpW = w;
    int vpH = h;
    int vpX = 0;
    int vpY = 0;

    if (widgetAspect > targetAspect)
    {
        vpH = h;
        vpW = int(vpH * targetAspect);
        vpX = (w - vpW) / 2;
        vpY = 0;
    }
    else
    {
        vpW = w;
        vpH = int(vpW / targetAspect);
        vpX = 0;
        vpY = (h - vpH) / 2;
    }

    m_viewport = QRect(vpX, vpY, vpW, vpH);
    m_dpr = devicePixelRatioF();
}

void MyOpenGL::paintGL()
{
#ifdef DEBUG_ON
    static auto start_time = std::chrono::steady_clock::now();
    static auto time_update = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);
    auto elapsed_ms_up = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_update);

    start_time = std::chrono::steady_clock::now();

    if(window != nullptr && elapsed_ms.count() > 0 && elapsed_ms_up.count() > 1000 && width1 == 256 && height1 == 240)
    {
        window->show_real_FPS(elapsed_ms.count());
        time_update = std::chrono::steady_clock::now();
    }
#endif

    glClear(GL_COLOR_BUFFER_BIT);

    if (!nesFrame.data() || textureId == 0)
        return;

    if(width1 == 256 && height1 == 240 && false)
    {
        int fullW = int(width() * m_dpr);
        int fullH = int(height() * m_dpr);

        glViewport(0, 0, fullW, fullH);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        glViewport(int(m_viewport.x() * m_dpr),
                   int(m_viewport.y() * m_dpr),
                   int(m_viewport.width() * m_dpr),
                   int(m_viewport.height() * m_dpr));
    }

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, width1, height1,
                    GL_RGBA, GL_UNSIGNED_BYTE,
                    nesFrame.data());

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

    if(width1 == 256 && height1 == 240) // условие для отсевания дебажных экранов от главного
    {
        std::lock_guard<std::mutex> lg(update_frame_mutex);
        _update = true;

        cv.notify_one();
    }
}

