// Example program for using Twitter class in Social Manufacturing Network
//
#include "Arduino.h"
// This example is for the Arduino environment

#include "environment.h"
// This is our environment (CPU, board, IDE) for conditional compiling

#include "socManNetUser.h"
// The parameters for the network, edit the file for your network

#include "LoopCheck.h"
// Controlling the program (Timing, Realtime, etc.)

#include "SocManNet.h"
// The program interface to the network

#include "Twitter.h"
// Create messages and cyclic send broadcast

LoopCheck   loopCheck;      // Instance of LoopCheck, we need only one here
SocManNet   socManNet;      // Instance of SocManNet, only one allowed
Twitter     testTwitter;    // Instance of Twitter, number depends on resources

// ---------------------------------------------------------------------------
// Initialisation functions (here in header to avoid forward references)
// ---------------------------------------------------------------------------
//

void initTwitter()
{
  testTwitter.init
  (
    &socManNet,                 // Twitter needs a reference to the network
    loopCheck.refDateTime(),    // and a reference to a time string (RTC)
    (char *)"MyFirstCommObj",   // and the name of the communication object
    3,                          // the number of Integer to send (<= 4)
    2,                          // the number of Floats to send (<= 4)
    1,                          // the number of Text strings to send (<= 4)
    normalSpeed                 // and the cyclic speed
  );                            // highSpeed=10Hz, normalSpeed=1Hz, lowSpeed=0.1Hz

  // There are more parameters with Twitter, which will be send
  //
  testTwitter.setDeviceKey(4711);   // Any number used as key

  testTwitter.setDeviceName((char *) "MyDeviceName");
  // Tell the world, who you are

  testTwitter.setDeviceState(12);   // A status of your device (handbook)

  testTwitter.posX = 3667;          // X-position of your device
                                    // (local coordinates in cm)

  testTwitter.posY = 1088;          // Y-position of your device
                                    // (local coordinates in cm)

  testTwitter.posZ = 80;            // Z-position of your device
                                    // (local coordinates in cm)

  testTwitter.baseState = 0;        // The state of your finite state machine
                                    // (if you have one, example in work)

  testTwitter.baseMode = 0;         // A number describing your plan/wish
                                    // (explain it in handbook)

  // Giving some default values for your twitter variables
  //
  testTwitter.setIntValue(0, 10);   // The first integer in broadcast message
  testTwitter.setIntValue(1, 22);   // Second integer
  testTwitter.setIntValue(2, -7);   // Third integer
  // That are 3 values for the 3 integer variables we declared with
  // testTwitter.init above

  testTwitter.setFloatValue(0, 12.88);  // First float in broadcast message
  testTwitter.setFloatValue(1, 10.003); // Second float
  // That are 2 values for the 2 float variables we declared with
  // testTwitter.init above

  testTwitter.setTextValue(0, (char *) "Hi, its me");
  // One value for the one string variable we declared with testTwitter.init

  // HINT:
  // The communication object name (here "MyFirstCommObj") and the data structure
  // (i.e. number of integer, float and text declared in Twitter.init) and
  // the meaning of their content are a matter of standardisation
  // (e.g. for the decentral homeautomation)
  // The other parameters (e.g. device key) are mandatory for all devices.

}

// ---------------------------------------------------------------------------
// ***************************  setup  ***************************************
// ---------------------------------------------------------------------------
//
void setup()
{
  Serial.begin(115200);         // This is an example program and there will be
                                // some information at the screen of the IDE

  socManNet.init(false);        // Start connecting to the network with the
                                // IP-Address from socManNetUser.h

  initTwitter();                // Initialisation of your Twitter (see above)

}

// ---------------------------------------------------------------------------
// *****************************  loop  **************************************
// ---------------------------------------------------------------------------
//
void loop()
{
  loopCheck.begin();        // always start the loop with this function
  // -------------------------------------------------------------------------

  if(socManNet.connected)
    testTwitter.enabled = true;
  else
    testTwitter.enabled = false;
  //
  // Switch Twitter on/off with the connection status

  if(loopCheck.timerMilli(0, 2, 0))
    testTwitter.run(500);   // giving the CPU to testTwitter for its tasks
  //
  // This happens every 2 milliseconds, which is a frequency of 500 Hz.
  // Twitter.run needs the frequency to calculate the twitter cycle time

  if(loopCheck.timerMicro(1, 10, 0))
    socManNet.run();        // giving the CPU to socManNet for its tasks
  //
  // This should happen every 10 microseconds. But ist should be expected,
  // that the loop cycle time is longer than 10 microseconds.
  // So run() is called with every loop, if no other timer finishes before.
  // Thus, such a usage of timerMicro can only work, if it is the last
  // timer in loop, because timers behind this too short timer will not
  // come to be finished, because we have allowed only one timer finishing
  // in the same loop cycle.

  // -------------------------------------------------------------------------
  loopCheck.end();          // always leave the loop with this function
}
