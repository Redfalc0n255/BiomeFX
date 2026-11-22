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

From the laptop over **Bluetooth Low Energy**. Wi-Fi is used only to get the time (see Timekeeping). There's no home server.

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

## Timekeeping

The ESP32 joins home Wi-Fi only to get the time from an internet time server (NTP, `pool.ntp.org`), using the `America/New_York` timezone so daylight saving is handled automatically. It re-syncs every few hours. Wi-Fi and Bluetooth run together on the ESP32's shared radio; the NimBLE Bluetooth library keeps memory use down.

- Wi-Fi name and password go in `secrets.h`, which is in `.gitignore` and never committed.
- If Wi-Fi is down at boot, the ESP32 keeps retrying in the background. Until it has the time, the lights follow a fallback (on for 14 h from power-up) and the heat mat thermostat works normally, since it doesn't need the clock.
- The laptop page can also set the time over Bluetooth as a backup.

## Open questions

- Which Waveshare e-ink model (size and version)?
