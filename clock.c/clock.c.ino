#include <Arduino.h>

enum Mode {
    MASTER,
    SLAVE
};

byte mode = MASTER;
byte ledPin = 3;
byte debugPin = 4;

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(debugPin, OUTPUT);
}

// void KeepOn(int totalMili, onTime)

void loop() {
  digitalWrite(ledPin, HIGH);
  digitalWrite(debugPin, LOW);
  delay(200);
  digitalWrite(ledPin, LOW);
  digitalWrite(debugPin, HIGH);
  delay(1000);
}
