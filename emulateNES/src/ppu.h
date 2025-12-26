#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <vector>
#include <mutex>
#include <thread>


class Bus;
class MainWindow;


struct Color {
    uint8_t r, g, b;
};


class PPU
{
public:
    PPU(MainWindow* _window, Bus* _bus);
    ~PPU();

    uint8_t get_register(uint16_t addr);
    void set_register(uint16_t addr, uint8_t data);
    void run(int cycles);

private:
    uint8_t PPUCTRL, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUDATA;
    uint16_t PPUADDR;                                   // регистры
    std::vector<uint8_t> oam;                           // 256 байт OAM (64 спрайта по 4 байта)
    std::vector<std::vector<Color>> frame_buffer;        // результат кадра (цвета ARGB/RGBA)

    int16_t scanline = 0;
    uint16_t cycle = 0;
    uint64_t frame = 0;

    uint16_t v = 0; // текущий VRAM адрес
    uint16_t t = 0; // временный VRAM адрес
    uint8_t x = 0;
    uint8_t ppu_data_buffer = 0;
    bool w;
    std::mutex mutex_render_frame;
    std::thread thread_render_frame;

    MainWindow* window;
    Bus* bus;


    void ppu_tick();
    uint8_t get_pixel(uint16_t x, uint16_t y);
    void incrementVRAMAddress();
    uint8_t read_vram_buffered();

    Color nesPalette[64] = { // системная NES‑палитра
        { 84,  84,  84}, {  0,  30, 116}, {  8,  16, 144}, { 48,   0, 136},
        { 68,   0, 100}, { 92,   0,  48}, { 84,   4,   0}, { 60,  24,   0},
        { 32,  42,   0}, {  8,  58,   0}, {  0,  64,   0}, {  0,  60,   0},
        {  0,  50,  60}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},

        {152, 150, 152}, {  8,  76, 196}, { 48,  50, 236}, { 92,  30, 228},
        {136,  20, 176}, {160,  20, 100}, {152,  34,  32}, {120,  60,   0},
        { 84,  90,   0}, { 40, 114,   0}, {  8, 124,   0}, {  0, 118,  40},
        {  0, 102, 120}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},

        {236, 238, 236}, { 76, 154, 236}, {120, 124, 236}, {176,  98, 236},
        {228,  84, 236}, {236,  88, 180}, {236, 106, 100}, {212, 136,  32},
        {160, 170,   0}, {116, 196,   0}, { 76, 208,  32}, { 56, 204, 108},
        { 56, 180, 204}, { 60,  60,  60}, {  0,   0,   0}, {  0,   0,   0},

        {236, 238, 236}, {168, 204, 236}, {188, 188, 236}, {212, 178, 236},
        {236, 174, 236}, {236, 174, 212}, {236, 180, 176}, {228, 196, 144},
        {204, 210, 120}, {180, 222, 120}, {168, 226, 144}, {152, 226, 180},
        {160, 214, 228}, {160, 162, 160}, {  0,   0,   0}, {  0,   0,   0}
    };
};

#endif // PPU_H
