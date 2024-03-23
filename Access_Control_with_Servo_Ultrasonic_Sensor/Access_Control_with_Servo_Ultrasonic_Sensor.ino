#include <Servo.h>

Servo monServo;            // Déclaration de l'objet pour le servomoteur
const int servoPin = 13;   // Broche utilisée pour le signal du servomoteur
const int echoPin = 12;    // Broche Echo du capteur ultrasonique
const int trigPin = 11;    // Broche Trig du capteur ultrasonique
const int accessPin = 10;  // Broche pour indiquer l'accès autorisé
const int deniedPin = 9;   // Broche pour indiquer l'accès refusé
int position = 100;        // Position initiale du servomoteur

unsigned long MESURE_TIMEOUT = 25000UL;  // Temps limite pour la mesure de distance

void setup() {
  monServo.attach(servoPin);   // Initialisation du servomoteur
  pinMode(trigPin, OUTPUT);    // Configuration du Trig comme sortie
  pinMode(echoPin, INPUT);     // Configuration de l'Echo comme entrée
  pinMode(accessPin, OUTPUT);  // Configuration de la broche d'accès comme sortie
  pinMode(deniedPin, OUTPUT);  // Configuration de la broche de refus comme sortie

  digitalWrite(trigPin, LOW);  // Mise à LOW pour le Trig
  Serial.begin(9600);          // Initialisation de la communication série
}

void loop() {
  delay(500);                   // Attente pour la stabilité des mesures
  digitalWrite(trigPin, HIGH);  // Envoi de l'impulsion au capteur ultrasonique
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, MESURE_TIMEOUT);  // Mesure de la durée de l'impulsion Echo
  float distance = duration * 0.0344 / 2;                  // Conversion en distance en cm

  if (distance <= 15) {             // Si la distance est inférieure à 15 cm
    digitalWrite(deniedPin, LOW);   // Accès refusé (LED rouge)
    digitalWrite(accessPin, HIGH);  // Accès autorisé (LED verte)

    if (position < 180) {                                     // Si la position du servomoteur est inférieure à 180 degrés
      for (position = 100; position <= 180; position += 5) {  // Ouverture de la porte
        monServo.write(position);                             // Déplacement du servomoteur
        delay(100);                                           // Pause pour le mouvement
      }
    }
  } else {                          // Si la distance est supérieure à 15 cm
    digitalWrite(accessPin, LOW);   // Accès refusé (LED verte)
    digitalWrite(deniedPin, HIGH);  // Accès autorisé (LED rouge)

    if (position > 100) {                                     // Si la position du servomoteur est supérieure à 100 degrés
      for (position = 180; position >= 100; position -= 5) {  // Fermeture de la porte
        monServo.write(position);                             // Déplacement du servomoteur
        delay(100);                                           // Pause pour le mouvement
      }
    }
  }
}