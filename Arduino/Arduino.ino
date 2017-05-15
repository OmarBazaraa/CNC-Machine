#include <Servo.h>

// ==========================================
// CONSTANTS & VARIABLES
// ==========================================

//
// Signals constants
//

// CONTROLLER -> ARDUINO
const char SERIAL_MOTOR_STEPS_COUNT = 'S';
const char SERIAL_CONTINUE_SIGNAL = 'C';
const char SERIAL_STOP_SIGNAL = 'E';
const char SERIAL_BEEP = 'B';

// ARDUINO -> CONTROLLER
const char SERIAL_ACKNOWLEDGMENT = 'A';
const char SERIAL_PHONE_POSITION_ERROR = 'P';
const char SERIAL_PHONE_POSITION_ERROR_FIXED = 'N';
const char SERIAL_POWER_SUPPLY_ERROR = 'S';
const char SERIAL_POWER_SUPPLY_ERROR_FIXED = 'F';
const char SERIAL_CNC_OUT_RANGE_ERROR = 'R';
const char SERIAL_CNC_OUT_RANGE_ERROR_FIXED = 'M';

//
// Interrupts pins
//
const int PHONE_POSITION_SENSOR = 2;
const int POWER_SUPPLY_SENSOR = 3;
// TODO: get pin numbers
const int CNC_RANGE_UP = x;
const int CNC_RANGE_DOWN = x;
const int CNC_RANGE_LEFT = x;
const int CNC_RANGE_RIGHT = x;

//
// Buzzer pin
//
const int BUZZER_PIN = 5;

//
// Motors pins
//
const int STEP_PIN_Y = 14;
const int DIR_PIN_Y = 15;
const int STEP_PIN_X = 16;
const int DIR_PIN_X = 17;
const int STEPPER_MODE_PIN = 18;

//
// Motor variables
//
Servo servoZ;
long motorStepsCount = 0;
long motorRemainingStepsCount = 0;
int motorDirection = -1;
int motorStepPin = -1;
int motorDirectionPin = -1;
boolean isMotorStepping = false;

//
// Sound variables
//
long lastBeepSoundTimestamp = 0;
long beepsRemainingCount = 0;
long beepDuration = 0;
long beepLoopIdx = 0;
int beepSignal = HIGH;
boolean isBuzzerBeeping = false;

//
// Interrupt flags
//
int phonePositionFlag = HIGH;
int powerSupplyFlag = HIGH;
int cncRangeUpFlag = LOW;
int cncRangeDownFlag = LOW;
int cncRangeLeftFlag = LOW;
int cncRangeRightFlag = LOW;

//
// Error variables
//
boolean errorExists = false;

// ==========================================
// ARDUINO FUNCTIONS
// ==========================================

void setup() {
  // Configure serial port
  Serial.begin(9600);

  // Flush serial buffer
  while (Serial.available() > 0) {
    Serial.read();
  }

  // Setup pins mode
  pinMode(13, OUTPUT);

  pinMode(PHONE_POSITION_SENSOR, INPUT);
  pinMode(POWER_SUPPLY_SENSOR, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  pinMode(STEPPER_MODE_PIN, OUTPUT);

  // Attach phone position sensor as intrrupt
  // attachInterrupt(digitalPinToInterrupt(PHONE_POSITION_SENSOR), phoneSensorListener, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(POWER_SUPPLY_SENSOR), powerSupplyListener, CHANGE);

  // Setup servo motor
  servoZ.attach(9);
  servoZ.write(90); // Set servo to mid-point
  moveServo(0);

  // Setup stepper motor to full step mode
  digitalWrite(STEPPER_MODE_PIN, HIGH);
}

void loop() {  
  checkInterrupts();
  beep();  
  moveStepper();

  // Check if serial available
  if (Serial.available() <= 0)
    return;

  int incommingByte = Serial.read();

  executeCommand(incommingByte);
}

// ==========================================
// CNC COMMANDS FUNCTIONS
// ==========================================

void executeCommand(int command) {
  //
  // Error handling commands
  //

  // Error
  if (command == SERIAL_STOP_SIGNAL)
    stopExecution();
  // Continue command exection
  else if (command == SERIAL_CONTINUE_SIGNAL)
    continueExecution();

  //
  // Motor commands
  //
  
  if (errorExists)
    return;
    
  // Set up moter step count
  if (command == SERIAL_MOTOR_STEPS_COUNT)
    readMotorStepsCount();
  else if (command == '^')  // Move X Backwards
    initStepper(STEP_PIN_X, DIR_PIN_X, 1);
  else if (command == 'v')  // Move X Forward
    initStepper(STEP_PIN_X, DIR_PIN_X, 0);
  else if (command == '>')  // Move Y Right
    initStepper(STEP_PIN_Y, DIR_PIN_Y, 1);
  else if (command == '<')  // Move Y Left
    initStepper(STEP_PIN_Y, DIR_PIN_Y, 0);
  else if (command == 'P') { // Press Pen
    moveServo(1);
    Serial.write(SERIAL_ACKNOWLEDGMENT);
  }
  else if (command == 'R') { // Release Pen
    moveServo(0);
    Serial.write(SERIAL_ACKNOWLEDGMENT);
  }

  //
  // Sound commands
  //

  else if (command == 'B')  // Beep sound
    initBuzzer(55, 2);
  else                      // Play piano note
    piano(command);
}

// ==========================================
// CONFIGURATIONS FUNCTIONS
// ==========================================

void stopExecution() {
  errorExists = true;

  // Error sound
  initBuzzer(400, 8);
}

void continueExecution() {
  errorExists = false;

  // Beep sound
  initBuzzer(55, 2);
}

void checkInterrupts() {
  // Check phone in position
  int newRead = digitalRead(PHONE_POSITION_SENSOR);
  if (newRead != phonePositionFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_PHONE_POSITION_ERROR);
    else
      Serial.write(SERIAL_PHONE_POSITION_ERROR_FIXED);

    delay(40); // TODO: to be checked
    phonePositionFlag = newRead;
  }

  // Check power supply
  newRead = digitalRead(POWER_SUPPLY_SENSOR);
  if (newRead != powerSupplyFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_POWER_SUPPLY_ERROR);
    else
      Serial.write(SERIAL_POWER_SUPPLY_ERROR_FIXED);

    delay(40); // TODO: to be checked
    phonePositionFlag = newRead;
  }

  // Out of range up
  newRead = digitalRead(CNC_RANGE_UP);
  if (newRead != cncRangeUpFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR);
    else
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR_FIXED);

    phonePositionFlag = newRead;
  }

  // Out of range down
  newRead = digitalRead(CNC_RANGE_DOWN);
  if (newRead != cncRangeDownFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR);
    else
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR_FIXED);

    phonePositionFlag = newRead;
  }

  // Out of range left
  newRead = digitalRead(CNC_RANGE_LEFT);
  if (newRead != cncRangeLeftFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR);
    else
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR_FIXED);

    phonePositionFlag = newRead;
  }

  // Out of range right
  newRead = digitalRead(CNC_RANGE_RIGHT);
  if (newRead != cncRangeRightFlag) {
    if (newRead == HIGH)
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR);
    else
      Serial.write(SERIAL_CNC_OUT_RANGE_ERROR_FIXED);

    phonePositionFlag = newRead;
  }
}

// ==========================================
// MOTOR FUNCTIONS
// ==========================================

void readMotorStepsCount() {
  readLong(motorStepsCount);

  Serial.write(SERIAL_ACKNOWLEDGMENT);

  // Beep sound
  initBuzzer(55, 2);
}

void initStepper(int stepPin, int directionPin, boolean dir) {
  motorStepPin = stepPin;
  motorDirectionPin = directionPin;
  motorDirection = dir;
  motorRemainingStepsCount = motorStepsCount;
  isMotorStepping = true;

  digitalWrite(motorDirectionPin, motorDirection);
}

void moveStepper() {
  if (!isMotorStepping || errorExists) {
    return;
  }

  // Checking left and right direction ranges
  if (motorStepPin == STEP_PIN_Y) {
    if (motorDirection && cncRangeRightFlag == HIGH)
      return;
    if (!motorDirection && cncRangeLeftFlag == HIGH)
      return;
  }
  // Checking up and down direction ranges
  else if (motorStepPin == STEP_PIN_X) {
    if (motorDirection && cncRangeUpFlag == HIGH)
      return;
    if (!motorDirection && cncRangeDownFlag == HIGH)
      return;
  }

  digitalWrite(motorStepPin, HIGH);
  delayMicroseconds(40);
  digitalWrite(motorStepPin, LOW);
  delayMicroseconds(40);

  if (--motorRemainingStepsCount == 0) {
    // Reset motor variables
    isMotorStepping = false;

    // Send acknowledgment
    Serial.write(SERIAL_ACKNOWLEDGMENT);
    return;
  }
}

void moveServo(boolean down) {
  servoZ.write(down == 1 ? 90 : 120);
  // ToDo: check after removing 'A'
  delay(200);
}

// ==========================================
// SOUND FUNCTIONS
// ==========================================
void initBuzzer(long duration, long count) {
  beepsRemainingCount = count;
  beepDuration = duration;
  beepLoopIdx = 0;
  lastBeepSoundTimestamp = 0;

  beepSignal = HIGH;

  isBuzzerBeeping = true;
}

void beep() {
  if (!isBuzzerBeeping)
    return;

  long currentTimestamp = millis();

  if (currentTimestamp - lastBeepSoundTimestamp < beepDuration)
    return;

  digitalWrite(BUZZER_PIN, beepSignal);

  beepSignal = (beepSignal == HIGH) ? LOW : HIGH;

  if (--beepsRemainingCount == 0) {
    isBuzzerBeeping = false;
    // Serial.write(SERIAL_ACKNOWLEDGMENT);
  }

  lastBeepSoundTimestamp = currentTimestamp;
}

void piano(int key) {
  key = key % 48;
  // tone(BUZZER_PIN, notes[key], 1000);
}

// ==========================================
// UTILITIES FUNCTIONS
// ==========================================

void readLong(long& val) {
  val = 0;
  int bitsReceived = 0;

  while (bitsReceived < 32) {
    if (Serial.available() <= 0)
      continue;

    long incomingByte = Serial.read();
    val |= incomingByte << bitsReceived;

    bitsReceived += 8;
  }
}

// void phoneSensorListener() {
//   if (digitalRead(PHONE_POSITION_SENSOR) == LOW) {
//     Serial.write(SERIAL_PHONE_POSITION_ERROR);
//   }
//   else {
//     Serial.write(SERIAL_PHONE_POSITION_ERROR_FIXED);
//   }
//   // ToDo: Check after using PD network
//   delay(40);
// }

// void powerSupplyListener() {
//   if (digitalRead(POWER_SUPPLY_SENSOR) == LOW) {
//     Serial.write(SERIAL_POWER_SUPPLY_ERROR);
//   }
//   else {
//     Serial.write(SERIAL_POWER_SUPPLY_ERROR_FIXED);
//   }
//   // ToDo: Check after using PD network
//   delay(40);
// }