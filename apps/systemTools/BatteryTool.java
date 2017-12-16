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

  public static long    timeCountMillis;
  public static long    timeCountSecond;
  public static long    timeCountMinute;
  public static long    timeCountHour;
  public static long    deltaCheckTime = 1000000L;
  public static long    restTimeMinutes;
  public static long    restTimeHours;
  public static int     restCheckLevel = 10;

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
    long  timeMillis;

    @Override
    public void onReceive(Context context, Intent intent)
    {
      updCounter++;
      timeMillis = System.currentTimeMillis();
      if(updCounter == 1)
        startTimeMillis = countTimeDiff = timeMillis;

      if(updCounter > 1)
      {
        long diffMillis = timeMillis - startTimeMillis;
        timeCountMillis = diffMillis % 1000;
        diffMillis /= 1000;
        timeCountSecond = diffMillis % 60;
        diffMillis /= 60;
        timeCountMinute = diffMillis % 60;
        diffMillis /= 60;
        timeCountHour = diffMillis % 60;
      }
      oldTimeMillis = timeMillis;

      batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL,0);
      scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE,100);
      oldLevel = level;

      if(scale != 0)
        level = (100 * batteryLevel) / scale;
      else
        level = batteryLevel;

      if(updCounter == 1)
        startLevel = countLevelDiff = level;

      plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED,1);
      present = intent.getBooleanExtra(BatteryManager.EXTRA_PRESENT,false);
      temperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE,0);
      int batVolt = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE,0);
      if(batVolt > 100)
        voltage = batVolt / 1000F;
      else
        voltage = batVolt;

      long diffTime = timeMillis - countTimeDiff;
      if(diffTime >= deltaCheckTime)
      {
        int diffLevel = level - countLevelDiff;
        int restLevel = level - restCheckLevel;
        long restMillis = restLevel * diffTime / diffLevel;
        long restMinutes = restMillis / 60000L;
        restTimeMinutes = restMinutes % 60;
        restTimeHours = restMinutes / 60;
      }


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
