/*
 * HC-SR04 Simple Test
 *
 * Wiring (Magicbit NEO extension connector):
 *   VCC  -> 5V
 *   TRIG -> Pin 32
 *   ECHO -> Pin 33
 *   GND  -> GND
 */

#define TRIG_PIN 32
#define ECHO_PIN 33

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(500);
  Serial.println("=== HC-SR04 Simple Test ===");
  Serial.print("ECHO pin state: ");
  Serial.println(digitalRead(ECHO_PIN));
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo duration (longer timeout)
  long duration = pulseIn(ECHO_PIN, HIGH, 50000);

  if (duration == 0) {
    Serial.println("No echo");
  } else {
    float distance = duration * 0.034 / 2.0;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500);
}
