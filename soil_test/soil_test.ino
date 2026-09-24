/*
 * Soil Moisture Calibration - Magicbit NEO
 *
 * Run this FIRST, before smart_green_house, to find your three numbers.
 *
 * Wiring:
 *   Soil sensor AO (signal) -> Pin 33
 *   Soil sensor VCC (+)     -> 3.3V
 *   Soil sensor GND (-)     -> GND
 *
 * How to calibrate:
 *   1. Hold the probe in the AIR (or fully dry soil) -> write down the number
 *   2. Push it into WELL WATERED soil               -> write down the number
 *   3. Push it into soil that needs water           -> write down the number
 *
 * If the number goes UP when dry, keep SOIL_DRY_IS_HIGH true in
 * smart_green_house.ino. If it goes DOWN when dry, set it to false.
 * Then set SOIL_WET / SOIL_WARN / SOIL_DRY between your readings.
 */

#define SOIL_PIN 33

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== SOIL MOISTURE CALIBRATION ===");
  Serial.println("Dry air/dry soil = one number, wet soil = another.");
  Serial.println("Write both down!");
  Serial.println();
}

void loop() {
  // Median of 15 samples - ignores random spikes
  const int N = 15;
  int sample[N];
  for (int i = 0; i < N; i++) {
    sample[i] = analogRead(SOIL_PIN);
    delay(4);
  }
  for (int i = 1; i < N; i++) {
    int v = sample[i], j = i - 1;
    while (j >= 0 && sample[j] > v) { sample[j + 1] = sample[j]; j--; }
    sample[j + 1] = v;
  }
  int value = sample[N / 2];
  int spread = sample[N - 1] - sample[0];

  Serial.print("Soil reading: ");
  Serial.print(value);
  Serial.print("   spread: ");
  Serial.print(spread);          // Should be small. Over ~300 = bad contact or noise
  Serial.print("   ");

  // Simple bar so a kid can see it move
  int bars = value / 200;
  for (int i = 0; i < bars; i++) Serial.print("#");
  Serial.println();

  delay(1000);
}
