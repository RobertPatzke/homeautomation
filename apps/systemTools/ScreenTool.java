package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.ContentResolver;
import android.provider.Settings.System;
import android.view.Window;
import android.view.WindowManager;

/**
 * Created by Robert Patzke on 11.12.17.
 */

public class ScreenTool
{
  public static boolean setBrightness(float brightness, Activity activity)
  {
    boolean retv;

    ContentResolver cr = activity.getContentResolver();
    Window win = activity.getWindow();

    try
    {
      int bnMode = System.getInt(cr, System.SCREEN_BRIGHTNESS_MODE);
      if(bnMode == System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC)
        System.putInt(cr, System.SCREEN_BRIGHTNESS_MODE, System.SCREEN_BRIGHTNESS_MODE_MANUAL);
      retv = true;
    }
    catch (Exception exc)
    {
      retv = false;
    }

    if(retv == true)
    {
      WindowManager.LayoutParams lp = win.getAttributes();
      if(brightness < 0F) brightness = 0F;
      if(brightness > 1.0F) brightness = 1.0F;
      lp.screenBrightness = brightness;
      win.setAttributes(lp);
    }

    return retv;
  }
}
