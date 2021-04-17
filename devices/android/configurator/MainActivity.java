package hsh.mplab.configurator;

import android.support.constraint.ConstraintSet;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Layout;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Timer;
import java.util.TimerTask;

import hsh.mplab.globals.GlobalDHA;

import hsh.mplab.progtools.OutText;
import hsh.mplab.progtools.RouiText;
import hsh.mplab.progtools.StateMachineHelper;
import hsh.mplab.progtools.SmState;
import hsh.mplab.progtools.LabSpinner;

import hsh.mplab.socmannet.*;
import hsh.mplab.socmannet.FollowMultDev.IntegerValueList;
import hsh.mplab.socmannet.FollowMultDev.FloatValueList;
import hsh.mplab.socmannet.FollowMultDev.TextValueList;
import hsh.mplab.socmannet.FollowMultDev.DeviceDHA;


public class MainActivity extends AppCompatActivity
{
  boolean debugSM;        // For debugging state machine

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    //requestWindowFeature(Window.FEATURE_NO_TITLE);  //hide the title does not work
    getSupportActionBar().hide(); //hide the title bar
    setContentView(R.layout.activity_main);

    // ***********************************************************************
    // Added to the original source code created with Android Studio
    graphInit();
    timerInit();
    smnInit();
    pageInit();
    // ***********************************************************************
    debugSM = true;
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
  // Page Handling
  // -------------------------------------------------------------------------
  //
  View  incInfo;
  View  incStart;
  View  incLog;
  View  incPos;
  View  incWlan;

  View  curPage;
  View  lastPage;

  LinearLayout  llHome;

  private void pageInit()
  {
    incStart  = findViewById(R.id.incStart);
    incInfo   = findViewById(R.id.incInfo);
    incLog    = findViewById(R.id.incLog);
    incPos    = findViewById(R.id.incPos);
    incWlan   = findViewById(R.id.incWlan);

    llHome    = (LinearLayout) findViewById(R.id.llHome);
    llHome.setVisibility(View.GONE);
    curPage   = incStart;
  }

  private void openPage(View inView)
  {
    incStart.setVisibility(View.GONE);
    lastPage = incStart;
    llHome.setVisibility(View.VISIBLE);
    inView.setVisibility(View.VISIBLE);
    curPage = inView;
  }

  public void onClickBtnInfo(View v)
  {
    openPage(incInfo);
  }

  public void onClickBtnPos(View v)
  {
    openPage(incPos);
  }

  public void onClickBtnWlan(View v)
  {
    openPage(incWlan);
  }

  public void onClickBtnLog(View v)
  {
    openPage(incLog);
  }

  public void onClickBtnHome(View v)
  {
    curPage.setVisibility(View.GONE);
    lastPage = curPage;
    llHome.setVisibility(View.GONE);
    incStart.setVisibility(View.VISIBLE);
    curPage = incStart;
  }

  boolean butSetClicked = false;
  public void onClickBtnSet(View v)
  {
    butSetClicked = true;
  }



  // -------------------------------------------------------------------------
  // Initialisation to use graphical elements in code
  // -------------------------------------------------------------------------
  //
  OutText     nrDevVal,ipVal,macVal,log;
  LabSpinner  lsDevIpAdr, lsDevMacAdr, lsDevName, lsDevId, lsDevPos,
              lsDevBasStat, lsDevBasMod, lsDevTimeOut, lsDevLostPdu;

  TextView    tvDevKeyVal, tvDevNameVal;
  RouiText devKeyVal, devNameVal;

  TextView    tvOldPosX, tvOldPosY, tvOldPosZ;
  RouiText    oldPosX, oldPosY, oldPosZ;

  EditText    etInPosX, etInPosY, etInPosZ;

  private void graphInit()
  {
    log = new OutText(this,(TextView) findViewById(R.id.tvLog),5);
    log.setStamp(true);
    log.prtlnDateTime();

    ipVal = new OutText(this, (TextView) findViewById(R.id.tvThisDevIpVal),1);
    macVal = new OutText(this, (TextView) findViewById(R.id.tvThisDevMacVal),1);
    nrDevVal = new OutText(this,findViewById(R.id.tvCfgDevCntVal),2);

    lsDevIpAdr    = findViewById(R.id.lsDevIpAdr);
    lsDevIpAdr.addToList();
    lsDevMacAdr   = findViewById(R.id.lsDevMacAdr);
    lsDevMacAdr.addToList();
    lsDevName     = findViewById(R.id.lsDevName);
    lsDevName.addToList();
    lsDevId       = findViewById(R.id.lsDevId);
    lsDevId.addToList();
    lsDevPos      = findViewById(R.id.lsDevPos);
    lsDevPos.addToList();
    lsDevBasStat  = findViewById(R.id.lsDevBasStat);
    lsDevBasStat.addToList();
    lsDevBasMod   = findViewById(R.id.lsDevBasMod);
    lsDevBasMod.addToList();
    lsDevTimeOut  = findViewById(R.id.lsDevTimeOut);
    lsDevTimeOut.addToList();
    lsDevLostPdu  = findViewById(R.id.lsDevLostPdu);
    lsDevLostPdu.addToList();

    tvDevKeyVal   = findViewById(R.id.tvDevKeyVal);
    devKeyVal     = new RouiText(this,tvDevKeyVal);
    tvDevNameVal  = findViewById(R.id.tvDevNameVal);
    devNameVal    = new RouiText(this, tvDevNameVal);

    tvOldPosX     = findViewById(R.id.tvOldPosX);
    oldPosX       = new RouiText(this, tvOldPosX);
    tvOldPosY     = findViewById(R.id.tvOldPosY);
    oldPosY       = new RouiText(this, tvOldPosY);
    tvOldPosZ     = findViewById(R.id.tvOldPosZ);
    oldPosZ       = new RouiText(this, tvOldPosZ);

    etInPosX      = findViewById(R.id.etInPosX);
    etInPosY      = findViewById(R.id.etInPosY);
    etInPosZ      = findViewById(R.id.etInPosZ);

  }



  // -------------------------------------------------------------------------
  // Initialisation of a timer for running a state machine (automat)
  // -------------------------------------------------------------------------
  //
  Timer     smnTimer;
  TimerTask smnTimerTask;
  boolean   taskBusy = false;
  int       taskViolationCounter = 0;

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
        if(taskBusy)
        {
          taskViolationCounter++;
          return;
        }

        // Test problems with ADB and breakpoints inside devTwitter/SocManNet
        // TimerTask creates a Nullpointer-Exception on devTwitter
        // With the following check, breakpoints work inside devTwitter/SocManNet
        // Task busy check (taskBusy-Flag here) did not solve the problem
        //
        if(devTwitter == null) return;

        taskBusy = true;
        // ----------------------------------------------------------------------------
        devTwitter.run(frequency);        // cyclic calling run method of twitter
        stateMachine();                   // cyclic calling our state machine
        // ----------------------------------------------------------------------------
        taskBusy = false;
      }
    };

    smnTimer = new Timer();
    smnTimer.scheduleAtFixedRate(smnTimerTask, smnTimerStartDelay, smnTimerPeriod);
    // Einrichten und Parametrieren eines Timers

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

  StateMachineHelper smh;

  private void smnInit()
  {
    devTwitter = new Twitter();  // Create instance (object) of the Twitter
    smh = new StateMachineHelper(SmState.Prepare, frequency);
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

  // Variables used for the state machine
  //
  int       globSeqCounter = 0;             // Counter for second interval
  int       waitCounter = 0;              // Counter for delays
  String    infoMsg;                      // Holding Messages
  boolean   oneShot;                      // Control single actions in loops
  int       stateLoopCounter = 0;         // Counting the ticks in state

  // Variables used for the Twitter application
  //
  int       intVal1 = 1,intVal2 = 2,intVal3 = 3,intVal4 = 4;
  float     floatVal1 = (float) 0.1,floatVal2 = (float) 0.2,floatVal3 = (float) 0.3,floatVal4 = (float) 0.4;
  String    textVal1 = "A",textVal2 = "B",textVal3 = "C",textVal4 = "D";

  void updateDevTwitter()
  {
    devTwitter.setIntValue(0,intVal1);
    devTwitter.setIntValue(1,intVal2);
    devTwitter.setIntValue(2,intVal3);
    devTwitter.setIntValue(3,intVal4);

    devTwitter.setFloatValue(0,floatVal1);
    devTwitter.setFloatValue(1,floatVal2);
    devTwitter.setFloatValue(2,floatVal3);
    devTwitter.setFloatValue(3,floatVal4);

    devTwitter.setTextValue(0, textVal1);
    devTwitter.setTextValue(1, textVal2);
    devTwitter.setTextValue(2, textVal3);
    devTwitter.setTextValue(3, textVal4);
  }

  // Reference to a Follower instance
  //
  FollowMultDev  devFollower;

  // Variables (management structures) used for the Follower application
  //
  IntegerValueList  intMan1, intMan2, intMan3, intMan4;           // Management structures for
  FloatValueList    floatMan1, floatMan2, floatMan3, floatMan4;   // receiving 4 Integer variables,
  TextValueList     textMan1, textMan2, textMan3, textMan4;       // 4 Float and 4 Text


  // Variables used for some calculations
  //
  int       tmpInt;
  long      tmpLong;

  // Variable global for the state machien
  //
  int       lastNrOfDevices = -1;
  int       nrOfDevices;
  int       loopDevIdx;
  int       newSelectedDevIdx = 0;
  int       lastSelectedDevIdx = -1;
  DeviceDHA dut;
  String    inTextX, inTextY, inTextZ;

  // -------------------------------------------------------------------------
  // The state machine itself
  // -------------------------------------------------------------------------
  //

  void stateMachine()
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
        case InitTwitter:                           // Initialisation
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("InitTwitter");
            }
          }

          // Initialising Twitter to send 4 Integer values, 4 Float values and
          // 4 text string with normal speed (every second a message)
          // and the object name "TestTwitter"
          //
          devTwitter.init("Configurator", 4, 4, 4, Twitter.Speed.normal);

          // If there is an error with initialisation of twitter
          // we will go to error state with the next timer tick
          //
          if (devTwitter.errorCode != 0)
          {
            infoMsg = devTwitter.errorMsg;  // to be displayed
            oneShot = true;
            smh.enter(SmState.Error);       // next in error state
            break;
          }

          // If there is no error, we inform about the network
          // and start configuration of twitter
          //
          smBaseState = SmBaseState.Init;   // This is for the world outside
          infoMsg = devTwitter.resultMsg;   // This for the display
          smh.enter(SmState.ConfigTwitter);  // next in configuration state
          break;


        // ---------------------------------------------------------------------
        case ConfigTwitter:                        // Configuration of Twitter
        // ---------------------------------------------------------------------
          //
          if (smh.firstEnter())
          {
            if(debugSM) log.println("ConfigTwitter");
            log.println(devTwitter.resultMsg);
            ipVal.replace(SocManNet.localIP);
            macVal.replace(SocManNet.localMAC);
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

          updateDevTwitter();

          devTwitter.enabled = true;       // Twitter may be started after
          // configuration

          smh.enter(SmState.InitFollower);
          break;

        // ---------------------------------------------------------------------
        case InitFollower:              // Initialisation of Follower
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("InitFollower");
            }
          }

          devFollower = new FollowMultDev("ConfigDev");
          // Following ConfigDev Twitter Arduino

          intMan1 = devFollower.getIntegerValueList(0);
          intMan2 = devFollower.getIntegerValueList(1);
          intMan3 = devFollower.getIntegerValueList(2);
          intMan4 = devFollower.getIntegerValueList(3);

          floatMan1 = devFollower.getFloatValueList(0);
          floatMan2 = devFollower.getFloatValueList(1);
          floatMan3 = devFollower.getFloatValueList(3);
          floatMan4 = devFollower.getFloatValueList(4);

          textMan1 = devFollower.getTextValueList(0);
          textMan2 = devFollower.getTextValueList(1);
          textMan3 = devFollower.getTextValueList(2);
          textMan4 = devFollower.getTextValueList(3);


          // If there is an error with initialisation of follower
          // we will go to error state with the next timer tick
          //
          if (devFollower.errorCode != 0)
          {
            infoMsg = devFollower.errorMsg;   // to be displayed
            oneShot = true;
            smh.enter(SmState.Error);          // next in error state
            break;
          }

          devFollower.enabled = true;

          smh.enter(SmState.Wait);  // Next state is Waiting
          break;

        // ---------------------------------------------------------------------
        case Wait:                          // Waiting
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("Wait");
            }
          }

          nrOfDevices = devFollower.deviceCount();

          // Warten, bis ein (oder mehrere) konfigurierbare Geräte da sind
          //
          if(lastNrOfDevices != nrOfDevices)
          {
            lastNrOfDevices = nrOfDevices;
            nrDevVal.replace(""+nrOfDevices);
            devFollower.clearErrors();
            smh.setTimeOut(1000);
            smh.enter(SmState.Evaluate);
            loopDevIdx = 0;
          }

          break;


        // ---------------------------------------------------------------------
        case Evaluate:                  // Evaluate Follower
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("Evaluate");
            }
          }

          dut = devFollower.getDevice(loopDevIdx);
          // getDevice liefert null, wenn Idx zu groß für die Liste
          if(dut == null)
          {
            loopDevIdx = 0;
            nrOfDevices = devFollower.deviceCount();
            // Anzahl der im Netz befindlichen Config-Twitter
            // Die inzwischen ausgeschalteten werden mitgezählt, weil die Liste bleibt
            if(lastNrOfDevices != nrOfDevices)
            {
              lastNrOfDevices = nrOfDevices;
              nrDevVal.replace(""+nrOfDevices);
            }
            break;
          }

          inputTimeOut = devFollower.deviceInputLag(loopDevIdx) > 1500;
          if(inputTimeOut)
            currentInfStatus = lsDevMacAdr.infoStatIdxTimeOut;
          else
            currentInfStatus = lsDevMacAdr.infoStatIdxOk;

          lsDevMacAdr.addOrReplace(dut.macAdrStr,loopDevIdx,currentInfStatus);
          lsDevIpAdr.addOrReplace(dut.ipAdrStr,loopDevIdx,currentInfStatus);
          lsDevId.addOrReplace("" + dut.deviceKey,loopDevIdx,currentInfStatus);
          lsDevName.addOrReplace(dut.deviceName,loopDevIdx,currentInfStatus);
          lsDevPos.addOrReplace
              ("x=" + dut.posX + " y=" + dut.posY + " z=" + dut.posZ, loopDevIdx, currentInfStatus);
          lsDevBasMod.addOrReplace("" + dut.baseMode, loopDevIdx, currentInfStatus);
          lsDevBasStat.addOrReplace("" + dut.baseState, loopDevIdx, currentInfStatus);
          lsDevTimeOut.addOrReplace
                ("" + devFollower.devInputLagCount(loopDevIdx,1500),
                    loopDevIdx, currentInfStatus);
          lsDevLostPdu.addOrReplace(""+dut.lostPduCount, loopDevIdx, currentInfStatus);

          if(smh.timeOut())
          {
            if(smh.oneShot())
              LabSpinner.clearAnySelected();
            if (newSelectedDevIdx >= 0)
              lastSelectedDevIdx = newSelectedDevIdx;
            int newSelection = LabSpinner.getLastNewSelectedPos();
            if (newSelection >= 0)
            {
              newSelectedDevIdx = newSelection;
              LabSpinner.setAllSelections(newSelectedDevIdx);
              smh.setOneShot();
              smh.setTimeOut(2000);
            }
            else
              smh.setTimeOut(1000);
          }
          loopDevIdx++;

          if(curPage == incPos)
          {
            dut = devFollower.getDevice(newSelectedDevIdx);
            devTwitter.baseMode = GlobalDHA.cmPOS;
            devTwitter.baseState = GlobalDHA.csStartPOS;
            devTwitter.setIntValue(0,dut.deviceKey);

            devKeyVal.print("" + dut.deviceKey);
            devNameVal.print(dut.deviceName);

            smh.enter(SmState.ConfigPos);
          }
          break;


        // ---------------------------------------------------------------------
        case ConfigPos:                     // Configurate local position
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("ConfigPos");
            }
          }

          oldPosX.print("" + dut.posX);
          oldPosY.print("" + dut.posY);
          oldPosZ.print("" + dut.posZ);

          int pos;

          if(butSetClicked)
          {
            inTextX = etInPosX.getText().toString();
            inTextY = etInPosY.getText().toString();
            inTextZ = etInPosZ.getText().toString();

            try
            {
              pos = Integer.parseInt(inTextX);
              devTwitter.setIntValue(1,pos);
            }
            catch (Exception e)
            {
              devTwitter.setIntValue(1,-1);
            }

            try
            {
              pos = Integer.parseInt(inTextY);
              devTwitter.setIntValue(2, pos);
            }
            catch (Exception e)
            {
              devTwitter.setIntValue(2,-1);
            }

            try
            {
              pos = Integer.parseInt(inTextZ);
              devTwitter.setIntValue(3, pos);
            }
            catch (Exception e)
            {
              devTwitter.setIntValue(3,-1);
            }

            devTwitter.baseState++;
            butSetClicked = false;
          }

          if(curPage == incInfo)
          {
            smh.enter(SmState.Evaluate);
            break;
          }


          break;

        // ---------------------------------------------------------------------
        case Error:                         // Error (severe)
        // ---------------------------------------------------------------------
          if(smh.firstEnter())
          {
            if (debugSM)
            {
              log.println("Error");
            }
            log.println(infoMsg);
          }

          break;

        default:
          throw new IllegalStateException("Unexpected value: " + smh.nextState);
      }
    }
    smh.end();
  }


}
