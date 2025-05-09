#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// ===================================================
// Paramètres WiFi et URL pour chaque porte
// ===================================================
const char* ssid = "Infinix NOTE";
const char* password = "SoutenanceCharlot@23032025##";

const String serverUrlEntrance = "http://192.168.225.77:8000/system/validate/entry/";
const String serverUrlExit    = "http://192.168.225.77:8000/system/validate/out/";

// Mutex pour protéger l'accès au WiFi/HTTP
SemaphoreHandle_t wifiMutex;

// ===================================================
// Constantes communes
// ===================================================
const long CAR_DISTANCE_THRESHOLD = 6;  // Seuil en cm

// ---------------------------------------------------
// Configuration commune du Keypad I2C (même mapping pour les 2 portes)
// ---------------------------------------------------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, 1, 0};

// ===================================================
// Structure pour définir le matériel d'une porte
// ===================================================
struct DoorHardware {
  String doorName;               // "Entree" ou "Sortie"
  Keypad_I2C* keypad;            // Clavier I2C (PCF8574)
  LiquidCrystal_I2C* lcd;        // Écran LCD I2C
  Servo* servo;                  // Servo pour la barrière
  int trigPin;                   // Broche TRIG du capteur ultrason
  int echoPin;                   // Broche ECHO du capteur ultrason
  int servoPin;                  // Broche de commande du servo
  int servo_open_angle;
  int servo_close_angle;
  String serverUrl;              // URL de validation pour cette porte
};

// ===================================================
// Structure pour la réponse serveur
// ===================================================
struct ServerResponse {
  bool success;
  String message;
};

// ===================================================
// Fonctions Utilitaires
// ===================================================

// Envoie le code au serveur (protégé par un mutex) et retourne la réponse
ServerResponse sendCodeToServer(String code, const String &baseUrl) {
  ServerResponse response;
  response.success = false;
  response.message = "Erreur";
  
  if(WiFi.status() == WL_CONNECTED) {
    xSemaphoreTake(wifiMutex, portMAX_DELAY);
    
    HTTPClient http;
    String url = baseUrl + code;
    http.begin(url);
    int httpCode = http.GET();
    if(httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(200);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        response.success = doc["success"];
        response.message = doc["message"].as<String>();
      } else {
        response.message = "Erreur JSON";
      }
    } else {
      response.message = "Erreur HTTP";
    }
    http.end();
    
    xSemaphoreGive(wifiMutex);
  } else {
    response.message = "WiFi non connecte";
  }
  
  return response;
}

// Mesure la distance via le capteur ultrason
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  vTaskDelay(pdMS_TO_TICKS(2));
  digitalWrite(trigPin, HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

// Retourne true si un véhicule est détecté (distance < seuil)
bool isCarPresent(int trigPin, int echoPin) {
  long distance = measureDistance(trigPin, echoPin);
  return (distance > 0 && distance < CAR_DISTANCE_THRESHOLD);
}

// Demande la saisie d'un code à 6 chiffres via le clavier
// - '*' : bascule le masquage (affichage en '*' ou clair)
// - 'D' : supprime le dernier caractère
// - '#' : valide la saisie ; si le code n'a pas 6 chiffres, affiche "6 chiffres SVP"
//   puis réaffiche le code déjà saisi.
String getUserCode(DoorHardware *door) {
  String code = "";
  bool mask = true;  // Affichage masqué par défaut
  door->lcd->clear();
  door->lcd->print("Entrez code:");
  door->lcd->setCursor(0, 1);
  
  while (true) {
    char key = door->keypad->getKey();
    if (key != NO_KEY) {
      if (key == '*') {
        mask = !mask; // Basculer l'affichage masqué/clair
      }
      else if (key == 'D') {
        if (code.length() > 0) {
          code.remove(code.length() - 1);
        }
      }
      else if (key == 'C') {  // Optionnel : effacer toute la saisie
        code = "";
      }
      else if (key == '#') {
        if (code.length() == 6) {
          return code;
        } else {
          door->lcd->clear();
          door->lcd->print("6 chiffres SVP");
          vTaskDelay(pdMS_TO_TICKS(2000));
          door->lcd->clear();
          door->lcd->print("Entrez code");
          door->lcd->setCursor(0, 1);
        }
      }
      else if (key >= '0' && key <= '9') {
        if (code.length() < 6) {
          code += key;
        }
      }
      
      // Mise à jour de l'affichage sur la deuxième ligne
      door->lcd->setCursor(0, 1);
      // Effacer la ligne en écrivant des espaces (selon la largeur de l'écran, ici 16 colonnes)
      door->lcd->print("                ");
      door->lcd->setCursor(0, 1);
      if (mask) {
        for (int i = 0; i < code.length(); i++) {
          door->lcd->print("*");
        }
      } else {
        door->lcd->print(code);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Ouvre la barrière (servo) et affiche l'action
void openBarrier(DoorHardware *door) {
  door->servo->write(door->servo_open_angle);
  Serial.println(door->doorName + ": Barriere ouverte");
  vTaskDelay(pdMS_TO_TICKS(500));
}

// Ferme la barrière (servo)
void closeBarrier(DoorHardware *door) {
  door->servo->write(door->servo_close_angle);
  Serial.println(door->doorName + ": Barriere fermee");
  vTaskDelay(pdMS_TO_TICKS(500));
}

// Ouvre la barrière, attend que le véhicule se retire, puis referme la barrière
void processBarrierOperation(DoorHardware *door) {
  openBarrier(door);
  unsigned long openTime = millis();
  bool carDetected = false;
  
  // Attente maximale de 30 sec pour détecter un véhicule
  while (millis() - openTime < 30000) {
    if (isCarPresent(door->trigPin, door->echoPin)) {
      carDetected = true;
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  
  // Une fois détecté, attendre que le véhicule quitte la zone
  if (carDetected) {
    while (isCarPresent(door->trigPin, door->echoPin)) {
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
  
  closeBarrier(door);
}

// Processus complet d'une porte : saisie du code, validation et gestion de la barrière
void processDoor(DoorHardware *door) {
  door->lcd->clear();
  door->lcd->print(door->doorName);
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  String code = getUserCode(door);
  Serial.println(door->doorName + " - Code saisi: " + code);
  
  door->lcd->clear();
  door->lcd->print("Envoi code...");
  
  ServerResponse serverResponse = sendCodeToServer(code, door->serverUrl);
  
  door->lcd->clear();
  door->lcd->setCursor(0,0);

  if(serverResponse.success){
    door->lcd->print("GRANTED !!!");
  }else{
    door->lcd->print("DENIED !!!");
  }

  door->lcd->setCursor(0,1);
  door->lcd->print(serverResponse.message);
  vTaskDelay(pdMS_TO_TICKS(1500));
  
  if (serverResponse.success) {
    processBarrierOperation(door);
  }
}

// Tâche FreeRTOS pour une porte
void doorTask(void *pvParameter) {
  DoorHardware *door = (DoorHardware*) pvParameter;
  for (;;) {
    processDoor(door);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

////////////////////////////////////
// Instanciation du matériel par porte
////////////////////////////////////

// ----------------- Porte d'entrée -----------------
// Clavier sur PCF8574 à l'adresse 0x20
#define PCF8574_ENTRANCE_ADDR 0x20
Keypad_I2C entranceKeypad = Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_ENTRANCE_ADDR);
// LCD I2C à l'adresse 0x25
LiquidCrystal_I2C entranceLCD(0x25, 16, 2);
// Servo sur PIN 14
Servo entranceServo;

// ----------------- Porte de sortie -----------------
// Clavier sur PCF8574 à l'adresse 0x24
#define PCF8574_EXIT_ADDR 0x24
Keypad_I2C exitKeypad = Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_EXIT_ADDR);
// LCD I2C à l'adresse 0x27
LiquidCrystal_I2C exitLCD(0x27, 16, 2);
// Servo sur PIN 23
Servo exitServo;

// Création des instances de DoorHardware
DoorHardware doorEntrance = {
  "Entree",           // Nom de la porte
  &entranceKeypad,    // Clavier I2C (PCF8574 0x20)
  &entranceLCD,       // LCD I2C (0x27)
  &entranceServo,     // Servo (PIN 23)
  12,                  // TRIG du capteur ultrason (entrée)
  13,                  // ECHO du capteur ultrason (entrée)
  14,                 // PIN servo (entrée)
  90,                  // Angle Ouverture
  0,                 // Angle Fermeture
  serverUrlEntrance   // URL de validation pour l'entrée
};

DoorHardware doorExit = {
  "Sortie",           // Nom de la porte
  &exitKeypad,        // Clavier I2C (PCF8574 0x24)
  &exitLCD,           // LCD I2C (0x25)
  &exitServo,         // Servo (PIN 14)
  2,                 // TRIG du capteur ultrason (sortie)
  4,                 // ECHO du capteur ultrason (sortie)
  23,                 // PIN servo (sortie)
  90,                 // Angle Ouverture
  0,                  // Angle Fermeture
  serverUrlExit       // URL de validation pour la sortie
};

////////////////////////////////////
// Setup et Boucle principale
////////////////////////////////////
void setup() {
  Serial.begin(115200);
  
  // Initialisation du bus I2C (pour LCD et claviers)
  Wire.begin(21, 22);
  
  // Initialisation des claviers
  entranceKeypad.begin(makeKeymap(keys));
  exitKeypad.begin(makeKeymap(keys));
  
  // Initialisation des LCD
  entranceLCD.init();
  entranceLCD.backlight();
  entranceLCD.clear();
  entranceLCD.print("Entrez code:");
  
  exitLCD.init();
  exitLCD.backlight();
  exitLCD.clear();
  exitLCD.print("Entrez code:");
  
  // Attacher les servos
  entranceServo.attach(14);
  entranceServo.write(0);
  
  exitServo.attach(23);
  exitServo.write(0);
  
  // Configuration des capteurs ultrason
  pinMode(2, OUTPUT);
  pinMode(4, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
  
  // Connexion au WiFi
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.print(".");
  }
  Serial.println("\nWiFi connecte !");
  
  // Création du mutex pour l'accès au WiFi/HTTP
  wifiMutex = xSemaphoreCreateMutex();
  
  // Création des tâches pour chaque porte avec xTaskCreatePinnedToCore
  xTaskCreatePinnedToCore(
    doorTask,             // Fonction de la tâche
    "DoorEntranceTask",   // Nom de la tâche
    4096,                 // Taille de la pile
    &doorEntrance,        // Paramètre de la tâche
    1,                    // Priorité
    NULL,                 // Handle de la tâche
    0                     // Core 0
  );
  
  xTaskCreatePinnedToCore(
    doorTask,             // Fonction de la tâche
    "DoorExitTask",       // Nom de la tâche
    4096,                 // Taille de la pile
    &doorExit,            // Paramètre de la tâche
    1,                    // Priorité
    NULL,                 // Handle de la tâche
    1                     // Core 1
  );
}

void loop() {
  // La boucle loop reste vide : toutes les opérations sont gérées par les tâches FreeRTOS.
  vTaskDelay(pdMS_TO_TICKS(1000));
}