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

    ui->verticalLayout->addWidget(my_openGL.get());

    outBuffer.resize(256 * 240);

    debug_tiles_widget.reset(new MyOpenGL(128, 128, this));
    debug_tiles_widget->setFixedSize(300, 300);
    ui->verticalLayout_3->addWidget(debug_tiles_widget.get());
    ui->verticalLayout_3->addStretch();
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

void MainWindow::render_debug_tiles(uint32_t *frame)
{
    debug_tiles_widget->set_frame_buffer(frame);
}

void MainWindow::slot_show_error_message()
{
    QMessageBox message(QMessageBox::Icon::Critical, "Error", "Ошибка эмуляции KIL", QMessageBox::StandardButton::Ok);
    message.exec();
}
