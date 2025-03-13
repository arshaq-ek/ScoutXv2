#include <Servo.h>

const int leftMotorPWM = 9;
const int leftMotorDir1 = 8;
const int leftMotorDir2 = 7;

const int rightMotorPWM = 10;
const int rightMotorDir1 = 6;
const int rightMotorDir2 = 5;

const int servoPin = 11;
const int motorSpeed = 95;

Servo myServo;

void setup() {
    Serial.begin(9600);   // Debugging
    Serial1.begin(9600);  // Bluetooth module

    pinMode(leftMotorPWM, OUTPUT);
    pinMode(leftMotorDir1, OUTPUT);
    pinMode(leftMotorDir2, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);
    pinMode(rightMotorDir1, OUTPUT);
    pinMode(rightMotorDir2, OUTPUT);

    myServo.attach(servoPin);
    myServo.write(0); // Default position

    Serial.println("Bluetooth Ready. Waiting for commands...");
}

void loop() {
    if (Serial1.available()) {  
        char command = Serial1.read();  // Read ONE character at a time
        Serial.print("Received: ");
        Serial.println(command);

        controlMotors(command);
    }

    if (Serial.available()) {
        String userMessage = Serial.readString();
        Serial1.println(userMessage);
        Serial.print("Sent to Laptop: ");
        Serial.println(userMessage);
    }
}

void controlMotors(char command) {
    if (command == 'S') {
        stopMotors();
        return; // Exit immediately to prevent other movements
    }

    switch (command) {
        case 'F': moveForward(); break;
        case 'B': moveBackward(); break;
        case 'L': turnLeft(); break;
        case 'R': turnRight(); break;
        case 'O': openServo(); break;
        case 'C': closeServo(); break;
        case 'T': packageDeliverySequence(); break;
        default: Serial.println("Invalid command"); break;
    }
}

void moveForward() {
    digitalWrite(leftMotorDir1, HIGH);
    digitalWrite(leftMotorDir2, LOW);
    analogWrite(leftMotorPWM, motorSpeed);

    digitalWrite(rightMotorDir1, HIGH);
    digitalWrite(rightMotorDir2, LOW);
    analogWrite(rightMotorPWM, motorSpeed);

    Serial.println("Moving Forward");
}

void moveBackward() {
    digitalWrite(leftMotorDir1, LOW);
    digitalWrite(leftMotorDir2, HIGH);
    analogWrite(leftMotorPWM, motorSpeed);

    digitalWrite(rightMotorDir1, LOW);
    digitalWrite(rightMotorDir2, HIGH);
    analogWrite(rightMotorPWM, motorSpeed);

    Serial.println("Moving Backward");
}

void turnLeft() {
    digitalWrite(leftMotorDir1, LOW);
    digitalWrite(leftMotorDir2, HIGH);
    analogWrite(leftMotorPWM, motorSpeed);

    digitalWrite(rightMotorDir1, HIGH);
    digitalWrite(rightMotorDir2, LOW);
    analogWrite(rightMotorPWM, motorSpeed);

    Serial.println("Turning Left");
}

void turnRight() {
    digitalWrite(leftMotorDir1, HIGH);
    digitalWrite(leftMotorDir2, LOW);
    analogWrite(leftMotorPWM, motorSpeed);

    digitalWrite(rightMotorDir1, LOW);
    digitalWrite(rightMotorDir2, HIGH);
    analogWrite(rightMotorPWM, motorSpeed);

    Serial.println("Turning Right");
}

void stopMotors() {
    analogWrite(leftMotorPWM, 0);
    analogWrite(rightMotorPWM, 0);
    Serial.println("Motors Stopped");
}

void openServo() {
    myServo.write(90);
    Serial.println("Servo Opened (90°)");
}

void closeServo() {
    myServo.write(0);
    Serial.println("Servo Closed (0°)");
}

void packageDeliverySequence() {
    Serial.println("Face Recognised. Starting package delivery...");

    openServo();  
    delay(500);  

    Serial.println("Please take the package in 10 seconds.");
    delay(10000);

    closeServo();
    delay(500);

    Serial.println("Package Delivered. Bye.");
}
