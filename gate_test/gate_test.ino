/*
 * Flood Gate Test - SG90 Servo
 * Opens and closes the gate every 5 seconds
 *
 * Wiring:
 *   Servo orange/yellow wire -> Pin 26
 *   Servo red wire           -> 5V
 *   Servo brown wire         -> GND
 */

#include <ESP32Servo.h>

#define SERVO_PIN  26

Servo gate;

void setup() {
  Serial.begin(115200);
  gate.attach(SERVO_PIN);

  // Start with gate closed
  gate.write(0);
  Serial.println("=== FLOOD GATE TEST ===");
  Serial.println("Gate CLOSED");
  delay(2000);
}

void loop() {
  // Open gate
  gate.write(90);
  Serial.println("Gate OPEN - water flowing");
  delay(5000);

  // Close gate
  gate.write(0);
  Serial.println("Gate CLOSED - water stopped");
  delay(5000);
}
