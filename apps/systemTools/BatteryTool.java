package hsh.mplab.systemtools;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;

/**
 * Created by Robert Patzke on 11.12.17.
 */

public class BatteryTool
{
  static Activity activity;

  static BroadcastReceiver batteryReceiver = new BroadcastReceiver()
  {
    int batteryLevel;

    @Override
    public void onReceive(Context context, Intent intent)
    {
      batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL,0);
    }
  };

  static void init(Activity parent)
  {
    activity = parent;
  }

}
