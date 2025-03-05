
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
    Serial.begin(9600);   // Serial Monitor (for debugging)
    Serial1.begin(9600);  // Bluetooth module connected to Serial1 (TX1, RX1)
    
    pinMode(leftMotorPWM, OUTPUT);
    pinMode(leftMotorDir1, OUTPUT);
    pinMode(leftMotorDir2, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);
    pinMode(rightMotorDir1, OUTPUT);
    pinMode(rightMotorDir2, OUTPUT);
    
    myServo.attach(servoPin);
    myServo.write(0); // Default position
    
    Serial.println("Bluetooth Module Ready. Waiting for messages...");
}

void loop() {
    if (Serial1.available()) {  
        char command = Serial1.read();
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
