# BiomeFX: Greenhouse Biome

A small indoor greenhouse for herbs (basil, rosemary), controlled by an ESP32. The goal is to keep plants alive through the winter: the ESP32 reads temperature and humidity from two BME280 sensors, switches a heat mat and grow lights through a relay module, and shows status on a Waveshare e-ink display. Later it will be controlled from a laptop over Bluetooth. See [docs/scope.md](docs/scope.md).

## Status

**Phase 1: hardware bring-up.** Confirm each part of the electronics works on its own before writing the control logic. The test firmware and instructions are in [firmware/bringup](firmware/bringup).

- [ ] Both BME280 sensors found on I2C and returning sane readings
- [ ] E-ink display draws text
- [ ] Relay channel 1 (heat mat) switches
- [ ] Relay channel 2 (grow lights) switches
- [ ] Board boots reliably with everything connected

## Hardware

| Part | Role |
|---|---|
| ESP32 DevKit V1, 30-pin (ESP-WROOM-32) | Controller |
| 2× BME280 (I2C, 0x76 / 0x77) | Air temperature, humidity, pressure |
| Waveshare e-ink display (SPI) | Local status screen |
| 2-channel 5 V relay module | Switches heat mat (CH1) and grow lights (CH2) |
| NOYITO isolated 120 VAC → 5 VDC module | Powers the low-voltage side |
| VIVOSUN seedling heat mat (plain on/off) | 120 VAC, about 17–20 W |
| Grow lights | 120 VAC |

Pin assignments are in [docs/hardware.md](docs/hardware.md). The schematic is in [hardware/](hardware/).

## Repo layout

```
cad/                  STL exports from Onshape (units: exported in metres, scale ×1000 in your slicer if needed)
  scaffolding-print/  Print-ready scaffolding parts
  reference/          Waveshare Solar Power Manager model (not used in this project)
hardware/             Wiring schematic (EasyEDA PDF)
firmware/bringup/     PlatformIO test firmware for each part
docs/                 Hardware notes, wiring review, decisions
```

## Safety

This project switches 120 VAC mains. Work on the mains side only with it unplugged, keep mains wiring enclosed, and fuse the input.
