//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Testing on Linux
// Datei:   testSmnLinux.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (see Wikipedia: Creative Commons)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "LoopCheck.h"
// environment.h will be included in LoopCheck.h
// You have to #define smnSimLinux there for IDE
// and comment out other definitions

#include "SocManNet.h"
#include "Follower.h"

LoopCheck   loopCheck;
SocManNet   socManNet;
Follower    follower;

// We assume, there is a Twitter (called "MyFirstCommObj"), who sends
// 3 Integer values, 2 Float values and 1 Text value.
// See example <smnTwitter.ino>
// For these 6 variables, we need management structures, which show
// us the state of the variabel (i.e. new receive and new value)
//
IntegerValue    intMan1;
IntegerValue    intMan2;
IntegerValue    intMan3;
FloatValue      floatMan1;
FloatValue      floatMan2;
TextValue       textMan1;

// ---------------------------------------------------------------------------
// Auxiliary programs to make main program better readable
// ---------------------------------------------------------------------------
//

void printValues
  (int iv1, int iv2, int iv3, float fv1, float fv2, char *tv, bool newVal)
{
  if(newVal)
    printf("*** ");
  else
    printf("    ");

  printf("Int: %d %d %d  Float: %f %f  Text: %s\n",iv1,iv2,iv3,fv1,fv2,tv);
}

// ---------------------------------------------------------------------------
// Main program organised in setup() and loop() as it is with Arduino
// ---------------------------------------------------------------------------
//

int main()
{
  bool  anyNewValue;
  bool  newTwitterMsg;

  // -------------------------------------------------------------------------
  // setup
  // -------------------------------------------------------------------------
  //
  printf("Test Arduino Software Environment\n");
  printf("Endless loop, terminate with Ctrl-C\n");

  socManNet.init(false);
  // Unlike with microcontroller boards, there is no network initialisation
  // (or basic connection to a network) necessary, because the operating
  // system did this with when the PC was started.
  // So with Linux we cannot decide for DHCP or not.

  while(!socManNet.connected)
  {
    // With Linux on a PC, there is less time consumption with initialisation.
    // So it makes no sense, to do some tasks while waiting
    if(socManNet.connected)
      printf("Connection established\n");
  }

  // Preparing the Follower to receive all "TestTwitter" objects
  //
  follower.init(&socManNet,"TestTwitter");

  // Preparing the variable management structures to work with the follower
  //
  intMan1.recDsc.idx = 0;       // Expected to be the first integer in Twitter message
  intMan2.recDsc.idx = 1;       // Expected to be the second ...
  intMan3.recDsc.idx = 2;       // Expected to be the third ...

  floatMan1.recDsc.idx = 0;     // Expected to be the first float in Twitter message
  floatMan2.recDsc.idx = 1;     // Expected to be the second ...

  textMan1.recDsc.idx = 0;      // Only one text variable expected

  follower.enabled = true;      // Enabling Follower to work

  anyNewValue   = false;
  newTwitterMsg = false;

  // -------------------------------------------------------------------------
  // loop
  // -------------------------------------------------------------------------
  //
  while(1)
  {
	loopCheck.begin();

	// Looking for new values every millisecond would make it possible to
	// handle 1000 normal speed Twitters with the same communication object
	// (e.g. the temperature at different positions in a room)
	// BUT !!!!
	// The used Follower is not buffered. (Use FollowMultDev instead.)
	// A new incoming message overwrites the content of the older one and if
	// the communication objects are different by position (posX, posY, posZ)
	// we may loose information about temperature at the one or the other
	// location.
	// The short timing here only reduces the probability of loosing content.
	//
	if(loopCheck.timerMilli(0, 1, 0))
	{
	  // Update variable management structures with content from Follower
	  //
	  follower.getValue(&intMan1);
      follower.getValue(&intMan2);
      follower.getValue(&intMan3);
      follower.getValue(&floatMan1);
      follower.getValue(&floatMan2);
      follower.getValue(&textMan1);

      // Check for new messages (newPdu) and/or new values (newVal)
      //
      anyNewValue = intMan1.newValue | intMan2.newValue | intMan3.newValue |
                    floatMan1.newValue | floatMan2.newValue | textMan1.newValue;
      // The newValue-Bit of variables is set, whenever an incoming message
      // has a new value for it (this is compared in Follower)

      newTwitterMsg = intMan1.newPdu;
      // To check for new messages, it is enough to check any variable,
      // because if there is a new message, the newPdu-Bit is set at any
      // variable. But call <getValue> before checking, because the bits
      // are reset at the Follower and copied to the variable management
      // structure in <getValue>.
	}

	// To demonstrate the functionality of LoopCheck timers,
	// we use another timer to do the task for new Follower information.
	// Thus the checking of the Follower and the reaction on it will not
	// happen within the same loop cycle.
	// Both timers have the same repetition time, but LoopCheck timers
	// never finish in the same loop cycle.
	//
	if(loopCheck.timerMilli(1, 1, 0))
	{
	  if(newTwitterMsg || anyNewValue)
	  {
	    printValues(intMan1.value, intMan2.value, intMan3.value,
	                floatMan1.value, floatMan2.value, textMan1.value, anyNewValue);

	    newTwitterMsg   = false;
	    anyNewValue     = false;
	  }
	}

	if(loopCheck.timerMicro(2, 10, 0))
	  socManNet.run();
	// Do not use another timer behind this short repetition timer,
	// which probably comes with every loop (may be not on a PC).
	// Because timers are not allowed to finish in the same loop,
	// following timers would not finish

	loopCheck.end();
  }
  return 0;
}
