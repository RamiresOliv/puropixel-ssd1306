#ifndef SSD1306_H__
#define SSD1306_H__

#include "wire.h"
#include "Arduino.h"
#include "splash.h"
#include "utility"

// Based on the adafruit and sparkfun libraries
#define SSD1306_MEMORYMODE          0x20 
#define SSD1306_COLUMNADDR          0x21 
#define SSD1306_PAGEADDR            0x22 
#define SSD1306_SETCONTRAST         0x81 
#define SSD1306_CHARGEPUMP          0x8D 
#define SSD1306_SEGREMAP            0xA0 
#define SSD1306_DISPLAYALLON_RESUME 0xA4 
#define SSD1306_NORMALDISPLAY       0xA6 
#define SSD1306_INVERTDISPLAY       0xA7 
#define SSD1306_SETMULTIPLEX        0xA8 
#define SSD1306_DISPLAYOFF          0xAE 
#define SSD1306_DISPLAYON           0xAF 
#define SSD1306_COMSCANDEC          0xC8 
#define SSD1306_SETDISPLAYOFFSET    0xD3 
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 
#define SSD1306_SETPRECHARGE        0xD9 
#define SSD1306_SETCOMPINS          0xDA 
#define SSD1306_SETVCOMDETECT       0xDB 
#define SSD1306_SETSTARTLINE        0x40 
#define SSD1306_DISABLE_SCROLL      0x2E

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

struct stringPos {
    int x;
    int y;
};

class puroPixel_SSD1306 {
public:
    puroPixel_SSD1306(uint8_t addr, uint8_t w, uint8_t h, TwoWire* i2c, bool ns = false);
    bool begin();
    void update();
    void clear();
    unsigned char* getBuffer();
    bool getPixel(int16_t x, int16_t y);
    void setBuffer(unsigned char* b);
    void setContrast(uint16_t i = 207);
    void fillScreen(uint16_t color = 1);
    void drawRect(int16_t x, int16_t y, int16_t h, int16_t w, int16_t color = 1);
    void drawFillRect(int16_t x, int16_t y, int16_t h, int16_t w, int16_t color = 1);
    void drawHorLine(int16_t x, int16_t y, int16_t w, int16_t color = 1);
    void drawVerLine(int16_t x, int16_t y, int16_t h, int16_t color = 1);
    void drawPixel(int16_t x, int16_t y, uint16_t color = 1);
    //stringPos drawString(int16_t x, int16_t y, const char* str, uint16_t color = 1);
    stringPos drawString(int16_t x, int16_t y, const char* str, uint8_t scale = 1, uint16_t color = 1);
    stringPos drawBgString(int16_t x, int16_t y, const char* str, uint16_t color = 1, uint16_t borderSize = 0);
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color = 1);
    void drawCircle(int16_t x, int16_t y, int16_t r, int16_t a = 0.1, uint16_t color = 1);
    void drawFillCircle(int16_t x, int16_t y, int16_t r, uint16_t color = 1);
    void stopScroll(bool update = true);
    void startScroll(int16_t direction = 1, uint8_t start = 0, uint8_t end = 7, uint8_t speed = 7);
    void invert();
private:
    uint8_t width, height;
    uint8_t address;
    TwoWire* wire;
    unsigned char* ssd1306_buffer;
    bool noSplash = false;
    void transmit_command(unsigned char c);
    void debugBuffer();
    bool checkI2CDevice(uint8_t address);
};

#endif