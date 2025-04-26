#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// =======================
// Paramètres WiFi & serveur
// =======================
const char* ssid = "Infinix NOTE";
const char* password = "SoutenanceCharlot@23032025##";
const String serverUrl = "http://192.168.205.77:8000/system/validate/entry/"; // URL du serveur

// =======================
// Configuration I2C
// =======================
#define I2C_SDA 21
#define I2C_SCL 22

// =======================
// Adresse du PCF8574 (pour le Keypad)
#define PCF8574_ADDR 0x20

// =======================
// Clavier 4x4
// =======================
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4};   // pour les lignes
byte colPins[COLS] = {3, 2, 1, 0};     // pour les colonnes
Keypad_I2C myKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_ADDR);

// =======================
// LCD I2C (adresse 0x27)
// =======================
LiquidCrystal_I2C lcd(0x25, 16, 2);  // Écran LCD 16x2

// =======================
// Servo pour la barrière
// =======================
Servo barrierServo;
const int servoPin = 14;          // Broche de commande du servo
const int SERVO_OPEN_ANGLE = 90;  // Angle d'ouverture
const int SERVO_CLOSED_ANGLE = 0; // Angle de fermeture

// =======================
// Capteur Ultrason (ex: HC-SR04)
// =======================
const int trigPin = 2;           // Broche TRIG
const int echoPin = 4;           // Broche ECHO
const int CAR_DISTANCE_THRESHOLD = 5;  // Seuil en cm

// =======================
// Variables Globales pour la saisie
// =======================
String userInput = "";          // Code saisi par l'utilisateur
const byte MAX_LENGTH = 6;      // Code à 6 chiffres
bool maskInput = true;          // true = affichage masqué, false = affichage clair

// Structure pour la réponse du serveur
struct ServerResponse {
  bool success;
  String message;
};

// =======================
// Fonctions modulaires
// =======================

// Envoie le code au serveur et retourne la réponse
ServerResponse sendCodeToServer(String code) {
  ServerResponse response;
  response.success = false;
  response.message = "Erreur";
  
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverUrl + code;
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
  } else {
    response.message = "WiFi non connecte";
  }
  return response;
}

// Mesure la distance (en cm) via le capteur ultrason
long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30 ms
  long distance = duration * 0.034 / 2;
  return distance;
}

// Affiche la réponse du serveur sur le LCD
void displayResponse(const ServerResponse &resp) {
  lcd.clear();
  if (resp.success) {
    lcd.setCursor(0, 0);
    lcd.print("GRANTED");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("DENIED");
  }
  lcd.setCursor(0, 1);
  lcd.print(resp.message);
}

// Ouvre la barrière et la referme lorsque le capteur ne détecte plus de voiture
void processBarrier() {
  barrierServo.write(SERVO_OPEN_ANGLE);
  Serial.println("Barriere ouverte");
  
  // Vérifie en continu si une voiture est présente (distance <= seuil)
  while (true) {
    long distance = measureDistance();
    Serial.print("Distance: ");
    Serial.println(distance);
    if (distance > CAR_DISTANCE_THRESHOLD) { // Plus de véhicule proche
      break;
    }
    delay(100);
  }
  
  delay(2000);
  barrierServo.write(SERVO_CLOSED_ANGLE);
  Serial.println("Barriere fermee");
}

// Réinitialise la saisie et remet l'écran à l'état initial
void resetInput() {
  userInput = "";
  maskInput = true;
  lcd.clear();
  lcd.print("Entrez code:");
}

// Met à jour l'affichage du LCD avec la saisie (masquée ou claire)
void updateLCD() {
  lcd.clear();
  if (maskInput) {
    String masked = "";
    for (int i = 0; i < userInput.length(); i++) {
      masked += "*";
    }
    lcd.print(masked);
  } else {
    lcd.print(userInput);
  }
}

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  
  // Connexion au WiFi
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecte !");
  
  // Initialisation du bus I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialisation du Keypad_I2C
  myKeypad.begin(makeKeymap(keys));
  
  // Initialisation du LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Entrez code:");
  delay(1000);
  lcd.clear();
  
  // Initialisation du servo (barrière)
  barrierServo.attach(servoPin);
  barrierServo.write(SERVO_CLOSED_ANGLE);  // Barrière fermée au démarrage
  
  // Configuration du capteur ultrason
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// =======================
// LOOP
// =======================
void loop() {
  char key = myKeypad.getKey();
  
  if (key) {
    Serial.print("Touche: ");
    Serial.println(key);
    
    if (key == '*') {
      // Touche '*' : bascule entre affichage masqué et clair
      maskInput = !maskInput;
    }
    else if (key == 'D') {
      // Touche 'D' : suppression du dernier caractère
      if (userInput.length() > 0) {
        userInput.remove(userInput.length() - 1, 1);
      }
    }
    else if (key == 'C') {
      // Touche 'C' : efface complètement la saisie et l'écran
      userInput = "";
      lcd.clear();
      lcd.print("Entrez code:");
      return;
    }
    else if (key == '#') {
      // Touche '#' : validation du code
      if (userInput.length() == MAX_LENGTH) {
        lcd.clear();
        lcd.print("Envoi code...");
        ServerResponse resp = sendCodeToServer(userInput);
        delay(500);
        
        // Affiche la réponse du serveur avant de vérifier le capteur
        displayResponse(resp);
        delay(3000);  // Attendre que l'utilisateur lise le message
        
        if (resp.success) {
          processBarrier();  // Ouvre la barrière et attend le départ du véhicule
        }
        
        delay(3000);
        resetInput();
      } else {
        // Code incomplet
        lcd.clear();
        lcd.print("6 chiffres SVP");
        delay(1500);
        updateLCD();
      }
    }
    // On accepte uniquement les chiffres (0-9)
    else if (key >= '0' && key <= '9') {
      if (userInput.length() < MAX_LENGTH) {
        userInput += key;
      }
    }
    // Les autres touches (A, B, etc.) sont ignorées

    updateLCD();
  }
  
  delay(50);  // Anti-rebond
}