#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>

// Adresse I2C du PCF8574 (A0=A1=A2=GND => 0x20)
#define I2C_ADDR 0x20

// Configuration du clavier 4x4
const byte ROWS = 4; // 4 lignes
const byte COLS = 4; // 4 colonnes

// Correspondance des touches
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Sur PCF8574, on connecte L1->P0, L2->P1, L3->P2, L4->P3
// et C1->P4, C2->P5, C3->P6, C4->P7
byte rowPins[ROWS] = {0, 1, 2, 3};  // P0-P3 du PCF8574
byte colPins[COLS] = {4, 5, 6, 7};  // P4-P7 du PCF8574

// Création de l'objet Keypad_I2C
Keypad_I2C myKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2C_ADDR);

void setup() {
  Serial.begin(115200);

  // Initialisation du bus I2C sur l'ESP32 (SDA=GPIO21, SCL=GPIO22)
  Wire.begin(21, 22);

  // Initialisation du Keypad_I2C
  myKeypad.begin(makeKeymap(keys));

  Serial.println("Clavier 4x4 via PCF8574 (0x20) prêt !");
}

void loop() {
  // Lecture de la touche appuyée
  char key = myKeypad.getKey();
  if (key) {
    Serial.print("Touche appuyée : ");
    Serial.println(key);
  }

  delay(50); // petit délai pour éviter les rebonds
}