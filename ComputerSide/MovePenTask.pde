import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class MovePenTask extends CNCTask {
  int motorStepsCount;
  int rows, cols;

  boolean isReleaseInstructionSent = false;

  public MovePenTask(int rows, int cols, int steps) {
    this.cols = cols;
    this.rows = rows;   

    this.motorStepsCount = steps;

    this.instructionDone = true;
  }

  public void start() {

    port.clear();
    
    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }

  public void onStart() {
    // Print game started
    System.out.println("Moving pen back...");
  }

  public void stop() {
    // Print game started
    System.out.println("Moving pen stopped...");

    isRunning = false;

    onStop();
  }

  protected int getConfigurations() {
    return this.motorStepsCount; // 4 bytes
  }

  public void onStop() {
    System.out.println("Moving pen stopped...");
  }

  public char getInstruction() {

    if (!isReleaseInstructionSent) { 
      isReleaseInstructionSent = true; 
      return 'R';
    } else if (this.cols < 0) {
      this.cols++;
      return '>';
    } else if (this.cols > 0) {
      this.cols--;
      return '<';
    } else if (this.rows < 0) {
      this.rows++;
      return 'v';
    } else if (this.rows > 0) {
      this.rows--;
      return '^';
    }
    return 0;
  }
}