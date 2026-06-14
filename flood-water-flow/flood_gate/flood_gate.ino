/*
 * Flood Simulation - Servo Gate + Pump
 * Board: Magicbit NEO (ESP32)
 *
 * The gate opens gradually to simulate rising water,
 * holds open for flooding, then closes to let it drain.
 * Pump runs continuously to recirculate water.
 *
 * Wiring:
 *   Servo signal (orange) → Pin 26
 *   Servo VCC (red)       → 5V
 *   Servo GND (brown)     → GND
 *   Pump +                → 5V (always on)
 *   Pump –                → GND
 */

#include <ESP32Servo.h>

#define SERVO_PIN     26

#define GATE_CLOSED   0
#define GATE_FULL     90

// ── Timing (adjust to match your model) ──────────────
#define FILL_TIME     12000   // 12s — gate open, water fills river bed
#define FLOOD_TIME     8000   //  8s — hold open, river bed overflows
#define DRAIN_TIME    15000   // 15s — gate closed, water drains back
// ──────────────────────────────────────────────────────

Servo gate;

void openGateSlowly() {
  Serial.println("Gate opening gradually...");
  for (int angle = GATE_CLOSED; angle <= GATE_FULL; angle += 5) {
    gate.write(angle);
    int pct = map(angle, GATE_CLOSED, GATE_FULL, 0, 100);
    Serial.print("  Gate: ");
    Serial.print(pct);
    Serial.println("%");
    delay(FILL_TIME / ((GATE_FULL - GATE_CLOSED) / 5));
  }
}

void closeGate() {
  gate.write(GATE_CLOSED);
  Serial.println("Gate CLOSED");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  gate.attach(SERVO_PIN);
  closeGate();

  Serial.println("================================");
  Serial.println("  Flood Simulation - Gate");
  Serial.println("  Gateway College - Smart Country");
  Serial.println("================================");
  Serial.println("Pump running continuously...");
  Serial.println("Starting in 3 seconds...");
  delay(3000);
}

void loop() {
  // Phase 1: Open gate gradually — water fills river bed
  openGateSlowly();

  // Phase 2: Hold gate open — river bed overflows (flood!)
  Serial.println("FLOODING — gate fully open");
  delay(FLOOD_TIME);

  // Phase 3: Close gate — water drains
  closeGate();
  Serial.println("Draining...");
  delay(DRAIN_TIME);

  Serial.println("── Next flood cycle ──");
  Serial.println();
}
