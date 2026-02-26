#include <Arduino.h>

const int LDR_PIN = A0;
const int LED_PIN = 9;             // use pin labeled 9 or ~9
const long BAUD_RATE = 9600;
const int ACTIVATION_LEVEL = 55;   // higher = needs more cover before LED brightens

// For wiring: 5V -> LDR -> A0 -> 10k -> GND
// true  = darker -> brighter LED
// false = brighter -> brighter LED
const bool USE_INVERTED_RESPONSE = true;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);

  // Arduino Mega does not require waiting for Serial, but this gives monitor time to attach.
  delay(500);
  Serial.println("LDR diagnostic starting...");
  Serial.println("Cover/uncover the photoresistor and watch raw values change.");
  Serial.println("Startup LED PWM sweep (0->255->0)...");

  // Quick hardware check: LED should visibly fade up/down here.
  for (int i = 0; i <= 255; i++) {
    analogWrite(LED_PIN, i);
    delay(4);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(LED_PIN, i);
    delay(4);
  }
}

void loop() {
  static int minSeen = 1023;
  static int maxSeen = 0;

  int ldrValue = analogRead(LDR_PIN);  // 0..1023
  if (ldrValue < minSeen) minSeen = ldrValue;
  if (ldrValue > maxSeen) maxSeen = ldrValue;

  int span = max(1, maxSeen - minSeen);
  long scaled = (long)(ldrValue - minSeen) * 255L;  // avoid 16-bit overflow on AVR
  int normalized = (int)(scaled / span);
  normalized = constrain(normalized, 0, 255);

  int level = USE_INVERTED_RESPONSE ? (255 - normalized) : normalized;
  level = constrain(level - ACTIVATION_LEVEL, 0, 255 - ACTIVATION_LEVEL);
  level = (int)((long)level * 255L / (255 - ACTIVATION_LEVEL));  // stretch back to full range

  // Non-linear response: small changes near ambient produce less visible brightness.
  int brightness = (int)((long)level * (long)level / 255L);
  brightness = constrain(brightness, 0, 255);

  analogWrite(LED_PIN, brightness);

  Serial.print("raw=");
  Serial.print(ldrValue);
  Serial.print("  min=");
  Serial.print(minSeen);
  Serial.print("  max=");
  Serial.print(maxSeen);
  Serial.print("  pwm=");
  Serial.print(brightness);
  Serial.print("  level=");
  Serial.print(level);
  Serial.print("  mode=");
  Serial.println(USE_INVERTED_RESPONSE ? "inverted" : "normal");

  if (ldrValue < 5 || ldrValue > 1018) {
    Serial.println("Warning: reading near limit. Check divider wiring at A0.");
  }

  delay(150);
}
