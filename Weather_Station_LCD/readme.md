# Weather Station LCD

Le projet "Weather Station LCD" est une station météo simple et efficace qui utilise un capteur DHT11 pour mesurer la température et l'humidité de l'environnement, un écran LCD pour afficher ces données, et un potentiomètre pour régler la luminosité de l'écran.

## Matériel Requis

- Capteur DHT11
- Écran LCD 16x2
- Potentiomètre
- Résistances
- Fils de connexion
- Arduino (ou carte compatible)

## Description du Projet

Ce projet utilise le capteur DHT11 pour mesurer la température et l'humidité, puis affiche ces données sur un écran LCD. Le potentiomètre permet de régler la luminosité de l'écran pour une lecture confortable dans différentes conditions d'éclairage.

## Comment Utiliser

1. Téléchargez le code sur votre carte Arduino à l'aide de l'IDE Arduino.
2. Connectez le capteur DHT11 au Pin 7 de votre Arduino.
3. Connectez l'écran LCD selon les étapes suivantes.
    - Les deux premiers pins tout à gauche servent à l'alimentation de l'écran. Pôle négatif ou GND pour 
    le premier et pôle positif (5V) pour le 2ème. 
    - Le 3ème pin est connecté à un potentiomètre et sert à régler le contraste de l'écran LCD. 
    - Le 4ème, noté RS pour Register Select, est connecté au pin 12 de l'Arduino. Il sert à sélectionner la zone mémoire de l'écran LCD dans laquelle nous allons écrire. 
    - Le 5ème doit être connecté au ground (GND). 
    - Le 6ème, noté E pour Enable, est connecté au pin 11 de l'Arduino. Il permet de lancer ou non l'écriture  dans les zones mémoires. 
    - Les quatre suivants (7, 8, 9 et 10ème) sont reliés au ground (GND). 
    - Les quatre qui suivent (11 à 14ème), notés 5, 4, 3, 2 sur le schéma ci-dessus, car ils se connectent sur les Pins 5, 4, 3, 2 de l'Arduino.  Ils servent pour la transmission des données à afficher. 
    - Les deux pins tout à droite (15 et 16ème) servent pour alimenter la LED du rétroéclairage de l’écran 
    LCD. 
    - Attention l’avant-dernier (pôle positif 5V) doit impérativement être protégé par une résistance 
    d’environ 220 Ω. Le dernier est relié au pôle négatif (GND). 
4. Assurez-vous que le potentiomètre est correctement câblé pour régler la luminosité de l'écran.
5. Téléversez le code sur votre Arduino et observez les mesures de température et d'humidité sur l'écran LCD.

**Remarque :** N'oubliez pas de connecter correctement tous les composants et de prendre les précautions nécessaires lors de la manipulation des circuits électroniques.