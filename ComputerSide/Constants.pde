class Constants {  

  static final String PATH_SCRIPTS_DIR = "/Users/ibrahimradwan/Development/puzzlesolver/puzzlesolver/";
  static final String PATH_INSTRUCTIONS_FILE = PATH_SCRIPTS_DIR + "instructions.txt";
  static final String PATH_PAINT_INSTRUCTIONS_FILE = PATH_SCRIPTS_DIR + "paint_instructions.txt";
  static final String PATH_IMAGE_FILE = PATH_SCRIPTS_DIR + "game.png";
  static final String PATH_FLOW_SOLVER = PATH_SCRIPTS_DIR + "flow_solver";
  static final String PATH_PAINTER = PATH_SCRIPTS_DIR + "painter";
  static final String PATH_ADB = "/Users/ibrahimradwan/Library/Android/sdk/platform-tools/adb";

  static final int USER_OPTIONS_FLOW_SOLVER = 1;
  static final int USER_OPTIONS_PAPER_PAINT = 2;
  static final int USER_OPTIONS_PHONE_PAINT = 3;
  static final int USER_OPTIONS_CALIBRATION = 4;
  static final int USER_OPTIONS_QUIT_SYSTEM = 5;

  static final int SERIAL_TRANSMISSION_DELAY = 50;
  static final int CALIBRATION_MODE_STEPS_COUNT = 1200;
  static final int PAINTING_MODE_STEPS_COUNT = 300;
  static final char SERIAL_INSTUCTION_DONE = 'A';
  static final char SERIAL_CONFIGURATIONS = 'S';
  static final float SCREEN_RATIO = 36350 / 1.3884455 ;


  static final String MODE_FLOW_SOLVER_SOLVE = "0"; // Solve
  static final String MODE_FLOW_SOLVER_NEXT_LEVEL = "1"; // Next level
}