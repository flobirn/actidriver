#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

extern Adafruit_ST7735 display;

void display_setup();

#endif