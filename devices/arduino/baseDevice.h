// ---------------------------------------------------------------------------
// File:        baseDevice.h
// Editors:     Robert Patzke,
// Start:       11. November 2017
// Last change: 11. November2017
// URI/URL:     www.mfp-portal.de
// Licence:     Creative Commons CC-BY-SA 
// ---------------------------------------------------------------------------
//
#ifndef baseDevice_h
#define baseDevice_h

// Type of function pointer
typedef void (*smnStatePtr)(void);

// Prototypes of state machine functions
//
void smInit();                  // Special Initialisation
void smWaitForSMN();            // Wait for connecting to the network
void smInitTwitter();           // Initialise Twitter
void smInitFollower();          // Initialise Follower
void smWaitForTestTwitter();    // Wait for any telegram from TestTwitter
void smDisplayValues();         // Display changed values
void smStartWithoutSMN();       // Start without communication
void smStartWithoutTTW();       // Start without TestTwitter
void smDisplayStatisticsSMN();  // Display statistics


// Public (twittered) state (base state)
//
enum smPublicState
{
  smpsUnborn,
  smpsInit,
  smpsRun
};

// ---------------------------------------------------------------------------
#endif  // baseDevice_h
