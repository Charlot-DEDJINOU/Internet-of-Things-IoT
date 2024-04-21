#define LightPin 0
#define PIR 2

int lightReading;  // Lecture analogique de la valeur de la photorésistance avec le diviseur de tension
int pirVal;        // Lecture du capteur de mouvement
int minute = 0; // Compter le nombre de minutes d'inactivité dans l'endroit


void setup() {

  pinMode(LightPin, INPUT);  // Designer la broche A0 comme input
  pinMode(PIR, INPUT);       // Designer la broche A0 comme input

  for (int ledPin = 3; ledPin < 13; ledPin++) {
    pinMode(ledPin, OUTPUT);  // Designer la broque ledPin comme OUTPUT
  }

  Serial.begin(9600);
}


void loop() {

  lightReading = analogRead(LightPin);  // Lecture de la valeur luminosité envoyé par la photorésistance
  pirVal = digitalRead(PIR);            // Lecture du capteur de mouvement

  if(pirVal)
    minute = 0; // Si on detecte un mouvement on ramene la minuterie à 0
  else {
    delay(1000);
    minute += 1; // Sinon on compte le nombre de seconde d'inactivité dans l'endroit
  }
 
  if (lightReading < 50 && minute < 60 * 5) {
    for (int ledPin = 3; ledPin < 13; ledPin++) {
      digitalWrite(ledPin, HIGH);  // Allumer le led qui se trouve sur la broche ledPin
      delay(100);
      digitalWrite(ledPin, LOW);  // Eteint le led qui se trouve sur la broche ledPin
    }

    for (int ledPin = 12; ledPin >= 3; ledPin--) {
      digitalWrite(ledPin, HIGH);  // Allumer le led qui se trouve sur la broche ledPin
      delay(100);
      digitalWrite(ledPin, LOW);  // Eteint le led qui se trouve sur la broche ledPin
    }
  }
}