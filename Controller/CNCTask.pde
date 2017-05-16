public class CNCTask {
  protected int motorStepsCount = 0;
  protected boolean acknowledgementReceived = true;
  protected boolean isTerminating = false;
  protected long lastCheckTimeStamp = 0; 
  protected CNCListener cncListener = null;
  protected ArrayList<Integer> errorsList = new ArrayList<Integer>();

  public void start() {
    try {
      scanEnvironment(false);
      
      if (errorsList.size() > 0) {
        throw new Exception();
      }

      setupTask();
    }
    catch (Exception e) {
      if (cncListener != null) {
        cncListener.onError(e.getMessage());
      }
    }
  }

  protected void setupTask() throws Exception {
  }

  public void restart()  throws Exception {
  }

  public void stop() {
    isTerminating = true;
  }

  public void execute() {
    receiveFeedback();
    check();

    if (errorsList.size() > 0 || !acknowledgementReceived) {
      return;
    }

    if (isTerminating) {
      if (cncListener != null) {
        cncListener.onStop();
      }
      return;
    }

    try {
      executeInstruction();
    }
    catch (Exception e) {
      if (cncListener != null) {
        cncListener.onError(e.getMessage());
      }
    }
  }

  protected void executeInstruction() throws Exception {
  }

  protected void receiveFeedback() {
    if (port.available() <= 0) return;

    int signal = port.read();

    if (signal == Constants.SERIAL_ACKNOWLEDGMENT) {
      acknowledgementReceived = true;
      return;
    } 

    handleFeedback(signal);
  }  

  protected void handleFeedback(int signal) {
    switch (signal) {
      case Constants.SERIAL_POWER_SUPPLY_ERROR:
      errorHandler(
        Constants.ERROR_POWER_LOST, 
        Constants.MSGS_ERRORS[Constants.ERROR_POWER_LOST], 
        false,
        true
      );
      break;

      case Constants.SERIAL_POWER_SUPPLY_ERROR_FIXED:
      errorHandler(
        Constants.ERROR_POWER_LOST, 
        Constants.MSGS_FIXED_ERRORS[Constants.ERROR_POWER_LOST], 
        true,
        true
      );
      break;
    }
  }

  protected void check() {
    long currentTime = System.currentTimeMillis();
    
    if (currentTime - lastCheckTimeStamp > 100) {
      scanEnvironment(true);
      lastCheckTimeStamp = currentTime;
    }
  }

  protected void scanEnvironment(boolean sendSignal) {
    boolean arduinoConnected = Arrays.asList(Serial.list()).contains(arduinoPortName);

    if (errorsList.contains(Constants.ERROR_ARDUINO_DISCONNECTION) != arduinoConnected)
      return;

    // Arduino is connected
    if (arduinoConnected) {
      System.out.println(Constants.MSGS_FIXED_ERRORS[Constants.ERROR_ARDUINO_DISCONNECTION]);
      errorsList.remove(Constants.ERROR_ARDUINO_DISCONNECTION);

      if (cncListener != null) {
        cncListener.onArduinoConnected();
      }

      port.write(Constants.SERIAL_CHECK_INTERRUPTS);

      // Assuming that the last instruction finished successfully
      acknowledgementReceived = true;
    }
    // Arduino disconnected!
    else {
      System.err.println(Constants.MSGS_ERRORS[Constants.ERROR_ARDUINO_DISCONNECTION]);
      errorsList.add(Constants.ERROR_ARDUINO_DISCONNECTION);
    }
  }

  protected void errorHandler(Integer code, String message, boolean isSolved, boolean sendSignal) {
    if (errorsList.contains(code) == isSolved) {
      if (isSolved) {
        System.out.println(message);
        errorsList.remove(code);
      } 
      else {
        System.err.println(message);
        errorsList.add(code);
      }

      if (!sendSignal) 
        return;

      if (errorsList.size() == 0 && isSolved) {
        port.write(Constants.SERIAL_CONTINUE_SIGNAL);
      } 
      else if (errorsList.size() == 1 && !isSolved) {
        port.write(Constants.SERIAL_STOP_SIGNAL);
      }
    }
  }

  public void setListener(CNCListener cncListener) {
    this.cncListener = cncListener;
  }

  public void setKeyStatus(char k, boolean status) {
  }

  public MovePenTask getMovePenBackTask() {
    return null;
  }

  public void sendConfigurations(char key, int value) {
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