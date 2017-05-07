import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class PaintingTask extends CNCTask {

  String instructions;
  int instructionsPointer = 0;

  int rows=0, cols=0;

  public void start() throws Exception {
    // Utilities.captureCameraShot();

    // Execute flow solver algorithm which generates instructions.txt file
    if (!this.executePaintingAlgorithm()) 
      throw new Exception("Painting algorithm failed to execute!");

    // Calculate grid block size in CMs, store in the file in the new units
    // Calculate path end grid point, store in variables to send later to flow solver
    if (!this.parseInstructionsFile()) 
      throw new Exception("Parsing instructions files failed!");


    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }


  public void onStart() {
    // Print game started
    System.out.println("Starting Painting...");
  }

  public void stop() {
    // Print game started
    System.out.println("Stopping Painting...");

    isRunning = false;

    onStop();
  }

  public void onStop() {
    System.out.println("\nPainting stopped...");
  }


  public char getInstruction() {
    // Return 0 if insturctions finished
    if (instructionsPointer == instructions.length()) {
      stop();
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

  protected MovePenTask getMovePenBackTask() {
    return new MovePenTask(rows, cols, Constants.PAINTING_MODE_STEPS_COUNT);
  }

  protected int getConfigurations() {
    return Constants.PAINTING_MODE_STEPS_COUNT; // 4 bytes
  }

  private boolean parseInstructionsFile() throws Exception {
    this.instructions = Utilities.getFileContents(Constants.PATH_PAINT_INSTRUCTIONS_FILE).trim();

    System.out.println("Log :: Instructions file read -> " + this.instructions);

    this.instructionsPointer = 0;

    return true;
  }

  private boolean executePaintingAlgorithm() {
    try
    {   
      String out = Utilities.executeSystemCommand(new String[]{
        Constants.PATH_PAINTER, 
        Constants.PATH_IMAGES_DIR + "painter.jpg",
        Constants.PATH_PAINT_INSTRUCTIONS_FILE,
        "1.0"
        });
        System.err.println(out);
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