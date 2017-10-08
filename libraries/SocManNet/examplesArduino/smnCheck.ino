// Test program for connecting to the network with SocManNet
//
#include "Arduino.h"

#include "environment.h"
// Here you can define your environment: The CPU, the board and the IDE
// to select the network and special adaptions to CPUs and/or boards

#include "LoopCheck.h"
// For controlling the sequences in the loop function
// We will never use the delay function

#include "socManNetUser.h"
// Here you can define the parameters of your network:
// IP-address, MAC-address, etc.

#include "SocManNet.h"
// Definition of Class SocManNet, Datatypes and Parameters

SocManNet socManNet;
// Creating an instance of class SocManNet
// Some compilers take a huge basic load of program space, if you use the
// new operator. That is the reason, why we use a static instantiation.

LoopCheck   loopCheck;
// For most Applications we will use class LoopCheck for time control

// Some Macros as shortcuts for often needed code fragments
//
#define SP(x)           Serial.print(x)
#define SN(x)           Serial.println(x)
#define SF1(x)          Serial.printf(x)
#define SF2(x,y)        Serial.printf(x,y)
#define SF3(x,y,z)      Serial.printf(x,y,z)
#define SF4(w,x,y,z)    Serial.printf(w,x,y,z)
#define SF5(a,b,c,d,e)  Serial.printf(a,b,c,d,e)

// Variables used in setup (and in loop)
//
SocManNetError  smnError;
unsigned long   smnConnStartMicros;

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void setup()
{
  Serial.begin(115200);
  // There is no reason to use a lower speed, because at present most
  // serial interfaces are realized via USB and there is no relation
  // between line quality and serial speed.

  SN("Check of network connection");

  smnError = socManNet.init(true);
  // The broadcast environment will be establisched.
  // Parameters are used as defined in socManNetUser.h
  // Argument <false> means, that DHCP is NOT used and the IP-Address
  // comes from socManNetUser.h

  if(smnError == smnError_none)
  {
    // The initialisation was started without error
    // Let us measure the time it takes, until the connection is established
    // We will present the result in loop()
    //
    smnConnStartMicros = micros();
  }
}

// Variables used in loop
// Typically we define the variables we use inside a function also inside.
// But with loop we might forget, that we cyclic leave and enter the function
// and variables declared inside (on the stack) will not have the expected
// values (stack is used also outside loop).
// So here we define variables outside loop (static) and they will hold their
// values if not changed by our program in loop.
//
SmnIfInfo   smnIfInfo;      // Information from inside SocManNet
SmnIfStatus smnIfStatus;    // Status of the network interface

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------
//
void loop()
{
  // define temporarily used variables
  //
  int   tmpInt;

  loopCheck.begin();
  // controlled loop function always starts with begin() of LoopCheck

  socManNet.run();
  // With run() we give the CPU to the internal management of SocManNet.
  // This is necessary for the self control of SocManNet.
  // Communication will not work, if you forget to call run() often

  if(smnError != smnError_none)
  {
    // we do not expect this, but if it happens, we must find the error
    //
    if(loopCheck.once(0, 1))
    {
      // Printing only one time, what error information we can get
      //
      SP("Error with start of initialisation: ");
      SF3("%s [%d]",socManNet.getErrorMsg(smnError), (int) smnError);
    }

    // In case of error with start of initialisation, it makes no sense
    // to run the rest of the loop function.
    // So we will leave it here via return.
    // But we use this as an example, how to deal with such errors,
    // which might be repaired after a while.
    // We should then call end() of LoopCheck to get correct measurements
    //
    loopCheck.end();
    return;
  }

  if(socManNet.connected)
  {
    // Connection to network is established
    //
    if(loopCheck.once(1, 1))
    {
      // we show information about the connection,
      // of course only one time
      //

      SF2("Connection established after %d microseconds\r\n",
         (int) (micros()-smnConnStartMicros));
      // the time elapsed since start of initialisation in microseconds
      // But !!!!
      // Be careful when using Serial.print to make some debug messages
      // after starting e.g. WiFi.begin. Connection time may increase.
      // With ESP32 and a TP-Link router the connection time was 0,89 seconds
      // with DHCP and nearly the same without DHCP
      //

      socManNet.getIfInfo(&smnIfInfo);
      // Get internal information from SocManNet

      SP("MAC-Address = "); SN(smnIfInfo.macAdrCStr);
      // If you have a CPU/Module with internal specified MAC-Address,
      // there will be the value of that address, not that of socManNetUser.h

      SP("IP-Address  = "); SN(smnIfInfo.ipAdrCStr);
      // If you use DHCP (init(true)), this address is that
      // given by the AP or router, otherwise that of socManNetUser.h
      // But e.g. with ESP32, there is no error message, if the IP-Address
      // of socManNetUser.h does not fit to the network.

      tmpInt = smnIfInfo.evtCounter;
      // There is a handling for events from the interface drivers of
      // the CPU/Module. The event codes are stored in a list and evtCounter
      // is the number of events since start.
      // But the list is only SMNmaxNrIFEvt long.
      // It is a ring buffer and SmnIfInfo.evtIdx points to the latest event.

      if(tmpInt > 0)
      {
        SP(tmpInt); SP(" Events from interface driver:");

        if(tmpInt > SMNmaxNrIFEvt)
          tmpInt = SMNmaxNrIFEvt;

        for(int i = 0; i < tmpInt; i++)
        {
          SP(" "); SP(smnIfInfo.evtList[i]);
        }
        SN();
      }

      // Final status information
      //

      socManNet.getIfStatus(&smnIfStatus);

      if(smnIfStatus.changed)
        SF5("IfStatus = connected: %d,  initPending: %d,  ifStatus: %d, connectionCounts: %d\r\n",
            (int) smnIfStatus.connected,
            (int) smnIfStatus.initPending,
            (int) smnIfStatus.ifStatus,
            (int) smnIfStatus.connectCount);
    }
  }
  else
  {
    // Waiting for the connection to the interface
    // Show status, if anything has changed
    //

    socManNet.getIfStatus(&smnIfStatus);

    if(smnIfStatus.changed)
      SF5("IfStatus = connected: %d,  initPending: %d,  ifStatus: %d, connectionCounts: %d\r\n",
          (int) smnIfStatus.connected,
          (int) smnIfStatus.initPending,
          (int) smnIfStatus.ifStatus,
          (int) smnIfStatus.connectCount);

  }

  loopCheck.end();
  // controlled loop function always ends with end() of LoopCheck
}
