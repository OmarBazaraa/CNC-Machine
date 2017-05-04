public abstract class CNCTask {
  private boolean instructionDone = true;
  protected boolean isRunning = false;

  public abstract void start();
  public abstract void onStart();
  public abstract void restart();
  public abstract void onRestart();
  public abstract void stop();
  public abstract char getInstruction();
  
  public void setKeyStatus(char k, boolean status) {
    
  }

  protected abstract String getConfigurations();


  protected void sendConfigurations() {
    System.out.println("Sending Configs...");
    String configs = getConfigurations();

    // Send configurations signal
    port.write(Constants.SERIAL_CONFIGURATIONS);

    // Send configs
    for (int i = 0; i < configs.length(); i++) {
      port.write(configs.charAt(i));
    }
    System.out.println("Configs Sent...");
  }
  
  public void executeInstruction() {
    // Check if can send
    if (!receiveAcknowledge()) return;

    // Get instruction (which increases the instruction pointer)
    char instruction = getInstruction();

    // Check if instructions not finished
    if (instruction != 0) {
      System.out.println("Executing instruction " + instruction + " ...");

      sendInstruction(instruction);
      instructionDone = false;
    }
  }

  public void sendInstruction(char instruction) {
    System.out.println("Sending instruction " + instruction + " ...");

    // Send the instruction
    port.write(instruction);
  }

  public boolean receiveAcknowledge() {
    if (instructionDone) return true;

    // Get from serial
    if (port.read() != Constants.SERIAL_INSTUCTION_DONE) return false;      

    // set instructionDone
    instructionDone = true;

    // return 
    return true;
  }

  public boolean isRunning() {
    return isRunning;
  }
}