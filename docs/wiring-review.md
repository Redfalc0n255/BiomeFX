# Wiring review: schematic rev 1.0

Issues found while reading the schematic, most important first. Fix these before writing control code, since the first two can make the board fail to boot or the relays misbehave.

## 1. Relay IN2 on GPIO12 can stop the ESP32 booting

GPIO12 (MTDI) is a strapping pin. If it is HIGH at reset, the ESP32 sets its flash voltage to 1.8 V and won't boot. Most relay modules have pull-ups on their IN pins (they're active-LOW), so this is likely to cause a boot loop.

**Fix:** move IN2 to a pin with no boot function, such as GPIO26 or GPIO27.

## 2. Relay IN1 on GPIO14 clicks at boot

GPIO14 outputs a PWM signal during boot, so the heat mat relay may click briefly on every reset.

**Fix:** move IN1 to GPIO25, 26, 27, 32 or 33.

## 3. 5 V relay module is powered from 3.3 V

Relay VDD is wired to +3.3 V. A 5 V relay coil may not pull in reliably at 3.3 V.

**Fix:** power VDD (or JD-VCC on opto-isolated modules) from the 5 V supply. Then check the input side: with VCC at 5 V, a 3.3 V HIGH may not turn the opto LED fully off. Use the JD-VCC jumper split (VCC = 3.3 V, JD-VCC = 5 V), a 3.3 V-trigger relay module, or a transistor driver.

## 4. The 5 V supply isn't connected to anything

The PSU's +5 V net doesn't connect to ESP32 VIN or the relay, so the board is presumably running from USB.

**Fix:** connect +5 V to ESP32 VIN and to the relay coil supply, with GND shared. The NOYITO 5 V module has plenty of headroom: the ESP32 with Bluetooth draws about 250 mA peak, each relay coil about 70 mA, and the e-ink only a few mA. Don't power the ESP32 from USB and the 5 V supply at the same time unless your DevKit has a diode on VIN (most V1 boards do; check yours).

## 5. No fuse, no earth

There is no fuse on the 120 VAC input, and the plug's ground pin isn't connected.

**Fix:** add an inline fuse sized for the total load, and carry earth through to the loads and any metal enclosure.

## 6. Nothing limits the heat mat temperature directly (low risk)

The BME280s measure air, not the mat's surface, and the VIVOSUN mat has no thermostat. It's a low-power mat that only warms 10–20 °F above ambient, so it can't run away dangerously. But if the firmware hangs with the relay on, it will cook the plant roots.

**Fix in firmware:** a hardware watchdog that resets the ESP32 (relays default off at boot), plus a 30 °C air cutoff. A DS18B20 probe under the tray is optional.

## Fine as drawn

- SPI e-ink on the standard VSPI pins (works with GxEPD2's default wiring).
- Two BME280s on one I2C bus at different addresses.
- Relays switch the line conductor, and the loads are on the NO contacts so they fail off.
