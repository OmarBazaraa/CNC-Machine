import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class FlowSolvingTask extends CNCTask {
  public boolean screenSpecsRetrieved = false;

  int screenWDPI;
  int screenHDPI;

  String instructions;
  int instructionsPointer = 0;

  int rows = 0, cols = 0;

  float blockSize;
  int motorStepsCount;
  String nextLevelButtonX = "0";
  String nextLevelButtonY = "0";

  public FlowSolvingTask () throws Exception {
    // Get screen specs
    this.getScreenSpecs();
  }

  public void start() throws Exception {
    System.out.println("Starting flow solver...");
    
    // Pull screenshot
    Utilities.captureScreenShot();

    // Execute flow solver algorithm which generates instructions.txt file
    this.executeSolveAlgorithm();

    // Calculate grid block size in CMs, store in the file in the new units
    // Calculate path end grid point, store in variables to send later to flow solver
    this.parseGameInstructionsFile();


    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }


  public void onStart() {
    // Print game started
    System.out.println("Flow Solving started...");
  }


  public void restart() throws Exception {
    System.out.println("Log :: Moving to next level ...  ");

    delay(3000);

    // Get screenshot
    Utilities.captureScreenShot();

    // Execute C++ to get next level button position
    try {
      Utilities.executeSystemCommand(new String[]{
        Constants.PATH_FLOW_SOLVER, 
        Constants.PATH_FLOW_IMAGE_FILE, 
        Constants.PATH_FLOW_INSTRUCTIONS_FILE, 
        Integer.toString(this.rows), 
        Integer.toString(this.cols), 
        Constants.MODE_FLOW_SOLVER_NEXT_LEVEL
        });
    }
    catch (Exception e) {
      throw new Exception ("Next level button cannot be located!");
    }
    // Get coordinates from file
    this.parseNextLevelInstructionsFile();

    try {
      // Send click command via adb
      Utilities.executeSystemCommand(new String[] {
        Constants.PATH_ADB, "shell", "input", "tap", 
        this.nextLevelButtonX, 
        this.nextLevelButtonY});
    }
    catch (Exception e) {
      throw new Exception ("Moving to next level failed!");
    }

    isRunning = true;

    delay(1500);

    port.clear();

    start();
  }

  public void stop() {
    // Print game started
    System.out.println("Stopping Flow Solver...");

    isRunning = false;

    onStop();
  }


  public void onStop() {
    System.out.println("Flow Solver stopped...");
  }


  protected MovePenTask getMovePenBackTask() {
    return new MovePenTask(rows, cols, motorStepsCount);
  }

  public char getInstruction() throws Exception {
    // Return 0 if insturctions finished
    if (instructionsPointer == instructions.length()) {
      restart(); 
      return 0;
    }

    char instruction = instructions.charAt(instructionsPointer++);

    // Update cols, rows
    if (instruction == '^')
      rows--;
    else if (instruction == 'v')
      rows++;
    else if (instruction == '>')
      cols++;
    else if (instruction == '<')
      cols--;

    // Move pointer to next
    return instruction;
  }

  protected int getConfigurations() {
    return this.motorStepsCount; // 4 bytes
  }

  private String[] parseInstructionsFile() throws Exception {
    String instructionFileContents = Utilities.getFileContents(Constants.PATH_FLOW_INSTRUCTIONS_FILE).trim();

    System.out.println("Log :: Instructions file read -> " + instructionFileContents);

    return instructionFileContents.split("\\s+", -1);
  }

  private void parseNextLevelInstructionsFile() throws Exception {
    String[] instructionFileContentsArray = parseInstructionsFile();

    this.nextLevelButtonY = instructionFileContentsArray[0];
    this.nextLevelButtonX = instructionFileContentsArray[1];

    if (this.nextLevelButtonY.equals("-1")) 
      throw new Exception("Next level button cannot be located!");
  }

  private void parseGameInstructionsFile() throws Exception {
    String[] instructionFileContentsArray = parseInstructionsFile();

    if (instructionFileContentsArray[0].equals("-1")) 
      throw new Exception("Maze is unsolveable!");

    // Get first 2 numbers (grid block width, height)
    int gridBlockWidth = Integer.parseInt(instructionFileContentsArray[0]);
    int gridBlockHeight = Integer.parseInt(instructionFileContentsArray[1]);
    this.instructions = instructionFileContentsArray[2] + Constants.SERIAL_DONE;
    this.instructionsPointer = 0;

    System.out.println("Log :: Instructions file parsed -> " + gridBlockWidth + " " + this.instructions);

    // Will save only grid width block to insructions file in CM (height ~= width)

    // Calculate grid block size in CMs
    this.blockSize = gridBlockWidth;
    System.out.println("Log :: Block size calculated -> " + blockSize + " micro cm");
    this.blockSize = ( this.blockSize / this.screenWDPI ) * 2.54; // convert to cm
    System.out.println("Log :: Block size calculated -> " + blockSize + " micro cm");
    this.motorStepsCount = (int) (this.blockSize * Constants.SCREEN_RATIO); // remove fraction part

    System.out.println("Log :: Block size calculated -> " + blockSize + " micro cm");
    System.out.println("Log :: Motor steps count calculated -> " + motorStepsCount);
  }

  private void executeSolveAlgorithm() throws Exception {

    String output = Utilities.executeSystemCommand(new String[]{
      Constants.PATH_FLOW_SOLVER, 
      Constants.PATH_FLOW_IMAGE_FILE, 
      Constants.PATH_FLOW_INSTRUCTIONS_FILE, 
      Integer.toString(this.rows), 
      Integer.toString(this.cols)
      });

    System.out.println("Log :: Game Statistics " + output );

    System.out.println("Log :: Game solved and instructions file generated");
  }

  private void getScreenSpecs() throws Exception {
    String screenSpecs = Utilities.getScreenSize().trim();

    // Format output size and fill member variables
    this.storeScreenSpecs(screenSpecs);

    System.out.println("Log :: Screen size stored");
  }

  private void storeScreenSpecs(String screenSpecs) {
    String[] screenSpecsArray = screenSpecs.split("\\s+", -1);
    this.screenWDPI = (int) Double.parseDouble(screenSpecsArray[0]);
    this.screenHDPI = (int) Double.parseDouble(screenSpecsArray[1]);
  }
}