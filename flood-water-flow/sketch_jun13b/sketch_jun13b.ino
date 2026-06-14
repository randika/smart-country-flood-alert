/*
 * Flood Simulation - Water Circulation System
 * Student: Mikeyla, Grade 4, Gateway College Colombo
 * Board: D1 WiFi (ESP8266)
 *
 * Wiring:
 *   Relay IN1  → D1 pin (GPIO5)
 *   Relay VCC  → D1 5V
 *   Relay GND  → D1 GND
 *   K1 COM     → SMPS + (12V)
 *   K1 NO      → Solenoid wire 1
 *   SMPS –     → Solenoid wire 2
 *   Pump +     → D1 5V (direct)
 *   Pump –     → D1 GND (direct)
 */

#define SOLENOID_PIN  4   // D2 on D1 WiFi = GPIO4

// ── Tune these to match your containers ──
#define VALVE_OPEN_TIME    8000   // milliseconds valve stays OPEN
#define VALVE_CLOSED_TIME 20000   // milliseconds valve stays CLOSED
// ─────────────────────────────────────────

void openValve() {
  digitalWrite(SOLENOID_PIN, HIGH);  // active HIGH = relay ON = valve opens
  Serial.print("Valve OPEN for ");
  Serial.print(VALVE_OPEN_TIME / 1000);
  Serial.println("s");
}

void closeValve() {
  digitalWrite(SOLENOID_PIN, LOW);   // relay OFF = valve closes
  Serial.print("Valve CLOSED for ");
  Serial.print(VALVE_CLOSED_TIME / 1000);
  Serial.println("s");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(SOLENOID_PIN, OUTPUT);
  closeValve();  // always start closed

  Serial.println("================================");
  Serial.println("  Flood Simulation System");
  Serial.println("  Gateway College - Smart Country");
  Serial.println("  By Mikeyla, Grade 4");
  Serial.println("================================");
  Serial.println("Starting relay test...");
  delay(2000);
}

int count = 0;

void loop() {
  count++;
  Serial.print(count);
  Serial.println(": HIGH");
  digitalWrite(SOLENOID_PIN, HIGH);
  delay(3000);

  Serial.print(count);
  Serial.println(": LOW");
  digitalWrite(SOLENOID_PIN, LOW);
  delay(3000);
}