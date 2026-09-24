/*
 * Smart Green House
 * Student: Mikeyla, Grade 5, Gateway College Colombo
 * Board: Magicbit NEO (ESP32)
 *
 * What it does (fully automatic):
 *   1. Soil moisture probe checks how dry the soil is
 *   2. Soil OK        -> Green LED, pump off
 *   3. Getting dry    -> Yellow LED + soft beep
 *   4. Too dry        -> Red LED + beep, PUMP ON (waters the plant)
 *   5. Soil wet again -> pump stops, happy beep, back to green
 *
 * Safety: the pump never runs longer than MAX_WATER_MS, and always waits
 * SOAK_TIME before watering again so the water can spread through the soil.
 *
 * Wiring:
 *   Soil moisture sensor AO (signal) -> Pin 33 (analog)
 *   Soil moisture sensor VCC (+)     -> 3.3V
 *   Soil moisture sensor GND (-)     -> GND
 *
 *   2-Channel Relay (active-low):
 *     IN1                            -> Pin 26
 *     VCC                            -> NEO 5V
 *     GND                            -> NEO GND
 *     COM1                           -> SMPS +12V
 *     NO1                            -> Pump + (red)
 *     Pump - (black)                 -> SMPS GND
 *
 *   MEASURED on our module: the K1 screw terminals are NC, COM, NO from the
 *   left - NOT the usual NO, COM, NC. So the pump goes in the THIRD terminal
 *   of the left group, not the first. Verified with a meter: 12V appears on
 *   the 3rd terminal only while the red LED is on.
 *
 *   Green LED  (+ 220ohm)            -> Pin 23
 *   Yellow LED (+ 220ohm)            -> Pin 22
 *   Red LED    (+ 220ohm)            -> Pin 21
 *   Buzzer                           -> Pin 25 (on-board)
 */

// --- External LEDs ---
#define GREEN_LED_PIN    23
#define YELLOW_LED_PIN   22
#define RED_LED_PIN      21

// --- On-board ---
#define BUZZER_PIN       25

// --- Soil Moisture Sensor ---
#define SOIL_PIN         33   // Analog

// --- Pump Relay ---
#define PUMP_RELAY_PIN   26

// --- Sensor direction ---
// Most soil modules read HIGH when dry and LOW when wet. If yours is the
// other way round, change this to false. Run soil_test to check.
#define SOIL_DRY_IS_HIGH true

// --- Thresholds (measured on this board with soil_test) ---
//   Probe in air        : 2559
//   Bone dry soil       : 2520
//   Well watered soil   : 1416
#define SOIL_WARN        1950  // Dryness above this -> yellow "getting dry"
#define SOIL_DRY         2250  // Dryness above this -> red + start pump
#define SOIL_WET         1600  // Dryness below this -> soil watered, stop pump

// --- Timing ---
#define MAX_WATER_MS     5000   // Pump never runs longer than this
#define SOAK_TIME        10000  // Wait this long after watering before re-checking
#define TICK             300    // Time between sensor checks
#define MAX_TRIES        3      // Give up after this many waterings that did nothing

// --- State ---
bool watering = false;
unsigned long wateringStartedAt = 0;
unsigned long wateringStoppedAt = 0;
int  triesWithoutSuccess = 0;   // Waterings in a row where the soil never got wet
bool fault = false;             // Probe fell out? Tank empty? Pump not working?
unsigned long lastFaultBeep = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);

  allLedsOff();
  noTone(BUZZER_PIN);
  stopPump();                 // Always start with the pump OFF

  Serial.println("================================");
  Serial.println("  Smart Green House");
  Serial.println("  Gateway College - Smart Country");
  Serial.println("  By Mikeyla, Grade 5");
  Serial.println("================================");

  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  noTone(BUZZER_PIN);

  digitalWrite(GREEN_LED_PIN, HIGH);
  Serial.println("Watching the plant...");

  // Pretend we just watered, so the pump waits one soak before its first run
  wateringStoppedAt = millis();
}

void loop() {
  int dryness = readSoil();
  unsigned long now = millis();

  // --- Did watering ever work? If not, stop and ask for help ---
  // The probe reads almost the same in dry soil (2520) and in the air (2559),
  // so we cannot spot a fallen-out probe from one reading. Instead we notice
  // that the pump has run several times and the soil never got wet.
  if (fault) {
    stopPump();
    if (dryness < SOIL_WET) {           // Someone fixed it - carry on
      fault = false;
      triesWithoutSuccess = 0;
      Serial.println(">> Fixed! Back to normal.");
      happyBeep();
    } else {
      allLedsOff();
      digitalWrite(RED_LED_PIN, ((now / 250) % 2) ? HIGH : LOW);   // Fast blink
      if (now - lastFaultBeep > 3000) {
        lastFaultBeep = now;
        tone(BUZZER_PIN, 400, 300);
        delay(350);
        noTone(BUZZER_PIN);
        Serial.println("!! CHECK ME: is the probe pushed into the soil? Is the tank empty?");
      }
      delay(TICK);
      return;
    }
  }

  // --- Pump control with hysteresis + safety timers ---
  if (!watering) {
    bool soaked = (now - wateringStoppedAt) > SOAK_TIME;
    if (dryness > SOIL_DRY && soaked) {
      startPump();
      wateringStartedAt = now;
      Serial.println(">> WATERING STARTED");
      tone(BUZZER_PIN, 800, 150);
      delay(200);
      noTone(BUZZER_PIN);
    }
  } else {
    bool wetEnough = (dryness < SOIL_WET);
    bool tooLong   = (now - wateringStartedAt) > MAX_WATER_MS;
    if (wetEnough || tooLong) {
      stopPump();
      wateringStoppedAt = now;
      Serial.print(">> WATERING STOPPED (");
      Serial.print(wetEnough ? "soil is wet" : "safety timer");
      Serial.println(")");

      if (wetEnough) {
        triesWithoutSuccess = 0;
        happyBeep();
      } else {
        triesWithoutSuccess++;
        if (triesWithoutSuccess >= MAX_TRIES) {
          fault = true;
          Serial.println("!! Watered 3 times and the soil is still dry - stopping the pump.");
        }
      }
    }
  }

  // --- Status lights ---
  allLedsOff();
  const char* state;
  if (watering || dryness > SOIL_DRY) {
    digitalWrite(RED_LED_PIN, HIGH);
    state = watering ? "TOO DRY - WATERING" : "TOO DRY - waiting to soak";
  } else if (dryness > SOIL_WARN) {
    digitalWrite(YELLOW_LED_PIN, HIGH);
    state = "GETTING DRY";
  } else {
    digitalWrite(GREEN_LED_PIN, HIGH);
    state = "HAPPY PLANT";
  }

  // --- One status line per check, for tuning the thresholds ---
  Serial.print("Soil: ");
  Serial.print(dryness);
  Serial.print("  Pump: ");
  Serial.print(watering ? "ON " : "OFF");
  Serial.print("  -> ");
  Serial.println(state);

  delay(TICK);
}

// --- Helpers ---

void startPump() {
  digitalWrite(PUMP_RELAY_PIN, LOW);   // LOW = relay ON (active-low module)
  watering = true;
}

void stopPump() {
  digitalWrite(PUMP_RELAY_PIN, HIGH);  // HIGH = relay OFF (active-low module)
  watering = false;
}

void allLedsOff() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

void happyBeep() {
  tone(BUZZER_PIN, 1200, 100);
  delay(150);
  tone(BUZZER_PIN, 1600, 100);
  delay(150);
  noTone(BUZZER_PIN);
}

// Returns how DRY the soil is: bigger number = drier soil
int readSoil() {
  int raw = readAverage(SOIL_PIN);
  return SOIL_DRY_IS_HIGH ? raw : (4095 - raw);
}

// Median of 15 samples - ignores random spikes better than an average
int readAverage(int pin) {
  const int N = 15;
  int s[N];
  for (int i = 0; i < N; i++) {
    s[i] = analogRead(pin);
    delay(4);
  }
  for (int i = 1; i < N; i++) {          // insertion sort
    int v = s[i], j = i - 1;
    while (j >= 0 && s[j] > v) { s[j + 1] = s[j]; j--; }
    s[j + 1] = v;
  }
  return s[N / 2];
}
