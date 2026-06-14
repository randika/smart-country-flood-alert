/*
 * Flood Alert v2 - Combined Water Flow + Alert System
 * Student: Mikeyla, Grade 4, Gateway College Colombo
 * Board: Magicbit NEO (ESP32)
 *
 * Flow:
 *   1. Pump fills reservoir (manual switch, not board-controlled)
 *   2. Reservoir sensor detects water level
 *   3. When reservoir full → solenoid opens (flood gate)
 *   4. Water flows to river bed
 *   5. River bed sensor detects level → GREEN/AMBER/RED + siren
 *   6. Water drains naturally, siren stops
 *   7. Repeat
 *
 * Wiring:
 *   Reservoir water sensor S   → Pin 35 (analog)
 *   Reservoir water sensor +   → 3.3V
 *   Reservoir water sensor -   → GND
 *
 *   River bed water sensor S   → Pin 33 (analog)
 *   River bed water sensor +   → 3.3V
 *   River bed water sensor -   → GND
 *
 *   Relay IN1                  → Pin 26
 *   Relay VCC                  → NEO 5V
 *   Relay GND                  → NEO GND
 *   Relay COM                  → SMPS +12V
 *   Relay NO                   → Solenoid wire 1
 *   Solenoid wire 2            → SMPS GND
 *
 * On-board:
 *   NeoPixel (Pin 13) — GREEN / AMBER / RED
 *   Buzzer (Pin 25)   — warning + flood siren
 */

#include <Adafruit_NeoPixel.h>

// --- NeoPixel ---
#define LED_PIN       13
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- Buzzer ---
#define BUZZER_PIN    25

// --- Water Sensors ---
#define RESERVOIR_PIN   35   // Analog — reservoir water level
#define RIVERBED_PIN    33   // Analog — river bed water level

// --- Solenoid Relay ---
#define RELAY_PIN       26

// --- Thresholds (tune these with actual readings) ---
#define RESERVOIR_FULL   500   // Reservoir level to open gate
#define RIVERBED_WARNING 100   // River bed warning level (amber)
#define RIVERBED_DANGER  1500  // River bed flood level (red)
#define RIVERBED_SAFE    50    // River bed drained enough to reset

// --- State ---
bool gateOpen = false;

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  noTone(BUZZER_PIN);
  closeGate();

  led.begin();
  led.setBrightness(50);
  setColor(0, 255, 0);  // Start green

  Serial.println("================================");
  Serial.println("  Flood Alert System v2");
  Serial.println("  Gateway College - Smart Country");
  Serial.println("  By Mikeyla, Grade 4");
  Serial.println("================================");

  // Startup beep
  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  noTone(BUZZER_PIN);

  Serial.println("Waiting for reservoir to fill...");
}

void loop() {
  int reservoir = readSensor(RESERVOIR_PIN);
  int riverbed = readSensor(RIVERBED_PIN);

  Serial.print("Reservoir: ");
  Serial.print(reservoir);
  Serial.print("  River: ");
  Serial.print(riverbed);

  // --- Gate control based on reservoir level ---
  if (!gateOpen && reservoir > RESERVOIR_FULL) {
    openGate();
    Serial.print("  -> GATE OPEN");
  } else if (gateOpen && reservoir < RIVERBED_SAFE) {
    closeGate();
    Serial.print("  -> GATE CLOSED");
  }

  // --- Alert based on river bed level ---
  if (riverbed > RIVERBED_DANGER) {
    // FLOOD — red flash + siren
    setColor(255, 0, 0);
    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    setColor(0, 0, 0);
    delay(200);
    Serial.println("  FLOOD!");
  } else if (riverbed > RIVERBED_WARNING) {
    // WARNING — amber + soft beep
    setColor(255, 80, 0);
    tone(BUZZER_PIN, 500, 100);
    delay(500);
    Serial.println("  WARNING");
  } else {
    // SAFE — green, no sound
    setColor(0, 255, 0);
    noTone(BUZZER_PIN);
    delay(500);
    Serial.println("  SAFE");
  }
}

// --- Helpers ---

void openGate() {
  digitalWrite(RELAY_PIN, HIGH);
  gateOpen = true;
}

void closeGate() {
  digitalWrite(RELAY_PIN, LOW);
  gateOpen = false;
}

void setColor(int r, int g, int b) {
  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
}

int readSensor(int pin) {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(5);
  }
  return total / 10;
}
