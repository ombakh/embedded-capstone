#include <Arduino.h>

const int LDR_PIN = A0;   // Voltage divider midpoint
const int LED_PIN = 9;    // PWM-capable pin on Mega 2560

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);               // 0..1023
  int brightness = map(ldrValue, 0, 1023, 255, 0); // invert response
  brightness = constrain(brightness, 0, 255);

  analogWrite(LED_PIN, brightness);
  delay(10);
}
