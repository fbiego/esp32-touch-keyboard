#include <Arduino_GFX_Library.h>
#include <CST816S.h>
#include "fonts.h"

//Arduino_DataBus *bus = new Arduino_HWSPI(2 /* DC */, 15 /* CS */); //SCK 18, MISO 19, MOSI 23Arduino_ILI9341 *gfx = new Arduino_ILI9341(bus, 4, 0 /* rotation */, false /* IPS */);
//Arduino_ILI9341 *gfx = new Arduino_ILI9341(bus, 4, 0 /* rotation */, false /* IPS */);

Arduino_DataBus *bus = new Arduino_ESP32SPI(21 /* DC */, 22 /* CS */, 18 /* SCK */, 23 /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, 19, 0 /* rotation */, true /* IPS */);


CST816S touch(2, 4, 15, 14); // sda, scl, rst, irq

#define TFT_BL 5

String top = "qQ1wW2eE3rR4tT5yY6uU7iI8oO9pP0";
String middle = "aA_sS/dD:fF;gG(hH)jJ$kK&lL@";
String bottom = "^^ zZ.xX,cC?vV!bB'nN\"mM-";

int caps = 0;
bool change = false;
int sz = 3;

String text = "";


void setup() {

  Serial.begin(115200);

  touch.begin();

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);


  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE, BLACK);
  gfx->setFont(&FreeMono8pt7b);

  gfx->setCursor(30, 80);
  gfx->print(text + "_");

  keyboard(caps);

}

void keyboard(int type) {
  // draw the keyboard
  for (int x = 0; x < 10; x++) {
    int a = (x * 4) + (20 * x) + 2;
    gfx->drawRoundRect(a, 120, 20, 25, 1, WHITE);
    gfx->setCursor(a + 5, 120 + 15);
    gfx->print(top.charAt((x * sz) + type));
  }

  for (int x = 0; x < 9; x++) {
    int a = (x * 4) + (20 * x) + 13;
    gfx->drawRoundRect(a, 150, 20, 25, 1, WHITE);
    gfx->setCursor(a + 5, 150 + 15);
    gfx->print(middle.charAt((x * sz) + type));
  }

  for (int x = 0; x < 8; x++) {
    int a = (x * 4) + (20 * x) + 25;
    gfx->drawRoundRect(a, 180, 20, 25, 1, x == 0 ? GREEN : WHITE);
    gfx->setCursor(a + 5, 180 + 15);
    gfx->print(bottom.charAt((x * sz) + type));
  }

  gfx->drawRoundRect(55, 210, 30, 25, 1, BLUE);
  gfx->drawRoundRect(90, 210, 60, 25, 1, WHITE);
  gfx->drawRoundRect(155, 210, 30, 25, 1, RED);
}

void loop() {
  if (change) {
    change = false;
    keyboard(caps);
  }

  if (touch.available()) {
    if (touch.data.gestureID == SINGLE_CLICK) {
      Serial.print(touch.data.x);
      Serial.print("\t");
      Serial.println(touch.data.y);

      text += key(touch.data.x, touch.data.y, caps);
      gfx->setCursor(30, 80);
      gfx->print(text + "_");

    } else if (touch.data.gestureID == SWIPE_RIGHT) {
      //change keyboard when swipe right
      caps++;
      if (caps >= sz) {
        caps = 0;
      }
      change = true;
    } else if (touch.data.gestureID == SWIPE_LEFT) {
      // backspace when swipe left
      gfx->fillRect(20, 60, 200, 40, BLACK);
      if (text.length() > 0) {
        text = text.substring(0, text.length() - 1);
      }
      gfx->setCursor(30, 80);
      gfx->print(text + "_");
    } else if (touch.data.gestureID == SWIPE_DOWN) {
      // clear all when swipe down
      gfx->fillRect(20, 60, 200, 40, BLACK);
      text = "";
      gfx->setCursor(30, 80);
      gfx->print(text + "_");
    }
  }
}

String key(int x, int y, int type) {

  // check which key was pressed

  if (y > 120 && y < 145) {
    //top row

    for (int z = 0; z < 10; z++) {
      int a = (z * 4) + (20 * z) + 2;
      int b = a + 20;

      if (x > a && x < b) {
        return String(top.charAt((z * sz) + type));
      }
    }
  }

  if (y > 150 && y < 175) {
    //middle row
    for (int z = 0; z < 9; z++) {
      int a = (z * 4) + (20 * z) + 13;
      int b = a + 20;

      if (x > a && x < b) {
        return String(middle.charAt((z * sz) + type));
      }
    }
  }

  if (y > 180 && y < 205) {
    //bottom row
    for (int z = 0; z < 8; z++) {
      int a = (z * 4) + (20 * z) + 25;
      int b = a + 20;

      if (x > a && x < b) {
        return String(bottom.charAt((z * sz) + type));
      }
    }
  }

  if (y > 210 && y < 235) {
    //space
    if (x < 85) {
      // action
      return "";
    }
    if ( x > 90 && x < 150) {
      return " ";
    }
    if (x > 155) {
      return "";
    }
  }

  return "";
}
