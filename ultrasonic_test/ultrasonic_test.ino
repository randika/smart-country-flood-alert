/*
 * HC-SR04 Test - filtered interrupts
 *
 * Wiring:
 *   VCC  -> 5V
 *   TRIG -> Pin 32
 *   ECHO -> Pin 33
 *   GND  -> GND
 */

#define TRIG_PIN  26
#define ECHO_PIN  23

volatile long startTime = 0;
volatile long duration = 0;
volatile bool newReading = false;

void IRAM_ATTR echoISR() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    startTime = micros();
  } else {
    long d = micros() - startTime;
    if (d > 100 && d < 30000) {  // Filter: ignore noise (<100us) and timeouts
      duration = d;
      newReading = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(50);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  Serial.println("=== HC-SR04 TEST ===");
  Serial.println("Point sensor at wall or hand");
}

void loop() {
  newReading = false;

  // Send trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for echo
  delay(100);

  if (newReading) {
    float distance = duration * 0.034 / 2;
    Serial.print("Duration: ");
    Serial.print(duration);
    Serial.print(" us  Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("No object detected");
  }

  delay(500);
}
