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
 *   Green LED (+ 220ohm)      → Pin 23
 *   Yellow LED (+ 220ohm)     → Pin 22
 *   Red LED (+ 220ohm)        → Pin 21
 *   Buzzer (+)                 → Pin 19
 *   All GND legs               → GND rail
 */

// --- External LEDs ---
#define GREEN_LED_PIN   23
#define YELLOW_LED_PIN  22
#define RED_LED_PIN     21

// --- External Buzzer ---
#define BUZZER_PIN      19

// --- Water Sensors ---
#define RESERVOIR_PIN   35   // Analog — reservoir water level
#define RIVERBED_PIN    33   // Analog — river bed water level

// --- Solenoid Relay ---
#define RELAY_PIN       26

// --- Thresholds (tune these with actual readings) ---
#define RESERVOIR_FULL   500   // Reservoir level to open gate
#define RESERVOIR_LOW    200   // Reservoir level to close gate
#define RIVERBED_WARNING 100   // River bed warning level (amber)
#define RIVERBED_DANGER  1500  // River bed flood level (red)

// --- State ---
bool gateOpen = false;

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  allLedsOff();
  noTone(BUZZER_PIN);
  closeGate();

  // Start green
  digitalWrite(GREEN_LED_PIN, HIGH);

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

  // --- Gate control based on reservoir water level ---
  if (!gateOpen && reservoir > RESERVOIR_FULL) {
    openGate();
    Serial.print("  -> GATE OPEN");
  } else if (gateOpen && reservoir < RESERVOIR_LOW) {
    closeGate();
    Serial.print("  -> GATE CLOSED");
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

void openGate() {
  digitalWrite(RELAY_PIN, LOW);   // LOW = relay ON = solenoid opens
  gateOpen = true;
}

void closeGate() {
  digitalWrite(RELAY_PIN, HIGH);  // HIGH = relay OFF = solenoid closes
  gateOpen = false;
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
