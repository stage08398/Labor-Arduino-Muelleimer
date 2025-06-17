// Include the Servo library
#include <Servo.h>


// VARIABLEN
int Fuellstand = 0;
bool KlappeGeoeffnet = false;
bool KlappeGesperrt = false;
int MotionState = 0;
float KlappenTimer = 10;


// PINS

// SENSOREN
const int AbstandsSensorTrigger1 = 8;
const int AbstandsSensorTrigger2 = 6;
const int AbstandsSensorEcho1 = 7;
const int AbstandsSensorEcho2 = 5;

const int BewegungsSensor = 12;
const int Knopf = 9;

const int RFID = 12;

// AKTOREN
const int ServoMotorPin = 10;
Servo ServoMotor;

const int Buzzer = 11;

//LEDS
const int ledRot  = 4;
const int ledBlau = 3;
const int ledGruen = 2;



void setup() {
  // SENSOREN
  pinMode(AbstandsSensorTrigger1, OUTPUT);
  pinMode(AbstandsSensorTrigger2, OUTPUT);
  pinMode(AbstandsSensorEcho1, INPUT);
  pinMode(AbstandsSensorEcho2, INPUT);
  pinMode(BewegungsSensor, INPUT);
  pinMode(Knopf, INPUT);
  pinMode(RFID, INPUT);



  // AKTOREN
  pinMode(Buzzer, OUTPUT);
  ServoMotor.attach(ServoMotorPin);
  ServoMotor.write(0);
  

  // LEDS  
  pinMode(ledRot, OUTPUT);
  pinMode(ledBlau, OUTPUT);
  pinMode(ledGruen, OUTPUT);

  Serial.begin(9600);

}


void loop() {
  klappenLogik();
  schreibeFuellstand();
  setzeLEDFarbe();
  delay(10);
}



void klappenLogik() {
  if (digitalRead(Knopf) == 1) {
    Serial.println("Button!");
    oeffneKlappe();
  }

 
  
  if (digitalRead(RFID) == 1 && KlappeGesperrt == true) {
    KlappeGesperrt = false;
    Serial.println("RFID!");
    oeffneKlappe();
  
  }
  
  if (KlappeGeoeffnet == true) {
  	KlappenTimer = KlappenTimer - 0.5;
    if (KlappenTimer <= 0) {
      Serial.println("Schließe Klappe!");
      schliesseKlappe();
      KlappenTimer = 10;
    }
  }

}


void buzz(){
  delay(100);
  tone(Buzzer, 220, 100);
  delay(100);
}



void oeffneKlappe() {
  if (KlappeGeoeffnet != true && KlappeGesperrt == false)  {
    Serial.println("Öffne Klappe!");
    ServoMotor.write(90);
    KlappeGeoeffnet = true;
  } else if (KlappeGesperrt == true) {
  	Serial.println("Klappe geblockt!");
  } else if (KlappeGeoeffnet == true) {
  	Serial.println("Klappe schon offen!");
  }
}

void schliesseKlappe() {
  if (KlappeGeoeffnet == true)  {
  	Serial.println("Schließe Klappe!");
    ServoMotor.write(0);
    KlappeGeoeffnet = false;
  } else {
  Serial.println("Klappe schon zu!");
  }
}


void schreibeFuellstand() {
  float distanz1 = schreibeDistanz(AbstandsSensorTrigger1,AbstandsSensorEcho1);
  float distanz2 = schreibeDistanz(AbstandsSensorTrigger2,AbstandsSensorEcho2);
  

  if ((distanz1 < 50 && distanz1 > 5) && (distanz2 < 50 && distanz2 > 5)) { // beide an
    Fuellstand = 100;
  } else if (distanz1 < 50 && distanz1 > 5 && distanz2 > 50) { // erster an
    Fuellstand = 50;
  
  } else if (distanz1 > 50 && distanz2 > 50) { // beide aus
    Fuellstand = 0;
  }
  else {
    // ERROR CODE
    Fuellstand = 999;
  }

}



float schreibeDistanz(int triggerPin, int echoPin) {
  // --- https://funduino.de/nr-10-entfernung-messen

  // Sender kurz ausschalten um Störungen des Signal zu vermeiden
  digitalWrite(triggerPin, LOW);
  delay(5);
  // Signal für 10 Microsekunden senden, danach wieder ausschalten
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // pulseIn -> Zeit messen, bis das Signal zurückkommt
  long Zeit = pulseIn(echoPin, HIGH);
  // Entfernung in cm berechnen
  // Zeit/2 -> nur eine Strecke
  float Entfernung = (Zeit / 2) * 0.03432;
  return Entfernung;
}




void setzeLEDFarbe() {



  if (Fuellstand == 100 || KlappeGesperrt == true) {
    Serial.println("ROT");
    buzz();
    KlappeGesperrt = true;
    schliesseKlappe();
  
  
    digitalWrite(ledRot, HIGH);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, LOW);

  } else if (Fuellstand == 50) {
    Serial.println("GELB");
    digitalWrite(ledRot, HIGH);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, HIGH);

  } else if (Fuellstand == 0) {
    Serial.println("GRÜN");
    digitalWrite(ledRot, LOW);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, HIGH);
  } else {
    Serial.println("FEHLER - Zweiter Sensor an aber Erster nicht");
  }
}
