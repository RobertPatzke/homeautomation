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
#include "smnFollowMult.h"

//#define	smnDebug

#include "LoopCheck.h"
// We will use LoopCheck to control the timing of the device independent from
// the resources (timers) of the microcontroller.

#include "SocManNet.h"
// SocManNet is the interface to the broadcast communication and is needed
// by Twitter and Follower

#include "Twitter.h"
// Twitter sends cyclic broadcast messages

#include "FollowMultDev.h"
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
LoopCheck       loopCheck;      // Instance of LoopCheck
SocManNet       socManNet;      // Instance of SocManNet
FollowMultDev   multFollower;   // Instance of Follower
smnStatePtr     nextState;      // Pointer to next state function

// Variables to be twittered
//
int     appIntVar1, appIntVar2, appIntVar3;
double  appFloatVar1, appFloatVar2;
char    *appTextPtr1;

// Variables (management structures) expected by Follower
//
IntegerValueMD  intMan1[MAXNRSRC], intMan2[MAXNRSRC], intMan3[MAXNRSRC];
FloatValueMD    floatMan1[MAXNRSRC], floatMan2[MAXNRSRC];
TextValueMD     textMan1[MAXNRSRC];

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

  if(loopCheck.timerMicro(1, 10, 0))
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


// ***************************************************************************
//                      S T A T E   M A C H I N E
// ***************************************************************************
//
int     smTimeOut;      // Counter for time out control
int     smDelay;        // Counter for delay control

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
    nextState = smInitFollower;     // continue there when connected

  if(smTimeOut <= 0)
  {
    Serial.println("Time-Out with network connection");
    nextState = smStartWithoutSMN;  // continue there when time out
  }
}


// ---------------------------------------------------------------------------
// Initialise Follower
// ---------------------------------------------------------------------------
//
void smInitFollower()
{
  multFollower.init(&socManNet, (char *) SMN_FOLLOWER_NAME);
  // with this example we follow "TestTwitter", which is the simple example
  // for Android smartphones on Github

  for(int i = 0; i < MAXNRSRC; i++)
  {
  intMan1[i].recDsc.idx = 0;   // expect intMan1 as first int of TestTwitter
  intMan1[i].recDsc.deviceIdx = i;
  intMan2[i].recDsc.idx = 1;   // expect intMan2 as second int of TestTwitter
  intMan2[i].recDsc.deviceIdx = i;
  intMan3[i].recDsc.idx = 2;   // expect intMan3 as third int of TestTwitter
  intMan3[i].recDsc.deviceIdx = i;

  floatMan1[i].recDsc.idx = 0; // expect floatMan1 as first float
  floatMan1[i].recDsc.deviceIdx = i;
  floatMan2[i].recDsc.idx = 1; // expect floatMan1 as second float
  floatMan2[i].recDsc.deviceIdx = i;

  textMan1[i].recDsc.idx = 0;  // only one text expected
  textMan1[i].recDsc.deviceIdx = i;
  }

  multFollower.enabled = true; // start Follower

  nextState = smWaitForTestTwitter;
  smTimeOut = 5 * SM_FREQUENCY;
}

// ---------------------------------------------------------------------------
// Waiting for a telegram from TestTwitter
// ---------------------------------------------------------------------------
//
void smWaitForTestTwitter()
{
  if(multFollower.recParseCounter > 2)  // recParseCounter is incremented
  {                                     // with each telegram recognised by
                                        // Follower. So we wait here, until
    nextState = smDisplayValues;        // Follower has got 3 telegrams.
    smDelay = SM_FREQUENCY / 10;        // next state parameter
    return;                             // Then we are in status "running"
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
  // The number of devices twittering "TestTwitter" is
  // FollowMultDev.maxDeviceIdx + 1
  //
  for(int i = 0; i <= multFollower.maxDeviceIdx; i++)
  {
    multFollower.getValue(&intMan1[i]);
    if(intMan1[i].newValue)
    {
      Serial.print("Device Key = ");
      Serial.print(intMan1[i].recDsc.deviceInfo.deviceKey);
      Serial.print("  Value = ");
      Serial.println(intMan1[i].value);
    }
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



