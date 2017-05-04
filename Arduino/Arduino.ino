#include <Servo.h>

const long STEP_PIN_X = 16;
const long DIR_PIN_X = 17;
const long STEP_PIN_Y = 14;
const long DIR_PIN_Y = 15;
const long STEPPER_MODE_PIN = 18;

// Motor variables
Servo servoZ;
long motorStepsCount = 0;

void setup() {
  // Configure serial port
  Serial.begin(9600);

  // Setup pins mode
  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  pinMode(STEPPER_MODE_PIN, OUTPUT);

  // Setup servo motor
  servoZ.attach(9);
  servoZ.write(90); // Set servo to mid-polong
  moveZ(0);

  // Setup stepper motor to full step mode
  digitalWrite(STEPPER_MODE_PIN, HIGH);
}

void loop() {
  if (Serial.available() <= 0)
    return;

  int incommingByte = Serial.read();

  // Set up moter step count
  if (incommingByte == 'S') {
    readMotorStepsCount();
  }
  // Else try to excute the command if support
  else {
    executeCommand(incommingByte);
  }
}

// ==========================================
//  CNC COMMANDS FUNCTIONS
// ==========================================

void executeCommand(int command) {
  
  if (command == '^')     // Move X Backwards
    moveStepper(STEP_PIN_X, DIR_PIN_X, 1);
  else if (command == 'v')  // Move X Forward
    moveStepper(STEP_PIN_X, DIR_PIN_X, 0);
  else if (command == '>')  // Move Y Right
    moveStepper(STEP_PIN_Y, DIR_PIN_Y, 1);
  else if (command == '<')  // Move Y Left
    moveStepper(STEP_PIN_Y, DIR_PIN_Y, 0);
  else if (command == 'P')  // Push Pen
    moveZ(1);
  else if (command == 'R')  // Release Pen
    moveZ(0);

  //Send Acknowldegement
  Serial.write('A');
}

// ==========================================
//  CONFIGURATIONS FUNCTIONS
// ==========================================

void readMotorStepsCount() {
  motorStepsCount = 0;
  int bytesReceived = 0;

  while (bytesReceived < 4) {
    if (Serial.available() <= 0)
      continue;

    long incomingByte = Serial.read();
    incomingByte <<= (8 * bytesReceived);
    motorStepsCount |= incomingByte;

    bytesReceived++;
  }
}

// ==========================================
//  MOTOR FUNCTIONS
// ==========================================

void moveZ(long d) {
  servoZ.write(d == 1 ? 90 : 120);
  delay(200);
}

void moveStepper(long stepPin, long directionPin, long dir) {
  digitalWrite(directionPin, dir);

  for (long i = 0; i < motorStepsCount; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(40);
  }
}
