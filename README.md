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
graph TB
    subgraph NEO["<b>Magicbit NEO (ESP32)</b>"]
        P33["Pin 33"]
        P26["Pin 26"]
        P23["Pin 23"]
        P22["Pin 22"]
        P21["Pin 21"]
        P25["Pin 25<br/><i>on-board buzzer</i>"]
        V33["3.3V"]
        V5["5V"]
        GND["GND"]
    end

    subgraph SENSOR["Water Sensor"]
        RB["River Bed Sensor<br/><i>detects flood level</i>"]
    end

    subgraph ALERTS["Alert Outputs"]
        GLED["🟢 Green LED<br/>+ 220Ω"]
        YLED["🟡 Yellow LED<br/>+ 220Ω"]
        RLED["🔴 Red LED<br/>+ 220Ω"]
        BUZ["🔊 Buzzer<br/><i>on-board</i>"]
    end

    subgraph PUMP_SYS["Pump System"]
        RELAY["Single Relay"]
        PUMP["💧 Water Pump<br/><i>12V</i>"]
        SMPS["12V SMPS<br/>Power Supply"]
    end

    %% Sensor wiring
    P33 -- "signal 🟡" --> RB
    V33 -- "3.3V 🔴" --> RB
    RB -- "GND ⚫" --> GND

    %% LED wiring
    P23 -- "🟢" --> GLED
    P22 -- "🟡" --> YLED
    P21 -- "🔴" --> RLED
    GLED -- "GND ⚫" --> GND
    YLED -- "GND ⚫" --> GND
    RLED -- "GND ⚫" --> GND

    %% Buzzer
    P25 -.- BUZ

    %% Relay control
    P26 -- "signal S" --> RELAY
    V5 -- "5V +" --> RELAY
    RELAY -- "GND –" --> GND

    %% Relay to pump (12V circuit)
    SMPS -- "+12V → COM" --> RELAY
    RELAY -- "NO" --> PUMP
    PUMP -- "GND ⚫" --> SMPS

    %% Styling
    style NEO fill:#4A90D9,stroke:#2C6FB5,color:#fff
    style SENSOR fill:#2ECC71,stroke:#27AE60,color:#fff
    style ALERTS fill:#F39C12,stroke:#E67E22,color:#fff
    style PUMP_SYS fill:#9B59B6,stroke:#8E44AD,color:#fff
    style RB fill:#2ECC71,stroke:#27AE60,color:#fff
    style GLED fill:#00CC00,stroke:#009900,color:#fff
    style YLED fill:#FFD700,stroke:#CCB300,color:#000
    style RLED fill:#FF3333,stroke:#CC0000,color:#fff
    style BUZ fill:#FF8C00,stroke:#CC7000,color:#fff
    style RELAY fill:#9B59B6,stroke:#8E44AD,color:#fff
    style PUMP fill:#3498DB,stroke:#2980B9,color:#fff
    style SMPS fill:#555555,stroke:#333333,color:#fff
```

## Pin Map

| Component | Pin | Wire Color |
|---|---|---|
| River bed sensor (signal) | 33 | Yellow |
| River bed sensor (VCC) | 3.3V | Red |
| River bed sensor (GND) | GND | Black |
| Relay S | 26 | Purple |
| Relay + | 5V | Red |
| Relay – | GND | Black |
| SMPS +12V | Relay COM | Red |
| Relay NO | Pump + (red) | Orange |
| Pump – (black) | SMPS GND | Black |
| Green LED + 220Ω | 23 | Green |
| Yellow LED + 220Ω | 22 | Yellow |
| Red LED + 220Ω | 21 | Red |
| Buzzer | 25 | On-board |

## Alert Levels

| Level | LED | Buzzer | Condition |
|---|---|---|---|
| **SAFE** | 🟢 Green | Off | River bed < 50 |
| **WARNING** | 🟡 Yellow | Soft beep | River bed > 100 |
| **FLOOD** | 🔴 Red (flashing) | Loud siren | River bed > 1500 |

## Hardware

- **Board:** Magicbit NEO (ESP32-WROOM-32UE)
- **Water Sensor:** Analog water level sensor
- **Pump:** 12V water pump
- **Relay:** Single channel relay module
- **Power:** 12V SMPS for pump, USB for NEO
- **LEDs:** 3x (green, yellow, red) with 220Ω resistors
- **Buzzer:** On-board (Pin 25)

## Code

Main sketch: [`flood_alert_v2/flood_alert_v2.ino`](flood_alert_v2/flood_alert_v2.ino)

Built with Arduino IDE — select **ESP32 Dev Module**, upload speed **115200**.
