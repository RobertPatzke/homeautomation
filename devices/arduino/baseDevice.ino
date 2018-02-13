// ---------------------------------------------------------------------------
// File:        baseDevice.ino
// Editors:     Robert Patzke,
// Start:       11. November 2017
// Last change: 11. November2017
// URI/URL:     www.mfp-portal.de
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
#include "baseDevice.h"

//#define	smnDebug

#include "LoopCheck.h"
// We will use LoopCheck to control the timing of the device independent from
// the resources (timers) of the microcontroller.

#include "SocManNet.h"
// SocManNet is the interface to the broadcast communication and is needed
// by Twitter and Follower

#include "Twitter.h"
// Twitter sends cyclic broadcast messages

#include "Follower.h"
// Follower filters broadcast messages on receiving

// ---------------------------------------------------------------------------
// DEFINITIONS
// Definitions are used here to adjust the device (e.g. time behaviour) to
// our needs.
// ---------------------------------------------------------------------------
//
#define SMN_DEVICE_NAME     "BaseDeviceDHA"
#define SMN_TWITTER_NAME    "MyFirstCommObj"
#define SMN_FOLLOWER_NAME   "TestTwitter"

#define SM_CYCLE        5
// The cycle time (clock) of the state machine in milliseconds

#define SM_FREQUENCY    (1000 / SM_CYCLE)
// The frequency of the state machine

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
smnStatePtr nextState;      // Pointer to next state function

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
  Serial.begin(115200);         // using serial interface for visualisation

  nextState = smInit;           // state machine will start at smInit

  smnError =
    socManNet.init(false);      // Start connecting to the network with the
                                // IP-Address from socManNetUser.h
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

  if(loopCheck.timerMilli(0, SM_CYCLE, 0))  // clock for the state machine
  {
    nextState();    // switching to the next state (defined by passed state)
  }

  if(loopCheck.timerMilli(1, 2, 0))
    devTwitter.run(500);   // giving the CPU to devTwitter for its tasks
  //
  // This happens every 2 milliseconds, which is a frequency of 500 Hz.
  // Twitter.run needs the frequency to calculate the twitter cycle time


  if(loopCheck.timerMicro(2, 10, 0))
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
int     smTimeOut;      // Counter for time out control
int     smDelay;        // Counter for delay control
boolean doTimeRefresh;  // One-shot for immediate setup time from twitter

// ---------------------------------------------------------------------------
// Special (first) Initialisation
// ---------------------------------------------------------------------------
//
void smInit()
{
  // Inform user about init parameters
  //
  if(smnError != smnError_none)
  {
    Serial.print(socManNet.getErrorMsg(smnError));
    nextState = smStartWithoutSMN;
    return;
  }

  // Tell user network parameters (for check)
  //
  socManNet.getIfInfo(&smnInfo);
  Serial.print(smnInfo.macAdrCStr);
  Serial.print(" / ");
  Serial.println(smnInfo.ipAdrCStr);

  // Set parameters for the next state
  //
  smTimeOut = 10 * SM_FREQUENCY;        // 10 seconds time-out for next state
  nextState = smWaitForSMN;
}

// ---------------------------------------------------------------------------
// Waiting for being connected to the network
// ---------------------------------------------------------------------------
//
void smWaitForSMN()
{
  smTimeOut--;                      // serve time out counter

  if(socManNet.connected)
    nextState = smInitTwitter;      // continue there when connected

  if(smTimeOut <= 0)
  {
    Serial.println("Time-Out with network connection");
    nextState = smStartWithoutSMN;  // continue there when time out
  }
}

// ---------------------------------------------------------------------------
// Initialise Twitter
// ---------------------------------------------------------------------------
//
void smInitTwitter()
{
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
  nextState = smInitFollower;
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

  nextState = smWaitForTestTwitter;
  smTimeOut = 5 * SM_FREQUENCY;
}

// ---------------------------------------------------------------------------
// Waiting for a telegram from TestTwitter
// ---------------------------------------------------------------------------
//
void smWaitForTestTwitter()
{
  if(devFollower.recParseCounter > 2)   // recParseCounter is incremented
  {                                     // with each telegram recognised by
    devTwitter.baseState = smpsRun;     // Follower. So we wait here, until
    nextState = smDisplayValues;        // Follower has got 3 telegrams.
    smDelay = SM_FREQUENCY / 10;        // next state parameter
    doTimeRefresh = true;               // setup time immediately
    return;
  }

  if(smTimeOut > 0)                     // we will wait 5 seconds for the
  {                                     // telegrams from TestTwitter
    smTimeOut--;
    return;
  }

  Serial.println("TestTwitter not detected");
  smDelay = 10 * SM_FREQUENCY;
  nextState = smStartWithoutTTW;        // Time-out, start without TestTwitter
}

// ---------------------------------------------------------------------------
// Show received values
// ---------------------------------------------------------------------------
//
void smDisplayValues()
{
  // We know, that TestTwitter is sending once a second.
  // So it makes no sence, to ask every 5 milliseconds (SM_CYCLE) for a new
  // value. It is quick enough, if we ask only ten times a second.
  //
  if(smDelay > 0)
  {
    smDelay--;
    return;
  }

  smDelay = SM_FREQUENCY / 10;  // Set delay for the next check of Follower

  // For this example we will show only the value of the first integer value
  // received from Testwitter, when the content changed.
  //
  devFollower.getValue(&intMan1);
  if(intMan1.newValue)
  {
    Serial.println(intMan1.value);
  }

  // And we will display the text from TestTwitter, whenever a new telegram
  // comes in without respect to the change of content.
  //
  devFollower.getValue(&textMan1);
  if(textMan1.newPdu)
  {
    Serial.println(textMan1.value);
  }

  // Update the time in LoopCheck from TestTwitter every hour
  //
  loopCheck.getDateTime(&dt);
  if(dt.Minute == 0 || doTimeRefresh)
  {
    loopCheck.setDateTime(devFollower.timeString);
    doTimeRefresh = false;
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
  if(smDelay > 0)
  {
    smDelay--;
    return;
  }

  smTimeOut = 5 * SM_FREQUENCY;
  nextState = smWaitForTestTwitter;
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
  if(smDelay > 0)
  {
    smDelay--;
    return;
  }

  smDelay = SM_FREQUENCY;
  socManNet.getStatistic(statData);
  Serial.print(statData);
}



