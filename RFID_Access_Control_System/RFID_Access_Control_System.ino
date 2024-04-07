/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * -----------------------------------------------------------------------------------------
 * Project link: https://www.electronique-mixte.fr/microcontrolleurs/rfid-controle-dacces-par-badge-avec-arduino/
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Définition des broches utilisées
#define SS_PIN 10
#define RST_PIN 9

#define AccessGranted_PIN A0
#define AccessDenied_PIN A1
#define Alarm_PIN A2
#define Servo_PIN 8

#define Max_Acces 4
#define NUMBER_Card_ACCESS 1
#define Delay_Before_Closing 3000

// Définition des degrés d'ouverture et de fermeture de la porte
const int doorOpen = 110;   // Degre pour l'ouverture de la porte
const int doorClose = 5;  // Degre pour la fermeture de la porte

// Initialisation des variables et tableaux
byte Count_acces = 0;
byte CodeVerif = 0;
int Code_Access[NUMBER_Card_ACCESS][4] = {
  { 232, 207, 181, 13 }
};  // Init array that contain access Card

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
Servo servo;                                                       //on crée l’objet LCD et on définit les Pins utilisés

// Init array that will store new NUID
byte nuidPICC[4];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(Servo_PIN);

  pinMode(AccessGranted_PIN, OUTPUT);
  pinMode(AccessDenied_PIN, OUTPUT);
  pinMode(Alarm_PIN, OUTPUT);
}

void loop() {
  servo.write(doorClose);
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Vérifier la présence d'un nouveau badge
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Afffichage
  Serial.println(F("Un badge est détecté"));

  // Enregistrer l’ID du badge (4 octets)
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Vérification du code
  CodeVerif = GetAccesState();
  if (CodeVerif != 1) {
    Count_acces += 1;
    if (Count_acces == Max_Acces) {
      // Dépassement des tentatives (clignotement infinie)
      while (1) {
        Serial.println("Trop de tentatives");
        AccessDenied();
      }
    } else {
      // Affichage
      Serial.println("Code érroné");
      AccessDenied();
    }
  } else {
    // Affichage
    Serial.println("Ouverture de la porte");
    AccessGranted();
    Count_acces = 0;
  }

  // Affichage de l'identifiant
  Serial.println(" L'UID du tag est:");
  for (byte i = 0; i < 4; i++) {
    Serial.print(nuidPICC[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Re-Init RFID
  rfid.PICC_HaltA();       // Halt PICC
  rfid.PCD_StopCrypto1();  // Stop encryption on PCD
}

// Fonction pour vérifier l'état d'accès
byte GetAccesState() {
  byte StateAcces = 0;
  for (int i = 0; i < NUMBER_Card_ACCESS; i++) {
    if ((Code_Access[i][0] == nuidPICC[0]) && (Code_Access[i][1] == nuidPICC[1]) && (Code_Access[i][2] == nuidPICC[2]) && (Code_Access[i][3] == nuidPICC[3]))
      return StateAcces = 1;
  }

  return StateAcces = 0;
}

// Fonction pour refuser l'accès
void AccessDenied() {
  analogWrite(AccessDenied_PIN, 255);
  analogWrite(Alarm_PIN, 255);
  delay(200);
  digitalWrite(AccessDenied_PIN, 0);
  analogWrite(Alarm_PIN, 0);
  delay(200);
}

// Fonction pour autoriser l'accès
void AccessGranted() {
  analogWrite(AccessGranted_PIN, 255);
  for (int position = doorClose; position <= doorOpen; position += 5) {  // Ouverture de la porte
    servo.write(position);                                               // Déplacement du servomoteur
    delay(100);                                                          // Pause pour le mouvement
  }
  delay(Delay_Before_Closing);
  for (int position = doorOpen; position >= doorClose; position -= 5) {  // Fermeture de la porte
    servo.write(position);                                               // Déplacement du servomoteur
    delay(100);                                                          // Pause pour le mouvement
  }

  analogWrite(AccessGranted_PIN, 0);
}