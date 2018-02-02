package hsh.mplab.smntestclient;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

/**
 * Simple file access for single lines of text
 */

public class LineFile
{
  static public String getString(String fileName, String emptyReturn)
  {
    String      retv = null;
    FileReader  reader = null;

            File file = new File(fileName);
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
}
