package hsh.mplab.smntestclient;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * Simple file access for single lines of text
 */

public class LineFile
{
  static String filesDir;

  static public void setFilesDir(String fDir)
  {
    filesDir = fDir + "/";
  }

  static public String getString(String fileName, String emptyReturn)
  {
    String      retv = null;
    FileReader  reader = null;

    File file = new File(filesDir + fileName);
    if(!file.exists())
      return(emptyReturn);

    try
    {
      reader = new FileReader(file);
      int nrEl = (int) file.length();
      char[] chrArr = new char[nrEl];
      reader.read(chrArr);
      reader.close();
      retv = new String(chrArr);
    }
    catch (IOException exc)
    {
      retv = exc.getMessage();
    }

    return(retv);
  }

  static public String putString(String fileName, String inStr)
  {
    try
    {
      PrintWriter wrFile = new PrintWriter(filesDir + fileName, "UTF-8");
      wrFile.println(inStr);
      wrFile.close();
      return(null);
    }
    catch (Exception exc)
    {
      return(exc.getMessage());
    }
  }

}
