int pinMerah1 = 8;
int pinKuning1 = 9;
int pinHijau1 = 10;
int pinMerah2 = 5;
int pinKuning2 = 6;
int pinHijau2 = 7;


void setup() {
  // put your setup code here, to run once:
  pinMode(pinMerah1, OUTPUT);
  pinMode(pinKuning1, OUTPUT);
  pinMode(pinHijau1, OUTPUT);
  pinMode(pinMerah2, OUTPUT);
  pinMode(pinKuning2, OUTPUT);
  pinMode(pinHijau2, OUTPUT);

  digitalWrite(pinMerah1, HIGH);
  digitalWrite(pinMerah2, HIGH);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // KUNING1 DAN MERAH 2 NYALA
  digitalWrite(pinMerah1, LOW);
  digitalWrite(pinKuning1, HIGH);
  digitalWrite(pinHijau1, LOW);  

  digitalWrite(pinMerah2, HIGH);
  digitalWrite(pinKuning2, LOW);
  digitalWrite(pinHijau2, LOW);  
  delay(1000);
  
  // HIJAU1 DAN MERAH 2 NYALA
  digitalWrite(pinMerah1, LOW);
  digitalWrite(pinKuning1, LOW);
  digitalWrite(pinHijau1, HIGH);  
  
  digitalWrite(pinMerah2, HIGH);
  digitalWrite(pinKuning2, LOW);
  digitalWrite(pinHijau2, LOW);  
  delay(1000);
  
  // KUNING1 DAN MERAH 2 NYALA
  digitalWrite(pinMerah1, LOW);
  digitalWrite(pinKuning1, HIGH);
  digitalWrite(pinHijau1, LOW);  
  
  digitalWrite(pinMerah2, HIGH);
  digitalWrite(pinKuning2, LOW);
  digitalWrite(pinHijau2, LOW);  
  delay(1000);

  // MERAH1 NYALA DAN KUNING 2 NYALA
  digitalWrite(pinMerah1, HIGH);
  digitalWrite(pinKuning1, LOW);
  digitalWrite(pinHijau1, LOW);  

  digitalWrite(pinMerah2, LOW);
  digitalWrite(pinKuning2, HIGH);
  digitalWrite(pinHijau2, LOW);  
  delay(1000);
  
  // MERAH1 NYALA DAN HIJAU 2 NYALA
  digitalWrite(pinMerah1, HIGH);
  digitalWrite(pinKuning1, LOW);
  digitalWrite(pinHijau1, LOW);  
  
  digitalWrite(pinMerah2, LOW);
  digitalWrite(pinKuning2, LOW);
  digitalWrite(pinHijau2, HIGH);  
  delay(1000);
  
  // MERAH1 NYALA DAN KUNING 2 NYALA
  digitalWrite(pinMerah1, HIGH);
  digitalWrite(pinKuning1, LOW);
  digitalWrite(pinHijau1, LOW);  
  
  digitalWrite(pinMerah2, LOW);
  digitalWrite(pinKuning2, HIGH);
  digitalWrite(pinHijau2, LOW);  
  delay(1000);
}
