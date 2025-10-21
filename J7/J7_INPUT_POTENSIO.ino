#include <Arduino.h>

// Pin Configuration
const int POTENSIO = A0;
const int PIN_BUZZER = 8;
const int LED1 = 9;
const int LED2 = 10;
const int LED3 = 11;

// Timing variables
unsigned long waktuSerialTerakhir = 0;
unsigned long waktuBuzzerTerakhir = 0;
int statusBuzzer = LOW;
int intervalBuzzer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(POTENSIO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

void loop() {
  unsigned long waktuSekarang = millis();
  
  int baca = analogRead(POTENSIO);
  int jarak = map(baca, 0, 1023, 0, 100);
  
  if (waktuSekarang - waktuSerialTerakhir >= 500) {
    waktuSerialTerakhir = waktuSekarang;
    Serial.print("Jarak terukur: ");
    Serial.print(jarak);
    Serial.println(" cm");
  }
  
  if (jarak > 75) {
    intervalBuzzer = 1000;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }
  else if (jarak > 40 && jarak <= 75) {
    intervalBuzzer = 500;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
  }
  else if (jarak > 10 && jarak <= 40) {
    intervalBuzzer = 250;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
  }
  else {
    intervalBuzzer = 0;  // 0 berarti menyala terus
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
  }
  
  if (waktuSekarang - waktuBuzzerTerakhir >= intervalBuzzer) {
    waktuBuzzerTerakhir = waktuSekarang;
    
    if (intervalBuzzer == 0) {
      digitalWrite(PIN_BUZZER, HIGH);
    } else {
      statusBuzzer = !statusBuzzer;
      digitalWrite(PIN_BUZZER, statusBuzzer);
    }
  }
}
