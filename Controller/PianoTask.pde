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

    currentKey = Character.toUpperCase(k);

    // Replace some keys (like S, >, <, ^, v, B, E, D) to avoid sending commands to motors
    if (k == 'S') 
      currentKey = 35;        
    else if (k == '>')
      currentKey = 14;        
    else if (k == '<')
      currentKey = 12;        
    else if (k == '^')
      currentKey = 46;          
    else if (k == 'v')
      currentKey = 22;             
    else if (k == 'B')
      currentKey = 18;         
    else if (k == 'E')
      currentKey = 21;        
    else if (k == 'D')
      currentKey = 20;     
    else if (k == 'P')
      currentKey = 32;        
    else if (k == 'R')
      currentKey = 34;

    lastKeyTimestamp = System.currentTimeMillis();
  }

  protected void executeInstruction() {
    if (currentKey == -1) return;
    
    char instructon = (char) currentKey;

    // if (Constants.ALLOWED_INSTRUCTIONS.contains(instructon) == -1) return;

    sendInstruction(instructon);
  }
}