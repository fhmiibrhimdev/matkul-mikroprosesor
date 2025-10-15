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

void updateLCD(int n, bool isPaused, uint8_t phase) {
  // phase: 0=Naik, 1=Pause, 2=Turun, 3=Selesai
  if (n < 0) n = 0;
  if (n > 9999) n = 9999;

  char buf[5];
  snprintf(buf, sizeof(buf), "%04d", n);
  lcd.setCursor(7, 0);
  lcd.print(buf);

  lcd.setCursor(7, 1);
  switch (phase) {
    case 0: lcd.print("Arah : Naik  "); break;
    case 1: lcd.print("Arah : PAUSE "); break;
    case 2: lcd.print("Arah : Turun "); break;
    case 3: lcd.print("Status: Seles"); break; // "Selesai" pas 16 kolom
  }
}

void loop() {
  static int n = 0;
  static unsigned long lastUpdate = 0;
  static unsigned long pauseStart = 0;

  // 0 = naik ke 60, 1 = pause di 60, 2 = turun ke 0, 3 = selesai
  static uint8_t phase = 0;

  // cache tampilan agar LCD tak sering di-update
  static int lastShown = -1;
  static uint8_t lastPhase = 255;

  const int batas = MAX_N;

  switch (phase) {
    case 0: // NAik 0 -> 60
      if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        if (n < batas) {
          n++;
          if (n >= batas) {
            n = batas;
            phase = 1;                 // masuk jeda
            pauseStart = millis();
          }
        }
        lastUpdate = millis();
      }
      break;

    case 1: // PAUSE di 60
      if (millis() - pauseStart >= PAUSE_DELAY) {
        phase = 2;                     // lanjut TURUN
        lastUpdate = millis();
      }
      break;

    case 2: // TURUN 60 -> 0
      if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        if (n > 0) {
          n--;
          if (n <= 0) {
            n = 0;
            phase = 3;                 // SELESAI
          }
        }
        lastUpdate = millis();
      }
      break;

    case 3: // SELESAI: tetap tampilkan 0, tidak counting lagi
      break;
  }

  // Update LCD hanya jika berubah
  if (n != lastShown || phase != lastPhase) {
    updateLCD(n, phase == 1, phase);
    lastShown = n;
    lastPhase = phase;
  }
}
