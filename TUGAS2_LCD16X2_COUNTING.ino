// ================== TUGAS2_LCD_COUNTING.ino ==================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const unsigned long UPDATE_INTERVAL = 500; // ms antar angka
const unsigned long PAUSE_DELAY = 3000;    // jeda 3 detik
const int MAX_N = 60;                      // batas 60 detik

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Count: 0000");
  lcd.setCursor(0, 1); lcd.print("Arah : Naik ");
}

void updateLCD(int n, int arah, bool isPaused) {
  if (n < 0) n = 0;
  if (n > 9999) n = 9999;

  char buf[5];
  snprintf(buf, sizeof(buf), "%04d", n);
  lcd.setCursor(7, 0);
  lcd.print(buf);

  lcd.setCursor(7, 1);
  if (isPaused) lcd.print("PAUSE ");
  else lcd.print(arah == 1 ? "Naik  " : "Turun ");
}

void loop() {
  static int n = 0;
  static int arah = 1; // 1=naik, -1=turun
  static unsigned long lastUpdate = 0;
  static unsigned long pauseStart = 0;
  static bool isPaused = false;
  static int lastShown = -1;
  static int lastArah = 1;
  static bool lastPauseState = false;

  int batas = MAX_N;

  // Mode PAUSE
  if (isPaused) {
    if (millis() - pauseStart >= PAUSE_DELAY) {
      isPaused = false;
      lastUpdate = millis();
    }
  }
  else {
    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
      n += arah;

      if (n >= batas) {
        n = batas;
        arah = -1;
        isPaused = true;
        pauseStart = millis();
      } else if (n <= 0) {
        n = 0;
        arah = 1;
        isPaused = true;
        pauseStart = millis();
      }

      lastUpdate = millis();
    }
  }

  // Update LCD hanya jika berubah
  if (n != lastShown || arah != lastArah || isPaused != lastPauseState) {
    updateLCD(n, arah, isPaused);
    lastShown = n;
    lastArah = arah;
    lastPauseState = isPaused;
  }
}
