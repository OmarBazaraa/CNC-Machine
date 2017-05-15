static class Constants {

  // 
  // PATHS AND FILES
  //
  static final String PATH_ADB = "/Users/ibrahimradwan/Library/Android/sdk/platform-tools/adb";

  static final String PATH_SCRIPTS_DIR = "/Users/ibrahimradwan/Development/puzzlesolver/puzzlesolver/";

  static final String PATH_IMAGES_DIR = "/Users/ibrahimradwan/Development/puzzlesolver/puzzlesolver/images/";
  static final String PATH_INSTRUCTIONS_DIR = "/Users/ibrahimradwan/Development/puzzlesolver/puzzlesolver/instructions/";

  static final String PATH_FLOW_INSTRUCTIONS_FILE = PATH_INSTRUCTIONS_DIR + "flow_instructions.txt";
  static final String FILE_NAME_FLOW_IMAGE = "flow_img.png";
  static final String PATH_FLOW_IMAGE_FILE = PATH_IMAGES_DIR + FILE_NAME_FLOW_IMAGE;
  static final String PATH_FLOW_SOLVER = PATH_SCRIPTS_DIR + "flow_solver";

  static final String PATH_PAINT_INSTRUCTIONS_FILE = PATH_INSTRUCTIONS_DIR + "paint_instructions.txt";
  static final String FILE_NAME_PAINTER_IMAGE = "painter_img.jpg";
  static final String PATH_PAINT_IMAGE_FILE = PATH_IMAGES_DIR + FILE_NAME_PAINTER_IMAGE;
  static final String PATH_PAINTER = PATH_SCRIPTS_DIR + "painter";

  //
  // USER OPTIONS
  // 
  static final int USER_OPTIONS_FLOW_SOLVER = 1;
  static final int USER_OPTIONS_PAPER_PAINT = 2;
  static final int USER_OPTIONS_PHONE_PAINT = 3;
  static final int USER_OPTIONS_PIANO = 4;
  static final int USER_OPTIONS_CALIBRATION = 5;
  static final int USER_OPTIONS_QUIT_SYSTEM = 6;

  // 
  // SERIAL CONFIGS
  //
  static final int CALIBRATION_MODE_STEPS_COUNT = 3000;
  static final int PAINTING_MODE_STEPS_COUNT = 300;
  static final float SCREEN_RATIO = 36350 / 1.3884455 ;

  static final int INTURREPT_ACTION_STOP = 0;
  static final int INTURREPT_ACTION_CONTINUE = 1;

  //
  // SERIAL PROTOCOL CODES
  //

  // CONTROLLER -> ARDUINO
  static final List<Character> ALLOWED_CALIBRATION_INSTRUCTIONS = Arrays.asList('>', '<', 'v', '^', 'P', 'R');

  static final char SERIAL_MOTOR_STEPS_COUNT = 'S';
  static final char SERIAL_CONTINUE_SIGNAL = 'C';
  static final char SERIAL_STOP_SIGNAL = 'E';
  static final char SERIAL_BEEP = 'B';

  // ARDUINO -> CONTROLLER
  static final char SERIAL_ACKNOWLEDGMENT = 'A';
  static final char SERIAL_PHONE_POSITION_ERROR = 'P';
  static final char SERIAL_PHONE_POSITION_ERROR_FIXED = 'N';
  static final char SERIAL_POWER_SUPPLY_ERROR = 'S';
  static final char SERIAL_POWER_SUPPLY_ERROR_FIXED = 'F';

  //
  // FLOW-SOLVER SCRIPT MODES
  //
  static final String MODE_FLOW_SOLVER_SOLVE = "0"; // Solve
  static final String MODE_FLOW_SOLVER_NEXT_LEVEL = "1"; // Next level
  
  //
  // ERRORS CODES
  //
  static final int ERROR_ARDUINO_DISCONNECTION = 0;
  static final int ERROR_PHONE_DISCONNECTION = 1;
  static final int ERROR_MISSING_GAME_ACTIVITY = 2;
  static final int ERROR_PHONE_OFF_POSITION = 3;
  static final int ERROR_POWER_LOST = 4;

  //
  // ERRORS MSGS
  //
  static final String[] MSGS_ERRORS = new String[]{
    "Please re-connect the Arduino cable!",
    "Please connect the phone cable",
    "Please return to game screen!",
    "Phone is off-position!",
    "We lost power :("
  };

  //
  // FIXED ERRORS MSGS
  //
  static final String[] MSGS_FIXED_ERRORS = new String[]{
    "Arduino is back :D",
    "Phone cable is connected",
    "Game screen detected",
    "Phone has been detected!",
    "Power is back :D"
  };
}