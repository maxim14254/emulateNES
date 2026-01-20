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

    my_openGL.reset(new MyOpenGL(256, 240, this));
    my_openGL->setMinimumSize(800, 600);
    ui->verticalLayout->addWidget(my_openGL.get());

    outBuffer.resize(256 * 240);

#ifdef DEBUG_ON

    debug_tiles_widget1.reset(new MyOpenGL(128, 128, this));
    debug_tiles_widget1->setFixedSize(300, 300);
    ui->horizontalLayout_2->addWidget(debug_tiles_widget1.get());

    debug_tiles_widget2.reset(new MyOpenGL(128, 128, this));
    debug_tiles_widget2->setFixedSize(300, 300);
    ui->horizontalLayout_2->addWidget(debug_tiles_widget2.get());

    cpu_debuger.reset(new QTextEdit(this));
    cpu_debuger->setPlainText("");
    cpu_debuger->setFixedSize(610, 610);
    cpu_debuger->setTextInteractionFlags(Qt::NoTextInteraction);
    ui->verticalLayout_3->addWidget(cpu_debuger.get());

    ui->verticalLayout_3->addStretch();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render_frame(const std::vector<std::vector<PPU::Color>>& frame_buffer, std::mutex& mutex_lock_frame_buffer)
{   
    {
        std::lock_guard lock(mutex_lock_frame_buffer);

        for(size_t y = 0; y < frame_buffer.size(); ++y)
        {
            for(size_t x = 0; x < frame_buffer[y].size(); ++x)
            {
                PPU::Color color = frame_buffer[y][x];

                uint32_t pixel = (static_cast<uint32_t>(color.r) << 16) | (static_cast<uint32_t>(color.g) << 8) | (static_cast<uint32_t>(color.b) << 0);

                outBuffer[y * 256 + x] = pixel;
            }
        }
    }

    my_openGL->set_frame_buffer(outBuffer.data());
}

void MainWindow::render_debug_tiles(uint32_t *frame1, uint32_t *frame2)
{
    debug_tiles_widget1->set_frame_buffer(frame1);
    debug_tiles_widget2->set_frame_buffer(frame2);
}

void MainWindow::render_cpu_debug(QString text)
{
    cpu_debuger->setText(text);
}

void MainWindow::clear_cpu_debug()
{
    cpu_debuger->clear();
}

void MainWindow::slot_show_error_message()
{
    QMessageBox message(QMessageBox::Icon::Critical, "Error", "Ошибка эмуляции KIL", QMessageBox::StandardButton::Ok);
    message.exec();
}
