// LDR CALIBRATION — Smart Greenhouse Robot B (Shading Control)
// Gateway College Colombo — IT Exhibition
// Student: Mikeyla
// Board: Arduino UNO
//
// WHAT THIS DOES
// This sketch does NOT move the servo. It only watches the light sensor and
// tells you the numbers you need to put into shade_control.ino.
//
// HOW TO USE IT
//   1. Upload this sketch. Open Serial Monitor at 9600.
//   2. Shine a torch on the LDR for ~5 seconds.   -> watch MIN and MAX move
//   3. Cover the LDR with your hand for ~5 sec.   -> watch MIN and MAX move
//   4. Write down the MIN and MAX it settles on.
//   5. Whichever number appeared under the TORCH is your BRIGHT_VALUE.
//      Whichever number appeared under your HAND is your DARK_VALUE.
//      (one of them will be MIN and the other MAX - which is which
//       depends on how you wired the divider, and either way is fine)
//   6. Type any letter + Enter in Serial Monitor to reset MIN/MAX and retry.
//
// WIRING
//   5V   ------> one leg of the LDR
//   A0   ------> other leg of the LDR  AND  one leg of a 10k resistor
//   GND  ------> other leg of the 10k resistor
//
//   The 10k resistor is NOT optional. Without it pin A0 is floating and the
//   reading barely changes between light and dark — which is exactly the
//   fault this sketch is here to catch.
//
//   (Swapping 5V and GND in the wiring above is also fine. It only flips
//    whether bright light makes the number bigger or smaller. This sketch
//    works out which way round you wired it.)

// --- Pins ---
#define LDR_PIN A0

// --- Filtering ---
#define SAMPLES 15          // readings per measurement (median, not mean)

// --- Timing ---
#define LOOP_DELAY 500      // ms between printed lines

// --- Sanity check ---
#define MIN_USEFUL_SPREAD 150   // MAX - MIN below this = wiring problem

// Running extremes since power-on / last reset
int seenMin = 1023;
int seenMax = 0;

// ==================================================
// SETUP
// ==================================================

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);

  delay(300);

  Serial.println();
  Serial.println("=====================================");
  Serial.println(" LDR CALIBRATION - Robot B Shading");
  Serial.println("=====================================");
  Serial.println("1. Shine a torch on the sensor.");
  Serial.println("2. Then cover it with your hand.");
  Serial.println("3. Write down MIN and MAX.");
  Serial.println("Send any letter to reset MIN/MAX.");
  Serial.println();
}


// ==================================================
// READ THE SENSOR (median of SAMPLES readings)
// ==================================================
// A single analogRead() is too jumpy to trust. We take 15 readings,
// sort them, and use the middle one. The middle value ignores the odd
// wild spike completely, which an average would not.

int readAverage(int pin) {
  int values[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    values[i] = analogRead(pin);
    delay(2);
  }

  // Simple sort (small list, so this is plenty fast)
  for (int i = 1; i < SAMPLES; i++) {
    int key = values[i];
    int j = i - 1;
    while (j >= 0 && values[j] > key) {
      values[j + 1] = values[j];
      j--;
    }
    values[j + 1] = key;
  }

  return values[SAMPLES / 2];
}


// ==================================================
// MAIN LOOP
// ==================================================

void loop() {

  // Reset the extremes if anything arrives on Serial
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      Serial.read();
    }
    seenMin = 1023;
    seenMax = 0;
    Serial.println(">>> MIN / MAX reset <<<");
  }

  int light = readAverage(LDR_PIN);

  if (light < seenMin) {
    seenMin = light;
  }
  if (light > seenMax) {
    seenMax = light;
  }

  int spread = seenMax - seenMin;

  // --- One status line per loop ---
  Serial.print("Now: ");
  Serial.print(light);
  Serial.print("   MIN: ");
  Serial.print(seenMin);
  Serial.print("   MAX: ");
  Serial.print(seenMax);
  Serial.print("   SPREAD: ");
  Serial.print(spread);

  if (spread < MIN_USEFUL_SPREAD) {
    Serial.println("   <-- TOO SMALL, check the 10k resistor / try a brighter torch");
  } else {
    Serial.println("   <-- good, usable range");
  }

  Serial.println();
  delay(LOOP_DELAY);
}
