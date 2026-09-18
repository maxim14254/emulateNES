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

    ui->stackedWidget->setCurrentIndex(0);

    setWindowIcon(QIcon(":/nintendoNES.ico"));

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setWrapping(true);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setUniformItemSizes(true);
    ui->listWidget->setIconSize(QSize(250, 250));
    ui->listWidget->setGridSize(QSize(300, 300));
    ui->listWidget->setSpacing(8);
    ui->listWidget->setWordWrap(true);
    ui->listWidget->setStyleSheet(R"(QListWidget::item {
                                        border-radius: 8px;
                                        padding: 4px;
                                    }
                                    QListWidget::item:selected {
                                        background: #501b6f96;
                                        color: white;
                                        border-radius: 8px;
                                    }
                                    QListWidget::item:hover {
                                        background: #502a3f55;
                                        color: #ffffff;
                                        border-radius: 8px;
                                    }
                                )");


    QSize size(970, 800);

    #ifdef DEBUG_ON
    size.setWidth(800);
    size.setHeight(600);
    #endif

    my_openGL.reset(new MyOpenGL(256, 240, this));
    my_openGL->setMinimumSize(size);
    ui->verticalLayout_10->addWidget(my_openGL.get());

    ui->cpu_debuger->setVisible(false);

    outBuffer.resize(256 * 240);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    ui->widget_4->setVisible(false);

    connect(ui->restart, &QAction::triggered, this, [&]()
    {
        std::lock_guard<std::mutex> lg(update_frame_mutex);
        _update = true;
        cv.notify_one();
        start = false;

        QMetaObject::invokeMethod(this, [&]()
                                  {
                                    emit signal_restart();
                                  },
                                  Qt::QueuedConnection);
    });

    connect(ui->exit, &QAction::triggered, this, [&]()
    {
        this->close();
    });

    connect(ui->pick_game, &QAction::triggered, this, [&]()
    {
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->params, &QAction::triggered, this, [&]()
    {
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(ui->listWidget, &QListWidget::itemActivated, this, [&](QListWidgetItem* item)
    {
        if (!item)
            return;

        QString path = QString(":/games/%1.nes").arg(item->text());

        ui->stackedWidget->setCurrentIndex(0);

        std::lock_guard<std::mutex> lg(update_frame_mutex);
        _update = true;
        cv.notify_one();
        start = false;

        QMetaObject::invokeMethod(this, [&, path = std::move(path)]()
                                  {
                                      emit signal_init_new_cartridge(path);
                                  },
                                  Qt::QueuedConnection);
    });


    QDir dir(":/games");
    for (const QFileInfo& fi : dir.entryInfoList(QDir::Files))
    {
        auto* it = new QListWidgetItem(QIcon(QString(":/games/images/%1.png").arg(fi.baseName())), QString("%1").arg(fi.baseName()));
        it->setTextAlignment(Qt::AlignCenter);
        it->setSizeHint(QSize(300, 300));

        ui->listWidget->addItem(it);
    }

#ifdef DEBUG_ON
    ui->widget_4->setVisible(true);

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
    ui->cpu_debuger->setFixedWidth(370);
    ui->cpu_debuger->setTextInteractionFlags(Qt::NoTextInteraction);

    ui->verticalLayout_3->addStretch();


#else
    delete ui->verticalLayout_6;
    resize(850, 750);
#endif

    QRect screen = QGuiApplication::primaryScreen()->availableGeometry();
    move(screen.center() - rect().center());
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
#ifdef DEBUG_ON
    debug_tiles_widget1->set_frame_buffer(frame1);
    debug_tiles_widget2->set_frame_buffer(frame2);
#endif
}

void MainWindow::render_debug_palettes(std::vector<uint32_t> &frame)
{
#ifdef DEBUG_ON
    debug_palettes_widget->set_frame_buffer(frame);
#endif
}

void MainWindow::render_cpu_debug(const QString& text,uint8_t PPUCTRL, uint8_t PPUMASK, uint8_t PPUSTATUS, uint8_t OAMADDR, uint8_t OAMDATA, uint8_t PPUSCROLL, uint8_t PPUDATA, uint16_t PPUADDR,
                                  uint16_t PC, uint8_t SP, uint8_t status, uint8_t A, uint8_t X, uint8_t Y, int16_t scanline, int16_t cycles)
{
#ifdef DEBUG_ON
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

    ui->toolButton_2->setText(QString("%1  %2").arg(">>").arg(scanline));
    ui->lineEdit_2->setText(QString("%1").arg(cycles));
#endif
}

void MainWindow::render_sprites_debug(std::vector<uint8_t> &oam)
{
#ifdef DEBUG_ON
    QString text;

    for(int i = 0; i < oam.size(); i += 4)
    {
        text += QString("%5.   Y = %1  X = %2  ID = %3  AT = %4\n").arg(oam[i], 3, 10, QChar('0')).arg(oam[i + 3], 3, 10, QChar('0')).
                arg(oam[i + 1], 2, 16, QChar('0')).toUpper().
                arg(oam[i + 2], 2, 16, QChar('0')).toUpper().
                arg(i / 4, 2, 10, QChar('0'));
    }

    ui->sprites_debug->setText(text);
#endif
}

void MainWindow::clear_cpu_debug()
{
#ifdef DEBUG_ON
    ui->cpu_debuger->clear();
#endif
}

void MainWindow::show_real_FPS(int microsec)
{
#ifdef DEBUG_ON
    ui->label_read_FPS->setText(QString("%1").arg(1000000 / microsec));
#endif
}

void MainWindow::show_text(const QString& text)
{
    my_openGL->show_text(text);
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
            run_without_ppu_mutex = false;
            std::lock_guard<std::mutex> lg(step_by_step_ppu_mutex);
            pause_ppu = false;

            if(!run_without_cpu_mutex)
            {
                std::lock_guard<std::mutex> lg(step_by_step_cpu_mutex);
                pause_cpu = false;
                run_without_cpu_mutex = !run_without_cpu_mutex;

            }

            if(!run_without_scanline_mutex)
            {
                std::lock_guard<std::mutex> lg(step_by_step_scanline_mutex);
                pause_scanline = false;
                run_without_scanline_mutex = !run_without_scanline_mutex;
            }
        }

        cv.notify_one();
    }
    else if(e->key() == Qt::Key_Shift)
    {
        if(!run_without_cpu_mutex)
        {
            std::lock_guard<std::mutex> lg(step_by_step_cpu_mutex);
            pause_cpu = false;
            run_without_cpu_mutex = !run_without_cpu_mutex;
        }

        if(!run_without_scanline_mutex)
        {
            std::lock_guard<std::mutex> lg(step_by_step_scanline_mutex);
            pause_scanline = false;
            run_without_scanline_mutex = !run_without_scanline_mutex;
        }

        //if(!run_without_ppu_mutex)
        {
            std::lock_guard<std::mutex> lg(step_by_step_ppu_mutex);
            pause_ppu = false;
            run_without_ppu_mutex = !run_without_ppu_mutex;
        }

        cv.notify_one();
    }
    else if(e->key() == Qt::Key_Plus)
    {
        ++numb_table_for_debug;
    }
    else if(e->key() == Qt::Key_Minus)
    {
        --numb_table_for_debug;
    }

#endif

    emit signal_press_key(e->key());

    QMainWindow::keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    emit signal_release_key(event->key());

    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

#ifdef DEBUG_ON
    {
        std::lock_guard<std::mutex> lg(step_by_step_ppu_mutex);
        pause_ppu = false;

        std::lock_guard<std::mutex> lg1(step_by_step_scanline_mutex);
        pause_scanline = false;

        std::lock_guard<std::mutex> lg2(step_by_step_cpu_mutex);
        pause_cpu = false;
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

void MainWindow::on_toolButton_clicked()
{
#ifdef DEBUG_ON
    if(!run_without_ppu_mutex)
    {
        std::lock_guard<std::mutex> lg(step_by_step_ppu_mutex);
        pause_ppu = false;
        run_without_ppu_mutex = !run_without_ppu_mutex;
    }

    if(!run_without_scanline_mutex)
    {
        std::lock_guard<std::mutex> lg(step_by_step_scanline_mutex);
        pause_scanline = false;
        run_without_scanline_mutex = !run_without_scanline_mutex;
    }

    {
        run_without_cpu_mutex = false;
        std::lock_guard<std::mutex> lg(step_by_step_cpu_mutex);
        pause_cpu = false;
    }

    cv.notify_one();
#endif
}


void MainWindow::on_toolButton_2_clicked()
{
#ifdef DEBUG_ON
    if(!run_without_ppu_mutex)
    {
        std::lock_guard<std::mutex> lg(step_by_step_ppu_mutex);
        pause_ppu = false;
        run_without_ppu_mutex = !run_without_ppu_mutex;
    }

    if(!run_without_cpu_mutex)
    {
        std::lock_guard<std::mutex> lg(step_by_step_cpu_mutex);
        pause_cpu = false;
        run_without_cpu_mutex = !run_without_cpu_mutex;
    }

    {
        run_without_scanline_mutex = false;
        std::lock_guard<std::mutex> lg(step_by_step_scanline_mutex);
        pause_scanline = false;
    }

    if(!ui->lineEdit->text().isEmpty())
    {
        go_scanline = ui->lineEdit->text().toInt();
        ui->lineEdit->setText(QString("%1").arg(go_scanline + 1));
    }
    else
        go_scanline = -1;

    cv.notify_one();
#endif
}


void MainWindow::on_back_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_back_btn_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

