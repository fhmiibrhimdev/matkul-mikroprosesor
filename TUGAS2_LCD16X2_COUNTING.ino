// ================== TUGAS2_LCD_COUNTING.ino (Fixed) ==================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const unsigned long UPDATE_INTERVAL = 200; // ms antar angka
const unsigned long PAUSE_DELAY = 3000;    // jeda 3 detik
const int MAX_N = 60;                      // batas 60 detik

// Helper: cetak 1 baris dipadatkan ke 16 kolom (menghapus sisa karakter lama)
void printRowPadded(uint8_t row, const char* text) {
  char buf[17];
  // potong ke 16 dan pad dengan spasi
  int len = 0;
  while (text[len] && len < 16) len++;
  for (int i = 0; i < 16; i++) buf[i] = (i < len) ? text[i] : ' ';
  buf[16] = '\0';
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  printRowPadded(0, "Count: 0000");   // baris 0
  printRowPadded(1, "Arah : Naik");   // baris 1
}

void updateLCD(int n, uint8_t phase) {
  // phase: 0=Naik, 1=Pause, 2=Turun, 3=Selesai
  if (n < 0) n = 0;
  if (n > 9999) n = 9999;

  // Update angka di baris 0, kolom setelah "Count: "
  char num[5];
  snprintf(num, sizeof(num), "%04d", n);
  lcd.setCursor(7, 0); // "Count: " = 7 char
  lcd.print(num);

  // Bangun teks baris 1 penuh lalu cetak dari kolom 0
  switch (phase) {
    case 0: printRowPadded(1, "Arah : Naik");        break;
    case 1: printRowPadded(1, "Arah : PAUSE");       break;
    case 2: printRowPadded(1, "Arah : Turun");       break;
    case 3: printRowPadded(1, "Status: Selesai");    break; // 15 char, auto dipad
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
    case 0: // Naik 0 -> 60
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

    case 1: // Pause di 60
      if (millis() - pauseStart >= PAUSE_DELAY) {
        phase = 2;                     // lanjut turun
        lastUpdate = millis();
      }
      break;

    case 2: // Turun 60 -> 0
      if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        if (n > 0) {
          n--;
          if (n <= 0) {
            n = 0;
            phase = 3;                 // selesai
          }
        }
        lastUpdate = millis();
      }
      break;

    case 3: // Selesai: tetap tampilkan 0, tidak counting lagi
      break;
  }

  // Update LCD hanya jika berubah
  if (n != lastShown || phase != lastPhase) {
    updateLCD(n, phase);
    lastShown = n;
    lastPhase = phase;
  }
}
