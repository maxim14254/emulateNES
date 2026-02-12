#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_opengl.h"
#include <QSurface>
#include <qmessagebox.h>
#include <QApplication>
#include "global.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/nintendoNES.ico"));

    my_openGL.reset(new MyOpenGL(256, 240, this));
    my_openGL->setMinimumSize(800, 600);
    ui->verticalLayout->addWidget(my_openGL.get());

    ui->cpu_debuger->setVisible(false);

    outBuffer.resize(256 * 240);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    ui->widget_2->setVisible(false);

#ifdef DEBUG_ON
    ui->widget_2->setVisible(true);

    debug_tiles_widget1.reset(new MyOpenGL(128, 128, this));
    debug_tiles_widget1->setFixedSize(300, 300);
    ui->horizontalLayout_2->addWidget(debug_tiles_widget1.get());

    debug_tiles_widget2.reset(new MyOpenGL(128, 128, this));
    debug_tiles_widget2->setFixedSize(300, 300);
    ui->horizontalLayout_2->addWidget(debug_tiles_widget2.get());

    debug_palettes_widget.reset(new MyOpenGL(610, 26, this));
    debug_palettes_widget->setFixedSize(610, 20);
    ui->horizontalLayout_5->addWidget(debug_palettes_widget.get());

    ui->cpu_debuger->setVisible(true);
    ui->cpu_debuger->setPlainText("");
    ui->cpu_debuger->setFixedSize(350, 425);
    ui->cpu_debuger->setTextInteractionFlags(Qt::NoTextInteraction);

    ui->verticalLayout_3->addStretch();

#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render_frame(std::vector<uint32_t>& frame_buffer)
{
    my_openGL->set_frame_buffer(frame_buffer);
}

void MainWindow::render_debug_tiles(std::vector<uint32_t>& frame1, std::vector<uint32_t>& frame2)
{
    debug_tiles_widget1->set_frame_buffer(frame1);
    debug_tiles_widget2->set_frame_buffer(frame2);
}

void MainWindow::render_debug_palettes(std::vector<uint32_t> &frame)
{
    debug_palettes_widget->set_frame_buffer(frame);
}

void MainWindow::render_cpu_debug(const QString& text,uint8_t PPUCTRL, uint8_t PPUMASK, uint8_t PPUSTATUS, uint8_t OAMADDR, uint8_t OAMDATA, uint8_t PPUSCROLL, uint8_t PPUDATA, uint16_t PPUADDR,
                                  uint16_t PC, uint8_t SP, uint8_t status, uint8_t A, uint8_t X, uint8_t Y)
{    
    ui->cpu_debuger->setText(text);

    ui->label_PC->setText(QString("%1").arg(PC, 4, 16, QChar('0')).toUpper());
    ui->label_SP->setText(QString("%1").arg(SP, 2, 16, QChar('0')).toUpper());
    ui->label_status->setText(QString("%1").arg(status, 8, 2, QChar('0')));
    ui->label_A->setText(QString("%1").arg(A, 2, 16, QChar('0')).toUpper());
    ui->label_X->setText(QString("%1").arg(X, 2, 16, QChar('0')).toUpper());
    ui->label_Y->setText(QString("%1").arg(Y, 2, 16, QChar('0')).toUpper());

    ui->label_ppustatus_2->setText(QString("%1").arg(PPUSTATUS, 2, 16, QChar('0')).toUpper());
    ui->label_ppuctrl->setText(QString("%1").arg(PPUCTRL, 2, 16, QChar('0')).toUpper());
    ui->label_ppumask->setText(QString("%1").arg(PPUMASK, 2, 16, QChar('0')).toUpper());
    ui->label_oamddr->setText(QString("%1").arg(OAMADDR, 2, 16, QChar('0')).toUpper());
    ui->label_oamdata->setText(QString("%1").arg(OAMDATA, 2, 16, QChar('0')).toUpper());
    ui->label_ppuscroll->setText(QString("%1").arg(PPUSCROLL, 2, 16, QChar('0')).toUpper());
    ui->label_ppudata->setText(QString("%1").arg(PPUDATA, 2, 16, QChar('0')).toUpper());
    ui->label_ppuadr->setText(QString("%1").arg(PPUADDR, 4, 16, QChar('0')).toUpper());

}

void MainWindow::render_sprites_debug(std::vector<uint8_t> &oam)
{
    QString text;

    for(int i = 0; i < oam.size(); i += 4)
    {
        text += QString("%5.   Y = %1  X = %2  ID = %3  AT = %4\n").arg(oam[i], 3, 10, QChar('0')).arg(oam[i + 3], 3, 10, QChar('0')).
                                                                    arg(oam[i + 1], 2, 16, QChar('0')).toUpper().
                                                                    arg(oam[i + 2], 2, 16, QChar('0')).toUpper().
                                                                    arg(i / 4, 2, 10, QChar('0'));
    }

    ui->sprites_debug->setText(text);
}

void MainWindow::clear_cpu_debug()
{
    ui->cpu_debuger->clear();
}

void MainWindow::show_real_FPS(int microsec)
{
    ui->label_read_FPS->setText(QString("%1").arg(1000000 / microsec));
}

void MainWindow::slot_show_error_message()
{
    QMessageBox message(QMessageBox::Icon::Critical, "Error", "Ошибка эмуляции KIL", QMessageBox::StandardButton::Ok);
    message.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

#ifdef DEBUG_ON

    if(e->key() == Qt::Key_Space)
    {
        {
            run_without_mutex = false;
            std::lock_guard<std::mutex> lg(step_by_step_mutex);
            pause = false;
        }

        cv.notify_one();
    }
    else if(e->key() == Qt::Key_Shift)
    {
        {
            std::lock_guard<std::mutex> lg(step_by_step_mutex);
            pause = false;
            run_without_mutex = !run_without_mutex;
        }

        cv.notify_one();
    }

#endif

    emit signal_press_key((Qt::Key)e->key());

    QMainWindow::keyPressEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

#ifdef DEBUG_ON
    {
        std::lock_guard<std::mutex> lg(step_by_step_mutex);
        pause = false;
    }

    cv.notify_one();
#endif

    {
        std::lock_guard<std::mutex> lg(update_frame_mutex);
        _update = true;
    }
    cv.notify_one();

    start = false;

    QMainWindow::closeEvent(event);
}
