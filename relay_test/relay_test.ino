/*
 * Relay Test - Magicbit NEO
 * Test if NEO's 3.3V GPIO can toggle the relay
 *
 * Wiring:
 *   Relay IN1  → NEO Pin 26
 *   Relay VCC  → NEO 5V
 *   Relay GND  → NEO GND
 *   (No solenoid/SMPS needed yet — just listen for relay clicks)
 */

#define RELAY_PIN 26

int count = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("=== NEO Relay Test ===");
  Serial.println("Listen for clicks every 3 seconds...");
  delay(2000);
}

void loop() {
  count++;

  Serial.print(count);
  Serial.println(": ON");
  digitalWrite(RELAY_PIN, HIGH);
  delay(3000);

  Serial.print(count);
  Serial.println(": OFF");
  digitalWrite(RELAY_PIN, LOW);
  delay(3000);
}
