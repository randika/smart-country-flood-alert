// SMART GREENHOUSE — Robot B: Shading Control
// Gateway College Colombo — IT Exhibition
// Student: Mikeyla
// Board: Arduino UNO
//
// WHAT IT DOES
// A light sensor (LDR) watches the greenhouse. When the sun gets too strong
// a servo motor pulls a shade over the plants. When the light drops again the
// shade opens back up so the plants get their light.
//
//   dark / cloudy  -> shade OPEN   (0 degrees)
//   too bright     -> shade CLOSED (90 degrees)
//
// The shade moves one degree at a time so it looks smooth and gentle,
// not like it is snapping shut.
//
// WIRING
//   LDR
//     5V   ------> one leg of the LDR
//     A0   ------> other leg of the LDR  AND  one leg of a 10k resistor
//     GND  ------> other leg of the 10k resistor
//     (the 10k resistor is required - without it A0 floats and the
//      reading hardly changes between light and dark)
//
//   SERVO
//     Orange / yellow wire -> pin 9
//     Red wire             -> 5V   (see note below)
//     Brown / black wire   -> GND
//
//   SERVO POWER NOTE
//   If the servo runs off the UNO's own 5V pin it drags the voltage down every
//   time it moves, and the LDR reading jumps around because of it. If the
//   readings look jumpy while the shade is moving, give the servo its own
//   5-6V supply and join the two GNDs together.
//
// BEFORE YOU USE THIS SKETCH
//   Run ldr_calibrate/ldr_calibrate.ino first and put the two numbers it
//   gives you into DARK_VALUE and BRIGHT_VALUE below. The defaults here are
//   only a guess - if they are wrong, the shade will either never move or
//   never stop moving.

#include <Servo.h>

// --- Pins ---
#define LDR_PIN     A0
#define SERVO_PIN   9

// --- LDR calibration (MEASURE THESE with ldr_calibrate) ---
// DARK_VALUE   = the raw reading with the sensor COVERED
// BRIGHT_VALUE = the raw reading with a TORCH on the sensor
// It does not matter which one is bigger - the sketch works either way round.
#define DARK_VALUE    900
#define BRIGHT_VALUE  200

// --- Decision thresholds (in %, with hysteresis) ---
// Two thresholds, not one. The shade closes at 70% but does not open again
// until the light drops to 55%. That gap stops the servo flapping open and
// shut when the light sits right on the line.
#define BRIGHT_CLOSE  70    // above this % -> close the shade
#define BRIGHT_OPEN   55    // below this % -> open the shade

// --- Shade positions (degrees) ---
#define SHADE_OPEN    0
#define SHADE_CLOSED  90

// --- Servo speed ---
// Time between each 1-degree step. Smaller = faster.
#define SERVO_SPEED   20

// --- Filtering ---
#define SAMPLES       15    // readings per measurement (median, not mean)

// --- Timing ---
#define LOOP_DELAY    1000  // ms between light checks

Servo shadeServo;

// Where the shade is right now
int currentPosition = SHADE_OPEN;

// true = shade is closed. Needed for the two-threshold (hysteresis) logic.
bool shadeIsClosed = false;


// ==================================================
// SETUP
// ==================================================

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);

  shadeServo.attach(SERVO_PIN);

  // Start with the shade open
  shadeServo.write(SHADE_OPEN);
  currentPosition = SHADE_OPEN;
  shadeIsClosed = false;

  delay(500);

  Serial.println();
  Serial.println("=====================================");
  Serial.println(" SMART GREENHOUSE - SHADING CONTROL");
  Serial.println("=====================================");
  Serial.print("Dark reading:   ");
  Serial.println(DARK_VALUE);
  Serial.print("Bright reading: ");
  Serial.println(BRIGHT_VALUE);
  Serial.print("Close shade above: ");
  Serial.print(BRIGHT_CLOSE);
  Serial.println("%");
  Serial.print("Open shade below:  ");
  Serial.print(BRIGHT_OPEN);
  Serial.println("%");
  Serial.println("Shade is OPEN. Waiting for sunshine...");
  Serial.println();
}


// ==================================================
// READ THE SENSOR (median of SAMPLES readings)
// ==================================================
// One analogRead() is too jumpy to make a decision from, especially with the
// servo sharing the power. We take 15 readings, sort them, and use the middle
// one. The middle value throws away odd spikes; an average would not.

int readAverage(int pin) {
  int values[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    values[i] = analogRead(pin);
    delay(2);
  }

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
// TURN A RAW READING INTO A LIGHT PERCENTAGE
// ==================================================
// 0% = as dark as DARK_VALUE, 100% = as bright as BRIGHT_VALUE.
// map() copes with BRIGHT_VALUE being smaller OR bigger than DARK_VALUE,
// so this works whichever way the LDR divider is wired.

int lightPercent(int raw) {

  // Guard against a mis-typed calibration (divide by zero in map)
  if (DARK_VALUE == BRIGHT_VALUE) {
    return 0;
  }

  return constrain(map(raw, DARK_VALUE, BRIGHT_VALUE, 0, 100), 0, 100);
}


// ==================================================
// MOVE THE SHADE SMOOTHLY
// ==================================================

void moveShade(int targetPosition) {

  if (currentPosition == targetPosition) {
    return;
  }

  // Opening -> closing
  if (currentPosition < targetPosition) {
    for (int pos = currentPosition; pos <= targetPosition; pos++) {
      shadeServo.write(pos);
      delay(SERVO_SPEED);
    }
  }

  // Closing -> opening
  else {
    for (int pos = currentPosition; pos >= targetPosition; pos--) {
      shadeServo.write(pos);
      delay(SERVO_SPEED);
    }
  }

  currentPosition = targetPosition;
}


// ==================================================
// MAIN LOOP
// ==================================================

void loop() {

  int raw = readAverage(LDR_PIN);
  int percent = lightPercent(raw);

  // --- One status line per loop ---
  Serial.print("LDR Raw: ");
  Serial.print(raw);
  Serial.print(" | Light: ");
  Serial.print(percent);
  Serial.print("% | Shade: ");
  Serial.print(currentPosition);
  Serial.print(" deg (");
  Serial.print(shadeIsClosed ? "CLOSED" : "OPEN");
  Serial.print(")");

  // --- Shade control, with hysteresis ---
  if (!shadeIsClosed && percent > BRIGHT_CLOSE) {
    Serial.println(" -> too bright, CLOSING shade");
    moveShade(SHADE_CLOSED);
    shadeIsClosed = true;
  }
  else if (shadeIsClosed && percent < BRIGHT_OPEN) {
    Serial.println(" -> light is fine, OPENING shade");
    moveShade(SHADE_OPEN);
    shadeIsClosed = false;
  }
  else {
    Serial.println(" -> no change");
  }

  delay(LOOP_DELAY);
}
