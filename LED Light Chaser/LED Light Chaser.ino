int lightPin = 0;

int lightReading;  // Lecture analogique de la valeur de la photorésistance avec le diviseur de tension


void setup() {

  pinMode(lightPin, INPUT);  // Designer la broche A0 comme input

  for (int ledPin = 6; ledPin < 14; ledPin++) {
    pinMode(ledPin, OUTPUT);  // Designer la broque ledPin comme OUTPUT
  }

  Serial.begin(9600);
}


void loop() {

  lightReading = analogRead(lightPin);  // Lecture de la valeur luminosité envoyé par la photorésistance

  //Affichage de la valeur de "lightReading" dans la console
  Serial.print("Lecture analogique = ");
  Serial.println(lightReading);  // Affichage de la valeur brute issue de la photorésistance.

  if (lightReading < 50) {
    for (int ledPin = 6; ledPin < 14; ledPin++) {
      digitalWrite(ledPin, HIGH);  // Allumer le led qui se trouve sur la broche ledPin
      delay(100);
      digitalWrite(ledPin, LOW);  // Eteint le led qui se trouve sur la broche ledPin
    }

    for (int ledPin = 13; ledPin >= 6; ledPin--) {
      digitalWrite(ledPin, HIGH);  // Allumer le led qui se trouve sur la broche ledPin
      delay(100);
      digitalWrite(ledPin, LOW);  // Eteint le led qui se trouve sur la broche ledPin
    }
  }
}