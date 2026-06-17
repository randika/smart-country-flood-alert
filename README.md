# Smart Country - Flood Alert System

**Gateway College Colombo - IT Exhibition 2026**
**Student:** Mikeyla, Grade 4

A smart flood warning system built with the Magicbit NEO (ESP32) board. The pump automatically fills a reservoir that overflows into a river bed model. Water level sensors detect flooding and trigger visual and audio alerts.

## How It Works

1. Power on → pump fills reservoir → water overflows to river bed
2. River bed sensor detects rising water
3. **GREEN** (safe) → **YELLOW** (warning) → **RED + siren** (flood!)
4. Flood detected → pump stops automatically
5. Water drains naturally → alerts stop
6. After 5 seconds at safe level → pump restarts
7. Cycle repeats

## Wiring Diagram

```mermaid
graph LR
    subgraph NEO["Magicbit NEO"]
        P26[Pin 26]
        P33[Pin 33]
        P23[Pin 23]
        P22[Pin 22]
        P21[Pin 21]
        P25[Pin 25 Buzzer]
        V5[5V]
        V3[3.3V]
        GND[GND]
    end

    subgraph RELAY["2-Ch Relay Module"]
        IN1[IN1]
        RVCC[VCC]
        RGND[GND]
        COM1[COM1]
        NO1[NO1]
    end

    subgraph SENSOR["Water Sensor"]
        SS[S signal]
        SV[+]
        SG[-]
    end

    subgraph LEDS["LEDs + 220Ω"]
        GLED["Green"]
        YLED["Yellow"]
        RLED["Red"]
    end

    subgraph POWER["12V SMPS"]
        SMPS12[+12V]
        SMPSGND[GND]
    end

    PUMP[Water Pump]

    P26 --> IN1
    V5 --> RVCC
    GND --> RGND
    SMPS12 --> COM1
    NO1 --> PUMP
    PUMP --> SMPSGND

    P33 --> SS
    V3 --> SV
    GND --> SG

    P23 --> GLED --> GND
    P22 --> YLED --> GND
    P21 --> RLED --> GND

    style NEO fill:#4A90D9,stroke:#2C6FB5,color:#fff
    style RELAY fill:#9B59B6,stroke:#8E44AD,color:#fff
    style SENSOR fill:#2ECC71,stroke:#27AE60,color:#fff
    style LEDS fill:#F39C12,stroke:#E67E22,color:#fff
    style POWER fill:#555555,stroke:#333333,color:#fff
```

### ASCII Wiring Reference

```
=== MAGICBIT NEO CONNECTIONS ===

NEO Pin 26  ──────→  Relay IN1 (signal)
NEO 5V      ──────→  Relay VCC
NEO GND     ──────→  Relay GND

NEO Pin 33  ←──────  Water Sensor S (signal)
NEO 3.3V    ──────→  Water Sensor + (VCC)
NEO GND     ──────→  Water Sensor – (GND)

NEO Pin 23  ──────→  Green LED (+) ──→ 220Ω ──→ GND
NEO Pin 22  ──────→  Yellow LED (+) ─→ 220Ω ──→ GND
NEO Pin 21  ──────→  Red LED (+) ───→ 220Ω ──→ GND

NEO Pin 25  ──────→  Buzzer (on-board)


=== RELAY TO PUMP (12V CIRCUIT) ===

SMPS +12V   ──────→  Relay COM1
Relay NO1   ──────→  Pump + (red)
Pump – (black) ───→  SMPS GND

Note: 2-channel relay module, active-low (LOW = ON)
```

## Pin Map

| Component | Pin | Wire Color |
|---|---|---|
| River bed sensor (signal) | 33 | Yellow |
| River bed sensor (VCC) | 3.3V | Red |
| River bed sensor (GND) | GND | Black |
| Relay IN1 | 26 | Purple |
| Relay VCC | 5V | Red |
| Relay GND | GND | Black |
| SMPS +12V | Relay COM1 | Red |
| Relay NO1 | Pump + (red) | Orange |
| Pump – (black) | SMPS GND | Black |
| Green LED + 220Ω | 23 | Green |
| Yellow LED + 220Ω | 22 | Yellow |
| Red LED + 220Ω | 21 | Red |
| Buzzer | 25 | On-board |

## Alert Levels

| Level | LED | Buzzer | Condition |
|---|---|---|---|
| **SAFE** | Green | Off | River bed < 400 |
| **WARNING** | Yellow | Soft beep | River bed > 500 |
| **FLOOD** | Red (flashing) | Loud siren | River bed > 1400 |

## Hardware

- **Board:** Magicbit NEO (ESP32-WROOM-32UE)
- **Water Sensor:** Analog water level sensor
- **Pump:** 12V water pump
- **Relay:** 2-channel relay module (active-low, using CH1)
- **Power:** 12V SMPS for pump, USB for NEO
- **LEDs:** 3x (green, yellow, red) with 220Ω resistors
- **Buzzer:** On-board (Pin 25)

## Code

Main sketch: [`flood_alert_v2/flood_alert_v2.ino`](flood_alert_v2/flood_alert_v2.ino)

Built with Arduino IDE — select **ESP32 Dev Module**, upload speed **115200**.
