// https://wokwi.com/projects/444113981297684481

const int tombol1 = 2;
const int tombol2 = 3;
int LED[5] = { 8, 9, 10, 11, 12 };


void setup() {
  pinMode(tombol1, INPUT_PULLUP);
  pinMode(tombol2, INPUT_PULLUP);
  for (int i = 0; i < 5; i++) {
    pinMode(LED[i], OUTPUT);
  }
}

void loop() {
  while (digitalRead(tombol1) == LOW) {
    for (int o = 0; o < 5; o++) {
      digitalWrite(LED[o], HIGH);
    }
    delay(200);
    for (int p = 5; p >= 0; p--) {
      digitalWrite(LED[p], LOW); 
    }
    delay(200);
  }

  while (digitalRead(tombol2) == LOW) {
    for (int r = 0; r <= 5; r++)
    {
      digitalWrite(LED[r], HIGH);
      delay(200);
      digitalWrite(LED[r], LOW);
      delay(200);
    }
  }

  for (int q = 0; q < 3; q++) {
    digitalWrite(LED[q], LOW);
  }
}
