import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class CalibrationTask extends CNCTask {
  int currentKey = -1;
  long lastKeyTimestamp = 0;

  public void start() {
    // Send configurations
    sendConfigurations();

    // Set flag
    isRunning = true;

    // Call onStart
    onStart();
  }


  public void onStart() {
    // Print game started
    System.out.println("Starting Cailbration, use arrows to move robot...");
  }

  public void stop() {
    // Print game started
    System.out.println("Calibraion stopped...");

    isRunning = false;

    onStop();
  }


  public void onStop() {
    System.out.println("Calibraion stopped...");
  }

  public void setKeyStatus(char k, boolean status) {
    if (status && System.currentTimeMillis() - lastKeyTimestamp > 1) { 
      lastKeyTimestamp = System.currentTimeMillis();

      currentKey = k;

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
    } else {
      currentKey = -1;
    }
  }
  public char getInstruction() {
    if (currentKey != -1) return (char) currentKey;
    return 0;
  }

  protected int getConfigurations() {
    return Constants.CALIBRATION_MODE_STEPS_COUNT; // 4 bytes
  }
}