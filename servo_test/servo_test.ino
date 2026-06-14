#include <ESP32Servo.h>

Servo myServo;
#define SERVO_PIN 13
#define GATE_OPEN_TIME 2000  // adjust this to match your gate mechanism

void openGate() {
  Serial.println("Opening flood gate...");
  myServo.write(0);
  delay(GATE_OPEN_TIME);
  myServo.write(90);
  Serial.println("Gate open.");
}

void closeGate() {
  Serial.println("Closing flood gate...");
  myServo.write(180);
  delay(GATE_OPEN_TIME);
  myServo.write(90);
  Serial.println("Gate closed.");
}

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
  myServo.write(90);  // make sure it starts stopped
  delay(1000);
  Serial.println("Continuous rotation servo test starting...");
}

void loop() {
  openGate();
  delay(3000);

  closeGate();
  delay(3000);
}