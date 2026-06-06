/*
 * NeoPixel WS2812B pin finder
 * Tries common pins one by one - watch which one lights up
 */

#include <Adafruit_NeoPixel.h>

// Try these pins one at a time
int testPins[] = {13, 4, 2, 15, 5, 23, 22, 21, 19};
int numPins = 9;

void setup() {
  Serial.begin(115200);
  Serial.println("=== NEOPIXEL PIN FINDER ===");

  for (int i = 0; i < numPins; i++) {
    int pin = testPins[i];
    Serial.print("Testing pin ");
    Serial.print(pin);
    Serial.println(" - GREEN");

    Adafruit_NeoPixel pixel(1, pin, NEO_GRB + NEO_KHZ800);
    pixel.begin();
    pixel.setBrightness(50);
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));  // Green
    pixel.show();
    delay(2000);

    Serial.print("Testing pin ");
    Serial.print(pin);
    Serial.println(" - RED");

    pixel.setPixelColor(0, pixel.Color(255, 0, 0));  // Red
    pixel.show();
    delay(2000);

    pixel.setPixelColor(0, pixel.Color(0, 0, 0));    // Off
    pixel.show();
    delay(500);
  }

  Serial.println("=== TEST DONE ===");
}

void loop() {
}
