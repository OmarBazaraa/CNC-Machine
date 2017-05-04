import processing.serial.*;
import java.util.Scanner; 

// ===========================
// Variables
// ===========================
Serial port;
CNCTask cncTask;

boolean userOptionsMenuDisplayed = false;
int userOption = 0;

boolean isKeyConsumed = false;

void setup() {
  // Intialize port for Arduino
  port = new Serial(this, Serial.list()[7], 9600);
}

// Application main loop function
void draw() {

  if (cncTask == null || !cncTask.isRunning()) {
    displayMenuAndGetUserOption();
  } 
  // operation is running
  else {
    cncTask.executeInstruction();
  }
}

void displayMenuAndGetUserOption() {
  // Display options menu for the user (if not already displayed)
  displayUserOptionsMenu();

  // Perform user action
  try {
    switch (userOption) {
    case Constants.USER_OPTIONS_FLOW_SOLVER:
      resetSystemVariables();
      cncTask = new FlowSolvingTask();
      cncTask.start();
      break;

    case Constants.USER_OPTIONS_PAPER_PAINT:
      resetSystemVariables();
      cncTask = new PaintingTask();
      cncTask.start();

      break;

    case Constants.USER_OPTIONS_PHONE_PAINT:
      break;

    case Constants.USER_OPTIONS_CALIBRATION:
      resetSystemVariables();
      cncTask = new CalibrationTask();
      cncTask.start();

      break;

    case Constants.USER_OPTIONS_QUIT_SYSTEM:
      System.out.println("Bye!");
      exit();
    }
  }
  catch (Exception e) {
    System.err.println("Sorry");
    cncTask = null;
  }
}
// Get user main option input 
void displayUserOptionsMenu () {
  if (!userOptionsMenuDisplayed) {
    userOption = 0;
    System.out.println("\n\nEnter a number: ");
    System.out.println("[" + Constants.USER_OPTIONS_FLOW_SOLVER + "] : Solve flow free game");
    System.out.println("[" + Constants.USER_OPTIONS_PAPER_PAINT + "] : Paint photo on paper");
    System.out.println("[" + Constants.USER_OPTIONS_PHONE_PAINT + "] : Paint photo on tablet");
    System.out.println("[" + Constants.USER_OPTIONS_CALIBRATION + "] : Cailbrate!");
    System.out.println("[" + Constants.USER_OPTIONS_QUIT_SYSTEM + "] : Quit the system!");

    userOptionsMenuDisplayed = true;
  }
}

// Catch user input
void keyPressed() {
  if (cncTask != null)
    cncTask.setKeyStatus(key, true); 

  if (key == 'T' && cncTask != null) {
    System.out.println("Log :: Terminatting ...  ");

    cncTask.stop();
    cncTask = null;
    resetSystemVariables();
  }

  userOption = key - '0';
}

void keyReleased() {
  if (cncTask != null)
    cncTask.setKeyStatus(key, false);
}

// Reset system variables
void resetSystemVariables() {
  userOption = 0;
  userOptionsMenuDisplayed = false;
}