/*
 * LED Test - find out which pin is which color
 * Each LED turns on for 3 seconds with its pin number printed
 */

void setup() {
  Serial.begin(115200);

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(27, OUTPUT);

  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
  digitalWrite(18, LOW);
  digitalWrite(27, LOW);

  delay(1000);

  Serial.println("--- LED TEST ---");

  Serial.println("Pin 16 ON - what color?");
  digitalWrite(16, HIGH);
  delay(3000);
  digitalWrite(16, LOW);
  delay(1000);

  Serial.println("Pin 17 ON - what color?");
  digitalWrite(17, HIGH);
  delay(3000);
  digitalWrite(17, LOW);
  delay(1000);

  Serial.println("Pin 18 ON - what color?");
  digitalWrite(18, HIGH);
  delay(3000);
  digitalWrite(18, LOW);
  delay(1000);

  Serial.println("Pin 27 ON - what color?");
  digitalWrite(27, HIGH);
  delay(3000);
  digitalWrite(27, LOW);
  delay(1000);

  Serial.println("--- TEST DONE ---");
}

void loop() {
}
