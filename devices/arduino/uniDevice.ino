// ---------------------------------------------------------------------------
// File:        uniDevice.ino
// Editors:     Robert Patzke,
// Start:       10. Februar 2021
// Last change: 17. Februar 2021
// URI/URL:     www.mfp-portal.de / homeautomation.x-api.de/wikidha
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//
// Note!
// This is an example, how devices for Decentral Home Automation (DHA) may be
// programmed.
// It is important, that such devices run a basic application task, if there
// is no communication via Social Manufacturing Network (SMN).
// DHA devices are software agents (at least partly).
//

#include "Arduino.h"
#include "uniDevice.h"

//#define	smnDebug

#include "LoopCheck.h"
// We will use LoopCheck to control the timing of the device independent from
// the resources (timers) of the microcontroller.

#include "StateMachine.h"
// The auxiliary class StateMachine provides methods for a comfortable
// handling of state sequences (timing, etc.)

#include "SocManNet.h"
// SocManNet is the interface to the broadcast communication and is needed
// by Twitter and Follower

#include "Twitter.h"
// Twitter sends cyclic broadcast messages

#include "Follower.h"
// Follower filters broadcast messages on receiving

#include  "ConfigMem.h"
#include  "ConfigDev.h"
// Store configuration data non volatile

// ---------------------------------------------------------------------------
// DEFINITIONS
// Definitions are used here to adjust the device (e.g. time behaviour) to
// our needs.
// ---------------------------------------------------------------------------
//
#define SMN_DEVICE_NAME     "BaseDeviceDHA"
#define SMN_TWITTER_NAME    "MyFirstCommObj"
#define SMN_FOLLOWER_NAME   "TestTwitter"


// ---------------------------------------------------------------------------
// INSTANCES and VARIABLES
// Class instances and variables defined outside functions to use them in
// any function
// ---------------------------------------------------------------------------
//
LoopCheck   loopCheck;      // Instance of LoopCheck
SocManNet   socManNet;      // Instance of SocManNet
Twitter     devTwitter;     // Instance of Twitter
Follower    devFollower;    // Instance of Follower

Twitter     confTwitter;    // Instance of Configuration Twitter
Follower    confFollower;   // Instance of Configuration Follower


// basic definitions for the state machine
//
#define AutCycleTime    5
void smInit();      // forward reference to state smInit
void smCycle();     // forward reference to state smCycle
StateMachine  automat(smInit, smCycle, AutCycleTime);

#define ConfCycleTime   10
StateMachine  config(ConfigDev::smInit,NULL,ConfCycleTime, micros);

ConfigMem confMem;          // Configuration data and tools

lcDateTime  dt;             // Time-Structure of LoopCheck

// Variables to be twittered
//
int     appIntVar1, appIntVar2, appIntVar3;
double  appFloatVar1, appFloatVar2;
char    *appTextPtr1;

// Variables (management structures) expected by Follower
//
IntegerValue    intMan1, intMan2, intMan3;
FloatValue      floatMan1, floatMan2;
TextValue       textMan1;

// Variables for debugging and error handling
//
SocManNetError	smnError;
SmnIfInfo       smnInfo;

// ---------------------------------------------------------------------------
// SETUP
// The setup function is called once at startup of the sketch
// ---------------------------------------------------------------------------
//
void setup()
{
  // -------------------------------------------------------------------------
  // High-Speed Area, no background process running
  // -------------------------------------------------------------------------

  confMem.begin(1);             // Initialise EEPROM with 1 page

#ifdef  DebConfigMem
  confMem.promClear();
#endif

  if(confMem.promHasData() == false)
    confMem.promInit();

  // -------------------------------------------------------------------------
  // Methods enabling background processes
  // -------------------------------------------------------------------------

  smnSerial.begin(115200);         // using serial interface for visualisation

  smnError =
    socManNet.init(true);       // Start connecting to the network with
                                // DHCP

  // -------------------------------------------------------------------------
  // Low-Speed Area, background processes running
  // -------------------------------------------------------------------------

  ConfigDev::begin(&confMem, &socManNet, &confTwitter, &confFollower,
                   &config, &loopCheck);
}

// ---------------------------------------------------------------------------
// LOOP
// The loop function is called in an endless loop
// ---------------------------------------------------------------------------
//
void loop()
{
  loopCheck.begin();    // mandatory call at loop beginning to use LoopCheck
  // -------------------------------------------------------------------------

  if(loopCheck.timerMilli(lcTimer0, AutCycleTime, 0)) // clock of state machine
  {
    automat.run();      // switching to the next state (defined by passed state)
  }

  if(loopCheck.timerMilli(lcTimer1, 2, 0))
    devTwitter.run(500);   // giving the CPU to devTwitter for its tasks
  //
  // This happens every 2 milliseconds, which is a frequency of 500 Hz.
  // Twitter.run needs the frequency to calculate the twitter cycle time

  if(loopCheck.timerMilli(lcTimer2, 10, 0, 1000))
  {
    config.run();
  }

  if(loopCheck.timerMilli(lcTimer3, 2, 0))
    confTwitter.run(500);   // giving the CPU to confTwitter for its tasks

  // Aktuelles Debugging
  //
  if(loopCheck.timerMilli(lcTimer4, 250, 0))
  {
    if(loopCheck.timerCycle(lcTimer4) == 4)
    {
      smnSerial.print("Duration begin() = ");
      smnSerial.println(confMem.durBegin);
    }

    if(loopCheck.timerCycle(lcTimer4) == 5)
    {
      smnSerial.print("Duration promClear() = ");
      smnSerial.println(confMem.durClear);
    }
    if(loopCheck.timerCycle(lcTimer4) == 6)
    {
      smnSerial.print("Duration promInit() = ");
      smnSerial.println(confMem.durInit);
    }
  }

#ifdef  DebConfigDev

  if(loopCheck.timerMilli(lcTimer5, 250, 0, 5000))
  {
    if( (loopCheck.timerCycle(lcTimer5) == 4) || (loopCheck.timerCycle(lcTimer5) == 16) )
    {
      smnSerial.print("MaxRuntime(0) = ");
      smnSerial.print(config.maxStateRuntime[0]);
      smnSerial.print("  StateNumber = ");
      smnSerial.println(config.maxRuntimeNumber[0]);
    }

    if( (loopCheck.timerCycle(lcTimer5) == 5) || (loopCheck.timerCycle(lcTimer5) == 17) )
    {
      smnSerial.print("MaxRuntime(1) = ");
      smnSerial.print(config.maxStateRuntime[1]);
      smnSerial.print("  StateNumber = ");
      smnSerial.println(config.maxRuntimeNumber[1]);
    }

    if( (loopCheck.timerCycle(lcTimer5) == 6) || (loopCheck.timerCycle(lcTimer5) == 18) )
    {
      smnSerial.print("MaxRuntime(2) = ");
      smnSerial.print(config.maxStateRuntime[2]);
      smnSerial.print("  StateNumber = ");
      smnSerial.println(config.maxRuntimeNumber[2]);
    }

    if( (loopCheck.timerCycle(lcTimer5) == 7) || (loopCheck.timerCycle(lcTimer5) == 19) )
    {
      smnSerial.print("MaxRuntime(3) = ");
      smnSerial.print(config.maxStateRuntime[3]);
      smnSerial.print("  StateNumber = ");
      smnSerial.println(config.maxRuntimeNumber[3]);
      smnSerial.println("---");
    }

  }

#endif


  if(loopCheck.timerMicro(lcTimer9, 10, 0))
    socManNet.run();        // giving the CPU to socManNet for its tasks
  //
  // This may happen every 10 microseconds. But we should expect,
  // that the loop cycle time (Arduino background loop() calling distance)
  // is longer than 10 microseconds.
  // So run() will be called with every loop(), if no other LoopCheck timer
  // finishes before in loop().
  // Thus, such a usage of timerMicro can only work, if it is the last
  // timer in loop, as timers behind this too short timer will not
  // come to be finished, because we have allowed only one timer finishing
  // in the same loop cycle (to avoid CPU load peaks).

  // -------------------------------------------------------------------------
  loopCheck.end();      // mandatory call at loop ending to use LoopCheck
}

// ---------------------------------------------------------------------------
// Hilfsfunktionen
// ---------------------------------------------------------------------------
//
void updateTwitter()
{
  devTwitter.setIntValue(0, appIntVar1);  // First integer in broadcast message
  devTwitter.setIntValue(1, appIntVar2);  // Second integer
  devTwitter.setIntValue(2, appIntVar3);  // Third integer
  // That are 3 values for the 3 integer variables we declared with
  // devTwitter.init

  devTwitter.setFloatValue(0, appFloatVar1); // First float in broadcast message
  devTwitter.setFloatValue(1, appFloatVar2); // Second float
  // That are 2 values for the 2 float variables we declared with
  // devTwitter.init

  devTwitter.setTextValue(0, appTextPtr1);
  // One value for the one string variable we declared with devTwitter.init
}

// ***************************************************************************
//                      S T A T E   M A C H I N E
// ***************************************************************************
//

// ---------------------------------------------------------------------------
// Special state called with each clock of state machine (without delay)
// ---------------------------------------------------------------------------
//
void smCycle()
{
  // if necessary, observe your processes here
}

// ---------------------------------------------------------------------------
// Special (first) Initialisation
// ---------------------------------------------------------------------------
//
bool setTestTwitterTime;

void smInit()
{
  // Inform user about init parameters
  //
  if(smnError != smnError_none)
  {
    smnSerial.print(socManNet.getErrorMsg(smnError));
    automat.enter(smStartWithoutSMN);   // next state is smStartWithoutSMN
    return;
  }

  // Set parameters for the next state
  //
  automat.setTimeOut(10000);            // 10 seconds time-out for next state
  automat.enter(smWaitForSMN);          // next state is smWaitForSMN
}

// ---------------------------------------------------------------------------
// Waiting for being connected to the network
// ---------------------------------------------------------------------------
//
void smWaitForSMN()
{
  if(socManNet.connected)
    automat.enter(smInitTwitter);       // continue there when connected

  if(automat.timeOut())
  {
    smnSerial.println("Time-Out with network connection");
    automat.enter(smStartWithoutSMN);   // continue there when time out
  }
}

// ---------------------------------------------------------------------------
// Initialise Twitter
// ---------------------------------------------------------------------------
//
void smInitTwitter()
{
  // Tell user network parameters (for check)
  //
  socManNet.getIfInfo(&smnInfo);
  smnSerial.print(smnInfo.macAdrCStr);
  smnSerial.print(" / ");
  smnSerial.println(smnInfo.ipAdrCStr);

  // There are some parameters, which have to be set befor the basic
  // initialisation of twitter is done. Because these values are used there.
  //
  devTwitter.setDeviceKey();    // The device key is used by FollowMultDev
                                // to distinguish different twitter with
                                // the same object name
  // Calling setDeviceKey without parameter (= device key) or omitting the call
  // creates a device key from the lower 2 bytes of MAC address.

  devTwitter.setApplicationKey(0);  // The application key defines a relation
                                    // (or connection) between different devices
  // with respect to their tasks in a common application.
  // The value 0 stands for "do not care".

  devTwitter.init
  (
    &socManNet,                 // Twitter needs a reference to the network
    loopCheck.refDateTime(),    // and a reference to a time string (RTC)
    (char *)SMN_TWITTER_NAME,   // and the name of the communication object
    3,                          // the number of Integer to send (<= 4)
    2,                          // the number of Floats to send (<= 4)
    1,                          // the number of Text strings to send (<= 4)
    normalSpeed                 // and the cyclic speed
  );                            // highSpeed=10Hz, normalSpeed=1Hz, lowSpeed=0.1Hz

  // There are more parameters with Twitter, which will be send
  //

  devTwitter.setDeviceName((char *) SMN_DEVICE_NAME);
  // Tell the world, who you are

  devTwitter.setDeviceState(12);    // status of your device (handbook)

  devTwitter.posX = 3667;           // X-position of your device
                                    // (local coordinates in cm)

  devTwitter.posY = 1088;           // Y-position of your device
                                    // (local coordinates in cm)

  devTwitter.posZ = 80;             // Z-position of your device
                                    // (local coordinates in cm)

  devTwitter.baseState = smpsInit;  // The state of your finite state machine
                                    // which you present to the world

  devTwitter.baseMode = 0;          // A number describing your plan/wish
                                    // (explain it in handbook)

  // Giving some default values for your twitter variables
  //
  appIntVar1 = 12;
  appIntVar2 = 345;
  appIntVar3 = -7;

  appFloatVar1 = 88.88;
  appFloatVar2 = 0.0758;

  appTextPtr1 = (char *) "Hi, it's me!";

  updateTwitter();

  // HINT:
  // The communication object name (here "MyFirstCommObj") and the data structure
  // (i.e. number of integer, float and text declared in Twitter.init) and
  // the meaning of their content are a matter of standardisation
  // (e.g. for the decentral homeautomation DHA)
  // The other parameters (e.g. device key) are mandatory for all devices.

  // There is no internal test for twitter running
  // so simply start Twitter and switch to next state
  //
  devTwitter.enabled = true;
  automat.enter(smInitFollower);
}

// ---------------------------------------------------------------------------
// Initialise Follower
// ---------------------------------------------------------------------------
//
void smInitFollower()
{
  devFollower.init(&socManNet, (char *) SMN_FOLLOWER_NAME);
  // with this example we follow "TestTwitter", which is the simple example
  // for Android smartphones on Github

  intMan1.recDsc.idx = 0;   // expect intMan1 as first int of TestTwitter
  intMan2.recDsc.idx = 1;   // expect intMan2 as second int of TestTwitter
  intMan3.recDsc.idx = 2;   // expect intMan3 as third int of TestTwitter

  floatMan1.recDsc.idx = 0; // expect floatMan1 as first float
  floatMan2.recDsc.idx = 1; // expect floatMan1 as second float

  textMan1.recDsc.idx = 0;  // only one text expected

  devFollower.enabled = true;   // start Follower

  smnSerial.println("Waiting for TestTwitter");
  automat.enter(smWaitForTestTwitter);
  automat.setTimeOut(5000);     // Time-out for next state 5 seconds
}

// ---------------------------------------------------------------------------
// Waiting for a telegram from TestTwitter
// ---------------------------------------------------------------------------
//
unsigned int recParseLastCount = 0;

void smWaitForTestTwitter()
{                                   // recParseCounter is incremented with
  if(                               // each telegram recognised by Follower
      devFollower.recParseCounter >     // So we wait here, until Follower
      (2 + recParseLastCount)           // received 3 telegrams more than
    )                                   // the latest marked value
  {
    devTwitter.baseState = smpsRun;     // Running with TestTwitter
    automat.enter(smDisplayValues,100); // Enter next state with delay 100 ms
    return;
  }

  if(automat.timeOut())
  {
    smnSerial.println("TestTwitter not detected");
    automat.enter(smStartWithoutTTW,1000); // Enter with delay 1 seconds
  }
}

// ---------------------------------------------------------------------------
// Show received values
// ---------------------------------------------------------------------------
//
unsigned int watchTestTwitter;

void smDisplayValues()
{
  if(automat.firstEnter())      // smDisplayValues will be repeated
  {                             // StateMachine.firstEnter() returns true
    watchTestTwitter = 0;       // only the first time a state is entered
    setTestTwitterTime = true;  // after another state was used
  }

  // For this example we will show only the value of the first integer value
  // received from Testwitter, when the content changed.
  //
  devFollower.getValue(&intMan1);
  if(intMan1.newValue)
  {
    smnSerial.println(intMan1.value);
  }

  // And we will display the text from TestTwitter, whenever a new telegram
  // comes in without respect to the change of content.
  //
  devFollower.getValue(&textMan1);
  if(textMan1.newPdu)
  {
    smnSerial.println(textMan1.value);
  }

  // Update the time in LoopCheck from TestTwitter on the minute
  //
  if(setTestTwitterTime)
  {
    loopCheck.getDateTime(&dt);
    if(dt.Second == 0)
    {
      loopCheck.setDateTime(devFollower.timeString);
      smnSerial.println(devFollower.timeString);
      setTestTwitterTime = false;
    }
  }

  // We know, that TestTwitter is sending once a second.
  // So it makes no sence, to ask every AutCycleTime for a new
  // value. It is quick enough, if we ask only ten times a second.
  //
  automat.setDelay(100);

  // Let us check the activity of TestTwitter again via recParseCounter
  // based on the status cycle time of 100 ms as defined by setDelay()
  //
  if(recParseLastCount != devFollower.recParseCounter)
  {
    // TestTwitter has sent a telegram
    recParseLastCount = devFollower.recParseCounter;
    watchTestTwitter = 0;
  }
  else
    watchTestTwitter++;

  if(watchTestTwitter > 30)
  {
    // no telegram from TestTwitter since 3 seconds
    //
    automat.enter(smStartWithoutTTW);
  }
}


// ---------------------------------------------------------------------------
// Start without TestTwitter
// ---------------------------------------------------------------------------
//
void smStartWithoutTTW()
{
  // With this simple example, the device has nothing to do if TestTwitter
  // is not present in the network. So here is another loop implemented
  // to wait until TestTwitter is online.
  //
  automat.setTimeOut(59000);
  automat.enter(smWaitForTestTwitter);
}

// ---------------------------------------------------------------------------
// Start without communication
// ---------------------------------------------------------------------------
//
void smStartWithoutSMN()
{

}


// ---------------------------------------------------------------------------
// Display SocManNet statistics
// ---------------------------------------------------------------------------
//
char statData[48];

void smDisplayStatisticsSMN()
{
  socManNet.getStatistic(statData);
  smnSerial.print(statData);
  automat.setDelay(1000);
}



