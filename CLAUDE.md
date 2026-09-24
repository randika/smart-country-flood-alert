# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**School IT Exhibition project** — Gateway College Colombo, IT Exhibition 2026.
Student: **Mikeyla, Grade 4**. Theme: **Smart Country** (students build a hypothetical
country showcasing technology, culture, and nature).

This is an Arduino sketch collection, not a software application. Every top-level
directory is a self-contained Arduino sketch folder (`name/name.ino`) — Arduino IDE
requires the folder and the `.ino` to share a name. There is no build system, no
package manifest, no tests.

The shipped exhibition project is **`flood_alert_v2/`**. Everything else is either a
superseded version, a hardware bring-up test, or an idea that was parked.

## Build / Upload / Debug

`arduino-cli` is **not installed** — all builds and uploads go through the Arduino IDE GUI:

1. Open `<sketch>/<sketch>.ino` in Arduino IDE.
2. Board: **ESP32 Dev Module** (for Magicbit NEO sketches) or
   **LOLIN(WEMOS) D1 R2 & mini** (for the two ESP8266 sketches — see below).
3. Upload speed: **115200**. 921600 fails on this board.
4. Serial Monitor at **115200** — every sketch prints its state each loop; this is the
   only debugging channel.

Recovery from a bricked/looping board: upload `recovery/recovery.ino` (a do-nothing
sketch that just prints "alive") to get a clean board back, then re-upload the real sketch.

Installed cores: `esp32`, `esp8266`. Libraries used: `Adafruit_NeoPixel`, `ESP32Servo`.

## Hardware: Magicbit NEO

ESP32-WROOM-32UE (240 MHz, 4MB flash, 520KB SRAM), WiFi + Bluetooth, made in Sri Lanka.
Tech spec: https://magicbit.cc/wp-content/uploads/2025/08/MagcibitNeo-TechSpec-01-scaled.jpg

- **On-board LEDs**: 16 (green), 17 (blue), 18 (yellow), 27 (red)
- **On-board buzzer**: 25 — driven with `tone()` / `noTone()`
- **On-board NeoPixel**: WS2812B (pin 13 — confirmed by `neopixel_test/`)
- **On-board sensors**: LDR, potentiometer. Two push buttons. Two S9110 motor drivers.
- **Extension connector GPIO**: 13, 02, 04, 00, 12, 26, 32 (top) / 23, 22, 21, 19, 14, 33, 35 (bottom)
- **Analog-capable on extension**: 32, 33, 35 — analog sensors must use one of these
- **Power pins**: VBAT, 5V, 3.3V, GND. No OLED on the NEO version.
- **USB driver**: CP2104 (Silicon Labs VCP)

The board is 3.3V logic. Relay modules in use are **active-low**: `digitalWrite(pin, LOW)`
turns the relay **on**. Pump/solenoid loads run off a separate 12V SMPS through the relay
contacts — never off board power.

## Sketch Inventory

| Sketch | Board | Purpose |
|---|---|---|
| `smart_green_house/` | NEO | **Current exhibition sketch (Grade 5).** Soil probe + auto-watering pump |
| `flood_alert_v2/` | NEO | Grade 4 exhibition sketch. Pump + water sensor + 3 LEDs + buzzer, fully automated cycle |
| `flood_alert/` | NEO | v1 — NeoPixel instead of discrete LEDs, switched sensor power, no pump |
| `flood-water-flow/flood_gate/` | NEO | Parked servo-gate + pump flood simulation |
| `flood-water-flow/sketch_jun13b/` | **ESP8266 D1** | Parked solenoid-valve version (pin numbers are GPIO, not Dxx labels) |
| `smart_bin/` | NEO | Ultrasonic lid opener (alternate Smart Country idea) |
| `smart-bin/` | — | Empty folder, placeholder |
| `soil_test/` | NEO | Soil probe calibration — run before tuning greenhouse thresholds |
| `ldr_test/` | NEO | On-board LDR pin finder. Unused now — the grow light was cut from the greenhouse |
| `relay_test/`, `gate_test/`, `servo_test/`, `led_test/`, `neopixel_test/` | NEO | Single-component bring-up tests |
| `ultrasonic_test/`, `ultrasonic_simple/` | NEO | HC-SR04 tests (interrupt-based vs `pulseIn`) |
| `ultrasonic_raw/` | **ESP8266 D1** | Raw echo-microseconds HC-SR04 test |
| `recovery/` | NEO | Blank recovery sketch |

Each exhibition project carries its own `README.md` (wiring diagram, pin map, threshold
table) and `PRESENTATION.md` (Mikeyla's spoken script) — at the repo root for the flood
project, inside `smart_green_house/` for the greenhouse. Keep them in sync when pins,
thresholds, or behaviour change: the Mermaid diagram, ASCII diagram, pin-map table and
status table all duplicate the same facts.

## Sketch Conventions

All sketches follow the same shape; match it when adding one:

- A header comment block with project/student/board, a plain-English description of the
  behaviour, and the **complete wiring list** — the wiring comment is the source of truth
  people actually read at the bench.
- `#define` for every pin and every tunable threshold/timing, grouped under `// --- Section ---`
  comments. No magic numbers in the logic.
- `Serial.begin(115200)` plus a banner print in `setup()`, and one status line per `loop()`
  iteration showing raw sensor value and current state — this is how thresholds get tuned.
- Analog sensors are read through a filtering helper, not a bare `analogRead()` — raw
  readings are too noisy to threshold directly. Older sketches use a 10-sample mean
  (`readSensor()`); `smart_green_house` uses a 15-sample **median** (`readAverage()`)
  because the soil probe throws ±600-count spikes that drag a mean around.
- Blocking `delay()`-driven loops are the norm and are fine here; the one exception is
  pump restart timing in `flood_alert_v2`, which uses `millis()` state
  (`safeAt` / `waitingToRestart`) so the alert loop keeps running while it waits.

## smart_green_house Behaviour

One analog soil probe (pin 33) drives both the status LEDs and the pump:

HAPPY (dryness `< 1950`, green) → GETTING DRY (`> 1950`, yellow) → TOO DRY (`> 2250`, red,
**pump on**) → soil wet (`< 1600`) or `MAX_WATER_MS` safety cutoff → pump off, happy beep
→ `SOAK_TIME` pause before it may water again. Hysteresis between `SOIL_DRY` and
`SOIL_WET` is what stops the pump chattering.

Thresholds are measured, not guessed: probe in air 2559, bone-dry soil 2520, well-watered
soil 1416. `SOIL_DRY_IS_HIGH` normalises `readSoil()` so the logic always reads
"bigger = drier"; flip it if a replacement probe is wired the other way. `readAverage()`
takes a **median** of 15 samples — a mean was dragged around by spikes on this probe.

Because air (2559) and dry soil (2520) are only 39 counts apart, a fallen-out probe is
indistinguishable from dry soil. `MAX_TRIES` covers that instead: 3 waterings that leave
the soil dry latch a fault state (pump off, fast red blink, low beep every 3s) that clears
itself once the soil reads wet. This is the guard against an empty tank or a dead pump
running unattended at the exhibition table.

Relay quirk, measured with a meter: **this module's K1 screw terminals are NC, COM, NO
left-to-right**, not the usual NO, COM, NC — the pump lands in the *third* terminal.

A grow-light feature (on-board LDR → blue LED) was built and then removed on request;
`ldr_test/` and the measurements it produced (bright ~2390, covered ~300, LDR on pin 36)
are still there if it is ever wanted back.

## flood_alert_v2 Behaviour

One analog water sensor (pin 33) drives both the alert level and the pump, so the model
runs an unattended loop for visitors:

pump ON → reservoir fills → overflows into river bed → sensor rises through
SAFE (`< 400`, green) → WARNING (`> 500`, yellow + soft beep) → FLOOD (`> 1400`, flashing
red + siren, **pump stops**) → water drains → SAFE for 5s (`RESTART_DELAY`) → pump restarts.

Thresholds (`RIVERBED_SAFE` / `RIVERBED_WARNING` / `RIVERBED_DANGER`) are empirical and
depend on the physical model, water conductivity, and how deep the sensor sits. They are
the first thing to re-tune after any physical rebuild — read the Serial output at each
water level, then set the constants.

## Working Style

The audience is a Grade 4 student demoing live to exhibition visitors. Prefer fewer
components and simpler wiring over cleverness; prefer changes that can be re-tuned from a
`#define` over changes that need re-wiring. Comments and Serial output should be readable
by a child and by a judge standing at the table.
