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
  }

  public void start() {    
    // Send configurations
    sendConfigurations(Constants.SERIAL_MOTOR_STEPS_COUNT, this.motorStepsCount);

    // Call onStart
    System.out.println("Moving pen back...");
  }

  public void stop() {
    super.stop();

    // Print game started
    System.out.println("Moving pen stopped...");
  }

  protected void executeInstruction() throws Exception {
    char instruction = 0;
    
    if (!isReleaseInstructionSent) { 
      isReleaseInstructionSent = true; 
      instruction = 'R';
    } else if (this.cols < 0) {
      this.cols++;
      instruction = '>';
    } else if (this.cols > 0) {
      this.cols--;
      instruction = '<';
    } else if (this.rows < 0) {
      this.rows++;
      instruction = 'v';
    } else if (this.rows > 0) {
      this.rows--;
      instruction = '^';
    } 

    sendInstruction(instruction);

    if (Math.abs(this.cols) + Math.abs(this.rows) == 0) {
      stop();
    }
  }
}