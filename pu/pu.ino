#include <Servo.h> 

const int STEP_PIN_X = 14;
const int DIR_PIN_X = 15;
const int STEP_PIN_Y = 16;
const int DIR_PIN_Y = 17;
const int STEPPER_MODE_PIN = 18;
int deltaX = 0;
int deltaY = 0;
Servo ZServo;

void setup()
{
  // Configure serial port
  Serial.begin(9600);
  
  // Setup pins mode
  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  pinMode(STEPPER_MODE_PIN, OUTPUT);
  
  // Setup servo motor
  ZServo.attach(9);
  ZServo.write(90); // Set servo to mid-point
  moveZ(0);
  
  // Setup stepper motor to full step mode
  digitalWrite(STEPPER_MODE_PIN, HIGH);
}

int x = 0;
int incomingByte =-1;
int numberOfSteps = 18000;

void moveZ(int d)
{
  if( d == 1)
    ZServo.write(90);
  else
    ZServo.write(120);
  delay(200);
}

void move(int stepPin, int directionPin, int dir) {
  digitalWrite(directionPin, dir);
  
  for(int i = 0; i < numberOfSteps; i++)
  {   
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(40);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(40);
  }
  
  delay(20);
}

void loop()
{
  while (Serial.available() > 0) {
    incomingByte = Serial.read();
    
    if (incomingByte == '^') {//Move X Backwards
      deltaX++;
      move(STEP_PIN_X, DIR_PIN_X, 0);
    }
    else if (incomingByte == 'v') {  //Move X Forward
      deltaX--;
      move(STEP_PIN_X, DIR_PIN_X, 1);
    }
    else if (incomingByte == '>') { //Move Y Right
      deltaY--;
      move(STEP_PIN_Y, DIR_PIN_Y, 1);
    }
    else if (incomingByte == '<') { //Move Y Left
      deltaY++;
      move(STEP_PIN_Y, DIR_PIN_Y, 0);
    }
    else if (incomingByte == 'P') { //Push Pen
      moveZ(1);
    }
    else if (incomingByte == 'R') { //Release Pen
      moveZ(0);
    }
    else if (incomingByte == 'E') { //End solving
      break;
    }
  }
  
  // Return back to where we started before solving 
  while (deltaX != 0){
    if (deltaX < 0){
      deltaX++;
      move(STEP_PIN_X, DIR_PIN_X, 0);
    } 
    else if (deltaX > 0){
      deltaX--;
      move(STEP_PIN_X, DIR_PIN_X, 1);
    }   
  }
  while (deltaY != 0){
    if (deltaY < 0) {
      deltaY++;
      move(STEP_PIN_Y, DIR_PIN_Y, 0);
    }
    else if (deltaY > 0) {
      deltaY--;
      move(STEP_PIN_Y, DIR_PIN_Y, 1);
    }
  }
}
