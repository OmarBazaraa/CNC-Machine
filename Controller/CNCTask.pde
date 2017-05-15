public class CNCTask {
  protected boolean acknowledgementReceived = true;
  protected boolean isTerminating = false;
  protected boolean checksFailed = false;
  protected long lastCheckTimeStamp = 0; 
  protected CNCListener cncListener = null;
  protected ArrayList<Integer> errorsList = new ArrayList<Integer>();

  public void start() throws Exception {
  }

  public void restart()  throws Exception {
  }

  public void stop() {
    isTerminating = true;
  }

  public void setListener(CNCListener cncListener) {
    this.cncListener = cncListener;
  }

  public void setKeyStatus(char k, boolean status) {
  }

  protected MovePenTask getMovePenBackTask() {
    return null;
  }

  protected void interruptListener(String message, int action) {
    if (action == Constants.INTURREPT_ACTION_STOP) {
      System.err.println(message);
      sendInstruction(Constants.SERIAL_STOP_SIGNAL);
    } else if (action == Constants.INTURREPT_ACTION_CONTINUE) {
      System.out.println(message);
      sendInstruction(Constants.SERIAL_CONTINUE_SIGNAL);
    }
  }

  protected void errorHandler(Integer code, String message, boolean isSolved) {
    // if (isSolved) {
    //   if (errorsList.contains(code)) {
    //     System.out.println(message);
    //     errorsList.remove(code);

    //     // Send continue signal if and only if all errors are handled
    //     if (errorsList.size() == 0) 
    //       sendInstruction(Constants.SERIAL_CONTINUE_SIGNAL);
    //   }
    // }
    // else {
    //   if (!errorsList.contains(code)) {
    //     System.err.println(message);

    //     if (errorsList.size() == 0) 
    //       sendInstruction(Constants.SERIAL_STOP_SIGNAL);

    //     errorsList.add(code);
    //   }
    // }


    if (errorsList.contains(code) == isSolved) {
      System.out.println(message);

      if (isSolved)
        errorsList.remove(code);
      else
        errorsList.add(code);

      if (errorsList.size() == 0 && isSolved) {
        sendInstruction(Constants.SERIAL_CONTINUE_SIGNAL);
      }
      else if (errorsList.size() == 1 && !isSolved) {
        sendInstruction(Constants.SERIAL_STOP_SIGNAL);
      }
    }
  }
  
  protected void handleFeedback(int signal) {
    switch (signal) {
      case Constants.SERIAL_POWER_SUPPLY_ERROR:
      errorHandler(Constants.ERROR_POWER_LOST, "We lost power :(", false);
      break;

      case Constants.SERIAL_POWER_SUPPLY_ERROR_FIXED:
      errorHandler(Constants.ERROR_POWER_LOST, "Power is back :D", true);
      break;
    }
  }

  protected void receiveFeedback() {
    if (port.available() <= 0) return;

    int signal = port.read();

    if (signal == Constants.SERIAL_ACKNOWLEDGMENT) {
      acknowledgementReceived = true;
      return;
    } 

    // handleFeedback(signal);
  }  

  protected void check() {
    if (System.currentTimeMillis() - lastCheckTimeStamp > 100) {
      scanEnvironment();
    }
  }

  protected void scanEnvironment() {
    // Arduino is connected
    if (Arrays.asList(Serial.list()).contains(arduinoPortName)) {
      errorHandler(Constants.ERROR_ARDUINO_DISCONNECTION, "Arduino is back :D", true);
    } 
    // Arduino disconnected!
    else {
      errorHandler(Constants.ERROR_ARDUINO_DISCONNECTION, "Please re-connect the Arduino cable!", false);
    }
  }

  public void execute() throws Exception {
    receiveFeedback();
    check();
    
    if (errorsList.size() > 0 || !acknowledgementReceived) {
      return;
    }
    
    if (isTerminating) {
      if (cncListener != null) {
        cncListener.onStop();
      }
    } 
    else {
      executeInstruction();
    }
  }

  protected void executeInstruction() throws Exception {
  }

  protected void sendConfigurations(char key, int value) {
    System.out.println("Sending Configs...");

    sendInstruction(key);
    sendInstruction(value);

    System.out.println("Configs Sent...");
  }

  public void sendInstruction(char instruction) {
    System.out.println("Sending byte " + instruction + " ...");

    // Send the instruction
    port.write(instruction);

    acknowledgementReceived = false;
  }

  public void sendInstruction(int instruction) {
    System.out.println("Sending instruction " + instruction + " ...");

    // Send the instruction
    for (int i = 0; i < 4; ++i) {
      port.write((char) instruction);
      instruction >>= 8;
    }

    acknowledgementReceived = false;
  }
}