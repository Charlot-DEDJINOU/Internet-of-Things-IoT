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
const char* password = "CMUAfrica@151224##";
const String serverUrl = "http://192.168.17.77:8000/system/validate/entry/"; // URL du serveur

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
const byte ROWS = 4; // 4 lignes
const byte COLS = 4; // 4 colonnes

// Mappage des touches
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Correspondance des pins du PCF8574 (selon ta configuration)
byte rowPins[ROWS] = {7, 6, 5, 4};   // Utilisées pour les lignes
byte colPins[COLS] = {3, 2, 1, 0};     // Utilisées pour les colonnes

// Création de l'objet Keypad_I2C
Keypad_I2C myKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_ADDR);

// =======================
// LCD I2C (adresse 0x27)
// =======================
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Écran LCD 16x2

// =======================
// Servo pour la barrière
// =======================
Servo barrierServo;
const int servoPin = 23;         // Broche pour le servo (à choisir librement)
const int SERVO_OPEN_ANGLE = 90;   // Angle d'ouverture
const int SERVO_CLOSED_ANGLE = 0;  // Angle de fermeture

// =======================
// Variables Globales
// =======================
String userInput = "";         // Code saisi par l'utilisateur
const byte MAX_LENGTH = 6;     // Code à 6 chiffres
bool maskInput = true;         // true = affichage masqué, false = affichage clair

// Structure pour la réponse du serveur
struct ServerResponse {
  bool success;
  String message;
};

// =======================
// Fonction d'envoi du code au serveur
// =======================
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

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  
  // Connexion au WiFi
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
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
      return; // Quitte la boucle pour éviter une mise à jour supplémentaire
    }
    else if (key == '#') {
      // Touche '#' : validation du code
      if (userInput.length() == MAX_LENGTH) {
        lcd.clear();
        lcd.print("Envoi code...");
        // Envoi du code au serveur
        ServerResponse resp = sendCodeToServer(userInput);
        delay(500);
        // Affichage de la réponse sur le LCD
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

        if(resp.success){
           // Ouvre la barrière
          barrierServo.write(SERVO_OPEN_ANGLE);
          delay(5000);  // La barrière reste ouverte 3 secondes
          barrierServo.write(SERVO_CLOSED_ANGLE);
        }

        delay(3000);
        // Réinitialisation de la saisie
        userInput = "";
        maskInput = true;
        lcd.clear();
        lcd.print("Entrez code:");
      } else {
        // Code incomplet
        lcd.clear();
        lcd.print("6 chiffres SVP");
        delay(1500);
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
    }
    // On accepte uniquement les chiffres (0-9)
    else if (key >= '0' && key <= '9') {
      if (userInput.length() < MAX_LENGTH) {
        userInput += key;
      }
    }
    // Les autres touches (A, B, etc.) sont ignorées

    // Mise à jour de l'affichage du LCD pour la saisie
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
  
  delay(50);  // Anti-rebond
}