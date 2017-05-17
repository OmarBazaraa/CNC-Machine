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
const char SERIAL_CHECK_INTERRUPTS = 'T';

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
const int POWER_SUPPLY_SENSOR_PIN = 6;
const int PHONE_POSITION_SENSOR_PIN = 7;
const int CNC_RANGE_UP_PIN = 10;
const int CNC_RANGE_DOWN_PIN = 11;
const int CNC_RANGE_LEFT_PIN = 12;
const int CNC_RANGE_RIGHT_PIN = 13;

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
long motorStepsCount = 12000;
long motorRemainingStepsCount = 0;
int motorDirection = -1;
int motorStepPin = -1;
int motorDirectionPin = -1;
boolean isMotorStepping = false;

int motorStepsCountAddress = 5;

//
// Sound variables
//
long lastBeepSoundTimestamp = 0;
long beepsRemainingCount = 0;
long beepDuration = 0;
int beepSignal = HIGH;
boolean isBuzzerBeeping = false;

//
// Interrupt variables
//
int phonePositionFlag = LOW;
int powerSupplyFlag = HIGH;
int cncRangeUpFlag = LOW;
int cncRangeDownFlag = LOW;
int cncRangeLeftFlag = LOW;
int cncRangeRightFlag = LOW;

int interruptPins[] = {
  PHONE_POSITION_SENSOR_PIN,
  POWER_SUPPLY_SENSOR_PIN,
  CNC_RANGE_UP_PIN,
  CNC_RANGE_DOWN_PIN,
  CNC_RANGE_LEFT_PIN,
  CNC_RANGE_RIGHT_PIN
};

int* interruptFlags[] = {
  &phonePositionFlag,
  &powerSupplyFlag,
  &cncRangeUpFlag,
  &cncRangeDownFlag,
  &cncRangeLeftFlag,
  &cncRangeRightFlag
};

char interruptErrorChar[] = {
  SERIAL_PHONE_POSITION_ERROR,
  SERIAL_POWER_SUPPLY_ERROR,
  SERIAL_CNC_OUT_RANGE_ERROR,
  SERIAL_CNC_OUT_RANGE_ERROR,
  SERIAL_CNC_OUT_RANGE_ERROR,
  SERIAL_CNC_OUT_RANGE_ERROR
};

char interruptErrorFixChar[] = {
  SERIAL_PHONE_POSITION_ERROR_FIXED,
  SERIAL_POWER_SUPPLY_ERROR_FIXED,
  SERIAL_CNC_OUT_RANGE_ERROR_FIXED,
  SERIAL_CNC_OUT_RANGE_ERROR_FIXED,
  SERIAL_CNC_OUT_RANGE_ERROR_FIXED,
  SERIAL_CNC_OUT_RANGE_ERROR_FIXED
};

int interruptErrorSignal[] = {
  HIGH,
  LOW,
  HIGH,
  HIGH,
  HIGH,
  HIGH
};

long lastErrorCheckTimestamp = 0;
long lastMotorCheckTimestamp = 0;

//
// Error variables
//
boolean errorExists = false;

boolean rangeChecksState = true;