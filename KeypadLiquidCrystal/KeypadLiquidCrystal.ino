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
// Adresse du PCF8574
// =======================
#define PCF8574_ADDR 0x24

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
byte rowPins[ROWS] = {7, 6, 5, 4}; // Utilisées pour les lignes
byte colPins[COLS] = {3, 2, 1, 0}; // Utilisées pour les colonnes

// Création de l'objet Keypad_I2C-- 
Keypad_I2C myKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, PCF8574_ADDR);

// =======================
// LCD I2C (adresse 0x27)
// =======================
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Écran LCD 16x2

// =======================
// Variables Globales
// =======================
String userInput = "";          // Chaîne saisie par l'utilisateur
const byte MAX_LENGTH = 16;     // Longueur maximale de saisie
bool maskInput = true;          // true = affichage masqué, false = affichage clair

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  
  // Initialisation du bus I2C sur l'ESP32 (GPIO 21 et 22)
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
    else if (key == '#') {
      // Touche '#' : validation de la saisie
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
    // On accepte uniquement les chiffres (0-9)
    else if (key >= '0' && key <= '9') {
      if (userInput.length() < MAX_LENGTH) {
        userInput += key;
      }
    }
    // Les autres touches (A, B, C) sont ignorées

    // Mise à jour de l'affichage du LCD
    lcd.clear();
    if (maskInput) {
      // Affiche un '*' pour chaque caractère saisi
      String masked = "";
      for (int i = 0; i < userInput.length(); i++) {
        masked += "*";
      }
      lcd.print(masked);
    } 
    else {
      // Affiche la saisie en clair
      lcd.print(userInput);
    }
  }
  
  delay(50);  // Petit délai anti-rebond
}
