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

  public static void captureScreenShot() throws Exception {
    try {
      // Execute adb command to capture screen and save on device memory
      executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "shell", "screencap", "-p", "/sdcard/" + Constants.FILE_NAME_FLOW_IMAGE
      });

      // Execute adb command to pull the screenshot
      executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "pull", "/sdcard/" + Constants.FILE_NAME_FLOW_IMAGE, Constants.PATH_IMAGES_DIR
      });

      // Execute adb command to remove the screenshot from device memory
      executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "shell", "rm", "/sdcard/" + Constants.FILE_NAME_FLOW_IMAGE
      });
    } 
    catch (Exception e) {
      throw new Exception ("Screenshot cannot be taken!");
    }
  }

  public static void captureCameraShot() throws Exception {
    try {
      // Execute adb command to capture camera shot
      executeSystemCommand(new String[]{Constants.PATH_ADB, 
        "shell", "am", "start", "-a", "android.media.action.STILL_IMAGE_CAMERA", "&& sleep 1", 
        "&& input keyevent KEYCODE_FOCUS", "&& sleep 2", "&& input keyevent 27", "&& sleep 4", 
        "&& cd /storage/3D13-1817/DCIM/Camera/ && IFS=$'\n' && output=(`ls -l`) && lines=${#output[@]} &&  file=${output[$((lines-1))]: -23} && cp $file /sdcard/painter_img.jpg"
      });

      // Execute adb command to pull camera shot
      executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "pull", "/sdcard/" + Constants.FILE_NAME_PAINTER_IMAGE, Constants.PATH_IMAGES_DIR
      });

      // Execute adb command to remove the picture from device memory
      executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "shell", "rm", "/sdcard/" + Constants.FILE_NAME_PAINTER_IMAGE
      });
    } 
    catch (Exception e) {
      throw new Exception ("Screenshot cannot be taken!");
    }
  }
  
  public static String getScreenSize() throws Exception {
    // Execute adb command to get SCREEN DPI
    String screenDPI = executeSystemCommand(new String[]{
      Constants.PATH_ADB, 
      "shell", "dumpsys", "display", "|", "grep", "mBaseDisplayInfo"
    });

    // Check if empty output
    if (screenDPI.trim().length() == 0) {
      throw new Exception("Couldn't get screen specs!");
    }

    // Format output and attach to output
    int dpiIdx = screenDPI.indexOf("dpi");

    // Get comma idx
    int separatorIdx = 0;
    for (int i = dpiIdx; i >= 0; --i) {
      if (screenDPI.charAt(i) == ',' || screenDPI.charAt(i) == '(') {
        separatorIdx = i;
        break;
      }
    }

    screenDPI = screenDPI.substring(separatorIdx + 1, dpiIdx - 1)
    .replace(")", "")
    .replace("(", "")
    .replace(" ", "")
    .replace("x", " ");

    return screenDPI;
  }

  public static String getFileContents(String path) throws Exception {
    StringBuffer output = new StringBuffer();

    try {
      BufferedReader reader = new BufferedReader(new FileReader(path));
      String line;

      while ((line = reader.readLine()) != null) {
        output.append(line);
      }

      reader.close();
      return output.toString();
    }
    catch (Exception e) {
      throw new Exception ("Instructions file cannot be opened!");
    }
  }

  public static void deleteFile(String path) {
    File f = new File(path);

    if (f.exists()) {
      f.delete();
    }
  }

  public static boolean checkPhoneConnection() {
    try {      
      String checkPhoneConnectionOutput = Utilities.executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "devices"
      });

      return !checkPhoneConnectionOutput.trim().equals("List of devices attached");
    } 
    catch(Exception e) {
      return false;
    }
  }

  public static boolean checkAndroidActivity(String activity) {
    try{
      String checkGameActivityOutput = Utilities.executeSystemCommand(new String[]{
        Constants.PATH_ADB, 
        "shell", "dumpsys", "window", "windows", "|", 
        "grep", "-E", "'mCurrentFocus|mFocusedApp'"
      });

      return (checkGameActivityOutput.indexOf(activity) != -1);
    }
    catch (Exception e) {
      return false;
    }
  }
}