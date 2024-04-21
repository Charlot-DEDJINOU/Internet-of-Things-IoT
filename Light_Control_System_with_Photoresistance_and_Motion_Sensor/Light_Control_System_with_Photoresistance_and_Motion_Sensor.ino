#define LightPin 0
#define PIR 2

int lightReading;  // Variable pour stocker la lecture analogique de la photorésistance
int pirVal;        // Variable pour stocker la lecture du capteur de mouvement
int minute = 0;    // Variable pour compter le nombre de minutes d'inactivité

void setup() {
  pinMode(LightPin, INPUT);  // Configure la broche A0 (LightPin) comme entrée pour la photorésistance
  pinMode(PIR, INPUT);       // Configure la broche A2 (PIR) comme entrée pour le capteur de mouvement

  // Configure les broches de 3 à 12 comme sorties pour contrôler les LEDs
  for (int ledPin = 3; ledPin < 13; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }

  Serial.begin(9600);  // Initialise la communication série à 9600 bauds
}

void loop() {
  lightReading = analogRead(LightPin);  // Lit la valeur de la photorésistance
  pirVal = digitalRead(PIR);            // Lit la valeur du capteur de mouvement

  // Si un mouvement est détecté, réinitialise la minuterie à zéro
  if (pirVal)
    minute = 0;
  else {
    delay(1000);  // Attend 1 seconde pour éviter les fausses détections de mouvement
    minute += 1;  // Incrémente la minuterie d'inactivité
  }

  // Si la lumière est faible et qu'il y a eu moins de 5 minutes d'inactivité
  if (lightReading < 50 && minute < 60 * 5) {
    // Allume et éteint toutes les LEDs de 3 à 12 pour créer un effet de clignotement
    for (int ledPin = 3; ledPin < 13; ledPin++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
    }

    // Allume et éteint toutes les LEDs de 12 à 3 pour créer un effet de clignotement inverse
    for (int ledPin = 12; ledPin >= 3; ledPin--) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
    }
  }
}