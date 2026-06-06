# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**School IT Exhibition Project** — Gateway College, Colombo
- **Student**: Mikeyla, Grade 4
- **Theme**: Smart Country — students build a hypothetical country showcasing technologies, culture, and nature
- **Hardware**: Magicbit (NEO board) — ESP32-based development board, made in Sri Lanka
- **Project idea**: Flood Alert Model (open to other Smart Country ideas)

## Magicbit NEO Board Details

- **Chip**: ESP32-WROOM-32UE (240 MHz, 4MB flash, 520KB SRAM)
- **Connectivity**: Built-in WiFi and Bluetooth
- **On-board LEDs**: Pin 16 (Green), Pin 17 (Blue), Pin 18 (Yellow), Pin 27 (Red)
- **On-board NeoPixel**: WS2812B (separate from LEDs)
- **Buzzer**: Pin 25
- **Buttons**: Push Button 01, Push Button 02
- **On-board sensors**: LDR (light), Potentiometer
- **Motor drivers**: S9110 x2 (M1A, M2B)
- **Battery**: TP4057 charger, Li-Ion connector
- **Extension connector GPIO**: 13, 02, 04, 00, 12, 26, 32 (top) / 23, 22, 21, 19, 14, 33, 35 (bottom)
- **Analog input on extension**: Pin 35, Pin 32, Pin 33
- **Power pins**: VBAT, 5V, 3.3V, GND
- **No OLED** on NEO version
- **Programming**: Arduino IDE — select "ESP32 Dev Module", install "esp32 by Espressif Systems" board package
- **Upload speed**: 115200 (921600 may fail)
- **USB driver**: CP2104 (Silicon Labs VCP driver)
- **Tech spec**: https://magicbit.cc/wp-content/uploads/2025/08/MagcibitNeo-TechSpec-01-scaled.jpg

## Project Ideas Under Consideration

### Flood Alert System (primary idea)
A smart flood warning model using Magicbit NEO with water level sensor. Uses on-board LEDs (green/yellow/red) for visual warning and on-board buzzer for alarm. Water sensor connects via extension connector (signal→pin 35, power→pin 32, GND). Great fit for "Smart Country" — disaster preparedness infrastructure.

### Other potential ideas
- **Smart Traffic Light** — sensor-based traffic signal that adapts to vehicle presence
- **Smart Street Lighting** — uses on-board LDR to auto-control lights based on ambient light
- **Weather Station** — DHT sensor + OLED display for temperature/humidity monitoring
- **Smart Waste Bin** — ultrasonic sensor detects when bin is full, alerts on OLED/buzzer
