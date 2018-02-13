// ---------------------------------------------------------------------------
// This module is for checking the battery of an Android device.
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      15.12.2017
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//
// This battery tools are foreseen only for the main activity.
// Therefore methods and variables are static and resources only prepared once
// with method init()


// 15.12.17   Robert Patzke
// This is the first draft of the BatteryTool and there are some experiments
// done which may confuse users.

package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;

import java.util.Date;

// This battery tools are foreseen only for the main activity.
// Therefore methods and variables are static
// and resources only prepared once with method init()

public final class BatteryTool
{
  static Activity       act;
  static boolean        isInit;
  static int            oldLevel;
  static int            startLevel;
  static int            countLevelDiff;
  static long           oldTimeMillis;
  static long           startTimeMillis;
  static long           countTimeDiff;
  static long           diffMillis;

  static int            diffLevel;
  static int            restLevel;

  public static long    timeCountMillis;
  public static long    timeCountSecond;
  public static long    timeCountMinute;
  public static long    timeCountHour;
  public static int     restTimeMinutes = -1;
  public static int     restTimeHours = -1;
  public static int     restCheckLevel = 10;
  public static int     deltaCheckLevel = 1;

  public static int     updCounter;

  public static int     level;
  public static int     plugged;
  public static boolean present;
  public static int     temperature;
  public static float   voltage;

  static class BatteryReceiver extends BroadcastReceiver
  {
    int   batteryLevel;
    int   scale;
    long  timeMillis, restMillis, restMinutes;

    @Override
    public void onReceive(Context context, Intent intent)
    {
      updCounter++;
      timeMillis = System.currentTimeMillis();
      if(updCounter == 1)
        startTimeMillis = countTimeDiff = timeMillis;

      if(updCounter > 1)
      {
        diffMillis = timeMillis - startTimeMillis;
        timeCountMillis = diffMillis % 1000;
        diffMillis /= 1000;
        timeCountSecond = diffMillis % 60;
        diffMillis /= 60;
        timeCountMinute = diffMillis % 60;
        diffMillis /= 60;
        timeCountHour = diffMillis % 60;
      }

      batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL,0);
      scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE,100);

      if(scale != 0)
        level = (100 * batteryLevel) / scale;
      else
        level = batteryLevel;

      if(updCounter == 1)
      {
        startLevel = countLevelDiff = oldLevel = level;
        oldTimeMillis = timeMillis;
      }
      else
      {
        diffLevel = oldLevel - level;
        if(diffLevel >= deltaCheckLevel)
        {
          restLevel = level - restCheckLevel;
          diffMillis = timeMillis - oldTimeMillis;
          restMillis = restLevel * diffMillis / diffLevel;
          restMinutes = restMillis / 60000L;
          restTimeMinutes = (int) (restMinutes % 60);
          restTimeHours = (int) (restMinutes / 60);
          oldLevel = level;
          oldTimeMillis = timeMillis;
        }
      }

      plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED,-1);
      present = intent.getBooleanExtra(BatteryManager.EXTRA_PRESENT,false);
      temperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE,0);
      int batVolt = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE,0);
      if(batVolt > 100)
        voltage = batVolt / 1000F;
      else
        voltage = batVolt;

    }

  }

  static public void init(Activity parent)
  {
    act = parent;
    BatteryReceiver batReceiver = new BatteryReceiver();
    parent.registerReceiver(batReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    isInit = true;
  }

}
