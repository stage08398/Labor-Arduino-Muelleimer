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

const int RFID = 13;

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

    MotionState = digitalRead(BewegungsSensor);
  if (MotionState == 1) {
    Serial.println("Motion!");
    oeffneKlappe();
  }  
  
  if (digitalRead(RFID) == 1 && KlappeGesperrt == true) {
    KlappeGesperrt = false;
    
    oeffneKlappe();
    while (Fuellstand != 0) {
      schreibeFuellstand();
    } 
  }
  
  if (KlappeGeoeffnet == true) {
  	KlappenTimer = KlappenTimer - 0.5;
    if (KlappenTimer <= 0) {
      Serial.println("CLOSE CLAP!");
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
    Serial.println("OPEN CLAP!");
    ServoMotor.write(90);
    KlappeGeoeffnet = true;
  } else if (KlappeGesperrt == true) {
  	Serial.println("Tried opening clap, but clap was blocked!");
  } else if (KlappeGeoeffnet == true) {
  	Serial.println("Tried opening clap, but clap was already open!");
  }
}

void schliesseKlappe() {
  if (KlappeGeoeffnet == true)  {
  	Serial.println("CLOSE CLAP!");
    ServoMotor.write(0);
    KlappeGeoeffnet = false;
  } else {
  Serial.println("Tried closing clap, but clap was already closed");
  }
}


void schreibeFuellstand() {
  float distance1 = schreibeDistanz(AbstandsSensorTrigger1,AbstandsSensorEcho1);
  float distance2 = schreibeDistanz(AbstandsSensorTrigger2,AbstandsSensorEcho2);

  if ((distance1 < 50 && distance1 > 5) && (distance2 < 50 && distance2 > 5)) { // beide an
    Fuellstand = 100;
  } else if (distance1 < 50 && distance1 > 5 && distance2 > 50) { // erster an
    Fuellstand = 50;
  
  } else if (distance1 > 50 && distance2 > 50) { // beide aus
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
  // Signal für 10 Micrsekunden senden, danach wieder ausschalten
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
    buzz();
    KlappeGesperrt = true;
    schliesseKlappe();
  
  
    digitalWrite(ledRot, HIGH);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, LOW);

  } else if (Fuellstand == 50) {
    digitalWrite(ledRot, HIGH);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, HIGH);

  } else if (Fuellstand == 0) {
    digitalWrite(ledRot, LOW);
    digitalWrite(ledBlau, LOW);
    digitalWrite(ledGruen, HIGH);
  } else {
    Serial.println("FEHLER - Zweiter Sensor an aber Erster nicht");
  }
}
