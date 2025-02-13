#include "display.h"
#include "solarized.h"

//Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Ucglib_ST7735_18x128x160_HWSPI display(/*cd=*/ TFT_DC, /*cs=*/ TFT_CS, /*reset=*/ TFT_RST);

RGB_t fg_color;
RGB_t bg_color;


void display_setup() {
#ifdef LIB_AF_ST7735
    display.initR(INITR_BLACKTAB);
    //start-up screen
    display.fillScreen(SOLARIZED_BASE2);
    //display.setTextColor(TEMP_STRING_FG_COLOR);
    display.setTextSize(1);
#endif
#ifdef LIB_UCGLIB
    display.begin(UCG_FONT_MODE_TRANSPARENT);
    display.clearScreen();
#endif
}

