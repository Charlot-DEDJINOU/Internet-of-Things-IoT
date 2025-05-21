#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>

// =======================
// Configuration I2C
// =======================
#define I2C_SDA 21
#define I2C_SCL 22

// =======================
// Adresse du PCF8574 pour le clavier I2C
// =======================
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

// Correspondance des pins du PCF8574 (adaptées à ta configuration)
byte rowPins[ROWS] = {7, 6, 5, 4}; // Lignes
byte colPins[COLS] = {3, 2, 1, 0}; // Colonnes

// Création de l'objet Keypad_I2C
Keypad_I2C myKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_ADDR);

// =======================
// LCD I2C (adresse 0x25)
// =======================
LiquidCrystal_I2C lcd(0x25, 16, 2);  // Écran LCD 16x2

// =======================
// Variables Globales pour le clavier/LCD
// =======================
String userInput = "";          // Chaîne saisie par l'utilisateur
const byte MAX_LENGTH = 16;     // Longueur maximale de saisie
bool maskInput = true;          // true = affichage masqué, false = affichage clair

// =======================
// Variables pour l'analyse I2C
// =======================
unsigned long lastScan = 0;
const unsigned long scanInterval = 5000;  // Intervalle de scan : 5000 ms

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  
  // Initialisation du bus I2C sur l'ESP32 (GPIO 21 et 22)
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialisation du clavier I2C
  myKeypad.begin(makeKeymap(keys));
  
  // Initialisation du LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Entrez code:");
  delay(1000);
  lcd.clear();
  
  // Message initial du scanner I2C
  Serial.println("\nI2C Scanner");
}

// =======================
// LOOP
// =======================
void loop() {
  // Gestion du clavier
  char key = myKeypad.getKey();
  
  if (key) {
    Serial.print("Touche: ");
    Serial.println(key);
    
    if (key == '*') {
      // Bascule entre affichage masqué et clair
      maskInput = !maskInput;
    }
    else if (key == 'D') {
      // Suppression du dernier caractère
      if (userInput.length() > 0) {
        userInput.remove(userInput.length() - 1, 1);
      }
    }
    else if (key == '#') {
      // Validation de la saisie
      lcd.clear();
      lcd.print("Valide: ");
      lcd.print(userInput);
      delay(2000);
      
      // Réinitialisation de la saisie et remise du masquage
      userInput = "";
      maskInput = true;
      lcd.clear();
      lcd.print("Entrez code:");
    }
    // On accepte uniquement les chiffres
    else if (key >= '0' && key <= '9') {
      if (userInput.length() < MAX_LENGTH) {
        userInput += key;
      }
    }
    
    // Mise à jour de l'affichage du LCD
    lcd.clear();
    if (maskInput) {
      String masked = "";
      for (int i = 0; i < userInput.length(); i++) {
        masked += "*";
      }
      lcd.print(masked);
    } 
    else {
      lcd.print(userInput);
    }
  }
  
  // Analyse périodique du bus I2C (toutes les 5 secondes)
  if (millis() - lastScan >= scanInterval) {
    Serial.println("Scanning...");
    int nDevices = 0;
    for (byte address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();
      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16) {
          Serial.print("0");
        }
        Serial.println(address, HEX);
        nDevices++;
      }
      else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        if (address < 16) {
          Serial.print("0");
        }
        Serial.println(address, HEX);
      }
    }
    if (nDevices == 0) {
      Serial.println("No I2C devices found\n");
    }
    else {
      Serial.println("done\n");
    }
    lastScan = millis();
  }
  
  delay(50);  // Petit délai anti-rebond
}
