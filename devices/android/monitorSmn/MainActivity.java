package hsh.mplab.monitorsmn;

//import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;


import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;

import java.util.Timer;
import java.util.TimerTask;

import hsh.mplab.progtools.LabSpinner;
import hsh.mplab.progtools.RouiText;
import hsh.mplab.progtools.SmState;
import hsh.mplab.progtools.StateMachineHelper;

import hsh.mplab.socmannet.*;
import hsh.mplab.socmannet.FollowMultDev;
import hsh.mplab.socmannet.FollowMultDev.*;


import hsh.mplab.socmannet.Follower.IntegerValue;
import hsh.mplab.socmannet.Follower.FloatValue;
import hsh.mplab.socmannet.Follower.TextValue;


public class MainActivity extends AppCompatActivity
{

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    getSupportActionBar().hide(); //hide the title bar
    setContentView(R.layout.activity_main);
    setRequestedOrientation(SCREEN_ORIENTATION_PORTRAIT);

    // ***********************************************************************
    // Added to the original source code created with Android Studio
    graphInit();
    timerSmnInit();
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
  RouiText    ipVal, macVal, nrTwitVal;
  RouiText[]  twInt = new RouiText[4];
  RouiText[]  twFloat = new RouiText[4];
  RouiText[]  twString = new RouiText[4];
  LabSpinner  lsDevIdTwitter, lsTwitPduNumber;
  LabSpinner  lsDevIpAdr, lsDevMacAdr, lsDevName, lsDevId, lsDevPos,
              lsDevBasStat, lsDevBasMod, lsDevLostPdu;

  private void graphInit()
  {
    ipVal     = new RouiText(this, (TextView) findViewById(R.id.tvThisDevIpVal));
    macVal    = new RouiText(this, (TextView) findViewById(R.id.tvThisDevMacVal));
    nrTwitVal = new RouiText(this, (TextView) findViewById(R.id.tvMonTwitCntVal));

    twInt[0]    = new RouiText(this, (TextView) findViewById(R.id.tvTwInt1));
    twInt[1]    = new RouiText(this, (TextView) findViewById(R.id.tvTwInt2));
    twInt[2]    = new RouiText(this, (TextView) findViewById(R.id.tvTwInt3));
    twInt[3]    = new RouiText(this, (TextView) findViewById(R.id.tvTwInt4));

    twFloat[0]  = new RouiText(this, (TextView) findViewById(R.id.tvTwFloat1));
    twFloat[1]  = new RouiText(this, (TextView) findViewById(R.id.tvTwFloat2));
    twFloat[2]  = new RouiText(this, (TextView) findViewById(R.id.tvTwFloat3));
    twFloat[3]  = new RouiText(this, (TextView) findViewById(R.id.tvTwFloat4));

    twString[0] = new RouiText(this, (TextView) findViewById(R.id.tvTwString1));
    twString[1] = new RouiText(this, (TextView) findViewById(R.id.tvTwString2));
    twString[2] = new RouiText(this, (TextView) findViewById(R.id.tvTwString3));
    twString[3] = new RouiText(this, (TextView) findViewById(R.id.tvTwString4));

    lsDevIdTwitter = (LabSpinner) findViewById(R.id.lsDevIdTwitter);
    lsDevIdTwitter.addToList();

    lsTwitPduNumber = (LabSpinner) findViewById(R.id.lsTwitPduNumber);
    lsTwitPduNumber.addToList();

    lsDevIpAdr    = (LabSpinner) findViewById(R.id.lsDevIpAdr);
    lsDevIpAdr.addToList();
    lsDevMacAdr   = (LabSpinner) findViewById(R.id.lsDevMacAdr);
    lsDevMacAdr.addToList();
    lsDevName     = (LabSpinner) findViewById(R.id.lsDevName);
    lsDevName.addToList();
    lsDevId       = (LabSpinner) findViewById(R.id.lsDevId);
    lsDevId.addToList();
    lsDevPos      = (LabSpinner) findViewById(R.id.lsDevPos);
    lsDevPos.addToList();
    lsDevBasStat  = (LabSpinner) findViewById(R.id.lsDevBasStat);
    lsDevBasStat.addToList();
    lsDevBasMod   = (LabSpinner) findViewById(R.id.lsDevBasMod);
    lsDevBasMod.addToList();
    lsDevLostPdu  = (LabSpinner) findViewById(R.id.lsDevLostPdu);
    lsDevLostPdu.addToList();


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
  // -------------------------------------------------------------------------
  // Initialisation of a timer for running a state machine (automat)
  // -------------------------------------------------------------------------
  //
  Timer smnTimer;
  TimerTask smnTimerTask;
  Twitter devTwitter;
  // This is the reference to the twitter which is cyclic sending
  StateMachineHelper  smh;
  int frequency;


  private void timerSmnInit()
  {
    long smnTimerPeriod = 10;         // repetition time in milliseconds
    long smnTimerStartDelay = 2000;  // start delay in milliseconds
    // Expecting some time extra for creating
    // all graphics we simply wait

    frequency = (int) (1000 / smnTimerPeriod);  // timer frequency is put to a
    // global variable for using it
    // in any environment

    devTwitter = new Twitter();  // Create instance (object) of the Twitter
    smh = new StateMachineHelper(hsh.mplab.progtools.SmState.Prepare, frequency);

    smnTimerTask = new TimerTask()
    {
      @Override                       // creating a timer task and overriding
      public void run()               // its run method with ower own run method
      {
        devTwitter.run(frequency);    // cyclic calling run method of twitter
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

  // Variables used for the state machine
  //
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
  IntegerValueList  intMan1, intMan2, intMan3, intMan4;           // Management structures for
  FloatValueList    floatMan1, floatMan2, floatMan3, floatMan4;   // receiving 4 Integer variables,
  TextValueList     textMan1, textMan2, textMan3, textMan4;       // 4 Float and 4 Text


  // Variables used in different states
  //
  int       loopDevIdx;
  int       nrOfTwitters;
  int       lastNrOfTwitters = -1;
  int       newSelectedDevIdx = 0;
  int       lastSelectedDevIdx = -1;

  DeviceDHA dut;

  // -------------------------------------------------------------------------
  // The state machine itself
  // -------------------------------------------------------------------------
  //

  void stateMachine(int inFreq)
  {
    int     currentInfStatus;
    boolean skipMachine;
    boolean inputTimeOut;

    // -----------------------------------------------------------------------
    // state independent activities
    // -----------------------------------------------------------------------
    //
    skipMachine = smh.begin();

    // -----------------------------------------------------------------------
    // state dependent activities
    // -----------------------------------------------------------------------
    //

    if(!skipMachine)
    {
      switch (smh.nextState)
      {
        // ---------------------------------------------------------------------
        case Prepare:                               // Preparation
        // ---------------------------------------------------------------------
          smh.enter(SmState.InitTwitter);
          break;

        // ---------------------------------------------------------------------
        case InitTwitter:                          // Initialisation
        // ---------------------------------------------------------------------
          // Initialising our application variables
          // that is 3 Integer values, 2 Float values and 1 text string
          //
          simInt01 = 100;
          simInt02 = -200;
          simInt03 = 0x555;

          simFloat01 = 1000f;
          simFloat02 = 0.001f;

          simText01 = "Hello world";

          // Initialising Twitter to send 3 Integer values, 2 Float values and
          // 1 text string with normal speed (every second a message)
          // and the object name "TestTwitter"
          //
          devTwitter.init("Monitor", 3, 2, 1, Twitter.Speed.normal);

          // If there is an error with initialisation of twitter
          // we will go to error state with the next timer tick
          //
          if (devTwitter.errorCode != 0)
          {
            infoMsg = devTwitter.errorMsg;  // to be displayed
            oneShot = true;
            smh.enter(SmState.Error);        // next in error state
            break;
          }

          // If there is no error, we inform about the network
          // and start configuration of twitter
          //
          smBaseState = SmBaseState.Init;     // This is for the world outside
          infoMsg = devTwitter.resultMsg;     // This for the display
          smh.enter(SmState.ConfigTwitter);   // next in configuration state
          break;


        // ---------------------------------------------------------------------
        case ConfigTwitter:                        // Configuration of Twitter
        // ---------------------------------------------------------------------

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

          devTwitter.deviceState = 1;

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

          smh.enter(SmState.InitFollower);
          break;

        // ---------------------------------------------------------------------
        case InitFollower:              // Initialisation of Follower
        // ---------------------------------------------------------------------

          FollowMultDev.monitorMode = true;
          // Special use as monitor setting in static variable

          monFollower = new FollowMultDev("Monitor");
          // Name is not relevant

          // If there is an error with initialisation of follower
          // we will go to error state with the next timer tick
          //
          if (monFollower.errorCode != 0)
          {
            infoMsg = monFollower.errorMsg;   // to be displayed
            oneShot = true;
            smh.enter(SmState.Error);          // next in error state
            break;
          }

          intMan1   = monFollower.getIntegerValueList(0);
          intMan2   = monFollower.getIntegerValueList(1);
          intMan3   = monFollower.getIntegerValueList(2);
          intMan4   = monFollower.getIntegerValueList(3);

          floatMan1 = monFollower.getFloatValueList(0);
          floatMan2 = monFollower.getFloatValueList(1);
          floatMan3 = monFollower.getFloatValueList(3);
          floatMan4 = monFollower.getFloatValueList(4);

          textMan1  = monFollower.getTextValueList(0);
          textMan2  = monFollower.getTextValueList(1);
          textMan3  = monFollower.getTextValueList(2);
          textMan4  = monFollower.getTextValueList(3);

          monFollower.enabled = true;
          ipVal.print(SocManNet.localIP);
          macVal.print(SocManNet.localMAC);
          loopDevIdx = 0;
          smh.setTimeOut(1000);
          smh.enter(SmState.Evaluate);    // Next state is Waiting and Display
          break;

        // ---------------------------------------------------------------------
        case Evaluate:                  // Evaluate Follower
        // ---------------------------------------------------------------------
          // Get all management data for the external Twitter
          // followed by devFollower

          dut = monFollower.getDevice(loopDevIdx);
          // getDevice liefert null, wenn Idx zu groß für die Liste
          if (dut == null)
          {
            loopDevIdx = 0;
            nrOfTwitters = monFollower.deviceCount();
            // Anzahl der im Netz befindlichen Twitter
            // Die inzwischen ausgeschalteten werden mitgezählt, weil die Liste bleibt
            if (lastNrOfTwitters != nrOfTwitters)
            {
              lastNrOfTwitters = nrOfTwitters;
              nrTwitVal.print("" + nrOfTwitters);
            }
            break;
          }

          inputTimeOut = monFollower.deviceInputLag(loopDevIdx) > 1500;
          if (inputTimeOut)
            currentInfStatus = lsDevIdTwitter.infoStatIdxTimeOut;
          else
            currentInfStatus = lsDevIdTwitter.infoStatIdxOk;

          lsDevIdTwitter.addOrReplace
              (dut.deviceKey + " / " + dut.twitterName, loopDevIdx, currentInfStatus);

          lsTwitPduNumber.addOrReplace("" + dut.pduCount, loopDevIdx, currentInfStatus);

          lsDevMacAdr.addOrReplace(dut.macAdrStr,loopDevIdx,currentInfStatus);
          lsDevIpAdr.addOrReplace(dut.ipAdrStr,loopDevIdx,currentInfStatus);
          lsDevId.addOrReplace("" + dut.deviceKey,loopDevIdx,currentInfStatus);
          lsDevName.addOrReplace(dut.deviceName,loopDevIdx,currentInfStatus);
          lsDevPos.addOrReplace
              ("x=" + dut.posX + " y=" + dut.posY + " z=" + dut.posZ, loopDevIdx, currentInfStatus);
          lsDevBasMod.addOrReplace("" + dut.baseMode, loopDevIdx, currentInfStatus);
          lsDevBasStat.addOrReplace("" + dut.baseState, loopDevIdx, currentInfStatus);
          lsDevLostPdu.addOrReplace(""+dut.lostPduCount, loopDevIdx, currentInfStatus);

          if(loopDevIdx == newSelectedDevIdx)
          {
            for(int i = 0; i < 4; i++)
            {
              if(i < dut.intCount)
                twInt[i].print(""+dut.intArray[i]);
              else
                twInt[i].print("");

              if(i < dut.floatCount)
                twFloat[i].print(""+dut.floatArray[i]);
              else
                twFloat[i].print("");

              if(i < dut.textCount)
                twString[i].print(""+dut.stringArray[i]);
              else
                twString[i].print("");
            }
          }

          
          if (smh.timeOut())
          {
            if (smh.oneShot())
              LabSpinner.clearAnySelected();
            if (newSelectedDevIdx >= 0)
              lastSelectedDevIdx = newSelectedDevIdx;
            int newSelection = LabSpinner.getLastNewSelectedPos();
            if (newSelection >= 0)
            {
              newSelectedDevIdx = newSelection;
              LabSpinner.setAllSelections(newSelectedDevIdx);
              smh.setOneShot();
              smh.setTimeOut(1000);
            } else
              smh.setTimeOut(500);
          }

          loopDevIdx++;
          break;

        // ---------------------------------------------------------------------
        case CheckTwitter:              // Looking for any Twitter
        // ---------------------------------------------------------------------
          // Get all management data for the external Twitter
          // followed by monFollower
          waitCounter--;
          if (waitCounter > 0)
            break;


          //tmpInt = monFollower.deviceCount();
          //info.print("Anzahl Twitter gesamt = " + tmpInt);
          //tmpInt = monFollower.twitterCount();
          //info.print("Anzahl unterschiedlicher Twitter = " + tmpInt);
          waitCounter = inFreq;
          break;

        // ---------------------------------------------------------------------
        case Error:                         // Error (severe)
        // ---------------------------------------------------------------------
          // In this example, the state machine will stay in error state
          // until power supply is switched off (or reset is done)
          //
          break;
      }
    }

    smh.end();
  }


}
