# RFID Access Control System

Ce projet utilise un lecteur RFID (MFRC522) avec Arduino pour créer un système de contrôle d'accès basé sur des cartes RFID. Le système permet d'ouvrir une porte en fonction des cartes d'accès autorisées. Et au delà de 03 tentatives d'echec une alarme se déclanche.

## Fonctionnalités

- Lecture des cartes RFID pour obtenir l'identifiant unique (UID).
- Vérification de l'UID par rapport à une liste de codes d'accès autorisés.
- Ouverture de la porte via un servomoteur en cas d'accès autorisé.
- Signaux visuels et sonores pour indiquer l'accès accordé ou refusé.

## Matériel Requis

- Arduino Uno/101/Mega/Nano v3/Leonardo/Micro/Pro Micro (ou équivalent).
- Lecteur RFID MFRC522.
- Servomoteur.
- LED et buzzer pour les signaux visuels et sonores.
- Câblage approprié.

## Configuration Matérielle

Connectez les composants selon le schéma suivant :

```
MFRC522      Arduino       
Reader/PCD   Uno/101/Mega/Nano v3/Leonardo/Micro/Pro Micro   
Signal       Pin          
--------------------------------------------
RST/Reset    RST          9            
SPI SS       SDA(SS)      10          
SPI MOSI     MOSI         11 / ICSP-4     
SPI MISO     MISO         12 / ICSP-1     
SPI SCK      SCK          13 / ICSP-3   
--------------------------------------------
```

Référez-vous au code source pour plus de détails sur le câblage des LED, du buzzer et du servomoteur.

## Installation

1. Clonez le dépôt GitHub sur votre machine locale.
2. Assurez-vous d'avoir les bibliothèques nécessaires installées (SPI, MFRC522, Servo).
3. Compilez et téléversez le code sur votre carte Arduino.

## Utilisation

1. Alimentez le système et assurez-vous que les composants sont correctement connectés.
2. Approchez une carte RFID du lecteur.
3. Suivez les signaux visuels et sonores pour connaître l'état de l'accès (accordé ou refusé).
4. Le système ouvrira la porte si l'accès est autorisé.