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

#define TestLed 23
// The built in LED of some Arduino boards (and similar development boards like
// ESP32 Dev) seem to be connected to a pin (most 13), which is also used for the
// serial transmit (TxD) or other purposes.
// The blink example of Arduino works, because they switch the pin and then
// they freeze the program bei function <delay(milliseconds)>.
// But if you leave loop() for Arduino internal functions, the pin is
// no more valid, it is used for TxD (or some other purpose).
// In other words, with some (many?) boards, the blink example of Arduino does
// not work without the delay in loop-function.
// Because we now use a software-timer instead of a delay,
// we must use another pin to connect our own LED or look for a built-in LED
// which is not used outside loop-function.
// E.g. the blue LED of ESP32 DEVKIT V1 (doit) is connected at pin 2.


LoopCheck   loopCheck;      // Instance of LoopCheck, normally only one needed
SocManNet   socManNet;      // Instance of SocManNet, only one allowed
Twitter     testTwitter;    // Instance of Twitter, number depends on resources

// ---------------------------------------------------------------------------
// Initialisation functions (here in header to avoid forward references)
// ---------------------------------------------------------------------------
//
LoopCheck   localLoopCheck; // Demonstrate another LoopCheck for a loop
                            // Normally you need only one instance to control
                            // Arduino loop function. But to avoid calling
                            // delay function here and for time measurement
                            // we use another LoopCheck here
SmnIfStatus   smnStatus;
// With testing on ESP32 there was a problem, when this structure was defined
// as a stack variable (defined inside <waitForConnection>)
// The value of smnStatus.changed was true, though it was changed to false via
// pointer in function SocManNet.getIfStatus().
// I did not find the reason for this behaviour. With the definition of the
// structure in global memory (outside <waitForConnection>) the error does
// not occur.
//

bool checkConnection()          // This function will be called in a loop
{                               // at Setup(). So it is cyclic repeated.
  OpHourMeter   timeMeasure;
  SmnIfInfo     smnInfo;

  localLoopCheck.begin();
  // -------------------------------------------------------------------------

  // Whenever status changes, we will display ist
  //
  socManNet.getIfStatus(&smnStatus);
  if(smnStatus.changed == true)
  {
    Serial.print("connected: "); Serial.print(smnStatus.connected);
    Serial.print("   pending: "); Serial.print(smnStatus.initPending);
    Serial.print("   connectCounter: "); Serial.print(smnStatus.connectCount);
    Serial.print("   ifStatus: "); Serial.println(smnStatus.ifStatus);
  }

  if(socManNet.connected == true)
  {
    localLoopCheck.operationTime(&timeMeasure);
    Serial.printf("\r\nDer Verbindungsaufbau hat %d,%03d Sekunden gedauert.\r\n",
                  timeMeasure.Seconds,timeMeasure.Milliseconds);

    socManNet.getIfInfo(&smnInfo);
    Serial.print("MAC-Address: ");      Serial.print(smnInfo.macAdrCStr);
    Serial.print("   IP-Address: ");    Serial.println(smnInfo.ipAdrCStr);
    return(false);
  }

  // -------------------------------------------------------------------------
  localLoopCheck.end();
  return(true);
}

void initTwitter()
{
  // There are some parameters, which have to be set befor the basic
  // initialisation of twitter is done. Because these values are used there.
  //
  testTwitter.setDeviceKey();   // The device key is used by FollowMultDev
                                // to distinguish different twitter with
                                // the same object name
  // Calling setDeviceKey without parameter (= device key) or omitting the call
  // creates a device key from the lower 2 bytes of MAC address.

  testTwitter.setApplicationKey(0); // The application key defines a relation
                                    // (or connection) between different devices
  // with respect to their tasks in a common application.
  // The value 0 stands for "do not care".

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
  bool  lokalLoopBusy;

  pinMode(TestLed, OUTPUT);
  // Setting a free pin for output (connect a LED via serial resistor)

  Serial.begin(115200);         // This is an example program and there will be
                                // some information at the screen of the IDE

  socManNet.init(false);        // Start connecting to the network with the
                                // IP-Address from socManNetUser.h

  // Before initialisation of Twitter the connection of SocManNet to the
  // network should be established. If using DHCP, it is mandatory to wait.
  // Twitter needs the IP-Address of the network as part of the message
  //
  lokalLoopBusy = true;         // For this example we take a loop,
                                // which (endlessly) waits until the connection
  while(lokalLoopBusy == true)  // to the network is established.
  {
    lokalLoopBusy = checkConnection();    // Check the connection

    delay(1);       // normally, this delay would not be necessary,
                    // but e.g. ESP8266 activates a watchdog timer
    // which has to be served at least every second.
    // Calling delay retriggers the watchdog timer.
  }

  initTwitter();    // Initialisation of your Twitter (see above)
                    // Twitter could be started now, but we will do that
                    // in loop function to demonstration the usage of <once>
}

// ---------------------------------------------------------------------------
// *****************************  loop  **************************************
// ---------------------------------------------------------------------------
//
LoopStatistics  statistic;

void loop()
{
  loopCheck.begin();        // always start the loop with this function
  // -------------------------------------------------------------------------

  //
  // Switch Twitter on after a delay of 1000 loop cycles
  //
  if(loopCheck.once(0, 1000))
    testTwitter.enabled = true;

  //
  // Show the real time behaviour of the loop
  //
  if(loopCheck.timerMilli(0, 100, 0))
  {
    loopCheck.getStatistics(&statistic);
    if(statistic.periodAlarm)
      digitalWrite(TestLed, HIGH);
    else
      digitalWrite(TestLed, LOW);
  }
  //
  // LoopCheck makes a periodAlarm, when the cycle time of the loop calls
  // are longer than a millisecond. It is not a principle problem, but
  // timers with repetition time less a millisecond would have an error
  // and the software clock as well as the operating time counter will
  // miss a millisecond.
  // Thus the basic real time behaviour depends on the call frequency of loop.
  // In most applications there is no influence of a lower loop frequency.
  // periodAlarm was installed for users, who want to make real time behaviour
  // in milliseconds resolution.

  if(loopCheck.timerMilli(1, 2, 0))
    testTwitter.run(500);   // giving the CPU to testTwitter for its tasks
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
  loopCheck.end();          // always leave the loop with this function
}
