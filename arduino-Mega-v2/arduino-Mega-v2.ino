#include <Servo.h>
#include "DFRobotDFPlayerMini.h"

const int leftMotorPWM = 9;
const int leftMotorDir1 = 8;
const int leftMotorDir2 = 7;

const int rightMotorPWM = 10;
const int rightMotorDir1 = 6;
const int rightMotorDir2 = 5;

const int servoPin = 11;
const int motorSpeed = 95;

// DFPlayer Mini Serial (Using Serial2 on Mega)
#define DFPLAYER_RX 16  // DFPlayer TX -> Mega RX2
#define DFPLAYER_TX 17  // DFPlayer RX -> Mega TX2 (Use 1kΩ Resistor)
DFRobotDFPlayerMini myDFPlayer;

Servo myServo;

void setup() {
    Serial.begin(9600);   // Serial Monitor (for debugging)
    Serial1.begin(9600);  // Bluetooth module connected to Serial1 (TX1, RX1)
    Serial2.begin(9600);  // DFPlayer Mini connected to Serial2 (TX2, RX2)

    pinMode(leftMotorPWM, OUTPUT);
    pinMode(leftMotorDir1, OUTPUT);
    pinMode(leftMotorDir2, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);
    pinMode(rightMotorDir1, OUTPUT);
    pinMode(rightMotorDir2, OUTPUT);

    myServo.attach(servoPin);
    myServo.write(0); // Default position
    
    Serial.println("Initializing DFPlayer Mini...");
    if (!myDFPlayer.begin(Serial2)) {  
        Serial.println("DFPlayer Mini not detected! Check wiring.");
        while (true);  // Halt execution if DFPlayer is not found
    }
    
    myDFPlayer.volume(20); // Set volume (0 to 30)

    Serial.println("Bluetooth Module Ready. Waiting for messages...");
}

void loop() {
    if (Serial1.available()) {  
        String command = Serial1.readString();
        command.trim();
        
        Serial.print("Received: ");
        Serial.println(command);
        
        if (command == "TRUE") {
            packageDeliverySequence();
        } else {
            controlMotors(command[0]);
        }
    }

    if (Serial.available()) {
        String userMessage = Serial.readString();
        Serial1.println(userMessage);
        Serial.print("Sent to Laptop: ");
        Serial.println(userMessage);
    }
}

void packageDeliverySequence() {
    Serial.println("Face Recognised. Starting package delivery...");

    myDFPlayer.play(4); // "Face Recognised"
    delay(2000);  // Wait for audio to finish

    openServo();  
    delay(500);  

    myDFPlayer.play(3); // "Please take the package in 10 seconds"
    delay(10000); // Wait 10 seconds

    closeServo();
    delay(500);

    myDFPlayer.play(2); // "Package Delivered"
    delay(2000);  

    myDFPlayer.play(1); // "Bye"
    delay(2000);  

    Serial.println("Package delivery sequence complete.");
}

void controlMotors(char command) {
    switch (command) {
        case 'F':
            moveForward();
            break;
        case 'B':
            moveBackward();
            break;
        case 'L':
            turnLeft();
            break;
        case 'R':
            turnRight();
            break;
        case 'S':
            stopMotors();
            break;
        case 'O':
            openServo();
            break;
        case 'C':
            closeServo();
            break;
        default:
            Serial.println("Invalid command");
            break;
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
    if (myServo.read() != 90) {
        myServo.write(90);
        Serial.println("Servo Opened (90°)");
    }
}

void closeServo() {
    if (myServo.read() != 0) {
        myServo.write(0);
        Serial.println("Servo Closed (0°)");
    }
}
