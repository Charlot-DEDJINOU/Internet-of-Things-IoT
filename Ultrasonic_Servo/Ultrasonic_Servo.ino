#include <Servo.h>
//on inclut la bilbliothèque pour piloter un servomoteur
Servo monServo;  //on créée l'objet monServo

const int trigPin = 12;
const int echoPin = 13;

long duration; 

int distance;

void setup() {
  monServo.attach(11);       //on définit le Pin utilisé par le servomoteur
  pinMode(trigPin, OUTPUT);  // Définit le trigPin comme sortie
  pinMode(echoPin, INPUT);   // Définit le echoPin comme entrée

  Serial.begin(9600);  // Commence la communication série
}

void loop() {
  // for (int position = 100; position <= 180; position+=10) {
  //     monServo.write(position);
  //     delay(500);
  // }

  // for (int position = 180; position >= 100; position-=10) {
  //     monServo.write(position);
  //     delay(500);
  // }
  digitalWrite(trigPin, LOW);  // Efface le trigPin
  delayMicroseconds(2);        // Met le trigPin à l'état HIGH pendant 2 microsecondes
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Met le trigPin à l'état HIGH pendant 10 microsecondes
  digitalWrite(trigPin, LOW);         // Lit le echoPin, renvoie le temps de trajet de l'onde sonore en microsecondes
  duration = pulseIn(echoPin, HIGH);  // Calcul de la distance
  distance = duration * 0.0344 / 2;    // La vitesse du son est d'environ 0.034 cm par microseconde
  // Affiche la distance sur le moniteur série
  Serial.print("Distance: ");
  Serial.println(distance);
}
