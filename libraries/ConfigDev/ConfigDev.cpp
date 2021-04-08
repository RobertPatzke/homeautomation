// ---------------------------------------------------------------------------
// File:        ConfigDevice.cpp
// Editors:     Robert Patzke,
// Start:       14. Februar 2021
// Last change: 14. Februar 2021
// URI/URL:     www.mfp-portal.de / homeautomation.x-api.de/wikidha
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include  "ConfigDev.h"


// ---------------------------------------------------------------------------
// construction and initilisation
// ---------------------------------------------------------------------------
//
byte            ConfigDev::workMem[256];

ConfigMem       *ConfigDev::cfmPtr;
SocManNet       *ConfigDev::smnPtr;
Twitter         *ConfigDev::twPtr;
Follower        *ConfigDev::foPtr;
StateMachine    *ConfigDev::stmPtr;
LoopCheck       *ConfigDev::lchkPtr;

unsigned long   ConfigDev::startOpTime;
unsigned long   ConfigDev::stopOpTime;
unsigned long   ConfigDev::diffOpTime;

int             ConfigDev::confPin;

int             ConfigDev::twInt1, ConfigDev::twInt2, ConfigDev::twInt3, ConfigDev::twInt4;
double          ConfigDev::twFloat1, ConfigDev::twFloat2, ConfigDev::twFloat3, ConfigDev::twFloat4;
char            ConfigDev::twStr1[32], ConfigDev::twStr2[32], ConfigDev::twStr3[32], ConfigDev::twStr4[32];

IntegerValue    ConfigDev::foInt1, ConfigDev::foInt2, ConfigDev::foInt3, ConfigDev::foInt4;
FloatValue      ConfigDev::foFloat1, ConfigDev::foFloat2, ConfigDev::foFloat3, ConfigDev::foFloat4;
TextValue       ConfigDev::foStr1, ConfigDev::foStr2, ConfigDev::foStr3, ConfigDev::foStr4;

bool            ConfigDev::useAppKey;    // Separation in one Network
int             ConfigDev::appKey;       // App identification

Twitter::Int    twInt01(0);
Twitter::Int    twInt02(1);
Twitter::Int    twInt03(2);
Twitter::Int    twInt04(3);

Twitter::Float  twFloat01(0);
Twitter::Float  twFloat02(1);
Twitter::Float  twFloat03(2);
Twitter::Float  twFloat04(3);

Twitter::Text   twStr01(0);
Twitter::Text   twStr02(1);
Twitter::Text   twStr03(2);
Twitter::Text   twStr04(3);


void ConfigDev::begin(ConfigMem *cfm, SocManNet *smn, Twitter *twi,
                      Follower *fol, StateMachine *sm, LoopCheck *lchk)
{
  cfmPtr  = cfm;
  smnPtr  = smn;
  twPtr   = twi;
  foPtr   = fol;
  stmPtr  = sm;
  lchkPtr = lchk;

  twPtr->pinVar(&twInt01);
  twPtr->pinVar(&twInt02);
  twPtr->pinVar(&twInt03);
  twPtr->pinVar(&twInt03);

  twPtr->pinVar(&twFloat01);
  twPtr->pinVar(&twFloat02);
  twPtr->pinVar(&twFloat03);
  twPtr->pinVar(&twFloat04);

  twPtr->pinVar(&twStr01);
  twPtr->pinVar(&twStr02);
  twPtr->pinVar(&twStr03);
  twPtr->pinVar(&twStr04);
}

// -------------------------------------------------------------------------
// local functions
// -------------------------------------------------------------------------
//
// set empty/default values for twitter
//
void ConfigDev::presetTwitter()
{
  twInt01 = 1;
  twInt02 = 2;
  twInt03 = 3;
  twInt04 = 4;

  twFloat01 = 0.1;
  twFloat02 = 0.2;
  twFloat03 = 0.3;
  twFloat04 = 0.4;

  twStr01 = "A";
  twStr02 = "B";
  twStr03 = "C";
  twStr04 = "D";
 }


// set parameters for follower value structures
//
void  ConfigDev::initFollower()
{
  foInt1.recDsc.idx   = 0;
  foInt2.recDsc.idx   = 1;
  foInt3.recDsc.idx   = 2;
  foInt4.recDsc.idx   = 3;

  foFloat1.recDsc.idx = 0;
  foFloat2.recDsc.idx = 1;
  foFloat3.recDsc.idx = 2;
  foFloat4.recDsc.idx = 3;

  foStr1.recDsc.idx   = 0;
  foStr2.recDsc.idx   = 1;
  foStr3.recDsc.idx   = 2;
  foStr4.recDsc.idx   = 3;
}

// get values from follower receive memory
//
void  ConfigDev::updateFollower()
{
  foPtr->resetAllValueFlags();

  foPtr->getValue(&foInt1);
  foPtr->getValue(&foInt2);
  foPtr->getValue(&foInt3);
  foPtr->getValue(&foInt4);

  foPtr->getValue(&foFloat1);
  foPtr->getValue(&foFloat2);
  foPtr->getValue(&foFloat3);
  foPtr->getValue(&foFloat4);

  foPtr->getValue(&foStr1);
  foPtr->getValue(&foStr2);
  foPtr->getValue(&foStr3);
  foPtr->getValue(&foStr4);
}

// -------------------------------------------------------------------------
// user functions   /   state machine
// -------------------------------------------------------------------------
//

// -------------------------------------------------------------------------
// subroutine states
// -------------------------------------------------------------------------
//

void ConfigDev::smcCheckRuntime()
{
  STNR(40)

  if(stmPtr->firstEnter())
  {
  }

  if(cfmPtr->getOnOffValue() < DevOnSECONDS)    // OnOff-Value is incremented
  {                                             // until it reaches maximum
    // Do periodically
    //
    if(stmPtr->getTimeMeasure(false) >= 2000)   // every 2 seconds
    {
      cfmPtr->incOnOffValue(false);             // Increment value
      cfmPtr->incOnOffValue(true);              // Increment value
      stmPtr->startTimeMeasure();               // Reset second counter
#ifdef DebConfigDev
      smnSerial.print("OnOffValue incremented to ");
      smnSerial.println(cfmPtr->getOnOffValue());
#endif
    }
  }
  else
  {
    // Do every hour
    //
    if(stmPtr->getExtTimeMeasure(false) >= 3600000)
    {
      cfmPtr->setOperCounter(startOpTime + lchkPtr->getRuntime(), true);
      stmPtr->startTimeMeasure();               // Reset second counter
#ifdef DebConfigDev
      smnSerial.print("Operation Time set to ");
      smnSerial.println(cfmPtr->getOperCounter());
#endif
    }
  }

  stmPtr->enter();
}


// -------------------------------------------------------------------------
// Initialisation
// -------------------------------------------------------------------------
//

// First state, has to be initialised for state machine by parent program
//
void ConfigDev::smInit()
{
  STNR(0)

  // Warning: firstEnter() does not work in initialisation state


  stmPtr->startTimeMeasure();           // measure time to define power
                                        // controlled configuration setting

  if(cfmPtr->getOnOffValue() < DevOnSECONDS)  // if the last power on was
  {                                           // less than DevOnSECONDS
    cfmPtr->incOnOffCounter(false);           // inrement power On/Off counter
#ifdef DebConfigDev
      smnSerial.print("smInit: OnOffValue is ");
      smnSerial.println(cfmPtr->getOnOffValue());
      smnSerial.print("OnOffCounter is ");
      smnSerial.println(cfmPtr->getOnOffCounter());
#endif

  }
  else
    cfmPtr->setOnOffCounter(0, false);  // else reset power On/Off counter

  cfmPtr->setOnOffValue(0, false);      // reset power On timer

  startOpTime = cfmPtr->getOperCounter();   // mark start time from prom

  stmPtr->enter(smCheck);
}

// Check if a configuration pin is defined in configuration data
// If so, it will be used for enabling configuration by level LOW (0)
//
void ConfigDev::smCheck()
{
  STNR(10)

  if(stmPtr->firstEnter())
  {
  }

  confPin = cfmPtr->getConfPin();       // Number of configuration pin
  if (confPin < 0)                      // not defined when negative
    stmPtr->enter(smNoConfPin);
  else
  {
    pinMode(confPin, INPUT_PULLUP);     // Prepare configuration for input
    stmPtr->enter(smWaitConfPin);       // Next state is waiting
  }

#ifdef DebConfigDev
  stmPtr->setCondCounter(1);            // Prepare a one-shot if debugging
#endif
}


void ConfigDev::smNoConfPin()           // Prepare devices without
{                                       // configuration pin
  STNR(12)

  if(stmPtr->firstEnter())
  {
  }

  if(cfmPtr->getOnOffCounter() == 1)    // configursation may be started by
  {                                     // one short power ON
    NEXT(smStart)
  }
  else
  {
    NEXT(smNoConfig)
  }

#ifdef DebConfigDev
  stmPtr->setCondCounter(1);        // Prepare a one-shot if debugging
#endif
}

void ConfigDev::smNoConfig()            // Handle normal devices
{                                       // without configuration
  STNR(15)

  if(stmPtr->firstEnter())
  {
#ifdef DebConfigDev
    if(stmPtr->condOpen())
    {
      smnSerial.println("Device cannot be configured");
    }
#endif
  }

  stmPtr->call(smcCheckRuntime,100);
}

// -------------------------------------------------------------------------
// Waiting for peripheral action
// -------------------------------------------------------------------------
//

void ConfigDev::smWaitConfPin()         // Waiting is split in two states
{                                       // to overcom switch jitters
  int   pinVal;                         // First is waiting endlessly
                                        // until pin is LOW
  STNR(20)

  if(stmPtr->firstEnter())
  {
#ifdef DebConfigDev
    if(stmPtr->condOpen())
    {
      smnSerial.println("Waiting for ConfPin = 0:");
      smnSerial.print("CP = ");
      smnSerial.print(confPin);
      smnSerial.print("    Val = ");
      smnSerial.println(digitalRead(confPin));
    }
#endif
  }

  pinVal = digitalRead(confPin);        // Read value of configuration pin
  if(pinVal == 0)                       // If configuration is requested
    stmPtr->enter(smAckConfPin,50);     // check for stability
  else
    stmPtr->call(smcCheckRuntime);      // continue waiting with power on control
}

void ConfigDev::smAckConfPin()          // Second is to check the input
{                                       // again after 0.1 s
  int   pinVal;

  STNR(21)

  if(stmPtr->firstEnter())
  {
  }

  pinVal = digitalRead(confPin);        // Read value of configuration pin
  if(pinVal == 0)                       // If request is stable
  {
#ifdef DebConfigDev
    stmPtr->setCondCounter(1);          // Prepare a one-shot if debugging
#endif
    NEXT(smStart)                       // start configuration
  }
  else
    NEXT(smWaitConfPin)                 // else go on waiting
}

// -------------------------------------------------------------------------
// Start configuration process
// -------------------------------------------------------------------------
//

void ConfigDev::smStart()
{
  STNR(30)

  if(stmPtr->firstEnter())
  {
#ifdef DebConfigDev
    if(stmPtr->condOpen())
    {
      smnSerial.println("Device is prepared for configuration");
    }
#endif
  }

  if(!smnPtr->connected)
  {
    stmPtr->call(smcCheckRuntime);  // over smcCheckRuntime
    return;                         // stay here
  }

  useAppKey = cfmPtr->getAppKey(&appKey);

  NEXT(smInitTwitter)
}

void ConfigDev::smInitTwitter()
{
  STNR(32)

  if(stmPtr->firstEnter())
  {
  }

  // There are some parameters, which have to be set befor the basic
  // initialisation of twitter is done. Because these values are used there.
  //
  twPtr->setDeviceKey();         // The device key is used by FollowMultDev
                                // to distinguish different twitter with
                                // the same object name
  // Calling setDeviceKey without parameter (= device key) or omitting the call
  // creates a device key from the lower 2 bytes of MAC address.

  twPtr->setApplicationKey(0);  // The application key defines a relation
                                    // (or connection) between different devices
  // with respect to their tasks in a common application.
  // The value 0 stands for "do not care".

  twPtr->init
  (
    smnPtr,                     // Twitter needs a reference to the network
    lchkPtr->refDateTime(),     // and a reference to a time string (RTC)
    "ConfigDev",                // and the name of the communication object
    4,                          // the number of Integer to send (<= 4)
    4,                          // the number of Floats to send (<= 4)
    4,                          // the number of Text strings to send (<= 4)
    normalSpeed                 // and the cyclic speed
  );                            // highSpeed=10Hz, normalSpeed=1Hz, lowSpeed=0.1Hz

  // There are more parameters with Twitter, which will be send
  //

  cfmPtr->getDeviceName(workMem);

  twPtr->setDeviceName((char *) workMem);
  // Tell the world, who you are

  twPtr->setDeviceState(12);        // status of your device (handbook)

  twPtr->posX = cfmPtr->getPos(0);  // X-position of your device
                                    // (local coordinates in cm)

  twPtr->posY = cfmPtr->getPos(1);  // Y-position of your device
                                    // (local coordinates in cm)

  twPtr->posZ = cfmPtr->getPos(2);  // Z-position of your device
                                    // (local coordinates in cm)

  twPtr->baseState = stmPtr->curStateNumber;  // Current state
                                    // which we present to the world

  twPtr->baseMode = 0;              // A number describing your plan/wish
                                    // (explain it in handbook)

  // Giving some default values for your twitter variables
  //
  presetTwitter();

  // Start twitter
  twPtr->setBurst(burst3,0);
  twPtr->enabled = true;

  NEXT(smInitFollower)
}


void ConfigDev::smInitFollower()
{
  STNR(34)

  if(stmPtr->firstEnter())
  {
  }

  // Initialize follower for the Configurator
  //
  foPtr->init(smnPtr, (char *) "Configurator");
  initFollower();
  foPtr->enabled = true;    // Start Follower

  NEXT(smWaitForConf)
}


void ConfigDev::smWaitForConf()
{
  STNR(36)

  if(stmPtr->firstEnter())
  {
  }

  updateFollower();             // get all follower data
  if(!foPtr->allNewPdu)         // if there is no new pdu
  {
    stmPtr->call(smcCheckRuntime);  // over smcCheckRuntime
    return;                         // stay here
  }

  switch(foPtr->baseMode)
  {
    case cmWLAN:
      NEXT(smStartWLAN)
      break;

    case cmPOS:
      NEXT(smStartPOS)
      break;

    default:
      break;
  }
}

// -------------------------------------------------------------------------
// Konfiguration of new WLAN Access Parameters
// -------------------------------------------------------------------------
//

void ConfigDev::smStartWLAN()
{
  STNR(cmWLAN)

  if(stmPtr->firstEnter())
  {
  }

  // Set Twitter for WLAN configuration
  //
  twPtr->baseState = cmWLAN;            // Basis-Status setzen

  cfmPtr->getNetName((byte *) twStr1);  // Net name from ConfigMem
  cfmPtr->getNetPass((byte *) twStr2);  // Net password from ConfigMem

  NEXT(smWaitWLAN)
}


void ConfigDev::smWaitWLAN()
{
  STNR(cmWLAN + 2)

  if(stmPtr->firstEnter())
  {
  }

  updateFollower();             // get all follower data
  if(!foPtr->allNewPdu)         // if there is no new pdu
  {
    stmPtr->call(smcCheckRuntime);
    return;
  }


}



// -------------------------------------------------------------------------
// Konfiguration of new POS Parameters
// -------------------------------------------------------------------------
//

void ConfigDev::smStartPOS()
{
  STNR(cmPOS)

  if(stmPtr->firstEnter())
  {
#ifdef DebConfigDev
    smnSerial.println("CD:StartPOS");
#endif
  }

  // Set Twitter for POS configuration
  //
  twPtr->baseState = cmPOS;             // set base state

  twPtr->posX = cfmPtr->getPos(0);
  twPtr->posY = cfmPtr->getPos(1);
  twPtr->posZ = cfmPtr->getPos(2);
  NEXT(smWaitPOS)
}


void ConfigDev::smWaitPOS()
{
  STNR(cmPOS + 2)

  if(stmPtr->firstEnter())
  {
  }

  updateFollower();             // get all follower data
  if(!foPtr->allNewPdu)         // if there is no new pdu
  {
    stmPtr->call(smcCheckRuntime);
    return;
  }


}


