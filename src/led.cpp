#include "led.h"

// Set all pixels to the same colour
void setColour(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, r, g, b);
    }
    strip->show();
}

// Flash the pixels
void flash(Adafruit_NeoPixel* strip) {
    for (int flash = 0; flash < 2; flash++) {
        for (int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, 255, 255, 255);
        }
        strip->show();
        delay(200);

        for (int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, 0, 0, 0);
        }
        strip->show();
        delay(200);
    }
}