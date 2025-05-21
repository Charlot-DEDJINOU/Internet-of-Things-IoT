/*********
  Rui Santos
  Modified to include ultrasonic sensor test
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);

  Serial.println("\nI2C Scanner");
}

void loop() {
  // I2C Scanner
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning for I2C devices...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("I2C scanning done");
  }

  delay(5000);  // Wait 5 seconds before next loop
}
