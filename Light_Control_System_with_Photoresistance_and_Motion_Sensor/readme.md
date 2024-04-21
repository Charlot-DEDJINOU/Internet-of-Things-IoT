# Light Control System with Photoresistance and Motion Sensor

Ce projet implémente un système de contrôle de l'éclairage qui utilise une photorésistance pour mesurer la luminosité environnante et un capteur de mouvement PIR pour détecter les mouvements à proximité. Lorsque la luminosité chute en dessous d'un certain seuil et qu'un mouvement a été détecté récemment, le système déclenche un effet de clignotement sur des LED connectées. Après un certain nombre de minutes sans détection de mouvement, les LED s'éteignent automatiquement.

## Composants Utilisés
- Photorésistance sur la broche A0 (LightPin)
- Capteur de mouvement PIR sur la broche A2 (PIR)
- LEDs connectées aux broches de 3 à 12 pour l'effet de clignotement

## Fonctionnement du Code
1. **Initialisation** :
   - Configuration des broches (LightPin, PIR, LEDs) dans la fonction `setup()`.
   - Démarrage de la communication série à 9600 bauds pour le suivi des messages.

2. **Boucle Principale** (`loop()`) :
   - Lecture de la valeur de luminosité à partir de la photorésistance (`lightReading`).
   - Lecture de la détection de mouvement à partir du capteur PIR (`pirVal`).
   - Si un mouvement est détecté, la minuterie (`minute`) est réinitialisée à zéro.
   - Sinon, la minuterie est incrémentée pour compter les secondes d'inactivité.

3. **Activation des LEDs** :
   - Si la luminosité est faible (lightReading < 50) et qu'il y a eu une inactivité récente (minute < 300, équivalent à 5 minutes), un effet de clignotement est activé sur les LEDs de la broche 3 à la broche 12.
   - L'effet de clignotement consiste à allumer et éteindre les LEDs en séquence dans les deux sens.

## Personnalisation
- **Seuil de Luminosité** : Modifier la valeur 50 dans la condition `lightReading < 50` pour ajuster le seuil de luminosité à partir duquel le système réagit.
- **Durée d'Inactivité** : Modifier le nombre de minutes (300) dans la condition `minute < 60 * 5` pour ajuster la durée d'inactivité avant que l'effet de clignotement ne soit activé.

**Remarque :** N'oubliez pas de connecter correctement tous les composants et de prendre les précautions nécessaires lors de la manipulation des circuits électroniques.
