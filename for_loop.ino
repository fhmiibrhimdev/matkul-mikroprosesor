int LED[6] = {2, 3, 4, 5, 6, 7};

void setup() {
  // put your setup code here, to run once:
  int i;
  for (i = 0; i <= 5; i++)
  {
    pinMode(LED[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int p;
  for (p = 0; p <= 5; p++)
  {
    digitalWrite(LED[p], HIGH);
    delay(1000);
  }

  int o;
  for (o = 5; o >= 0; o--)
  {
    digitalWrite(LED[o], LOW);
    delay(1000);
  }
}
