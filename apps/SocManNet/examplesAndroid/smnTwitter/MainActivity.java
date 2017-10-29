package hsh.mplab.smntwitter;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

import hsh.mplab.socmannet.*;

public class MainActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    smnInit();
  }

  Twitter twitter;
  // This is the reference to the twitter which is cyclic sending

  Timer     smnTimer;
  TimerTask smnTimerTask;
  TextView  tvInfo;

  private void smnInit()
  {
    tvInfo = findViewById(R.id.tvInfo);

    long  smnTimerPeriod      = 10;   // Die Wiederholzeit des Timers in Millisekunden
    long  smnTimerStartDelay  = 500;  // Startverzögerung des Timers in Millisekunden
    final int   frequency = (int) (1000 / smnTimerPeriod);          // Frequenz des Timers

    twitter = new Twitter();          // Instanz (Objekt) eines Twitters einrichten
    twitter.init("TestTwitter",3,2,1, Twitter.Speed.normal);

    twitter.deviceKey = 10;
    twitter.deviceState = 1;
    twitter.deviceName = "Android SP1";
    twitter.posX = 1111;
    twitter.posY = 2345;
    twitter.posZ = 22;
    twitter.baseState = 33;
    twitter.baseMode = 34;

    twitter.setIntValue(0, 1234);
    twitter.setIntValue(1,-12);
    twitter.setIntValue(2, 0x37F);

    twitter.setFloatValue(0, 12.345f);
    twitter.setFloatValue(1,0.00034f);

    twitter.setTextValue(0,"Hallo Welt");

    smnTimerTask = new TimerTask()
    {
      @Override                       // Einrichten eines Timer-Task,
      public void run()               // in dessen run-Funktion
      {                               // die run-Funktion des Twitters
        twitter.run(frequency);       // zyklisch aufgerufen wird
      }
    };

    smnTimer = new Timer();
    smnTimer.scheduleAtFixedRate(smnTimerTask, smnTimerStartDelay, smnTimerPeriod);
    // Einrichten und Parametrieren eines Timers

    if(twitter.errorCode == 0)
    {
      tvInfo.setText(twitter.resultMsg);
      twitter.enabled = true;
    }
    else
    {
      tvInfo.setText(twitter.errorMsg);
      twitter.enabled = false;
      smnTimer.cancel();
    }
  }
}
