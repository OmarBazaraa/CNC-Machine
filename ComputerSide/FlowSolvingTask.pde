import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class FlowSolvingTask extends CNCTask {
  public boolean screenSpecsRetrieved = false;

  int screenWidth;
  int screenHeight;
  int screenWDPI;
  int screenHDPI;
  int gameEndPointRow = 0;
  int gameEndPointCol = 0;

  String instructions;
  int instructionsPointer = 0;

  float blockSize;
  int motorStepsCount;
  String nextLevelButtonX = "0";
  String nextLevelButtonY = "0";

  public FlowSolvingTask () throws Exception {
    // Get screen specs
    if (!this.getScreenSpecs()) 
      throw new Exception("Couldn't get screen specs!");
  }

  public void start() {

    // Pull screenshot
    if (!this.pullScreenShot()) return;

    // Execute flow solver algorithm which generates instructions.txt file
    if (!this.executeSolveAlgorithm()) return;

    // Calculate grid block size in CMs, store in the file in the new units
    // Calculate path end grid point, store in variables to send later to flow solver
    if (!this.parseInstructionsFile(Constants.MODE_FLOW_SOLVER_SOLVE)) return;


    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }


  public void onStart() {
    // Print game started
    System.out.println("\n\nStarting Flow Solver...\n");
  }


  public void restart() {
    System.out.println("Log :: Moving to next level ...  ");

    delay(1500);

    // Get screenshot
    this.pullScreenShot();

    // Execute C++ to get next level button position
    try {
      Utilities.executeSystemCommand(new String[]{
        Constants.PATH_FLOW_SOLVER, 
        Integer.toString(this.gameEndPointRow), 
        Integer.toString(this.gameEndPointCol), 
        Constants.MODE_FLOW_SOLVER_NEXT_LEVEL
        });

      // Get coordinates from file
      if (!this.parseInstructionsFile(Constants.MODE_FLOW_SOLVER_NEXT_LEVEL)) return;

      // Send click command via adb
      Utilities.executeSystemCommand(new String[] {
        Constants.PATH_ADB, "shell", "input", "tap", 
        this.nextLevelButtonX, 
        this.nextLevelButtonY});
    }
    catch (Throwable th) {
      th.printStackTrace();
      return;
    }
    isRunning = true;

    delay(1500);

    start();
  }


  public void onRestart() {
  }


  public void stop() {
    // Print game started
    System.out.println("\n\nStopping Flow Solver...\n");

    isRunning = false;
    
    onStop();
  }


  public void onStop() {
    System.out.println("\n\nFlow Solver stopped...\n");
  }


  public char getInstruction() {
    // Return 0 if insturctions finished
    if (instructionsPointer == instructions.length()) {
      restart(); 
      return 0;
    }

    // Move pointer to next
    return instructions.charAt(instructionsPointer++);
  }

  protected String getConfigurations() {
    String configs = "";
    int count = this.motorStepsCount; // 4 bytes

    for (int i = 0; i < 4; i++) {
      char firstChar = (char) count;
      configs+=firstChar;
      count = count >> 8;
    }

    return configs;
  }

  private boolean parseInstructionsFile(String mode) {
    String instructionFileContents = Utilities.getFileContents(Constants.PATH_INSTRUCTIONS_FILE).trim();
    if (instructionFileContents.length() == 0) {
      System.err.println("Error :: Please make sure that instructions file exists\n");
      return false;
    }
    System.out.println("Log :: Instructions file read -> " + instructionFileContents);

    String[] instructionFileContentsArray = instructionFileContents.split("\\s+", -1);

    // Solve mode 
    if (mode.equals(Constants.MODE_FLOW_SOLVER_SOLVE)) {
      // Get first 2 numbers (grid block width, height)
      int gridBlockWidth = Integer.parseInt(instructionFileContentsArray[0]);
      int gridBlockHeight = Integer.parseInt(instructionFileContentsArray[1]);
      this.instructions = instructionFileContentsArray[2];
      this.instructionsPointer = 0;

      System.out.println("Log :: Instructions file parsed -> " + gridBlockWidth + " " + this.instructions);

      // Will save only grid width block to insructions file in CM (height ~= width)

      // Calculate grid block size in CMs
      this.blockSize = gridBlockWidth;
      this.blockSize = ( this.blockSize / this.screenWDPI ) * 2.54; // convert to cm
      this.motorStepsCount = (int) (this.blockSize * Constants.SCREEN_RATIO); // remove fraction part

      System.out.println("Log :: Block size calculated -> " + blockSize + " micro cm");

      // Calculate end path point on grid
      this.calculateGameEndPoint(this.instructions);
    } 
    // Move to next level mode
    else if (mode.equals(Constants.MODE_FLOW_SOLVER_NEXT_LEVEL)) {
      this.nextLevelButtonY = instructionFileContentsArray[0];
      this.nextLevelButtonX = instructionFileContentsArray[1];
    }
    return true;
  }

  private void calculateGameEndPoint(String instructions) {
    instructions = instructions.replace("P", "").replace("R", "");

    this.gameEndPointRow += (instructions.length() - instructions.replace("v", "").length());
    this.gameEndPointRow -= (instructions.length() - instructions.replace("^", "").length());
    this.gameEndPointCol += (instructions.length() - instructions.replace(">", "").length());
    this.gameEndPointCol -= (instructions.length() - instructions.replace("<", "").length());

    System.out.println("Log :: End point (row,col) -> " + gameEndPointRow + ","  + gameEndPointCol);
  }

  private boolean executeSolveAlgorithm() {
    try
    {   
      String output = Utilities.executeSystemCommand(new String[]{Constants.PATH_FLOW_SOLVER, 
        Integer.toString(this.gameEndPointRow), Integer.toString(this.gameEndPointCol)});

      System.out.println("Log :: Game Statistics " + output );
    } 
    catch (Throwable t)
    {
      t.printStackTrace();
      return false;
    }
    System.out.println("Log :: Game solved and instructions file generated");
    return true;
  }

  private boolean pullScreenShot() {
    boolean success = Utilities.captureScreenShot();
    if (success)
      System.out.println("Log :: Screenshot captured");

    return success;
  }


  private boolean getScreenSpecs() {
    String screenSpecs = Utilities.getScreenSize().trim();
    if (screenSpecs.length() == 0) {
      System.err.println("Error :: Please make sure that your device is connected\n");
      return false;
    }

    // Format output size and fill member variables
    this.storeScreenSpecs(screenSpecs);

    System.out.println("Log :: Screen size stored");
    return true;
  }

  private void storeScreenSpecs(String screenSpecs) {
    String[] screenSpecsArray = screenSpecs.split("\\s+", -1);
    this.screenWidth = Integer.parseInt(screenSpecsArray[0]);
    this.screenHeight = Integer.parseInt(screenSpecsArray[1]);
    this.screenWDPI = (int) Double.parseDouble(screenSpecsArray[2]);
    this.screenHDPI = (int) Double.parseDouble(screenSpecsArray[3]);
  }
}