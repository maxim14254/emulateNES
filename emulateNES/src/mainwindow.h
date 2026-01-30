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

    void render_frame(std::vector<uint32_t>& frame_buffer);

    void render_debug_tiles(std::vector<uint32_t>& frame1, std::vector<uint32_t>& frame2);
    void render_debug_palettes(std::vector<uint32_t>& frame);
    void render_cpu_debug(const QString& text, uint8_t PPUCTRL, uint8_t PPUMASK, uint8_t PPUSTATUS, uint8_t OAMADDR, uint8_t OAMDATA, uint8_t PPUSCROLL, uint8_t PPUDATA, uint16_t PPUADDR,
                          uint16_t PC, uint8_t SP, uint8_t status, uint8_t A, uint8_t X, uint8_t Y);
    void clear_cpu_debug();
    void show_real_FPS(int microsec);

public slots:
    void slot_show_error_message();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;
    std::unique_ptr<MyOpenGL> my_openGL;
    std::vector<uint32_t> outBuffer;
    std::unique_ptr<MyOpenGL> debug_tiles_widget1;
    std::unique_ptr<MyOpenGL> debug_tiles_widget2;
    std::unique_ptr<MyOpenGL> debug_palettes_widget;
    std::unique_ptr<QTextEdit> cpu_debuger;

signals:
    void signal_init_new_cartridge(const QString& path);

};

#endif // MAINWINDOW_H
