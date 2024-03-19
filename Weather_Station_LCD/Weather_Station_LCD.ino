#include <dht.h>            //on inclut la bibliothèque pour le capteur DHT11
#include <LiquidCrystal.h>  //on inclut la bibliothèque pour l’écran LCD

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //on crée l’objet LCD et on définit les Pins utilisés

dht DHT;  // on crée l’objet du capteur DHT11

#define DHT11_PIN 7  //on définit le Pin utilisé pour les données du DHT11

void setup() {
  lcd.begin(16, 2);  //on initialise la communication avec l’écran LCD
}

void loop() {
  int chk = DHT.read11(DHT11_PIN);  //on lit les données du capteur DHT
  lcd.setCursor(0, 0);              //on place le curseur de l'écran LCD au début de la 1ère ligne
  lcd.print("Temp: ");              //on écrit le mot "Temp: " à l'emplacement du curseur
  lcd.print(DHT.temperature, 1);    //on écrit la température lue par le capteur, avec 1 chiffre derrière la virgule
  lcd.print((char)223);             //on ajoute le symbole ° après la valeur de la température
  lcd.print("C");                   //on ajoute la lettre C pour degré Celsius
  lcd.setCursor(0, 1);              //on déplace le curseur de l'écran LCD au début de la 2èmeligne
  lcd.print("Humidity: ");          //on écrit le mot "Hum. rel: " à l'emplacement du curseur
  lcd.print(DHT.humidity, 1);       //on écrit l'humidité relative lue par le capteur, avec 1 chiffre derrière la virgule
  lcd.print("%");                   //on ajoute le symbole "%" après la valeur de l'humidité
  delay(1000);                      //on attend une seconde avant de procéder à la lecture suivante
}