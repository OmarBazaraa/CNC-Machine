import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class PaintingTask extends CNCTask {

  String instructions;
  int instructionsPointer = 0;

  public void start() {

    // Execute flow solver algorithm which generates instructions.txt file
    if (!this.executePaintingAlgorithm()) return;

    // Calculate grid block size in CMs, store in the file in the new units
    // Calculate path end grid point, store in variables to send later to flow solver
    if (!this.parseInstructionsFile()) return;


    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }


  public void onStart() {
    // Print game started
    System.out.println("\n\nStarting Painting...\n");
  }

  // ToDo
  public void restart() {
  }


  public void onRestart() {
  }


  public void stop() {
    // Print game started
    System.out.println("\n\nStopping Painting...\n");

    isRunning = false;

    onStop();
  }


  public void onStop() {
    System.out.println("\n\nPainting stopped...\n");
  }


  public char getInstruction() {
    // Return 0 if insturctions finished
    if (instructionsPointer == instructions.length()) {
      stop();
      return 0;
    }

    // Move pointer to next
    return instructions.charAt(instructionsPointer++);
  }

  protected String getConfigurations() {
    String configs = "";
    int count = Constants.PAINTING_MODE_STEPS_COUNT; // 4 bytes

    // ToDo
    for (int i = 0; i < 4; i++) {
      char firstChar = (char) count;
      configs+=firstChar;
      count = count >> 8;
    }

    return configs;
  }

  private boolean parseInstructionsFile() {
    this.instructions = Utilities.getFileContents(Constants.PATH_PAINT_INSTRUCTIONS_FILE).trim();

    if (this.instructions.length() == 0) {
      System.err.println("Error :: Please make sure that instructions file exists\n");
      return false;
    }
    System.out.println("Log :: Instructions file read -> " + this.instructions);

    this.instructionsPointer = 0;

    return true;
  }

  private boolean executePaintingAlgorithm() {
    try
    {   
      Utilities.executeSystemCommand(new String[]{Constants.PATH_PAINTER});
    } 
    catch (Throwable t)
    {
      t.printStackTrace();
      return false;
    }
    System.out.println("Log :: Image processed and instructions file generated");
    return true;
  }
}