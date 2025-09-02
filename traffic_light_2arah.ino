// DEKLARASI : variabel, data, alamat pin

int merah1 = 8;
int kuning1 = 9;
int hijau1 = 10;
int merah2 = 5;
int kuning2 = 6;
int hijau2 = 7;


void setup() {
  pinMode(merah1, OUTPUT);
  pinMode(kuning1, OUTPUT);
  pinMode(hijau1, OUTPUT);
  pinMode(merah2, OUTPUT);
  pinMode(kuning2, OUTPUT);
  pinMode(hijau2, OUTPUT);

  digitalWrite(merah1, HIGH);
  digitalWrite(merah2, HIGH);
  delay(1000);
}

void loop() {

  digitalWrite(merah1, LOW);
  digitalWrite(kuning1, HIGH);
  digitalWrite(hijau1, LOW);  

  digitalWrite(merah2, HIGH);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, LOW);  
  delay(1000);
  

  digitalWrite(merah1, LOW);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, HIGH);  
  
  digitalWrite(merah2, HIGH);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, LOW);  
  delay(1000);
  

  digitalWrite(merah1, LOW);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, HIGH);  
  
  digitalWrite(merah2, HIGH);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, LOW);  
  delay(1000);


  digitalWrite(merah1, LOW);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, HIGH);  

  digitalWrite(merah2, HIGH);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, LOW);  
  delay(1000);
  

  digitalWrite(merah1, LOW);
  digitalWrite(kuning1, HIGH);
  digitalWrite(hijau1, LOW);  
  
  digitalWrite(merah2, HIGH);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, LOW);  
  delay(1000);
  

  digitalWrite(merah1, HIGH);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, LOW);  
  
  digitalWrite(merah2, LOW);
  digitalWrite(kuning2, HIGH);
  digitalWrite(hijau2, LOW);  
  delay(1000);

  digitalWrite(merah1, HIGH);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, LOW);  
  
  digitalWrite(merah2, LOW);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, HIGH);  
  delay(1000);

  digitalWrite(merah1, HIGH);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, LOW);  
  
  digitalWrite(merah2, LOW);
  digitalWrite(kuning2, LOW);
  digitalWrite(hijau2, HIGH);  
  delay(1000);

  digitalWrite(merah1, HIGH);
  digitalWrite(kuning1, LOW);
  digitalWrite(hijau1, LOW);  
  
  digitalWrite(merah2, LOW);
  digitalWrite(kuning2, HIGH);
  digitalWrite(hijau2, LOW);  
  delay(1000);
}
