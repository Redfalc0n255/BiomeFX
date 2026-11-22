#pragma once
// Pin assignments: match hardware/Schematic_Biome-Wiring_2026-11-22.pdf (rev 1.0).

// I2C: two BME280s
#define PIN_I2C_SDA   21
#define PIN_I2C_SCL   22
#define BME_ADDR_A    0x76   // U1 (SDO -> GND)
#define BME_ADDR_B    0x77   // U6 (SDO -> 3.3V)

// SPI e-ink (VSPI)
#define PIN_EPD_MOSI  23
#define PIN_EPD_CLK   18
#define PIN_EPD_CS     5
#define PIN_EPD_DC    17
#define PIN_EPD_RST   16
#define PIN_EPD_BUSY   4

// Relays.
// WARNING: GPIO12 is a boot strapping pin and GPIO14 pulses at boot.
// If the board won't boot with the relay connected (or the mat relay clicks on reset),
// move the wires to GPIO26 / GPIO27 and change these two numbers. See docs/wiring-review.md.
#define PIN_RELAY_HEAT   14   // IN1 -> heat mat
#define PIN_RELAY_LIGHT  12   // IN2 -> grow lights

// Most blue 2-channel relay boards switch ON when IN is pulled LOW.
// If "ON" in the menu turns the relay off, set this to false.
#define RELAY_ACTIVE_LOW true

// Bring-up safety: any relay you switch on turns itself off after this long.
#define RELAY_TEST_TIMEOUT_MS (60UL * 1000UL)

// E-ink display. Set ENABLE_DISPLAY to 1 once EPD_PANEL matches your Waveshare model.
// Panel class names are listed in the GxEPD2 README, e.g.:
//   1.54" 200x200 V2  -> GxEPD2_154_D67
//   2.13" 250x122 V4  -> GxEPD2_213_BN
//   2.9"  296x128 V2  -> GxEPD2_290_T94_V2
//   4.2"  400x300 V2  -> GxEPD2_420_GDEY042T81
#define ENABLE_DISPLAY 0
#define EPD_PANEL GxEPD2_213_BN
