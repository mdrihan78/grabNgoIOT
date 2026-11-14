
#include <IRremote.hpp>
#include <AFMotor.h>
#include <Servo.h>

// Motor setup
AF_DCMotor motor1(3); // M3
AF_DCMotor motor2(4); // M4

// Servo setup
Servo myservo1;
Servo myservo2;

// IR receiver
#define IR_RECEIVE_PIN A4

uint8_t receivedCode = 0;

// Servo position tracking
int servo1_pos = 70;
int servo2_pos = 90;

void setup() {

  Serial.begin(9600);

  // Attach servos
  myservo1.attach(9);
  myservo2.attach(10);

  // Initial servo positions
  myservo1.write(servo1_pos);
  myservo2.write(servo2_pos);

  delay(1000);

  // Motor speeds
  motor1.setSpeed(180);
  motor2.setSpeed(180);

  // Stop motors initially
  motor1.run(RELEASE);
  motor2.run(RELEASE);

  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Robot ready!");
}

void loop() {

  if (IrReceiver.decode()) {

    receivedCode = IrReceiver.decodedIRData.command;

    Serial.print("Received IR: 0x");
    Serial.println(receivedCode, HEX);

    // MOVEMENT CONTROL

    if (receivedCode == 0x8) {

      Serial.println("LEFT");

      motor2.run(BACKWARD);
      delay(300);
      motor2.run(RELEASE);

    }

    else if (receivedCode == 0xA) {

      Serial.println("RIGHT");

      motor2.run(FORWARD);
      delay(300);
      motor2.run(RELEASE);

    }

    else if (receivedCode == 0x5) {

      Serial.println("FORWARD");

      motor1.run(FORWARD);
      delay(300);
      motor1.run(RELEASE);

    }

    else if (receivedCode == 0xD) {

      Serial.println("BACKWARD");

      motor1.run(BACKWARD);
      delay(300);
      motor1.run(RELEASE);

    }

    // CLAW CONTROL

    else if (receivedCode == 0x1) {

      Serial.println("CLAW CLOSE");

      for (int pos = servo2_pos; pos >= 0; pos -= 2) {
        myservo2.write(pos);
        delay(15);
      }

      servo2_pos = 0;

    }

    else if (receivedCode == 0x2) {

      Serial.println("CLAW OPEN");

      for (int pos = servo2_pos; pos <= 180; pos += 2) {
        myservo2.write(pos);
        delay(15);
      }

      servo2_pos = 180;

    }

    // ARM CONTROL

    else if (receivedCode == 0x3) {

      Serial.println("ARM UP");

      int target = max(0, servo1_pos - 30);

      for (int pos = servo1_pos; pos >= target; pos -= 2) {
        myservo1.write(pos);
        delay(15);
      }

      servo1_pos = target;

    }

    else if (receivedCode == 0x4) {

      Serial.println("ARM DOWN");

      int target = min(140, servo1_pos + 30);

      for (int pos = servo1_pos; pos <= target; pos += 2) {
        myservo1.write(pos);
        delay(15);
      }

      servo1_pos = target;

    }

    else {

      Serial.println("UNKNOWN COMMAND");

    }

    IrReceiver.resume();
  }

  delay(50);
}
