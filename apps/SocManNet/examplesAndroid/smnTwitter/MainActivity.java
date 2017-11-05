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

    // -----------------------------------------------------------------------
    // Added to the original source code created with Android Studio
    graphInit();
    timerInit();
    smnInit0();
    // ----------------------------------------------------------------------

  }

  // -------------------------------------------------------------------------
  // Added to the original source code created with Android Studio
  // All from here to the end of file (before closing class bracket)

  // -------------------------------------------------------------------------
  // Initialisation to use graphical elements in code
  // -------------------------------------------------------------------------
  //
  TextView  tvInfo;         // Reference to the text box

  private void graphInit()
  {
    tvInfo = findViewById(R.id.tvInfo);   // textview had to be named in
  }                                       // activity_main.xml (id)

  // -------------------------------------------------------------------------
  // Part of Twitter initialisation (needed for Timer task) [0]
  // -------------------------------------------------------------------------
  // We cannot do the full initialisation of Twitter here, because there is
  // network library access with it.
  // Android throws an exception, if we access network in the main thread
  // (we are in onCreate() here, which is the main thread)
  //
  Twitter twitter;
  // This is the reference to the twitter which is cyclic sending

  private void smnInit0()
  {
    twitter = new Twitter();  // Create instance (object) of the Twitter
  }

  // -------------------------------------------------------------------------
  // Part of Twitter initialisation (network access) [1]
  // -------------------------------------------------------------------------
  // This function is called from the state machine driven by the timer
  // So it is an extra thread which calls this function
  //
  private void smnInit1()
  {
    // Initialising Twitter to send 3 Integer values, 2 Float values and
    // one text string with normal speed (every second a message)
    //
    twitter.init("TestTwitter", 3, 2, 1, Twitter.Speed.normal);
    //
    // this function may invoke an exception, if we call it in main thread

    // -----------------------------------------------------------------------
    // Setting meta data (or static data), which will change not so often
    //
    twitter.deviceKey = 10;     // the device key ist the individualisation
                                // of the device and could be something like
                                // the serial number

    twitter.deviceState = 1;    // device state describes a kind of physical
                                // status (operable, defect, need maintenance)

    twitter.deviceName = "Android SP1";
    // device Name may also used for a kind of individualisation
    // but it cannot replace device key, because device key is used
    // by special class FollowMultDev when receiving messages

    twitter.posX = 1111;
    twitter.posY = 2345;
    twitter.posZ = 22;
    // Position of device in local coordinates, resolution is centimeter
    // there will be tools in future to configure devices and setting the
    // local position is a matter of configuration.
    // Mobile devices will have to change the content of these variables
    // when beeing moved.

    twitter.baseState = 33;     // This is the status of the state machine
                                // it will be changed by the state machine
    // the base state is used to synchronise the behaviour of many devices
    // of the same kind to achieve group activities for commonly working
    // on the same task

    twitter.baseMode = 34;      // In the base mode variable, a device
                                // tells the environment, what its plan is
    // for the next step. The combination of base state and base mode tells
    // the environment the process movement of a device.

    // -----------------------------------------------------------------------
    // Setting process data, which depends on observations and calculations
    // The structure is defined with the initialisation of Twitter
    // The values are initialised here.
    // The real content depends on the application and will be set
    // whenever the application creates a new value.

    twitter.setIntValue(0, 1234);
    twitter.setIntValue(1, -12);
    twitter.setIntValue(2, 0x37F);

    twitter.setFloatValue(0, 12.345f);
    twitter.setFloatValue(1, 0.00034f);

    twitter.setTextValue(0, "Hallo Welt");

    // After initialisation of all values, we may start
    //
    if (twitter.errorCode == 0)
    {
      // If there is no error with initialisation, we will display information
      // about the network at the text box (TextView)
      // and start the Twitter
      //
      tvInfo.setText(twitter.resultMsg);
      twitter.enabled = true;
    }
    else
    {
      // If there is an error with initialisation, we will display error
      // information at the text box, block the Twitter and stop timer
      //
      tvInfo.setText(twitter.errorMsg);
      twitter.enabled = false;
      smnTimer.cancel();
    }

  }


  // -------------------------------------------------------------------------
  // Initialisation of a timer for running a state machine (automat)
  // -------------------------------------------------------------------------
  //
  Timer     smnTimer;
  TimerTask smnTimerTask;
  int       frequency;

  private void timerInit() {
    long smnTimerPeriod = 10;   // Die Wiederholzeit des Timers in Millisekunden
    long smnTimerStartDelay = 500;  // Startverzögerung des Timers in Millisekunden

    frequency = (int) (1000 / smnTimerPeriod);          // Frequenz des Timers


    smnTimer = new Timer();
    smnTimer.scheduleAtFixedRate(smnTimerTask, smnTimerStartDelay, smnTimerPeriod);
    // Einrichten und Parametrieren eines Timers

    smnTimerTask = new TimerTask() {
      @Override                       // Einrichten eines Timer-Task,
      public void run()               // in dessen run-Funktion
      {                               // die run-Funktion des Twitters
        twitter.run(frequency);       // zyklisch aufgerufen wird
      }
    };


  }
}
