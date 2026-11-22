// BiomeFX hardware bring-up.
// Tests each part on its own from the Serial Monitor (115200 baud). Type a letter and press Enter.
//
//   h  help             s  scan I2C bus          r  read both BME280s once
//   w  watch readings every 2 s (toggle)         d  draw a test screen on the e-ink
//   1  toggle heat mat relay                     2  toggle grow light relay
//   0  all relays off
//
// Relays switch themselves off after RELAY_TEST_TIMEOUT_MS so nothing stays on by accident.

#include <Arduino.h>
#include <Wire.h>
#include <esp_system.h>
#include <Adafruit_BME280.h>
#include "config.h"

#if ENABLE_DISPLAY
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
GxEPD2_BW<EPD_PANEL, EPD_PANEL::HEIGHT> display(
    EPD_PANEL(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
#endif

// ---------- Relays ----------

struct Relay {
  uint8_t pin;
  const char *name;
  bool on;
  uint32_t onSince;
};

Relay relays[] = {
    {PIN_RELAY_HEAT, "Heat mat", false, 0},
    {PIN_RELAY_LIGHT, "Grow lights", false, 0},
};

static inline uint8_t relayLevel(bool on) {
  return (on ^ RELAY_ACTIVE_LOW) ? HIGH : LOW;
}

void setRelay(Relay &r, bool on) {
  r.on = on;
  r.onSince = millis();
  digitalWrite(r.pin, relayLevel(on));
  Serial.printf("[relay] %-11s -> %s (GPIO%d = %s)\n", r.name, on ? "ON " : "OFF", r.pin,
                relayLevel(on) ? "HIGH" : "LOW");
}

void allRelaysOff() {
  for (auto &r : relays) setRelay(r, false);
}

// ---------- BME280 ----------

Adafruit_BME280 bmeA, bmeB;
bool bmeOkA = false, bmeOkB = false;

// Reads the chip ID register directly so we can tell a real BME280 (0x60)
// from a BMP280 (0x58, no humidity) sold under the same name.
int readChipId(uint8_t addr) {
  Wire.beginTransmission(addr);
  Wire.write(0xD0);
  if (Wire.endTransmission(false) != 0) return -1;
  if (Wire.requestFrom(addr, (uint8_t)1) != 1) return -1;
  return Wire.read();
}

const char *chipName(int id) {
  switch (id) {
    case 0x60: return "BME280";
    case 0x58: return "BMP280 (no humidity!)";
    case 0x56:
    case 0x57: return "BMP280 sample";
    case -1:   return "no response";
    default:   return "unknown";
  }
}

void initSensors() {
  for (uint8_t addr : {BME_ADDR_A, BME_ADDR_B}) {
    int id = readChipId(addr);
    Serial.printf("[bme] 0x%02X chip id: %s (0x%02X)\n", addr, chipName(id), id < 0 ? 0 : id);
  }
  bmeOkA = bmeA.begin(BME_ADDR_A, &Wire);
  bmeOkB = bmeB.begin(BME_ADDR_B, &Wire);
  Serial.printf("[bme] sensor A (0x%02X): %s\n", BME_ADDR_A, bmeOkA ? "OK" : "FAILED");
  Serial.printf("[bme] sensor B (0x%02X): %s\n", BME_ADDR_B, bmeOkB ? "OK" : "FAILED");
}

void printReading(const char *label, Adafruit_BME280 &bme, bool ok) {
  if (!ok) {
    Serial.printf("  %s: not available\n", label);
    return;
  }
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0f;
  Serial.printf("  %s: %6.2f C  (%5.1f F)   %5.1f %%RH   %7.1f hPa\n", label, t, t * 9 / 5 + 32, h, p);
}

void readSensors() {
  Serial.println("[bme] readings:");
  printReading("A 0x76", bmeA, bmeOkA);
  printReading("B 0x77", bmeB, bmeOkB);
  if (bmeOkA && bmeOkB) {
    Serial.printf("  difference A-B: %.2f C\n", bmeA.readTemperature() - bmeB.readTemperature());
  }
}

void scanI2C() {
  Serial.println("[i2c] scanning...");
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  found device at 0x%02X\n", addr);
      found++;
    }
  }
  Serial.printf("[i2c] %d device(s). Expect 0x76 and 0x77.\n", found);
}

// ---------- Display ----------

void drawTestScreen() {
#if ENABLE_DISPLAY
  Serial.println("[epd] drawing test screen (full refresh takes a few seconds)...");
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(4, 18);
    display.print("BiomeFX bring-up");
    display.setCursor(4, 40);
    if (bmeOkA) display.printf("A: %.1fC %.0f%%", bmeA.readTemperature(), bmeA.readHumidity());
    else display.print("A: --");
    display.setCursor(4, 60);
    if (bmeOkB) display.printf("B: %.1fC %.0f%%", bmeB.readTemperature(), bmeB.readHumidity());
    else display.print("B: --");
    display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
  } while (display.nextPage());
  display.hibernate();
  Serial.println("[epd] done. If the screen didn't change, check EPD_PANEL in config.h.");
#else
  Serial.println("[epd] display disabled: set ENABLE_DISPLAY 1 and EPD_PANEL in config.h.");
#endif
}

// ---------- Menu ----------

void printHelp() {
  Serial.println();
  Serial.println("=== BiomeFX bring-up ===");
  Serial.println(" s  scan I2C        r  read sensors     w  watch readings (toggle)");
  Serial.println(" d  e-ink test      1  heat mat relay   2  grow light relay");
  Serial.println(" 0  all relays off  h  this help");
  Serial.printf(" Relays auto-off after %lu s.\n\n", RELAY_TEST_TIMEOUT_MS / 1000);
}

bool watching = false;
uint32_t lastWatch = 0;

void setup() {
  // Drive relay pins to OFF before making them outputs, so they don't glitch on.
  for (auto &r : relays) {
    digitalWrite(r.pin, relayLevel(false));
    pinMode(r.pin, OUTPUT);
  }

  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nBiomeFX bring-up starting");
  Serial.printf("Reset reason: %d  (1 = power on; repeated resets point to the GPIO12 strapping issue)\n",
                (int)esp_reset_reason());

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  scanI2C();
  initSensors();

#if ENABLE_DISPLAY
  display.init(115200, true, 2, false);
  Serial.printf("[epd] init done, %dx%d\n", display.width(), display.height());
#endif

  allRelaysOff();
  printHelp();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case 'h': case '?': printHelp(); break;
      case 's': scanI2C(); break;
      case 'r': readSensors(); break;
      case 'w':
        watching = !watching;
        Serial.printf("[watch] %s\n", watching ? "on" : "off");
        break;
      case 'd': drawTestScreen(); break;
      case '1': setRelay(relays[0], !relays[0].on); break;
      case '2': setRelay(relays[1], !relays[1].on); break;
      case '0': allRelaysOff(); break;
      default: break;  // ignore newlines etc.
    }
  }

  for (auto &r : relays) {
    if (r.on && millis() - r.onSince > RELAY_TEST_TIMEOUT_MS) {
      Serial.printf("[relay] %s timed out\n", r.name);
      setRelay(r, false);
    }
  }

  if (watching && millis() - lastWatch > 2000) {
    lastWatch = millis();
    readSensors();
  }
}
