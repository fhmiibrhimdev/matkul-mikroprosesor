#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Pin 7-Segment =====
const int segA = 2, segB = 3, segC = 4, segD = 5, segE = 6, segF = 7, segG = 8, segDP = 9;
const int digit1 = 10, digit2 = 11, digit3 = 12, digit4 = 13;
int segPins[] = {segA, segB, segC, segD, segE, segF, segG, segDP};
int digitPins[] = {digit1, digit2, digit3, digit4};

// ===== Tombol =====
const int SW1_PIN = A0;
const int SW2_PIN = A1;
const unsigned long DEBOUNCE_MS = 50;
const unsigned long HOLD_THRESHOLD_MS = 350; // <— PEMBEDA klik vs hold

// ===== Pola Angka (Common Anode) =====
byte angka[10][8] = {
  {0, 0, 0, 0, 0, 0, 1, 1}, {1, 0, 0, 1, 1, 1, 1, 1}, {0, 0, 1, 0, 0, 1, 0, 1},
  {0, 0, 0, 0, 1, 1, 0, 1}, {1, 0, 0, 1, 1, 0, 0, 1}, {0, 1, 0, 0, 1, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 1, 1, 1, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1, 0, 0, 1}
};

// ===== Setting =====
const int MAX_N_SW1 = 60;
const unsigned long UPDATE_SW1 = 500;
const unsigned long PAUSE_DELAY = 3000;
const unsigned long UPDATE_SW2 = 1000;
const unsigned long BLINK_TOTAL_MS = 1500;
const unsigned long BLINK_INTERVAL_MS = 300;
const bool LEADING_ZERO = true;

// ===== Variabel =====
int n = 0, arah = 1, savedValue = 0;
bool isPaused = false;
unsigned long lastUpdate = 0, pauseStart = 0;

bool blinkActive = false, blinkVisible = true;
unsigned long lastBlinkTick = 0, blinkStopAt = 0;

enum Mode {MODE_IDLE, MODE_SW1, MODE_SW2_RECORD, MODE_SW2_SAVED, MODE_SW2_PLAY, MODE_SW2_FINISH};
Mode mode = MODE_IDLE;

// Debounce & edge
int sw1Stable = HIGH, sw2Stable = HIGH, sw1PrevStable = HIGH, sw2PrevStable = HIGH;
unsigned long sw1LastChange = 0, sw2LastChange = 0;

// Press-timing SW2
unsigned long sw2PressStart = 0;

// ===== Utility tombol =====
bool btnPress(int pin, int &stable, int &prev, unsigned long &last) {
  int raw = digitalRead(pin); unsigned long now = millis();
  if (raw != stable && (now - last) > DEBOUNCE_MS) {
    prev = stable; stable = raw; last = now;
    if (prev == HIGH && stable == LOW) return true; // falling edge
  }
  return false;
}
bool btnHold(int pin) {
  return digitalRead(pin) == LOW;
}

// ===== Blink =====
void startBlink() {
  blinkActive = true;
  blinkVisible = true;
  lastBlinkTick = millis();
  blinkStopAt = millis() + BLINK_TOTAL_MS;
}
bool blinkDone() {
  return millis() > blinkStopAt;
}
bool shouldToggle() {
  return (millis() - lastBlinkTick) > BLINK_INTERVAL_MS;
}

// ===== LCD helper (center) =====
void lcdClearRow(int r) {
  lcd.setCursor(0, r);
  lcd.print("                ");
}
void lcdCenter(int row, const char* t) {
  int len = strlen(t); int pad = (16 - len) / 2; if (pad < 0) pad = 0;
  lcdClearRow(row); lcd.setCursor(pad, row); lcd.print(t);
}
void lcdCenter2(const char* a, const char* b) {
  lcdCenter(0, a);
  lcdCenter(1, b);
}

// ===== 7-Seg =====
void tampilAngka(int num, int d) {
  for (int i = 0; i < 4; i++)digitalWrite(digitPins[i], LOW);
  for (int i = 0; i < 8; i++)digitalWrite(segPins[i], angka[num][i]);
  digitalWrite(digitPins[d], HIGH);
  delay(2);
}
void blankDigit() {
  for (int i = 0; i < 4; i++)digitalWrite(digitPins[i], LOW);
  for (int i = 0; i < 8; i++)digitalWrite(segPins[i], HIGH);
}
void displayNumber(int x, bool lead) {
  if (x < 0)x = 0; if (x > 9999)x = 9999;
  int d0 = (x / 1000) % 10, d1 = (x / 100) % 10, d2 = (x / 10) % 10, d3 = x % 10;
  if (lead || x >= 1000)tampilAngka(d0, 0); else blankDigit();
  if (lead || x >= 100 || d0)tampilAngka(d1, 1); else blankDigit();
  if (lead || x >= 10 || d0 || d1)tampilAngka(d2, 2); else blankDigit();
  tampilAngka(d3, 3);
}

// ===== Setup =====
void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }
  pinMode(SW1_PIN, INPUT_PULLUP); pinMode(SW2_PIN, INPUT_PULLUP);
  lcd.init(); lcd.backlight(); lcd.clear();
  lcdCenter2("SMART COUNTER", "Ready...");
  delay(800);
}

// ===== Loop =====
void loop() {
  unsigned long now = millis();

  // Edge & state tombol
  bool sw1Click = btnPress(SW1_PIN, sw1Stable, sw1PrevStable, sw1LastChange); // falling
  bool sw2Fall  = btnPress(SW2_PIN, sw2Stable, sw2PrevStable, sw2LastChange); // falling
  bool sw2Held  = btnHold(SW2_PIN);
  static bool sw2PrevHeld = false;
  bool sw2ReleaseEdge = (!sw2Held && sw2PrevHeld); // rising (debounced by stable state)
  sw2PrevHeld = sw2Held;

  // ----- SW1: Auto up-down + pause -----
  if (sw1Click) {
    mode = MODE_SW1;
    arah = 1;
    isPaused = false;
    lastUpdate = now;
    lcdCenter2("SW1 MODE", "Counting...");
  }
  if (mode == MODE_SW1) {
    if (!isPaused && now - lastUpdate >= UPDATE_SW1) {
      n += arah;
      if (n >= MAX_N_SW1) {
        n = MAX_N_SW1;
        arah = -1;
        isPaused = true;
        pauseStart = now;
        lcdCenter2("Pause", " ");
      }
      else if (n <= 0) {
        n = 0;
        arah = 1;
        isPaused = true;
        pauseStart = now;
        lcdCenter2("Pause", " ");
      }
      else {
        char l1[17], l2[17];
        snprintf(l1, 17, "Count: %04d", n);
        snprintf(l2, 17, "Mode: %s", (arah == 1) ? "Naik" : "Turun");
        lcdCenter2(l1, l2);
      }
      lastUpdate = now;
    }
    if (isPaused && now - pauseStart >= PAUSE_DELAY) {
      isPaused = false;
      char l1[17], l2[17];
      snprintf(l1, 17, "Count: %04d", n);
      snprintf(l2, 17, "Mode: %s", (arah == 1) ? "Naik" : "Turun");
      lcdCenter2(l1, l2);
    }
  }

  // ====== SW2: Klik vs Hold pakai durasi ======

  // 1) FALLING EDGE: catat waktu mulai tekan
  if (sw2Fall) {
    sw2PressStart = now;
  }

  // 2) Saat sedang di-hold dan durasi >= threshold -> mulai RECORD (sekali saja)
  if (sw2Held && (now - sw2PressStart >= HOLD_THRESHOLD_MS) && mode != MODE_SW2_RECORD) {
    mode = MODE_SW2_RECORD;
    n = 0; lastUpdate = now;
    lcdCenter2("Recording...", "Hold to Save");
  }
  // Selama record & tombol masih ditekan -> naik tiap UPDATE_SW2
  if (mode == MODE_SW2_RECORD && sw2Held && now - lastUpdate >= UPDATE_SW2) {
    n++; if (n > 9999) n = 9999;
    lastUpdate = now;
    lcdCenter2("Recording...", "Hold to Save");
  }

  // 3) RELEASE EDGE: tentukan klik vs hold berdasar durasi
  if (sw2ReleaseEdge) {
    unsigned long dur = now - sw2PressStart;
    if (dur < HOLD_THRESHOLD_MS) {
      // === KLIK SINGKAT === -> jalankan playback 0 → savedValue
      mode = MODE_SW2_PLAY;
      n = 0; lastUpdate = now;
      lcdCenter2("CoFFee", "Counting");
    } else {
      // === HOLD === -> simpan nilai terakhir (Saved! blink)
      savedValue = n;
      startBlink();
      mode = MODE_SW2_SAVED;
      lcdCenter2("Saved!", " ");
    }
  }

  // 4) Blink “Saved!” (habis hold)
  if (mode == MODE_SW2_SAVED && blinkActive) {
    if (shouldToggle()) {
      blinkVisible = !blinkVisible;
      lastBlinkTick = now;
    }
    if (blinkDone()) {
      blinkActive = false;
      blinkVisible = true;
      mode = MODE_IDLE;
    }
    if (blinkVisible) lcdCenter2("Saved!", " ");
    else lcd.clear();
  }

  // 5) Playback 0 → savedValue (klik)
  if (mode == MODE_SW2_PLAY) {
    if (now - lastUpdate >= UPDATE_SW2) {
      if (n < savedValue) {
        n++;
        lcdCenter2("CoFFee", "Counting");
      }
      else {
        mode = MODE_SW2_FINISH;
        startBlink();
      }
      lastUpdate = now;
    }
  }

  // 6) Blink Finish / Take a Break!
  if (mode == MODE_SW2_FINISH && blinkActive) {
    if (shouldToggle()) {
      blinkVisible = !blinkVisible;
      lastBlinkTick = now;
    }
    if (blinkDone()) {
      blinkActive = false;
      blinkVisible = true;
      mode = MODE_IDLE;
    }
    if (blinkVisible) lcdCenter2("Finish", "Take a Break!");
    else lcd.clear();
  }

  // 7-Seg always refresh
  displayNumber(n, LEADING_ZERO);
}
