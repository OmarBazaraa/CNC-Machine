public class CNCTask {
  protected boolean instructionDone = true;
  protected boolean isRunning = false;

  public CNCTask() {
    // Flush serial buffer
    port.clear();
  }

  public void start() throws Exception {
  }

  public void onStart() {
  }

  public void restart()  throws Exception {
  }

  public void onRestart() {
  }

  public void stop() {
  }

  public char getInstruction() throws Exception {
    return 0;
  }

  protected int getConfigurations() {
    return 0;
  }

  public void setKeyStatus(char k, boolean status) {
  }

  protected MovePenTask getMovePenBackTask() {
    return null;
  }

  public void executeInstruction() throws Exception {
    // Check if can send
    if (!receiveAcknowledge()) return;

    // Get instruction (which increases the instruction pointer)
    char instruction = getInstruction();

    // Check if instructions not finished
    if (instruction != 0) {

      if (!Constants.ALLOWED_INSTRUCTIONS.contains(instruction)) return;

      System.out.println("Executing instruction " + instruction + " ...");

      instructionDone = false;

      sendInstruction(instruction);
    }
  }

  protected void sendConfigurations() {

    System.out.println("Sending Configs...");

    int configs = getConfigurations();

    port.write(Constants.SERIAL_CONFIGURATIONS);
    for (int i = 0; i < 4; ++i) {
      port.write((char) configs);
      configs >>= 8;
    }
    
    System.out.println("Configs Sent...");
  }

  public void sendInstruction(char instruction) {

    System.out.println("Sending instruction " + instruction + " ...");

    // Send the instruction
    port.write(instruction);
  }

  public boolean receiveAcknowledge() {

    if (instructionDone) return true;

    if (port.available() <= 0) return false;

    int x = port.read();

    // Get from serial
    if (x != Constants.SERIAL_INSTUCTION_DONE) return false;      

    // set instructionDone
    instructionDone = true;

    // return 
    return true;
  }

  public boolean isRunning() {
    return isRunning;
  }
}