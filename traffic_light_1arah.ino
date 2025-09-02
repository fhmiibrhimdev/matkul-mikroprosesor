// FAHMI IBRAHIM (2403321073) EC-3D

int pinMerah = 8;
int pinKuning = 9;
int pinHijau = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinMerah, OUTPUT);
  pinMode(pinKuning, OUTPUT);
  pinMode(pinHijau, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pinMerah, HIGH);
  digitalWrite(pinKuning, LOW);
  digitalWrite(pinHijau, LOW);
  delay(5000);
  
  digitalWrite(pinMerah, LOW);
  digitalWrite(pinKuning, HIGH);
  digitalWrite(pinHijau, LOW);
  delay(1000);
  
  digitalWrite(pinMerah, LOW);
  digitalWrite(pinKuning, LOW);
  digitalWrite(pinHijau, HIGH);
  delay(3000);

  digitalWrite(pinMerah, LOW);
  digitalWrite(pinKuning, HIGH);
  digitalWrite(pinHijau, LOW);
  delay(1000);
}
