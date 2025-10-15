// ================== TUGAS1_7SEG_COUNTING.ino ==================
const int segA = 2;
const int segB = 3;
const int segC = 4;
const int segD = 5;
const int segE = 6;
const int segF = 7;
const int segG = 8;
const int segDP = 9;

const int digit1 = 10; // ribuan
const int digit2 = 11; // ratusan
const int digit3 = 12; // puluhan
const int digit4 = 13; // satuan

int segPins[]   = {segA, segB, segC, segD, segE, segF, segG, segDP};
int digitPins[] = {digit1, digit2, digit3, digit4};

// ================== Tabel Angka 0–9 ==================
byte angka[10][8] = {
  {0,0,0,0,0,0,1,1}, // 0
  {1,0,0,1,1,1,1,1}, // 1
  {0,0,1,0,0,1,0,1}, // 2
  {0,0,0,0,1,1,0,1}, // 3
  {1,0,0,1,1,0,0,1}, // 4
  {0,1,0,0,1,0,0,1}, // 5
  {0,1,0,0,0,0,0,1}, // 6
  {0,0,0,1,1,1,1,1}, // 7
  {0,0,0,0,0,0,0,1}, // 8
  {0,0,0,0,1,0,0,1}  // 9
};

const unsigned long UPDATE_INTERVAL = 200; // ms antar perubahan angka
const unsigned long PAUSE_DELAY = 3000;    // jeda 3 detik saat ganti arah
const int MAX_N = 60;
const bool LEADING_ZERO = true;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }
}

// ================== Fungsi Tampilan ==================
void tampilAngka(int num, int digit) {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], LOW);
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], angka[num][i]);
  digitalWrite(digitPins[digit], HIGH);
  delay(2);
}

void blankDigit(int digit) {
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], LOW);
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], HIGH);
  delay(2);
}

void displayNumber(int n, bool leadingZero) {
  if (n < 0) n = 0;
  if (n > 9999) n = 9999;

  int d0 = (n / 1000) % 10;
  int d1 = (n / 100) % 10;
  int d2 = (n / 10) % 10;
  int d3 = n % 10;

  if (leadingZero || d0 != 0 || n >= 1000) tampilAngka(d0, 0);
  else blankDigit(0);

  if (leadingZero || d0 != 0 || d1 != 0 || n >= 100) tampilAngka(d1, 1);
  else blankDigit(1);

  if (leadingZero || d0 != 0 || d1 != 0 || d2 != 0 || n >= 10) tampilAngka(d2, 2);
  else blankDigit(2);

  tampilAngka(d3, 3);
}

// ================== Loop utama ==================
void loop() {
  static int n = 0;
  static unsigned long lastUpdate = 0;
  static unsigned long pauseStart = 0;

  // 0 = naik ke 60s
  // 1 = pause di 60
  // 2 = turun ke 0
  // 3 = selesai (tampil 0 terus, tidak update lagi)
  static uint8_t phase = 0;

  const int batas = MAX_N;

  switch (phase) {
    case 0: // naik
      if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        if (n < batas) {
          n++;
          if (n >= batas) {
            n = batas;
            phase = 1;                 // masuk fase pause
            pauseStart = millis();
          }
        }
        lastUpdate = millis();
      }
      break;

    case 1: // pause di 60
      if (millis() - pauseStart >= PAUSE_DELAY) {
        phase = 2;                     // lanjut turun
        lastUpdate = millis();
      }
      break;

    case 2: // turun
      if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        if (n > 0) {
          n--;
          if (n <= 0) {
            n = 0;
            phase = 3;                 // selesai, stop counting
          }
        }
        lastUpdate = millis();
      }
      break;

    case 3: // selesai (do nothing; tetap tampilkan 0)
      break;
  }

  displayNumber(n, LEADING_ZERO);
}
