#include "ssd1306.h"
#include "font.h"

int lerp(int valor, int in_min, int in_max, int out_min, int out_max) {
    return (valor - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool puroPixel_SSD1306::checkI2CDevice(uint8_t address) {
    Wire.beginTransmission(address);
    return Wire.endTransmission();
}

void puroPixel_SSD1306::transmit_command(unsigned char c) {
    wire->beginTransmission(address);
    wire->write(0x00); // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    wire->write(c);
    wire->endTransmission();
}

/*!
@brief creates a new SSD1306 class. Use this for each display that you have. After defining your display, you are able to begin him and also update his display.
@return SSD1306 object.
@note   after defining your display, make sure to begin wire before the own class begin function.
@param addr
    adress of your SSD1306, usually is 0x3C.
@param start
    width of the display.
@param stop
    height of the display.
@param i2c
    wire pointer. Defines as: &Wire
@param ns
    no splash screen, set to true to disable it. :(
*/
puroPixel_SSD1306::puroPixel_SSD1306(uint8_t addr, uint8_t w, uint8_t h, TwoWire* i2c, bool ns) {
    width = w;
    height = h;
    wire = i2c;
    address = addr;
    noSplash = ns;
    ssd1306_buffer = new unsigned char[width * (height / 8)];
}


/*
@brief clears the current buffer, requires an update to make effect.
@note   after use, call update(). To applay effects.
*/
void puroPixel_SSD1306::clear() {
    memset(ssd1306_buffer, 0, 1024); // make every bit a 0, memset in string.h
}

/*!
@brief load the current buffer to your display. You call this function after a draw or a clear function. For example: drawPixel(...); update(); // loads buffer
*/
void puroPixel_SSD1306::update() {
    transmit_command(SSD1306_PAGEADDR);
    transmit_command(0);
    transmit_command(0xFF);
    transmit_command(SSD1306_COLUMNADDR);
    transmit_command(0);
    transmit_command(width - 1);
    for (unsigned char page = 0; page < 8; page++) {
        transmit_command(0xB0 + page);    // Select the page to write to
        for (unsigned char col = 0; col < width; col++) {
            Wire.beginTransmission(address);
            Wire.write(0x40);  // Send pixel data
            Wire.write(ssd1306_buffer[col + (page * width)]);  // Send the data byte for this column
            Wire.endTransmission();
        }
    }
}

void puroPixel_SSD1306::debugBuffer() {
    for (int i = 0; i < 1024; i++) {
        Serial.print(ssd1306_buffer[i], HEX);
        Serial.print(" ");
        if ((i + 1) % 16 == 0) {
            Serial.println();
        }
    }
}

/*!
@brief begins the class display. Loads all the required commands and in the end running an clear and a update.
@note   remember to aways put this function after the wire begin function, or else expect errors.
*/
bool puroPixel_SSD1306::begin() {
    //Serial.println("Error [checkI2CDevice(...)]: Display SSD1306 not started. Did you forget Wire.begin()?");
    transmit_command(SSD1306_DISPLAYOFF);
    transmit_command(SSD1306_SETDISPLAYCLOCKDIV);
    transmit_command(0x80);
    transmit_command(SSD1306_SETMULTIPLEX);
    transmit_command(0x3F); // Multiplex ratio (pode precisar de ajustes dependendo da tela)
    transmit_command(SSD1306_SETDISPLAYOFFSET);
    transmit_command(0x0);
    transmit_command(SSD1306_SETSTARTLINE);
    transmit_command(SSD1306_CHARGEPUMP);
    transmit_command(0x14); // Para telas OLED de 128x64, pode ser necessário
    transmit_command(SSD1306_MEMORYMODE);
    transmit_command(0x00); // Horizontal addressing mode
    transmit_command(SSD1306_SEGREMAP | 0x1); // Inverter de mapeamento de segmentos socorro
    transmit_command(SSD1306_COMSCANDEC);
    transmit_command(SSD1306_SETCOMPINS);
    transmit_command(0x12); // Se o display for 128x64
    transmit_command(SSD1306_SETCONTRAST);
    transmit_command(0xCF); // Contraste máximo
    transmit_command(SSD1306_SETPRECHARGE);
    transmit_command(0xF1);
    transmit_command(SSD1306_SETVCOMDETECT);
    transmit_command(0x40); // VCOMH deselected voltage
    transmit_command(SSD1306_DISABLE_SCROLL);
    transmit_command(SSD1306_DISPLAYON);

    if (!noSplash) { // why no splash? bruh come on
        clear();
        drawBitmap(0, 0, epd_bitmap_splash_puro_pixel, 128, 64, 1);
        update();
        delay(3000);
    }

    clear();
    update();

    return true;
}

/*!
@brief gets the current pixel (from the buffer)
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@return bool
*/
bool puroPixel_SSD1306::getPixel(int16_t x, int16_t y) {
    return ssd1306_buffer[x + (y / 8) * width];
}

/*!
@brief draws an single pixel. Usefull if you wish to do details or lines.
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
        return;
    }

    uint16_t index = x + (y / 8) * width;
    if (color == 1) {
        ssd1306_buffer[index] |= (1 << (y & 7));
    }
    else {
        ssd1306_buffer[index] &= ~(1 << (y & 7));
    }
}

/*!
@brief as you might expect, it fills the entire screen.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::fillScreen(uint16_t color) {
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 128; x++) {
            drawPixel(x, y, color);
        }
    }
}

/*!
@brief draws an perfect single pixel line in the vertical by given axys and height.
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@param h
    height of the line. Basecally an x + h.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawVerLine(int16_t x, int16_t y, int16_t h, int16_t color) {
    for (int cH = 0; cH < h; cH++) {
        drawPixel(x + cH, y, color);
    }
}

/*!
@brief draws an perfect single pixel line in the horizontal by given axys and height.
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@param w
    width of the line. Basecally an y + w.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawHorLine(int16_t x, int16_t y, int16_t w, int16_t color) {
    for (int cW = 0; cW < w; cW++) {
        drawPixel(x, y + cW, color);
    }
}

/*!
@brief draws an perfect rectangle.
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@param h
    height of the line. Basecally an x + h.
@param w
    width of the line. Basecally an y + w.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawRect(int16_t x, int16_t y, int16_t h, int16_t w, int16_t color) {
    drawVerLine(x, y, h, color); // right
    drawHorLine(x + h, y, w, color); // down
    drawVerLine(x, y + w, h, color); // right (left but starts from right)
    drawHorLine(x, y, w, color); // down (up but starts from down)
}

/*!
@brief draws an perfect rectangle but filled.
@param x
    X vector of the pixel, if width is 128, the middle would be 62.
@param y
    Y vector of the pixel, if height is 64, the middle would be 32.
@param h
    height of the line. Basecally an x + h.
@param w
    width of the line. Basecally an y + w.
@param color
    defines the pixel state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawFillRect(int16_t x, int16_t y, int16_t h, int16_t w, int16_t color) {
    drawVerLine(x, y, h, color); // right
    drawHorLine(x + h, y, w, color); // down
    drawVerLine(x, y + w, h, color); // right (left but starts from right)
    drawHorLine(x, y, w, color); // down (up but starts from down)

    for (int cH = 0; cH < h; cH++) {
        for (int cW = 0; cW < w; cW++) {
            drawPixel(x + cH, y + cW, color);
        }
    }
}

/*stringPos puroPixel_SSD1306::drawString(int16_t x, int16_t y, const char* str, uint16_t color) {
    int xOffeset = 0;
    int yOffeset = 0;

    bool justChangedLine = false;
    for (int i = 0; i < strlen(str); i++) {
        char character = str[i];

        const char* charF = ASCII[character - 0x20];

        for (int cx = 0; cx < 5; cx++) {
            for (int j = 0; j < 7; j++) {
                uint8_t pixel = (charF[cx] >> j) & 1;

                if (pixel == 1) {
                    // Se color for 1, desenha o pixel com fundo preto
                    if (color == 1) {
                        drawPixel((cx + xOffeset + x), (j + yOffeset + y), pixel);
                    }
                    // Se color for 0, desenha o pixel com fundo branco
                    else {
                        drawPixel((cx + xOffeset + x), (j + yOffeset + y), !pixel);
                    }
                }
            }
        }
        if ((xOffeset + 8) >= 128) {
            xOffeset = 0;
            yOffeset += 8;
            justChangedLine = true;
        }
        else {
            if (justChangedLine) {
                if (character != ' ') {
                    xOffeset += 6;
                }
                justChangedLine = false;
            }
            else {
                xOffeset += 6;
            }
        }
    }
    return { xOffeset, yOffeset + 7 }; // y + 7 of the default offeset
}*/


/*!
@brief draws an string. (to do more stuff here)
@param x
    X vector of the string.
@param y
    Y vector of the string, expect an offset of +7 so, if you wanna put it in the center, (64+7)/2: y should be 35.
@param str
    your string. You can costumize the font later.
@param scale
    the scale of the font. 1 = 1x, 2 = 2x, 3 = 3x, etc. Default is 1.
@param color
    defines the pixels state, 1 = on, 0 = off.
@param textBg
    text should have background? true or false (default is false)
@param textWrap
    defines if text breaks line if not fits. true or false (default is true)
@note   the string is not centered, you have to do it manually. The function will return the offset of the string, so you can use it to center it.
*/
stringPos puroPixel_SSD1306::drawString(int16_t x, int16_t y, const char* str, uint8_t scale, uint16_t color, bool textBg, bool textWrap) {
    int xOffset = 0;
    int yOffset = 0;
    int screenWidth = 128;
    int charWidth = 6 * scale;
    int charHeight = 8 * scale;

    for (int i = 0; str[i] != '\0'; i++) {
        char character = str[i];

        // Quebra de linha manual
        if (character == '\n') {
            xOffset = 0;
            yOffset += charHeight;
            continue;
        }

        // Ignora caracteres inválidos
        if (character < 0x20 || character > 0x7F) continue;

        // Verifica quebra automática de linha
        if (textWrap && (xOffset + charWidth > screenWidth)) {
            xOffset = 0;
            yOffset += charHeight;
        }

        const char* charF = ASCII[character - 0x20];

        // Desenha fundo com borda, se ativado
        if (textBg) {
            for (int cx = -1; cx <= 5; cx++) {
                for (int j = -1; j <= 7; j++) {
                    for (int dx = 0; dx < scale; dx++) {
                        for (int dy = 0; dy < scale; dy++) {
                            int16_t px = x + xOffset + (cx * scale) + dx;
                            int16_t py = y + yOffset + (j * scale) + dy;
                            drawPixel(px, py, !color);
                        }
                    }
                }
            }
        }

        // Desenha o caractere
        for (int cx = 0; cx < 5; cx++) {
            for (int j = 0; j < 7; j++) {
                uint8_t pixel = (charF[cx] >> j) & 1;
                if (pixel) {
                    for (int dx = 0; dx < scale; dx++) {
                        for (int dy = 0; dy < scale; dy++) {
                            int16_t px = x + xOffset + (cx * scale) + dx;
                            int16_t py = y + yOffset + (j * scale) + dy;
                            drawPixel(px, py, color);
                        }
                    }
                }
            }
        }

        xOffset += charWidth;
    }

    return { xOffset, yOffset + 7 * scale };
}

/*!
@brief draws an "image" from an bitmap. Kinda complex to use, but not that hard to understand.
@param x
    X vector of the image. You also have to expect an offeset dependind on the bitmap width.
@param y
    Y vector of the image. Same as X, you have to expect an offeset dependind on the bitmap height.
@param bitmap
    your bitmap.
@param w
    your bitmap width.
@param h
    your bitmap height.
@param color
    defines the pixels state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t b = 0;

    for (int16_t j = 0; j < h; j++, y++) {
        for (int16_t i = 0; i < w; i++) {
            if (i & 7)
                b <<= 1;
            else
                b = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            if (b & 0x80)
                drawPixel(x + i, y, color);
        }
    }
}

/*!
@brief draws an simple circle.
@param x
    X vector of the circle. You also have to expect an offeset dependind on the circle size.
@param y
    Y vector of the image. Same as X, you have to expect an offeset dependind on the circle size.
@param r
    the circle radius.
@param a
    agle/steps, do SMALL increments on this number if you wanna an smooth circle. Recommended is 0.1
@param color
    defines the pixels state, 1 = on, 0 = off.
*/
void puroPixel_SSD1306::drawCircle(int16_t x, int16_t y, int16_t r, int16_t a, uint16_t color) {
    for (float angle = 0; angle < 2 * PI; angle += a) {
        int fx = x + r * cos(angle);
        int fy = y + r * sin(angle);
        drawPixel(fx, fy, color);
    }
}

void puroPixel_SSD1306::drawFillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    for (int16_t dy = -r; dy <= r; dy++) {
        int16_t dx_limit = sqrt(r * r - dy * dy); // Calcula a largura da linha usando a equação do círculo
        for (int16_t dx = -dx_limit; dx <= dx_limit; dx++) {
            drawPixel(x + dx, y + dy, color); // Desenha os pixels horizontalmente
        }
    }
}
/*!
@brief this function invert the polarity. If 1 = 0, 0 = 1, white to black, black to white.
@note   Needs update().
*/
void puroPixel_SSD1306::invert() {
    for (int i = 0; i < (128 * 64 / 8); i++) { // Percorre todo o buffer
        ssd1306_buffer[i] = ~ssd1306_buffer[i]; // Inverte os bits do byte
    }
}

/*!
@brief gets the current buffer
*/
unsigned char* puroPixel_SSD1306::getBuffer() {
    return ssd1306_buffer;
}

/*!
@brief replace the buffer to the new one. Before you send make the math and check the buffer size. do: width * (height / 8) and then, you should have it.
@note depending on your display size it MUST MATCH! 8 PAGES! DO THE MATH!

@param b the buffer to be sent. Req Size: [width * (height / 8)]
*/
void puroPixel_SSD1306::setBuffer(unsigned char* newBuffer) {
    if (newBuffer == nullptr) return;
    ssd1306_buffer = newBuffer;
}

/*!
@brief scrolling left or right!
@note   this function doesn't needs an update().
@param direction
    which direction to scroll? SCROLL_LEFT, SCROLL_RIGHT, SCROLL_DIAG_RIGHT, SCROLL_DIAG_LEFT
@param start
    which row to start? 0 = top, 7 = bottom (recomended 0)
@param end
    which row to stop? 0 = top, 7 = bottom (recomended 7)
@param speed
    scroll speed. SPEED_256_FRAMES = slowest, SPEED_2_FRAMES = fastest i.g
*/
void puroPixel_SSD1306::startScroll(ScrollDirection direction, uint8_t start, uint8_t end, ScrollSpeed speed) {
    stopScroll(); // Sempre parar qualquer scroll ativo

    if (direction == SCROLL_DIAG_LEFT || direction == SCROLL_DIAG_RIGHT) {
        // 1. Configura área de scroll vertical antes de tudo
        transmit_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
        transmit_command(0x00);    // Área fixa no topo
        transmit_command(height);  // Área que vai rolar

        // 2. Escolhe tipo de scroll
        if (direction == SCROLL_DIAG_LEFT) {
            transmit_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
        }
        else {
            transmit_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
        }

        // 3. Argumentos do scroll diagonal
        transmit_command(0x00);  // Dummy
        transmit_command(start); // Página inicial
        transmit_command(speed); // Velocidade
        transmit_command(end);   // Página final
        transmit_command(0x01);  // Vertical offset (mínimo 1 pra se mover)
        transmit_command(0xFF);  // Dummy

        // 4. Ativa o scroll
        transmit_command(SSD1306_ACTIVATE_SCROLL);
    }
    else {
        // Scroll horizontal simples
        if (direction == SCROLL_RIGHT) {
            transmit_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
        }
        else {
            transmit_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
        }

        // Argumentos do scroll horizontal
        transmit_command(0x00);  // Dummy
        transmit_command(start); // Página inicial
        transmit_command(speed); // Velocidade
        transmit_command(end);   // Página final
        transmit_command(0x00);  // Dummy
        transmit_command(0xFF);  // Dummy

        transmit_command(SSD1306_ACTIVATE_SCROLL);
    }
}



/*!
@brief stops the scroll, usually when stops it not return to the middle. To fix that after the stop command, is executed automatically an update() but you can disable by setting the update param.
@note happening unwanted updates after stopping scroll? try set update param to false.
@param update
    sets at the end an update() to return the display to the center.
*/
void puroPixel_SSD1306::stopScroll(bool upd) {
    transmit_command(SSD1306_DISABLE_SCROLL);
    if (upd) update();
}

/*!
@brief changes the contrast of the OLED display.
@note this command doesn't needs update() and i don't know if highest contrast can damage the leds, but just in case, use 207 as the maximum. And wow, the 0 is really nothing.
@param con
    min: 0, max: 255, recommended: 207 (also u can use hex 0xFF etc...)
*/
void puroPixel_SSD1306::setContrast(uint16_t con) {
    transmit_command(SSD1306_SETCONTRAST);
    transmit_command(con);
}