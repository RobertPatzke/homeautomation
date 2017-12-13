package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;

// This battery tools are foreseen only for the main activity.
// Therefore methods and variables are static
// and resources only prepared once with method init()

public final class BatteryTool
{
  static Activity     act;
  static boolean      isInit;
  static int          oldLevel;

  public static int   level;
  public static int   updCounter;

  static BroadcastReceiver batteryReceiver = new BroadcastReceiver()
  {
    int batteryLevel;

    @Override
    public void onReceive(Context context, Intent intent)
    {
      updCounter++;
      batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL,0);
      oldLevel = level;
      level = batteryLevel;
    }
  };

  static public void init(Activity parent)
  {
    act = parent;
    parent.registerReceiver(batteryReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    isInit = true;
  }

}
