import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class CalibrationTask extends CNCTask {
  int currentKey = -1;
  long lastKeyTimestamp = 0;

  protected void initTask() {
    // Send configurations
    sendConfigurations(Constants.SERIAL_MOTOR_STEPS_COUNT, Constants.CALIBRATION_MODE_STEPS_COUNT);

    System.out.println("Starting Cailbration, use arrows to move robot...");
  }

  public void stop() {
    super.stop();
    
    // Print game started
    System.out.println("Calibraion stopped...");
  }

  public void setKeyStatus(char k, boolean status) {
    if (!status || System.currentTimeMillis() - lastKeyTimestamp <= 1) {
      currentKey = -1;
      return;
    }

    currentKey = Character.toUpperCase(k);

    if (k == CODED) {
      if (keyCode == RIGHT)
        currentKey = '>';        
      else if (keyCode == LEFT)
        currentKey = '<';        
      else if (keyCode == UP)
        currentKey = 'v';
      else if (keyCode == DOWN)
        currentKey = '^';
    }
    
    lastKeyTimestamp = System.currentTimeMillis();
  }

  protected void executeInstruction() {
    if (currentKey == -1) return;
    
    char instructon = (char) currentKey;

    if (!Constants.ALLOWED_CALIBRATION_INSTRUCTIONS.contains(instructon)) return;

    sendInstruction(instructon);
  }
}