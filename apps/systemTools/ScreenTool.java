// ---------------------------------------------------------------------------
// This module is for controlling the screen of an Android device.
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      15.12.2017
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//
// This screen tools are foreseen only for the main activity.
// Therefore methods and variables are static and resources only prepared once
// with method init()

package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.ContentResolver;
import android.provider.Settings.System;
import android.view.Window;
import android.view.WindowManager;

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
      // Since Android 6 there are two extra actions necessary to pass the setting without
      // exception:
      // 1. Set permission in manifest with
      // <uses-permission android:name="android.permission.WRITE_SETTINGS"/>
      // 2. Ask the user of your app to allow manipulating settings
      // We avoided 2. by setting target version to a lower API in build.gradle
      // (Module Settings for app, that is click app and press F4 in Android Studio)
      // but in the near future it should be implemented here.
      //
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
