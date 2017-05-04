import java.io.*;
import java.lang.Runtime;
import java.util.*;

static class Utilities {

  // Execute given ADB command
  public static String executeSystemCommand (String[] command) throws IOException, InterruptedException {

    StringBuffer output = new StringBuffer();

    // Make command to get SCREEN PIXELS
    ProcessBuilder processBuilder = new ProcessBuilder(command);

    // Run process
    Process proc = processBuilder.start();

    // Print output
    proc.waitFor();

    // Get output
    BufferedReader reader = new BufferedReader(new InputStreamReader(proc.getInputStream()));
    String line = "";
    while ((line = reader.readLine())!= null) {
      output.append(line);
    }
    return output.toString();
  }


  public static boolean captureScreenShot() {
    try
    {    
      // Execute adb command to capture screen and save on device memory
      executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "shell", "screencap", "-p", "/sdcard/game.png"});

      // Execute adb command to pull the screenshot
      executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "pull", "/sdcard/game.png", Constants.PATH_SCRIPTS_DIR});

      // Execute adb command to remove the screenshot from device memory
      executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "shell", "rm", "/sdcard/game.png"});

      // =======================================================
    } 
    catch (Throwable t)
    {
      t.printStackTrace();
      return false;
    }

    return true;
  }

  public static String getScreenSize() {
    String output = "";

    try
    {    
      // Execute adb command to get SCREEN PIXELS
      String screenPixelsSize = executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "shell", "wm", "size"});

      // Check if empty output
      if (screenPixelsSize.trim().length() == 0)
        return "";

      // Format output and attach to output
      output += screenPixelsSize.toString().replace("Physical size: ", "").replace("x", " ");

      // =======================================================

      // Execute adb command to get SCREEN DPI
      String screenDPI = executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "shell", "dumpsys", "display", "|", "grep", "mBaseDisplayInfo"});

      // Check if empty output
      if (screenDPI.trim().length() == 0)
        return "";

      // Format output and attach to output
      screenDPI = screenDPI.substring(screenDPI.indexOf("(") + 1, screenDPI.indexOf(")")).replace(" x ", " ");
      output += (" " + screenDPI);

      // =======================================================
    } 
    catch (Throwable t)
    {
      t.printStackTrace();
    }
    return output.toString();
  }

  public static String getFileContents(String path) {
    StringBuffer output = new StringBuffer();
    try
    {
      BufferedReader reader = new BufferedReader(new FileReader(path));
      String line;
      while ((line = reader.readLine()) != null)
      {
        output.append(line);
      }
      reader.close();
      return output.toString();
    }
    catch (Exception e)
    {
      System.err.format("Exception occurred trying to read '%s'.", path);
      e.printStackTrace();
      return null;
    }
  }
  public static void deleteFile(String path) {
    File f = new File(path);
    if (f.exists()) {
      f.delete();
    }
  }
}