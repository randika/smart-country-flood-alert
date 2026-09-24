/*
 * LDR Pin Finder + Light Calibration - Magicbit NEO
 *
 * The on-board light sensor (LDR) is not on the extension connector, so this
 * sketch prints every candidate analog pin at once. Cover the board's light
 * sensor with your hand: the pin whose number changes a lot IS the LDR.
 *
 * Put that pin number into LDR_PIN in smart_green_house.ino, and set
 * DARK_LEVEL to a value between your "covered" and "uncovered" readings.
 *
 * If the number goes DOWN when you cover it, keep LIGHT_BRIGHT_IS_HIGH true.
 * If it goes UP when covered, set it to false.
 *
 * No wiring needed - the LDR is on the board.
 */

int candidatePins[] = {36, 39, 34, 35, 32};
int numPins = 5;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== LDR PIN FINDER ===");
  Serial.println("Cover the light sensor with your hand.");
  Serial.println("Watch which column changes the most.");
  Serial.println();

  for (int i = 0; i < numPins; i++) {
    Serial.print("pin");
    Serial.print(candidatePins[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void loop() {
  for (int i = 0; i < numPins; i++) {
    int total = 0;
    for (int j = 0; j < 10; j++) {
      total += analogRead(candidatePins[i]);
      delay(2);
    }
    Serial.print(total / 10);
    Serial.print("\t");
  }
  Serial.println();

  delay(500);
}
