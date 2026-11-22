# Bring-up test

Tests each part of the electronics on its own, from the Serial Monitor.

## Setup

1. Install VS Code and the **PlatformIO IDE** extension.
2. Open this folder (`firmware/bringup`) in VS Code. PlatformIO downloads the ESP32 toolchain and libraries the first time.
3. Plug in the ESP32 by USB and click **Upload** (→ arrow in the bottom bar), then **Serial Monitor** (plug icon).
4. Type a letter and press Enter.

| Key | Test |
|---|---|
| `s` | Scan I2C. You should see `0x76` and `0x77`. |
| `r` | Read both BME280s once. |
| `w` | Print readings every 2 s (toggle). Breathe on a sensor and watch humidity jump. |
| `d` | Draw a test screen on the e-ink. |
| `1` / `2` | Toggle the heat mat / grow light relay. You should hear a click and see the LED. |
| `0` | All relays off. |

Relays switch themselves off after 60 s.

## Test order

1. **USB only, nothing else connected.** Upload and confirm the menu appears.
2. **Add the BME280s.** `s` should find both; the boot log shows each chip ID. `BMP280` means you have the cheaper sensor without humidity.
3. **Add the e-ink.** Set `ENABLE_DISPLAY 1` and `EPD_PANEL` in `src/config.h` for your model, upload, press `d`.
4. **Add the relay board, mains unplugged.** Press `1` and `2` and listen for clicks. If the board won't boot or keeps resetting with the relay attached, that's the GPIO12 issue in `docs/wiring-review.md`.
5. **Only then, mains.** Plug in the mat and lights with the enclosure closed and test `1` and `2` again.

## If something fails

| Symptom | Likely cause |
|---|---|
| Boot loop, or upload fails, with relay attached | GPIO12 pulled high by the relay board. Move IN2 to GPIO26 and update `config.h`. |
| Relay ON/OFF is reversed | Set `RELAY_ACTIVE_LOW false`. |
| Relay LED lights but no click | Relay powered from 3.3 V. Feed VCC/JD-VCC from 5 V. |
| I2C scan finds nothing | SDA/SCL swapped, or sensor not powered. |
| Only one address found | Both sensors have SDO tied the same way. |
| E-ink doesn't change | Wrong `EPD_PANEL`, or BUSY/RST wired wrong. |
