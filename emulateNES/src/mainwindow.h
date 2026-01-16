#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ppu.h"
#include <QTextEdit>


class MyOpenGL;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void render_frame(const std::vector<std::vector<PPU::Color>>& frame_buffer, std::mutex& mutex_lock_frame_buffer);

    void render_debug_tiles(uint32_t* frame);
    void render_cpu_debug(QString text);
    void clear_cpu_debug();

public slots:
    void slot_show_error_message();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<MyOpenGL> my_openGL;
    std::vector<uint32_t> outBuffer;
    std::unique_ptr<MyOpenGL> debug_tiles_widget;
    std::unique_ptr<QTextEdit> cpu_debuger;

signals:
    void signal_init_new_cartridge(const QString& path);

};

#endif // MAINWINDOW_H
