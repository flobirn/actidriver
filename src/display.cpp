#include "display.h"

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void display_setup() {
    display.initR(INITR_BLACKTAB);
    //display.fillScreen(TEMP_STRING_BG_COLOR);
    //display.setTextColor(TEMP_STRING_FG_COLOR);
    display.setTextSize(1);
}