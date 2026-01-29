#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <vector>
#include <mutex>
#include <thread>
#include <map>


class Bus;
class MainWindow;



class PPU
{
public:
    struct Color
    {
        uint8_t r, g, b;
    };

    struct Sprite
    {
        uint8_t y;
        uint8_t tile;
        uint8_t attr;
        uint8_t x;
        size_t index;
    };

    PPU(MainWindow* _window, Bus* _bus);
    ~PPU();

    uint8_t get_register(uint16_t addr, bool onlyRead);
    void set_register(uint16_t addr, uint8_t data);
    void set_oam(const uint8_t* _oam);
    void run(int cycles);
    void run_watch_all_tiles();
    void run_watch_cpu_instr(uint16_t PC);
    void run_watch_palettes();
    void reset();
    uint8_t getppustatus(){return PPUSTATUS;}

private:
    uint8_t PPUCTRL, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUDATA; // регистры
    // PPUSTATUS 0x80 VBlank
    // PPUSTATUS 0x40 попадание по спрайту 0
    // PPUSTATUS 0x20 переполнение спрайтов не более 8

    // PPUCTRL 0x80 Формирование запроса прерывания NMI
    // PPUCTRL 0x20 Размер спрайтов (0 - 8x8; 1 - 8x16)
    // PPUCTRL 0x10 Выбор номера таблицы chr
    // PPUCTRL 0x08 Выбор знакогенератора спрайтов (0/1)
    // PPUCTRL 0x04 Выбор режима инкремента (0 – увеличение на единицу; 1 - увеличение на 32)
    // PPUCTRL 0x03 Адрес активной экранной страницы

    uint16_t PPUADDR;                                   // регистр
    std::vector<uint8_t> oam;                           // 256 байт OAM (64 спрайта по 4 байта)
    std::vector<std::vector<Color>> frame_buffer;       // результат кадра RGB
    std::vector<Sprite> sprites_current_scanline;       // спрайты для текущей линии

    int16_t scanline = 0;
    uint16_t cycle = 0;
    uint64_t frame = 0;

    //5бит - X тайла
    //5бит - Y тайла
    //2бита - nametable по x, y
    //3бита - сдвиг по Y у пикселя внутри тайла
    uint16_t current_VRAM = 0;
    uint16_t render_VRAM = 0;
    uint16_t temp_VRAM = 0;
    uint8_t numb_pixelX = 0; // сдвиг по X у пикселя внутри тайла

    uint8_t ppu_data_buffer = 0;
    bool w;
    uint8_t openBus = 0;

    uint16_t shift_tile_lsb = 0;
    uint16_t shift_tile_msb = 0;

    uint16_t shift_attrib_msb = 0;
    uint16_t shift_attrib_lsb = 0;

    std::mutex mutex_lock_frame_buffer;

    MainWindow* window;
    Bus* bus;


    void ppu_tick();
    uint8_t get_background();
    uint8_t get_sprite(const Sprite& sprite);
    void increment_VRAM_address();
    uint8_t read_vram_buffered();
    void increment_x();
    void increment_y();
    void shifts_calculation();
    void get_current_sprites();
    void download_asm_buffer(std::map<uint16_t, std::string>& assembler_buf);

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
