package hsh.mplab.stcheckall;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import hsh.mplab.systemtools.BatteryTool;
import hsh.mplab.systemtools.ScreenTool;

public class MainActivity extends AppCompatActivity
{

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    initSbScreen();
    initTvBattery();
    initTvOptime();

    ScreenTool.init(this);
    BatteryTool.init(this);
  }

  // -------------------------------------------------------------------------
  // Button id = butScreen
  // -------------------------------------------------------------------------
  //
  boolean butScreenOn = false;

  public void onClickButScreen(View view)
  {
    Button button = (Button) view;

    if(butScreenOn)
    {
      butScreenOn = false;
      button.setText(getString(R.string.staysAlive));
      ScreenTool.keepScreenOn(true);
    }
    else
    {
      butScreenOn = true;
      button.setText(getString(R.string.isFading));
      ScreenTool.keepScreenOn(false);
    }
  }


  // -------------------------------------------------------------------------
  // SeekBar id = sbScreen
  // -------------------------------------------------------------------------
  //
  SeekBar sbScreen;

  public void initSbScreen()
  {
    sbScreen = findViewById(R.id.sbScreen);
    sbScreen.setOnSeekBarChangeListener(new sbListener());
  }

  class sbListener implements SeekBar.OnSeekBarChangeListener
  {
    @Override
    public void onProgressChanged(SeekBar sb, int progress, boolean fromUser)
    {
      int relMill = (1000 * progress) / sb.getMax();
      // * 1000 for getting higher resolution with integer values

      float rel = (float) relMill / 1000;
      ScreenTool.setBrightness(rel);
    }

    @Override
    public void onStartTrackingTouch(SeekBar sb)
    {

    }

    @Override
    public void onStopTrackingTouch(SeekBar sb)
    {

    }
  }

  // -------------------------------------------------------------------------
  // Button id = butBattery
  // -------------------------------------------------------------------------
  //
  boolean butBatteryOn = false;

  public void onClickButBattery(View view)
  {
    Button button = (Button) view;

    if (butBatteryOn)
    {
      butBatteryOn = false;
    } else
    {
      butBatteryOn = true;
    }

    String note = "Event Count = " + BatteryTool.updCounter +
                  "   Level = " + BatteryTool.level;
    tvBattery.setText(note);

    note = "Operation Time = " + BatteryTool.timeCountHour   + "h " +
                                 BatteryTool.timeCountMinute + "m " +
                                 BatteryTool.timeCountSecond + "s";
    tvOptime.setText(note);
  }

  // -------------------------------------------------------------------------
  // TextView id = tvBattery
  // -------------------------------------------------------------------------
  //
  TextView tvBattery;

  void initTvBattery()
  {
    tvBattery = findViewById(R.id.tvBattery);
  }

  // -------------------------------------------------------------------------
  // TextView id = tvOptime
  // -------------------------------------------------------------------------
  //
  TextView tvOptime;

  void initTvOptime()
  {
    tvOptime = findViewById(R.id.tvOptime);
  }



}
