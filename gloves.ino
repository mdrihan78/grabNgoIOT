
#include <Wire.h>
#include <MPU6050.h>
#include <IRremote.hpp>

// MPU6050 setup
MPU6050 mpu;

// Button and IR setup
const int buttonPin = 11;
const int irPin = 3;

bool grab = false;

// Button debouncing
int lastButtonState = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize IR transmitter
  IrSender.begin(irPin);

  // Initialize MPU6050
  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("MPU6050 initialized!");
  Serial.println("Current mode: MOVEMENT");
}

void loop() {

  // Read button
  int buttonReading = digitalRead(buttonPin);

  if (buttonReading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {

    if (buttonReading != stableButtonState) {
      stableButtonState = buttonReading;

      if (stableButtonState == LOW) {
        grab = !grab;

        Serial.print("Mode changed to: ");
        Serial.println(grab ? "CLAW CONTROL" : "MOVEMENT CONTROL");
      }
    }
  }

  lastButtonState = buttonReading;

  // Read MPU6050 acceleration
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float AccX = ax / 16384.0;
  float AccY = ay / 16384.0;

  const float threshold = 0.3;

  uint8_t codeToSend = 0;
  String direction = "No tilt";

  // Gesture detection
  if (grab) {

    // CLAW CONTROL MODE

    if (AccX > threshold) {
      direction = "Claw Close";
      codeToSend = 0x1;
    }
    else if (AccX < -threshold) {
      direction = "Claw Open";
      codeToSend = 0x2;
    }
    else if (AccY > threshold) {
      direction = "Arm Higher";
      codeToSend = 0x3;
    }
    else if (AccY < -threshold) {
      direction = "Arm Lower";
      codeToSend = 0x4;
    }

  }
  else {

    // MOVEMENT CONTROL MODE

    if (AccX > threshold) {
      direction = "Tilt Left";
      codeToSend = 0x8;
    }
    else if (AccX < -threshold) {
      direction = "Tilt Right";
      codeToSend = 0xA;
    }
    else if (AccY > threshold) {
      direction = "Tilt Backward";
      codeToSend = 0xD;
    }
    else if (AccY < -threshold) {
      direction = "Tilt Forward";
      codeToSend = 0x5;
    }
  }

  // Send IR command
  if (codeToSend != 0) {

    Serial.print("Mode: ");
    Serial.print(grab ? "CLAW" : "MOVEMENT");

    Serial.print(" | Direction: ");
    Serial.println(direction);

    Serial.print("Sending IR: 0x");
    Serial.println(codeToSend, HEX);

    IrSender.sendNEC(0x00, codeToSend, 0);

    delay(200);
  }

  delay(50);
}
