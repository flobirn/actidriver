#ifndef __DISPLAY_H__
#define __DISPLAY_H__

//#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include <SPI.h>
#include "Ucglib.h"

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB_t;

#define TFT_CS        10
#define TFT_RST       -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

#define FG_COLOR_IDX 0
#define BG_COLOR_IDX 1

extern RGB_t fg_color;
extern RGB_t bg_color;

#define RGB_COPY(from, to) (to.red = from.red; to.green = from.green; to.blue = from.blue;)

// extern Adafruit_ST7735 display;
#define LIB_UCGLIB
extern Ucglib_ST7735_18x128x160_HWSPI display;

void display_setup();

#endif