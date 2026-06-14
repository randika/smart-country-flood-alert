/*
 * Smart Bin - Smart Country Project
 * Gateway College Colombo
 * Board: Magicbit NEO (ESP32)
 *
 * Wiring:
 *   HC-SR04 TRIG  -> Pin 26
 *   HC-SR04 ECHO  -> Pin 19
 *   HC-SR04 VCC   -> 5V
 *   HC-SR04 GND   -> GND
 *
 *   SG90 Servo signal -> Pin 4
 *   SG90 Servo VCC    -> 5V
 *   SG90 Servo GND    -> GND
 */

#include <ESP32Servo.h>

// --- HC-SR04 Ultrasonic Sensor ---
#define TRIG_PIN  19
#define ECHO_PIN  26

// --- SG90 Servo ---
#define SERVO_PIN  4
Servo lid;

// --- Settings ---
#define OPEN_DISTANCE  15
#define LID_OPEN_ANGLE  90
#define LID_CLOSED_ANGLE  0
#define OPEN_TIME  3000

bool lidOpen = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  lid.attach(SERVO_PIN);
  lid.write(LID_CLOSED_ANGLE);

  Serial.println("=== SMART BIN ===");
  Serial.println("Testing sensor...");

  // Quick test - 5 readings
  for (int i = 0; i < 5; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long d = pulseIn(ECHO_PIN, HIGH, 100000);
    Serial.print("Raw echo: ");
    Serial.print(d);
    Serial.print(" us -> ");
    Serial.print(d * 0.034 / 2);
    Serial.println(" cm");
    delay(500);
  }

  Serial.println("Starting main loop...");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 100000);
  float distance = duration * 0.034 / 2;

  Serial.print("Echo: ");
  Serial.print(duration);
  Serial.print(" us  Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < OPEN_DISTANCE && !lidOpen) {
    Serial.println(">> LID OPEN");
    lid.write(LID_OPEN_ANGLE);
    lidOpen = true;
    delay(OPEN_TIME);
    Serial.println(">> LID CLOSED");
    lid.write(LID_CLOSED_ANGLE);
    lidOpen = false;
  }

  delay(200);
}
