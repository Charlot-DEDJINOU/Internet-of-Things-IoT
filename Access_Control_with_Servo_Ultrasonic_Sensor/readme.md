# Access Control with Servo and Ultrasonic Sensor

## Description

Ce projet implémente un Système de Contrôle d'Accès utilisant un capteur ultrasonique (HC-SR04) pour détecter la proximité d'un objet, un servomoteur pour simuler un verrou de porte, et des LEDs pour indiquer l'accès autorisé ou refusé. Le système est conçu pour ouvrir la porte (servomoteur se déplaçant de 100 à 180 degrés) lorsqu'un objet se trouve à moins de 15 cm du capteur et fermer la porte (servomoteur se déplaçant de 180 à 100 degrés) sinon.

## Matériel Requis

- Carte Arduino
- Capteur ultrasonique HC-SR04
- Servomoteur
- LEDs
- Résistances
- Fils de connexion

## Installation

1. Connectez le capteur ultrasonique HC-SR04 à l'Arduino :
   - Broche Trig au digital pin 11
   - Broche Echo au digital pin 12
2. Connectez le servomoteur à l'Arduino :
   - Broche Signal au digital pin 13
3. Connectez les LEDs :
   - LED d'accès autorisé au digital pin 10
   - LED d'accès refusé au digital pin 9
4. Téléversez le code fourni sur votre carte Arduino à l'aide de l'IDE Arduino.

## Utilisation

1. Mettez sous tension le système et ajustez la position du capteur ultrasonique et du servomoteur selon vos besoins.
2. Le système détectera la proximité d'un objet à l'aide du capteur ultrasonique.
3. Si un objet se trouve à moins de 15 cm, le servomoteur ouvrira la porte et la LED d'accès autorisé s'allumera.
4. Si aucun objet n'est détecté ou si l'objet est au-delà de 15 cm, le servomoteur fermera la porte et la LED d'accès refusé s'allumera.

**Remarque :** N'oubliez pas de connecter correctement tous les composants et de prendre les précautions nécessaires lors de la manipulation des circuits électroniques.