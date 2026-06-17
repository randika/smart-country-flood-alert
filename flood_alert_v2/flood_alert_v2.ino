/*
 * Flood Alert v2 - Flood by Pump (No Solenoid)
 * Student: Mikeyla, Grade 4, Gateway College Colombo
 * Board: Magicbit NEO (ESP32)
 *
 * Flow (fully automated):
 *   1. Power on → pump fills reservoir → overflows to river bed
 *   2. River bed sensor: SAFE → WARNING → FLOOD
 *   3. Flood detected → pump stops
 *   4. Water drains naturally → back to SAFE
 *   5. SAFE for a few seconds → pump starts again
 *   6. Repeat
 *
 * Wiring:
 *   River bed water sensor S   → Pin 33 (analog)
 *   River bed water sensor +   → 3.3V
 *   River bed water sensor -   → GND
 *
 *   Single Relay:
 *     S                        → Pin 26
 *     +                        → NEO 5V
 *     –                        → NEO GND
 *     COM                      → SMPS +12V
 *     NO                       → Pump + (red)
 *     Pump – (black)           → SMPS GND
 *
 *   Green LED (+ 220ohm)      → Pin 23
 *   Yellow LED (+ 220ohm)     → Pin 22
 *   Red LED (+ 220ohm)        → Pin 21
 *   Buzzer                     → Pin 25 (on-board)
 */

// --- External LEDs ---
#define GREEN_LED_PIN   23
#define YELLOW_LED_PIN  22
#define RED_LED_PIN     21

// --- On-board Buzzer ---
#define BUZZER_PIN      25

// --- Water Sensor ---
#define RIVERBED_PIN    33   // Analog — river bed water level

// --- Pump Relay ---
#define PUMP_RELAY_PIN  26

// --- Thresholds (tune these with actual readings) ---
#define RIVERBED_WARNING 500   // River bed warning level (yellow)
#define RIVERBED_DANGER  1400  // River bed flood level (red) → stop pump
#define RIVERBED_SAFE    400   // River bed drained → restart pump

// --- Timing ---
#define RESTART_DELAY    5000  // Wait 5s after safe before restarting pump

// --- State ---
bool pumpOn = false;
unsigned long safeAt = 0;
bool waitingToRestart = false;

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);

  allLedsOff();
  noTone(BUZZER_PIN);
  startPump();

  digitalWrite(GREEN_LED_PIN, HIGH);

  Serial.println("================================");
  Serial.println("  Flood Alert System v2");
  Serial.println("  Gateway College - Smart Country");
  Serial.println("  By Mikeyla, Grade 4");
  Serial.println("================================");

  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  noTone(BUZZER_PIN);

  Serial.println("Pump ON — flooding cycle starts...");
}

void loop() {
  int riverbed = readSensor(RIVERBED_PIN);
  unsigned long now = millis();

  Serial.print("River: ");
  Serial.print(riverbed);
  Serial.print("  Pump: ");
  Serial.print(pumpOn ? "ON" : "OFF");

  // --- Pump control based on river bed ---
  if (pumpOn && riverbed > RIVERBED_DANGER) {
    stopPump();
    waitingToRestart = false;
    Serial.print("  -> PUMP OFF (flood!)");
  }

  if (!pumpOn && riverbed < RIVERBED_SAFE) {
    if (!waitingToRestart) {
      safeAt = now;
      waitingToRestart = true;
    } else if ((now - safeAt) > RESTART_DELAY) {
      startPump();
      waitingToRestart = false;
      Serial.print("  -> PUMP ON");
    }
  } else if (riverbed >= RIVERBED_SAFE) {
    waitingToRestart = false;
  }

  // --- Alert based on river bed level ---
  if (riverbed > RIVERBED_DANGER) {
    // FLOOD — red flash + siren
    allLedsOff();
    digitalWrite(RED_LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    digitalWrite(RED_LED_PIN, LOW);
    delay(200);
    Serial.println("  FLOOD!");
  } else if (riverbed > RIVERBED_WARNING) {
    // WARNING — yellow + soft beep
    allLedsOff();
    digitalWrite(YELLOW_LED_PIN, HIGH);
    tone(BUZZER_PIN, 500, 100);
    delay(500);
    Serial.println("  WARNING");
  } else {
    // SAFE — green, no sound
    allLedsOff();
    digitalWrite(GREEN_LED_PIN, HIGH);
    noTone(BUZZER_PIN);
    delay(500);
    Serial.println("  SAFE");
  }
}

// --- Helpers ---

void startPump() {
  digitalWrite(PUMP_RELAY_PIN, LOW);   // LOW = relay ON (active-low module)
  pumpOn = true;
}

void stopPump() {
  digitalWrite(PUMP_RELAY_PIN, HIGH);  // HIGH = relay OFF (active-low module)
  pumpOn = false;
}

void allLedsOff() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

int readSensor(int pin) {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(5);
  }
  return total / 10;
}
