package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.ContentResolver;
import android.provider.Settings.System;
import android.view.Window;
import android.view.WindowManager;

// This screen tools are foreseen only for the main activity.
// Therefore methods and variables are static
// and resources only prepared once with method init()

public final class ScreenTool
{
  static ContentResolver  cr;
  static Window           win;
  static Activity         act;
  static boolean          isInit;

  public static void init(Activity activity)
  {
    act     = activity;
    cr      = activity.getContentResolver();
    win     = activity.getWindow();
    isInit  = true;
  }

  public static boolean setBrightness(float brightness)
  {
    boolean retv;

    if(isInit == false) return false;

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

  public static boolean keepScreenOn(boolean onOff)
  {
    if(isInit == false) return false;

    if(onOff == true)
      win.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    else
      win.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

    return true;
  }
}
