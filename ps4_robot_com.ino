#include <Ultrasonic.h>
#include <PS4Controller.h>
#include <ESP32Servo.h>

#define TRIG_PIN 14
#define ECHO_PIN 12
#define LED_PIN 15 // Define the LED pin

// Define servo objects
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

// Define GPIO pins for the servos
const int servoPin1 = 27;
const int servoPin2 = 26;
const int servoPin3 = 25;
const int servoPin4 = 32;
const int servoPin5 = 33;

// Define GPIO pins for the H-Bridge motor driver
const int motor1Pin1 = 18;
const int motor1Pin2 = 5;
const int motor1EN = 19; // PWM-capable pin
const int motor2Pin1 = 17;
const int motor2Pin2 = 16;
const int motor2EN = 4; // PWM-capable pin

Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

void setup() {
  Serial.begin(115200);

  // Initialize PS4 controller
  if (!PS4.begin("94:b5:55:f8:18:ba")) {
    Serial.println("PS4 controller not connected!");
    while (true); // Stop if the controller is not connected
  }
  
  Serial.println("PS4 controller connected!");

  // Attach servos to GPIO pins
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  servo5.attach(servoPin5);

  // Set motor control pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1EN, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(motor2EN, OUTPUT);

  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (PS4.isConnected()) {
    // Read ultrasonic sensor
    float distance_cm = ultrasonic.distanceRead(CM);

    // Map left joystick (LStickY) to servo1 and servo2 positions
    int servo1Pos = map(PS4.LStickY(), -128, 127, 0, 180);
    int servo2Pos = map(PS4.LStickX(), -128, 127, 0, 180);

    // Map right joystick (RStickY) to servo3 and servo4 positions
    int servo3Pos = map(PS4.RStickY(), -128, 127, 0, 180);
    int servo5Pos = map(PS4.RStickX(), -128, 127, 0, 180);

    // Map right trigger (R2) to servo5 position
    int servo4Pos = map(PS4.L2Value(), 0, 1, 0, 180);

    // Write positions to servos
    servo1.write(servo1Pos);
    servo2.write(servo2Pos);
    servo3.write(servo3Pos);
    servo4.write(servo4Pos);
    servo5.write(servo5Pos);

    // Control LED based on distance
    if (distance_cm < 10) {
      digitalWrite(LED_PIN, HIGH); // Turn on the LED
      delay(100); // Blink delay
      digitalWrite(LED_PIN, LOW); // Turn off the LED
      delay(100); // Blink delay
    } else {
      digitalWrite(LED_PIN, LOW); // Ensure LED is off if not triggered
    }

    // Control motors using PS4 buttons
    int motorSpeed = 1000; // Adjust as needed

    if (PS4.Triangle()) { // Forward
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
    } else if (PS4.Cross()) { // Backward
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH);
    } else if (PS4.Square()) { // Left
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
    } else if (PS4.Circle()) { // Right
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH);
    } else { // Stop
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);
    }

    // Set motor speed
    analogWrite(motor1EN, motorSpeed);
    analogWrite(motor2EN, motorSpeed);

    // Debugging information
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    Serial.print("Servo1: "); Serial.print(servo1Pos);
    Serial.print(" | Servo2: "); Serial.print(servo2Pos);
    Serial.print(" | Servo3: "); Serial.print(servo3Pos);
    Serial.print(" | Servo4: "); Serial.print(servo4Pos);
    Serial.print(" | Servo5: "); Serial.println(servo5Pos);
    Serial.print("Circle: "); Serial.print(PS4.Circle());
    Serial.print(" | Cross: "); Serial.print(PS4.Cross());
    Serial.print(" | Square: "); Serial.print(PS4.Square());
    Serial.print(" | Triangle: "); Serial.println(PS4.Triangle());
    
    delay(10); // Small delay to avoid overwhelming the servos and motors
  }
}
