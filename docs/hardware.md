# Hardware notes

Taken from `hardware/Schematic_Biome-Wiring_2026-11-22.pdf` (EasyEDA, rev 1.0, drawn 2026-04-19).

## ESP32 pin map

| GPIO | Connected to | Bus / notes |
|---|---|---|
| 21 | BME280 ×2 SDA | I2C |
| 22 | BME280 ×2 SCL | I2C |
| 23 | E-ink MOSI | VSPI |
| 18 | E-ink CLK | VSPI |
| 5  | E-ink CS | VSPI (strapping pin, OK as CS) |
| 17 | E-ink DC | |
| 16 | E-ink RST | |
| 4  | E-ink BUSY | |
| 14 | Relay IN1 → heat mat | ⚠ outputs PWM during boot, see review |
| 12 | Relay IN2 → grow lights | ⚠ strapping pin, see review |

## I2C addresses

| Sensor | SDO | CSB | Address |
|---|---|---|---|
| U1 | GND | 3.3 V | 0x76 |
| U6 | 3.3 V | 3.3 V | 0x77 |

(U1's SDO looks like it goes to GND on the schematic, but the label nearby is ambiguous. An I2C scan will confirm.)

## Mains side

- 120 VAC line → PSU AC/L, relay COM1 and COM2.
- Relay NO1 → heat mat L; NO2 → grow lights L. Loads are off when the relay is unpowered, which is the safe default.
- Neutral → PSU AC/N, heat mat N, grow lights N.
- Ground pin of the input plug is not connected to anything.

## CAD parts

Exported from Onshape in metres.

| File | Size (mm) | Notes |
|---|---|---|
| scaffolding 1 | 152 × 173 × 23 | U-shaped frame with cut-outs |
| scaffolding 4 (top level) | 152 × 153 × 23 | U-shaped frame with cut-outs |
| scaffolding-print/2, 3 | 35 × 94 × 11 | Open rectangular frame |
| scaffolding-print/4, 5 | 35 × 114 × 11 | Open rectangular frame |
| reference/Solar Power Manager | 45 × 41 × 12 | Waveshare board model (not yet in schematic) |
