package hsh.mplab.monitorsmn;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

import hsh.mplab.socmannet.*;
import hsh.mplab.socmannet.Follower.IntegerValue;
import hsh.mplab.socmannet.Follower.FloatValue;
import hsh.mplab.socmannet.Follower.TextValue;

public class MainActivity extends AppCompatActivity
{

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    // ***********************************************************************
    // Added to the original source code created with Android Studio
    graphInit();
    timerInit();
    smnInit();
    // ***********************************************************************

  }

  // *************************************************************************
  // Added to the original source code created with Android Studio
  // All from here to the end of file (before closing class bracket)
  // *
  // *

  // -------------------------------------------------------------------------
  // Killing the whole Activity when Back-Key (Enter-Key) is pressed
  // -------------------------------------------------------------------------
  // The problem is, that if you close the APP by pressing Back-Key and
  // start it again, You have 2 twitters running.
  // And many more, if you repeat that scenary.
  // Ok, the correct solution would be to implement software for all Activity
  // states (onStart, onResume, etc.) and close sockets, cancel timers, etc.
  // whenever it is necessary.
  // But we thought, that this is to much effort and makes the handling of
  // problems bigger than the application itself.
  // Therefore we kill the whole Activity here with closing all contained
  // resources. A new start gives a new twitter and only one instance of our
  // application is running.
  //
  @Override
  public void onBackPressed()
  {
    super.onBackPressed();
    android.os.Process.killProcess(android.os.Process.myPid());
  }


  // -------------------------------------------------------------------------
  // Initialisation to use graphical elements in code
  // -------------------------------------------------------------------------
  //
  TextView tvInfo;         // Reference to the text box

  private void graphInit()
  {
    tvInfo =
      (TextView) findViewById(R.id.tvInfo);   // TextView has to be named in
  }                                           // activity_main.xml (id)

  // -------------------------------------------------------------------------
  // Using graphical elements invoked by other threads (state machine)
  // -------------------------------------------------------------------------
  // Graphical elements may not be accessed in other threads than in main.
  // Therefore it is necessary to provide a shell for other threads, which
  // puts the request of threads onto the queue of the main thread.
  //
  String msgForTvInfo;    // A global variable, not to use a stack variable of
                          // a method to store information for another thread

  void info(String msg)
  {
    msgForTvInfo = msg;   // use global reference for the message

    runOnUiThread
    (
      new Runnable()
      {
        @Override
        public void run()   // will be called by UI-Thread
        {
          if(tvInfo == null) return;
          tvInfo.setText(msgForTvInfo);
        }
      }
    );
  }


  // -------------------------------------------------------------------------
  // Part of Twitter initialisation (needed for Timer task) [0]
  // -------------------------------------------------------------------------
  // We cannot do the full initialisation of Twitter here, because there is
  // network library access with it.
  // Android throws an exception, if we access network in the main thread
  // (we are in onCreate() here, which is the main thread)
  // The rest of initialisation is done with the state machine.
  //
  Twitter devTwitter;
  // This is the reference to the twitter which is cyclic sending

  private void smnInit()
  {
    devTwitter = new Twitter();  // Create instance (object) of the Twitter
  }

  // -------------------------------------------------------------------------
  // Initialisation of a timer for running a state machine (automat)
  // -------------------------------------------------------------------------
  //
  Timer smnTimer;
  TimerTask smnTimerTask;
  int frequency;

  private void timerInit()
  {
    long smnTimerPeriod = 10;       // repetition time in milliseconds
    long smnTimerStartDelay = 500;  // start delay in milliseconds
    // Expecting some time extra for creating
    // all graphics we simply wait

    frequency = (int) (1000 / smnTimerPeriod);  // timer frequency is put to a
    // global variable for using it
    // in any environment

    smnTimerTask = new TimerTask()
    {
      @Override                       // creating a timer task and overriding
      public void run()               // its run method with ower own run method
      {
        devTwitter.run(frequency);       // cyclic calling run method of twitter

        stateMachine(frequency);      // cyclic calling our state machine
      }
    };

    smnTimer = new Timer();
    smnTimer.scheduleAtFixedRate(smnTimerTask, smnTimerStartDelay, smnTimerPeriod);
    // Einrichten und Parametrieren eines Timers

  }


// ---------------------------------------------------------------------------
// State Machine (automat)
// ---------------------------------------------------------------------------

  // -------------------------------------------------------------------------
  // Definitions for the basic state
  // -------------------------------------------------------------------------
  // The basic state of our state machine is communicated via broadcast to
  // the network (the world outside this device). The time behaviour of the
  // basic state is oriented to the situation outside of this device.
  // As we distinguish the speeds of twitter with low, normal and high, the
  // changes of basic state should fit to the selected twitter speed.
  // To be free with the internal behaviour of our state machine, the basic
  // state is managed independent from the current state of our state machine.

  enum SmBaseState      // Definition of possible basic states
  {
    Init,     // For the world, we are initialising
    Error,    // For the world, there is an error
    Run       // For the world, our application runs
  }

  SmBaseState   smBaseState = SmBaseState.Init;    // Initialising basic state

  // -------------------------------------------------------------------------
  // Definitions for the state machine
  // -------------------------------------------------------------------------
  // We define the states of our state machine as needed from the internal
  // view of our device

  enum SmState          // Definition of possible internal states
  {
    InitTwitter,        // Our internal process is initialising
    ConfigTwitter,      // Configuration of Twitter will be done here
    InitFollower,       // next initialisation
    DelayMsg,           // Delay an display Follower init result
    Error,              // Error state if initialisation fails
    Wait,               // Some Waiting time as part of the process
    Simulate,           // Simulating value changes (for this example)
    Evaluate,           // Do something with other Twitters values
    CheckTwitter        // Check for any Twitter
  }

  // Variables used for the state machine
  //
  SmState   smState = SmState.InitTwitter;  // Initialising state machine
  int       globSeqCounter = 0;             // Counter for second interval
  int       waitCounter = 0;              // Counter for delays
  String    infoMsg;                      // Holding Messages
  boolean   oneShot;                      // Control single actions in loops
  int       stateLoopCounter = 0;         // Counting the ticks in state

  // Variables used for the Twitter application
  //
  int       simInt01,simInt02,simInt03;   // Application variables
  float     simFloat01,simFloat02;        // For this example, the application
  String    simText01;                    // is a simulation of values

  // Referenz to a Follower instance
  //
  FollowMultDev  monFollower;

  // Variables (management structures) used for the Follower application
  //


  // Variables used for some calculations
  //
  int       tmpInt;

  // -------------------------------------------------------------------------
  // The state machine itself
  // -------------------------------------------------------------------------
  //

  void stateMachine(int inFreq)
  {
    // -----------------------------------------------------------------------
    // state independent activities
    // -----------------------------------------------------------------------
    //

    stateLoopCounter++;     // simply counting the enters

    // do this independent from state every half second
    //
    globSeqCounter++;
    if(globSeqCounter >= inFreq/2)
    {
      globSeqCounter = 0;

      devTwitter.baseState = smBaseState.ordinal();
      // With the next automatic twitter message, this baseState is transferred
    }

    // -----------------------------------------------------------------------
    // state dependent activities
    // -----------------------------------------------------------------------
    //

    switch(smState)
    {
      // ---------------------------------------------------------------------
      case InitTwitter:                          // Initialisation
      // ---------------------------------------------------------------------
        // Initialising our application variables
        // that is 3 Integer values, 2 Float values and 1 text string
        //
        simInt01   = 100;
        simInt02   = -200;
        simInt03   = 0x555;

        simFloat01 = 1000f;
        simFloat02 = 0.001f;

        simText01  = "Hello world";

        // Initialising Twitter to send 3 Integer values, 2 Float values and
        // 1 text string with normal speed (every second a message)
        // and the object name "TestTwitter"
        //
        devTwitter.init("BaseAndroid", 3, 2, 1, Twitter.Speed.normal);

        // If there is an error with initialisation of twitter
        // we will go to error state with the next timer tick
        //
        if(devTwitter.errorCode != 0)
        {
          infoMsg = devTwitter.errorMsg;  // to be displayed
          oneShot = true;
          smState = SmState.Error;        // next in error state
          break;
        }

        // If there is no error, we inform about the network
        // and start configuration of twitter
        //
        smBaseState = SmBaseState.Init;   // This is for the world outside
        infoMsg = devTwitter.resultMsg;   // This for the display
        oneShot = true;
        smState = SmState.ConfigTwitter;  // next in configuration state
        break;


      // ---------------------------------------------------------------------
      case ConfigTwitter:                        // Configuration of Twitter
      // ---------------------------------------------------------------------
        //
        if(oneShot)
        {
          info(infoMsg);      // Display initialisation result message
          oneShot = false;    // But only once (if we come again here)
        }

        // -------------------------------------------------------------------
        // Setting meta data (or static data), which will change not so often
        //

        devTwitter.applicationKey = 0;
        // application key marks the usage of a device in a device overlapping
        // application e.g. regulation circuit with several inkluded devices
        // (0 = not used)

        devTwitter.deviceKey = SocManNet.getSmallDeviceId();
        // the device key ist the individualisation of the device and could be
        // something like the serial number

        devTwitter.deviceState = SocManNet.DeviceState.Run.ordinal();
        // device state describes a kind of physical status
        // (operable, defect, need maintenance, etc.)

        devTwitter.deviceName = "Android SP1";
        // device Name may also used for a kind of individualisation
        // but it cannot replace device key, because device key is used
        // by special class FollowMultDev when receiving messages

        devTwitter.posX = 1111;
        devTwitter.posY = 2345;
        devTwitter.posZ = 22;
        // Position of device in local coordinates, resolution is centimeter
        // there will be tools in future to configure devices and setting the
        // local position is a matter of configuration.
        // Mobile devices will have to change the content of these variables
        // when beeing moved.

        devTwitter.baseState = SmBaseState.Init.ordinal();
        // This is the status of the state machine presented to the world
        // it will be changed by the state machine
        // the base state is used to synchronise the behaviour of many devices
        // of the same kind to achieve group activities for commonly working
        // on the same task

        devTwitter.baseMode = 34;
        // In the base mode variable, a device tells the environment, what its
        // plan is for the next step. The combination of base state and base mode
        // tells the environment the process movement of a device.

        // --------------------------------------------------------------------
        // Setting process data, which depends on observations and calculations
        // The structure is defined with the initialisation of Twitter
        // The values are initialised here.
        // The real content depends on the application and will be set
        // whenever the application creates a new value (see state Simulation).

        devTwitter.setIntValue(0, simInt01);
        devTwitter.setIntValue(1, simInt02);
        devTwitter.setIntValue(2, simInt03);

        devTwitter.setFloatValue(0, simFloat01);
        devTwitter.setFloatValue(1, simFloat02);

        devTwitter.setTextValue(0, simText01);

        devTwitter.enabled = true;    // Twitter may be started after
                                      // configuration

        smState = SmState.InitFollower;
        break;

      // ---------------------------------------------------------------------
      case InitFollower:              // Initialisation of Follower
      // ---------------------------------------------------------------------

        FollowMultDev.monitorMode = true;
        // Special use as monitor setting in static variable

        monFollower = new FollowMultDev("MyFirstCommObj");
        // First following any baseDevice from the Arduino environment

        // If there is an error with initialisation of follower
        // we will go to error state with the next timer tick
        //
        if(monFollower.errorCode != 0)
        {
          infoMsg = monFollower.errorMsg;   // to be displayed
          oneShot = true;
          smState = SmState.Error;          // next in error state
          break;
        }

        smState = SmState.DelayMsg;   // Next state is Waiting and Display
        waitCounter = 2 * inFreq;     // Delay time is 2 seconds
        break;

      // ---------------------------------------------------------------------
      case DelayMsg:                      // Wait and display
      // ---------------------------------------------------------------------
        // In this example, this is part of a loop and the delay
        // time has to be set in waitCounter in the state passed before
        //
        if(waitCounter > 0)
        {
          waitCounter--;        // staying here until waitCounter is
          break;                // decremented down to 0
        }

        info(monFollower.resultMsg);  // Follower init result
        monFollower.enabled = true;

        smState = SmState.Wait;       // Next state is Waiting (again)
        waitCounter = 2 * inFreq;     // Delay time is 2 seconds
        break;

      // ---------------------------------------------------------------------
      case Wait:                          // Waiting
      // ---------------------------------------------------------------------
        // In this example, this is part of a loop and the delay
        // time has to be set in waitCounter in the state passed before
        //
        waitCounter--;
        if(waitCounter <= 0)              // if waitCounter finished
        {                                 // change state
          smState = SmState.CheckTwitter; // to check for Twitters
          waitCounter = inFreq;           // with delay
          smBaseState = SmBaseState.Run;  // tell the world we are running
          break;
        }

        break;

      // ---------------------------------------------------------------------
      case Simulate:                      // Simulating values
      // ---------------------------------------------------------------------
        // In this example, values are simply changed and Twitter is updated
        //

        simInt01++;
        if(simInt01 > 200)
          simInt01 = 0;

        simInt02++;
        if(simInt02 > 0)
          simInt02 = -200;

        simInt03 <<= 1;
        simInt03 &= 0xFFF;
        if(simInt03 == 0)
          simInt03 = 0x555;

        simFloat01 -= 0.1f;
        if(simFloat01 <= 0)
          simFloat01 = 1000f;

        simFloat02 += 0.77;
        if(simFloat02 >= 500)
          simFloat02 = 0.01f;

        tmpInt = stateLoopCounter % 9;

        if((stateLoopCounter % 3) == 0)
          simText01 = "Here I am. _" + tmpInt;
        else
          simText01 = "Again. _" + tmpInt;

        devTwitter.setIntValue(0, simInt01);
        devTwitter.setIntValue(1, simInt02);
        devTwitter.setIntValue(2, simInt03);

        devTwitter.setFloatValue(0, simFloat01);
        devTwitter.setFloatValue(1, simFloat02);

        devTwitter.setTextValue(0, simText01);

        smState = SmState.Wait;       // Next state is Waiting (delay)
        waitCounter = 2 * inFreq;     // Delay time is 2 seconds
        break;

      // ---------------------------------------------------------------------
      case Evaluate:                  // Evaluate Follower
      // ---------------------------------------------------------------------
        // Get all management data for the external Twitter
        // followed by devFollower

        smState = SmState.Wait;       // Next state is Waiting (delay)
        waitCounter = 2 * inFreq;     // Delay time is 2 seconds
        break;

      // ---------------------------------------------------------------------
      case CheckTwitter:              // Looking for any Twitter
      // ---------------------------------------------------------------------
        // Get all management data for the external Twitter
        // followed by monFollower
        waitCounter--;
        if(waitCounter > 0)
          break;

        FollowMultDev.PortFollowMult followerList =
                FollowMultDev.portFollowList.getPortFollow();
        tmpInt = followerList.commObjectList.size();
        info("Anzahl unterschiedlicher Twitter = " + tmpInt);
        waitCounter = inFreq;
        break;

      // ---------------------------------------------------------------------
      case Error:                         // Error (severe)
      // ---------------------------------------------------------------------
        // In this example, the state machine will stay in error state
        // until power supply is switched off (or reset is done)
        //
        if(oneShot)
        {
          info(infoMsg);      // Display error message
          oneShot = false;    // But only once
        }
        break;
    }


  }


}
