/*
 * Raw HC-SR04 test - Arduino D1 Mini (ESP8266)
 * Board: "LOLIN(WEMOS) D1 R2 & mini"
 *
 * Wiring:
 *   TRIG -> D1 (GPIO5)
 *   ECHO -> D2 (GPIO4)
 *   VCC  -> 5V
 *   GND  -> GND
 */

#define TRIG_PIN 5   // D1 = GPIO5
#define ECHO_PIN 4   // D2 = GPIO4

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Raw HC-SR04 test");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long d = pulseIn(ECHO_PIN, HIGH, 50000);
  Serial.println(d);  // Raw microseconds — 0 means no echo

  delay(1000);
}
