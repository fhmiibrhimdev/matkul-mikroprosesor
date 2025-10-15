// ================== SMART COUNTER: 7-Seg + LCD + SW1/SW2 (Anti-Flicker v2, Centered LCD) ==================
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Pin 7-Segment =====
const int segA = 2, segB = 3, segC = 4, segD = 5, segE = 6, segF = 7, segG = 8, segDP = 9;
const int digit1 = 10, digit2 = 11, digit3 = 12, digit4 = 13;
int segPins[]   = {segA, segB, segC, segD, segE, segF, segG, segDP};
int digitPins[] = {digit1, digit2, digit3, digit4};

// ===== Tombol =====
const int SW1_PIN = A0;
const int SW2_PIN = A1;
const unsigned long DEBOUNCE_MS = 50;
const unsigned long HOLD_THRESHOLD_MS = 350; // pembeda klik vs hold (SW2)

// ===== Pola Angka (Common Anode: 0=ON, 1=OFF) =====
const uint8_t angka[10][8] = {
  {0,0,0,0,0,0,1,1}, {1,0,0,1,1,1,1,1}, {0,0,1,0,0,1,0,1},
  {0,0,0,0,1,1,0,1}, {1,0,0,1,1,0,0,1}, {0,1,0,0,1,0,0,1},
  {0,1,0,0,0,0,0,1}, {0,0,0,1,1,1,1,1}, {0,0,0,0,0,0,0,1},
  {0,0,0,0,1,0,0,1}
};

// ===== Setting =====
const int MAX_N_SW1 = 60;
const unsigned long UPDATE_SW1 = 1000;      // interval SW1 (ms)
const unsigned long PAUSE_DELAY = 3000;     // jeda 3 detik di 60
const unsigned long UPDATE_SW2 = 1000;      // interval SW2 (ms)
const unsigned long BLINK_TOTAL_MS = 1500;
const unsigned long BLINK_INTERVAL_MS = 300;
const bool LEADING_ZERO = true;

// ===== Variabel umum =====
int n = 0;
int savedValue = 0;
unsigned long lastUpdate = 0, pauseStart = 0;

bool blinkActive = false, blinkVisible = true;
unsigned long lastBlinkTick = 0, blinkStopAt = 0;

enum Mode { MODE_IDLE, MODE_SW1, MODE_SW2_RECORD, MODE_SW2_SAVED, MODE_SW2_PLAY, MODE_SW2_FINISH };
Mode mode = MODE_IDLE;

// ===== SW1 state machine: 0=Naik, 1=Pause di 60, 2=Turun, 3=Selesai =====
uint8_t sw1Phase = 3;

// Debounce & edge
int sw1Stable = HIGH, sw2Stable = HIGH, sw1PrevStable = HIGH, sw2PrevStable = HIGH;
unsigned long sw1LastChange = 0, sw2LastChange = 0;

// Press-timing SW2
unsigned long sw2PressStart = 0;

// ======================================================================
// Utility tombol
// ======================================================================
bool btnPress(int pin, int &stable, int &prev, unsigned long &last) {
  int raw = digitalRead(pin);
  unsigned long now = millis();
  if (raw != stable && (now - last) > DEBOUNCE_MS) {
    prev = stable; stable = raw; last = now;
    if (prev == HIGH && stable == LOW) return true; // falling edge
  }
  return false;
}
bool btnHold(int pin) { return digitalRead(pin) == LOW; }

// ======================================================================
// Blink timing
// ======================================================================
void startBlink() { blinkActive = true; blinkVisible = true; lastBlinkTick = millis(); blinkStopAt = millis() + BLINK_TOTAL_MS; }
bool blinkDone() { return millis() > blinkStopAt; }
bool shouldToggle() { return (millis() - lastBlinkTick) > BLINK_INTERVAL_MS; }

// ======================================================================
// LCD throttle + CENTERING (tanpa clear berulang)
// ======================================================================
char prevL1[17] = "", prevL2[17] = "";
unsigned long nextLcdOkAt = 0;

static void makeCentered(const char* in, char out[17]) {
  // Center string 'in' ke lebar 16, trim jika lebih dari 16
  size_t len = strlen(in);
  if (len > 16) len = 16;
  int leftPad = (16 - (int)len) / 2;
  if (leftPad < 0) leftPad = 0;
  // isi spasi
  for (int i = 0; i < 16; ++i) out[i] = ' ';
  // copy di tengah
  for (size_t i = 0; i < len; ++i) out[leftPad + i] = in[i];
  out[16] = '\0';
}

void lcdShowCentered(const char* l1, const char* l2) {
  unsigned long now = millis();
  if (now < nextLcdOkAt) return;       // throttle ~20 Hz
  nextLcdOkAt = now + 50;

  char buf1[17], buf2[17];
  makeCentered(l1, buf1);
  makeCentered(l2, buf2);

  if (strncmp(prevL1, buf1, 16) || strncmp(prevL2, buf2, 16)) {
    lcd.setCursor(0, 0); lcd.print(buf1);
    lcd.setCursor(0, 1); lcd.print(buf2);
    strncpy(prevL1, buf1, 16); prevL1[16] = 0;
    strncpy(prevL2, buf2, 16); prevL2[16] = 0;
  }
}

// ======================================================================
// 7-Segment: Double-buffer + Scanner non-blocking
// ======================================================================
volatile uint8_t activeDigits[4]  = {255,255,255,255};
volatile uint8_t pendingDigits[4] = {255,255,255,255};
volatile bool pendingDirty = false;

void setPendingFromN(int x, bool lead) {
  uint8_t p0=255,p1=255,p2=255,p3=0;
  if (x < 0) x = 0; if (x > 9999) x = 9999;
  int d0 = (x / 1000) % 10, d1 = (x / 100) % 10, d2 = (x / 10) % 10, d3 = x % 10;

  p0 = (lead || x >= 1000) ? d0 : 255;
  p1 = (lead || x >= 100  || d0) ? d1 : 255;
  p2 = (lead || x >= 10   || d0 || d1) ? d2 : 255;
  p3 = d3;

  noInterrupts();
  pendingDigits[0]=p0; pendingDigits[1]=p1; pendingDigits[2]=p2; pendingDigits[3]=p3;
  pendingDirty = true;
  interrupts();
}

// dwell ~0.8 ms/digit → ~312 Hz total
void scanDisplay() {
  static uint8_t idx = 0;
  static unsigned long t = 0;
  if ((micros() - t) < 800) return;
  t = micros();

  // Commit pending di boundary frame
  if (idx == 0 && pendingDirty) {
    noInterrupts();
    activeDigits[0]=pendingDigits[0];
    activeDigits[1]=pendingDigits[1];
    activeDigits[2]=pendingDigits[2];
    activeDigits[3]=pendingDigits[3];
    pendingDirty = false;
    interrupts();
  }

  // 1) Matikan semua digit
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], LOW);

  // 2) Set pola segmen
  uint8_t digitVal = activeDigits[idx];
  if (digitVal <= 9) {
    for (int s = 0; s < 8; s++) digitalWrite(segPins[s], angka[digitVal][s]);
  } else {
    for (int s = 0; s < 8; s++) digitalWrite(segPins[s], HIGH); // blank
  }

  // 3) Nyalakan digit aktif
  digitalWrite(digitPins[idx], HIGH);

  // 4) Next digit
  idx = (idx + 1) & 0x03;
}

// ======================================================================
// Setup
// ======================================================================
void setup() {
  Wire.begin();
  Wire.setClock(400000); // percepat I2C (turunkan jika LCD bermasalah)

  for (int i = 0; i < 8; i++) { pinMode(segPins[i], OUTPUT); digitalWrite(segPins[i], HIGH); } // seg off
  for (int i = 0; i < 4; i++) { pinMode(digitPins[i], OUTPUT); digitalWrite(digitPins[i], LOW); } // digit off
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);

  lcd.init(); lcd.backlight(); lcd.clear();
  lcdShowCentered("SMART COUNTER", "Ready...");
  delay(800);

  setPendingFromN(n, LEADING_ZERO);
}

// ======================================================================
// Loop
// ======================================================================
void loop() {
  unsigned long now = millis();

  // Edge & state tombol
  bool sw1Click = btnPress(SW1_PIN, sw1Stable, sw1PrevStable, sw1LastChange); // falling
  bool sw2Fall  = btnPress(SW2_PIN, sw2Stable, sw2PrevStable, sw2LastChange); // falling
  bool sw2Held  = btnHold(SW2_PIN);
  static bool sw2PrevHeld = false;
  bool sw2ReleaseEdge = (!sw2Held && sw2PrevHeld); // rising
  sw2PrevHeld = sw2Held;

  // ----- SW1: 0 -> 60 -> (pause 3s) -> 60 -> 0 -> stop -----
  if (sw1Click) {
    mode = MODE_SW1;
    n = 0;
    sw1Phase = 0;          // mulai naik
    lastUpdate = now;
    lcdShowCentered("Counting", "Mode: Naik");
  }

  if (mode == MODE_SW1) {
    switch (sw1Phase) {
      case 0: // Naik 0 -> 60
        if (now - lastUpdate >= UPDATE_SW1) {
          if (n < MAX_N_SW1) {
            n++;
            lcdShowCentered("Counting", "Mode: Naik");
            if (n >= MAX_N_SW1) {
              n = MAX_N_SW1;
              sw1Phase = 1;            // jeda
              pauseStart = now;
              // kalau ingin tetap "Mode: Naik" saat jeda, pakai baris di bawah:
              // lcdShowCentered("Counting", "Mode: Naik");
              // atau tampilkan "Pause":
              lcdShowCentered("Counting", "Mode: Pause");
            }
          }
          lastUpdate = now;
        }
        break;

      case 1: // Pause di 60
        if (now - pauseStart >= PAUSE_DELAY) {
          sw1Phase = 2;                // lanjut turun
          lastUpdate = now;
          lcdShowCentered("Counting", "Mode: Turun");
        }
        break;

      case 2: // Turun 60 -> 0
        if (now - lastUpdate >= UPDATE_SW1) {
          if (n > 0) {
            n--;
            lcdShowCentered("Counting", "Mode: Turun");
            if (n <= 0) {
              n = 0;
              sw1Phase = 3;            // selesai
              lcdShowCentered("Counting", "Mode: Selesai");
            }
          }
          lastUpdate = now;
        }
        break;

      case 3: // Selesai
        // LCD tetap "Counting" + "Mode: Selesai"
        break;
    }
  }

  // ====== SW2: Klik vs Hold pakai durasi ======
  if (sw2Fall) { sw2PressStart = now; }

  if (sw2Held && (now - sw2PressStart >= HOLD_THRESHOLD_MS) && mode != MODE_SW2_RECORD) {
    mode = MODE_SW2_RECORD;
    n = 0; lastUpdate = now;
    lcdShowCentered("Recording...", "Hold to Save");
  }

  if (mode == MODE_SW2_RECORD && sw2Held && now - lastUpdate >= UPDATE_SW2) {
    n++; if (n > 9999) n = 9999;
    lastUpdate = now;
    lcdShowCentered("Recording...", "Hold to Save");
  }

  if (sw2ReleaseEdge) {
    unsigned long dur = now - sw2PressStart;
    if (dur < HOLD_THRESHOLD_MS) {
      mode = MODE_SW2_PLAY;
      n = 0; lastUpdate = now;
      lcdShowCentered("CoFFee", "Counting");
    } else {
      savedValue = n;
      startBlink();
      mode = MODE_SW2_SAVED;
      lcdShowCentered("Saved!", " ");
    }
  }

  if (mode == MODE_SW2_SAVED && blinkActive) {
    if (shouldToggle()) { blinkVisible = !blinkVisible; lastBlinkTick = now; }
    if (blinkDone()) { blinkActive = false; blinkVisible = true; mode = MODE_IDLE; }
    if (blinkVisible) lcdShowCentered("Saved!", " ");
    else              lcdShowCentered(" ", " ");
  }

  if (mode == MODE_SW2_PLAY) {
    if (now - lastUpdate >= UPDATE_SW2) {
      if (n < savedValue) {
        n++;
        lcdShowCentered("CoFFee", "Counting");
      } else {
        mode = MODE_SW2_FINISH;
        startBlink();
      }
      lastUpdate = now;
    }
  }

  if (mode == MODE_SW2_FINISH && blinkActive) {
    if (shouldToggle()) { blinkVisible = !blinkVisible; lastBlinkTick = now; }
    if (blinkDone()) { blinkActive = false; blinkVisible = true; mode = MODE_IDLE; }
    if (blinkVisible) lcdShowCentered("Finish", "Take a Break!");
    else              lcdShowCentered(" ", " ");
  }

  // ===== Refresh 7-seg =====
  setPendingFromN(n, LEADING_ZERO);  // update buffer pending
  scanDisplay();                     // scanner commit di boundary frame
}
