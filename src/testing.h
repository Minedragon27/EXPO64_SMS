#include <hmi.h>
short R[8]={5,6,7,8,9,10,11,12};
int LED[4]={5,6,7,12};

Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, -1);
Arduino_GFX *gfx =new Arduino_ST7735(bus, TFT_RST, 1 /* rotation */, false /* IPS */);

HMI hmi1=HMI(1,2,3,4,R,LED,gfx);

void drawText(const String &text, int textSize, int y);

void setup()
{
    gfx->begin();//initialize screen
    gfx->fillScreen(BLACK);
    gfx->setTextColor(RGB565(150,150,150));
    
}
void loop()
{
    hmi1.drawText("test hmi",2,50);
    //drawText("test drawText",2,50);
}

void drawText(const String &text, int textSize, int y) {
  int textWidth = text.length() * 6 * textSize; // approx. width per character
  int x = (gfx->width() - textWidth) / 2;
  gfx->setTextSize(textSize);
  gfx->setCursor(x, y);
  gfx->print(text);
}