# LED Light Chaser

Le projet "LED Light Chaser" est une démonstration de la façon dont la technologie de l'Internet des Objets (IoT) peut être utilisée pour créer un système réactif à la lumière ambiante en utilisant une photoresistance, des LEDs 8, et une carte Arduino.

## Description du Projet

Ce projet utilise une photoresistance pour détecter la luminosité de l'environnement. Lorsque l'obscurité est détectée (lorsque la valeur de luminosité est inférieure à 50), les LEDs 8 s'allument successivement dans un motif de chenillard à partir de la broche 6 jusqu'à la broche 13, puis s'éteignent progressivement dans le même ordre. Cela crée un effet visuel dynamique qui réagit en temps réel aux variations de lumière.

## Matériel Requis

- Carte Arduino
- Photoresistance
- 8 LEDs
- 8 Résistances (220 ohm)
- Fils de connexion

## Configuration

1. Connectez la photoresistance à la broche analogique A0 de la carte Arduino.
2. Connectez les LEDs 8 aux broches numériques de 6 à 13 de la carte Arduino.
3. Assurez-vous d'avoir les résistances appropriées pour protéger les composants.

## Comment Utiliser

1. Téléchargez le code sur votre carte Arduino à l'aide de l'IDE Arduino.
2. Placez la photoresistance dans un environnement où la luminosité varie pour observer le fonctionnement du chenillard lumineux.
3. Consultez la console série de l'IDE Arduino pour suivre les valeurs de luminosité détectées par la photoresistance.

**Remarque :** N'oubliez pas de connecter correctement tous les composants et de prendre les précautions nécessaires lors de la manipulation des circuits électroniques.