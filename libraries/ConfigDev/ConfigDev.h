// ---------------------------------------------------------------------------
// File:        ConfigDevice.h
// Editors:     Robert Patzke,
// Start:       14. Februar 2021
// Last change: 14. Februar 2021
// URI/URL:     www.mfp-portal.de / homeautomation.x-api.de/wikidha
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#ifndef _ConfigDev_h
#define _ConfigDev_h
//-----------------------------------------------------------------------------

#include  "LoopCheck.h"
#include  "SocManNet.h"
#include  "Twitter.h"
#include  "Follower.h"
#include  "StateMachine.h"
#include  "ConfigMem.h"

#include  "ConfiguratorCom.h"

#define   DevOnSECONDS  10

#define   STNR(x)   stmPtr->curStateNumber = x;
#define   NEXT(x)   stmPtr->enter(x);

class ConfigDev
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //


private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  static  byte            workMem[256];

  static  ConfigMem       *cfmPtr;
  static  SocManNet       *smnPtr;
  static  Follower        *foPtr;
  static  StateMachine    *stmPtr;
  static  LoopCheck       *lchkPtr;

  static  unsigned long   startOpTime;
  static  unsigned long   stopOpTime;
  static  unsigned long   diffOpTime;

  static  int             confPin;

  // Twitter values
  static  int             twInt1, twInt2, twInt3, twInt4;
  static  double          twFloat1, twFloat2, twFloat3, twFloat4;
  static  char            twStr1[32], twStr2[32], twStr3[32], twStr4[32];

  // Follower values
  static  IntegerValue    foInt1, foInt2, foInt3, foInt4;
  static  FloatValue      foFloat1, foFloat2, foFloat3, foFloat4;
  static  TextValue       foStr1, foStr2, foStr3, foStr4;

  static  bool            useAppKey;    // Separation in one Network
  static  int             appKey;       // App identification

  // -------------------------------------------------------------------------
  // local functions
  // -------------------------------------------------------------------------
  //
  static  void  presetTwitter();  // set empty values for twitter
  static  void  updateFollower(); // refresh follower value structures
  static  void  initFollower();   // setting follower structurea

public:
  // -------------------------------------------------------------------------
  // construction and initilisation
  // -------------------------------------------------------------------------
  //
  static void begin(ConfigMem *cfm, SocManNet *smn, Twitter *twi,
                    Follower *fol, StateMachine *sm, LoopCheck *lchk);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  static void smcCheckRuntime();  // test runtime and update prom

  static void smInit();           // Some Initialisation for the state machine
  static void smCheck();          // Switcher for several tests
  static void smNoConfPin();      // Prepare configuration without pin
  static void smNoConfig();       // Normalzustand, keine Konfiguration
  static void smWaitConfPin();    // Wait for setting of configuration pin
  static void smAckConfPin();     // Internal acknowledge setting of pin
  static void smStart();          // Start the configuration process
  static void smInitTwitter();    // Initialise Twitter
  static void smInitFollower();   // Initialise Follower
  static void smWaitForConf();    // Waiting for Configurator

  static void smStartWLAN();      // start WLAN configuration
  static void smWaitWLAN();       // wait for new values and set

  static void smStartPOS();       // start POS configuration
  static void smWaitPOS();        // wait for new values
  static void smProgPOS();        // store new values




  // -------------------------------------------------------------------------
  // user variables
  // -------------------------------------------------------------------------
  //
  static  Twitter         *twPtr;

};

//-----------------------------------------------------------------------------
#endif //_ConfigDev_h
