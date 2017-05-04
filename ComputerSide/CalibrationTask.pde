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
    System.out.println("\n\nStarting Cailbration, use arrows to move robot...\n");
  }


  public void restart() {
  }


  public void onRestart() {
  }


  public void stop() {
    // Print game started
    System.out.println("\n\nCalibraion stopped...\n");

    isRunning = false;

    onStop();
  }


  public void onStop() {
    System.out.println("\n\nCalibraion stopped...\n");
  }

  public void setKeyStatus(char k, boolean status) {
    if (status && System.currentTimeMillis() - lastKeyTimestamp > 1) { 
      lastKeyTimestamp = System.currentTimeMillis();

      if (k == CODED) {
        if (keyCode == RIGHT)
          currentKey = '>';        
        else if (keyCode == LEFT)
          currentKey = '<';        
        else if (keyCode == UP)
          currentKey = 'v';
        else if (keyCode == DOWN)
          currentKey = '^';
      } else currentKey = k;
    } else currentKey = -1;
     
  }
  public char getInstruction() {
    if ( currentKey != -1) return (char) currentKey;
    return 0;
  }

  protected String getConfigurations() {
    String configs = "";
    int count = Constants.CALIBRATION_MODE_STEPS_COUNT; // 4 bytes

    for (int i = 0; i < 4; i++) {
      char firstChar = (char) count;
      configs+=firstChar;
      count = count >> 8;
    }

    return configs;
  }
}