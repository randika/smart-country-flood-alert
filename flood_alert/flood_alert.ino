/*
 * Flood Alert System - Smart Country Project
 * Student: Mikeyla, Grade 4, Gateway College Colombo
 * Board: Magicbit NEO (ESP32)
 *
 * How it works:
 *   - Normal      -> Green light + no sound
 *   - Warning     -> Amber light + soft beep
 *   - Flood!      -> Red flashing + loud beeps
 *
 * Wiring (water sensor to extension connector):
 *   Water level sensor S (signal) -> Pin 33
 *   Water level sensor + (VCC)    -> 3.3V
 *   Water level sensor - (GND)    -> GND
 *
 * On-board components used:
 *   - WS2812B NeoPixel (Pin 13)
 *   - Buzzer (Pin 25)
 */

#include <Adafruit_NeoPixel.h>

// --- NeoPixel RGB LED ---
#define LED_PIN       13
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- Buzzer ---
#define BUZZER_PIN    25

// --- Water Level Sensor ---
#define SENSOR_PIN    33   // Analog input
#define SENSOR_POWER  32   // Powers the sensor on/off

// --- Water Level Thresholds ---
#define LEVEL_WARNING  100   // Above = WARNING (amber)
#define LEVEL_DANGER   1500  // Above = FLOOD (red)

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SENSOR_POWER, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);
  noTone(BUZZER_PIN);

  // Start NeoPixel
  led.begin();
  led.setBrightness(50);
  led.show();

  Serial.println("=== FLOOD ALERT SYSTEM ===");
  Serial.println("Smart Country - Gateway College");
  Serial.println();

  // Startup beep
  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  noTone(BUZZER_PIN);

  // Start with green
  led.setPixelColor(0, led.Color(0, 255, 0));
  led.show();
}

void loop() {
  // Average 10 readings to smooth out noise
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += readWaterSensor();
    delay(30);
  }
  int waterLevel = total / 10;

  if (waterLevel > LEVEL_DANGER) {
    // FLOOD - flash red + beep
    led.setPixelColor(0, led.Color(255, 0, 0));
    led.show();
    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    led.setPixelColor(0, led.Color(0, 0, 0));
    led.show();
    delay(200);
    Serial.print("Water Level: ");
    Serial.print(waterLevel);
    Serial.println(" -> FLOOD!");
  } else if (waterLevel > LEVEL_WARNING) {
    // WARNING - amber + soft beep
    led.setPixelColor(0, led.Color(255, 80, 0));
    led.show();
    tone(BUZZER_PIN, 500, 100);
    delay(500);
    Serial.print("Water Level: ");
    Serial.print(waterLevel);
    Serial.println(" -> WARNING");
  } else {
    // SAFE - steady green, no sound
    led.setPixelColor(0, led.Color(0, 255, 0));
    led.show();
    noTone(BUZZER_PIN);
    delay(500);
    Serial.print("Water Level: ");
    Serial.print(waterLevel);
    Serial.println(" -> SAFE");
  }
}

int readWaterSensor() {
  digitalWrite(SENSOR_POWER, HIGH);
  delay(10);
  int value = analogRead(SENSOR_PIN);
  digitalWrite(SENSOR_POWER, LOW);
  return value;
}
