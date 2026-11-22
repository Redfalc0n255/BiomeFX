# Scope and decisions

Agreed 2026-11-22.

## What it is

An indoor mini greenhouse for small herbs (basil, rosemary and similar) kept alive through winter.

## Hardware, as on the schematic, nothing more

- ESP32 DevKit V1, 30-pin (ESP-WROOM-32)
- 2× BME280 on I2C
- Waveshare e-ink, SPI (model TBD)
- 2-channel 5 V relay module
- NOYITO isolated 120 VAC → 5 VDC module
- VIVOSUN seedling heat mat: plain on/off, 120 V, roughly 17–20 W for the 10″×20.75″ size, warms 10–20 °F above ambient. No built-in thermostat.
- Grow lights, 120 V

**Not in scope:** solar or battery power. The Waveshare Solar Power Manager model stays in `cad/reference` but isn't used.

## Control

From the laptop over **Bluetooth Low Energy**. No Wi-Fi or home server.

Plan: the ESP32 runs the greenhouse on its own and exposes BLE characteristics (readings, relay states, setpoints, light schedule). A single web page uses Web Bluetooth (Chrome or Edge) to connect, show readings and change settings. No app to install, and it can be hosted free on GitHub Pages.

## Phases

1. **Bring-up** (`firmware/bringup`): confirm every part works.
2. **Standalone control:** thermostat with hysteresis for the heat mat, timed lights, e-ink status. Runs without the laptop.
3. **Bluetooth control:** BLE service plus Web Bluetooth page.

## Starting targets for herbs (adjustable later)

| | Value | Why |
|---|---|---|
| Heat mat on below | 18 °C (64 °F) air | Basil stalls and gets damaged when it's cold. Rosemary is fine cooler. |
| Heat mat off above | 21 °C (70 °F) | 3 °C hysteresis so the relay isn't chattering. |
| Over-temp cutoff | 30 °C (86 °F) | Mat forced off whatever else is happening. |
| Lights | 14 h on (06:00–20:00) | Typical for basil and herbs indoors in winter. |

## Open questions

- Which Waveshare e-ink model (size and version)?
- **Keeping time:** with no Wi-Fi, the ESP32 only knows the time if the laptop sets it over Bluetooth. After a power cut the light schedule would be wrong until you reconnect. A DS3231 real-time clock module (about $5, same I2C bus) would fix that.
