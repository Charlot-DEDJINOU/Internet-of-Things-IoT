#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
// #include <LiquidCrystal.h>  //on inclut la bibliothèque pour l’écran LCD
// #include "pitches.h" 

#define SS_PIN 10
#define RST_PIN 9
#define NUMBER_Card_ACCESS 1

MFRC522 rfid(SS_PIN, RST_PIN);                                                                  // Instance of the class
Servo monServo;                                                                                 // Déclaration de l'objet pour le servomoteur
// LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                                                          //on crée l’objet LCD et on définit les Pins utilisés
const int servoPin = 8;                                                                        // Broche utilisée pour le signal du servomoteur
// const int accessPin = 7;                                                                        // Broche pour indiquer l'accès autorisé
// const int deniedPin = 6;                                                                        // Broche pour indiquer l'accès refusé
const int doorOpen = 180;                                                                       // Degre pour l'ouverture de la porte
const int doorClose = 0;                                                                      // Degre pour la fermerture de la porte
// const int buzPin = 8;                                                                           //buzzer pin
byte nuidPICC[4];                                                                               // Init array that will store new NUID
const int accessCards[NUMBER_Card_ACCESS][4] = { { 232, 207, 181, 13 }};  // Init array that contain access Card
int delayBeforeClosing = 1500;                                                                  //delay pour fermer la porte une fois que c'est ouverte


void setup() {
  Serial.begin(9600);
  SPI.begin();                 // Init SPI bus
  rfid.PCD_Init();             // Init MFRC522
  // lcd.begin(16, 2);            //on initialise la communication avec l’écran LCD
  monServo.attach(servoPin);   // Initialisation du servomoteur
  // pinMode(accessPin, OUTPUT);  // Configuration de la broche d'accès comme sortie
  // pinMode(deniedPin, OUTPUT);  // Configuration de la broche de refus comme sortie

  Serial.println(F("Scan RFID NUID..."));
}

void loop() {
  // display(2);
  if(readRFID())
    if (checkAccess()) {
      // display(1);
      // digitalWrite(accessPin, HIGH);
      Serial.println("momo");
      door(1);
      delay(delayBeforeClosing);
      door(0);
      // digitalWrite(accessPin, LOW);
    } else {
      // display(0);
      // digitalWrite(deniedPin, HIGH);
      // emitSound();
      // digitalWrite(deniedPin, LOW);
    }
  else
    return;

  // resetMemoryCard();
}

bool readRFID() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return false;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return false;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return false;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("New card"));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

  } else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  return true;
}

bool checkAccess() {
  for (int i = 0; i < NUMBER_Card_ACCESS; i++) {
    if (accessCards[i][0] == nuidPICC[0] && accessCards[i][1] == nuidPICC[1] && accessCards[i][2] == nuidPICC[2] && accessCards[i][3] == nuidPICC[3])
      return true;
  }

  return false;
}

// void display(int status) {
//   lcd.setCursor(0, 0);  //on place le curseur de l'écran LCD au début de la 1ère ligne
//   switch (status) {
//     case 0:
//       lcd.print("ACCESS DENIED");
//       break;
//     case 1:
//       lcd.print("ACCESS GRANTED");
//       break;
//     default:
//       lcd.print("Scan RFID NUID");
//       break;
//   }
// }

void door(int status) {
  if (status == 0) {
    for (int position = doorOpen; position >= doorClose; position -= 5) {  // Fermeture de la porte
      monServo.write(position);                                            // Déplacement du servomoteur
      delay(100);                                                          // Pause pour le mouvement
    }
  } else {
    for (int position = doorClose; position <= doorOpen; position += 5) {  // Ouverture de la porte
      monServo.write(position);                                            // Déplacement du servomoteur
      delay(100);                                                          // Pause pour le mouvement
    }
  }
}

// void resetMemoryCard() {
//   nuidPICC[0] = 0;
//   nuidPICC[1] = 0;
//   nuidPICC[2] = 0;
//   nuidPICC[3] = 0;
// }

// void emitSound() { 
//   int melodyOn[] = {NOTE_C5, NOTE_C6, NOTE_D5, NOTE_A6};
//   int durationOn = 200;
//   for (int note = 0; note < 4; note++) {
//     tone(buzPin, melodyOn[note], durationOn);
//     delay(200);
//   }
// }