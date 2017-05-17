import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class PianoTask extends CNCTask {
  int currentKey = -1;
  long lastKeyTimestamp = 0;

  public void start() {
    this.motorStepsCount = 0;

    // Send configurations
    sendConfigurations(Constants.SERIAL_MOTOR_STEPS_COUNT, this.motorStepsCount);

    System.out.println("Starting Piano, use keyboard to play the piano...");
  }

  public void stop() {
    super.stop();

    // Print game started
    System.out.println("Piano mode stopped...");
  }

  public void setKeyStatus(char k, boolean status) {
    if (!status || System.currentTimeMillis() - lastKeyTimestamp <= 1) {
      currentKey = -1;
      return;
    }

    if (k >= 'a' && k <= 'z') {
      currentKey = k;
    }

    lastKeyTimestamp = System.currentTimeMillis();
  }

  protected void executeInstruction() {
    if (currentKey == -1) return;
    
    char instructon = (char) currentKey;

    sendInstruction(instructon);
  }
}