// Demo-Program for using library SerialCom with Arduino
#include "Arduino.h"

#include "SerialCom.h"

// Important !!!!!!!
//
// The class SerialCom is at the time only working with SAM3X processor
// (or compatible CPU with same UART and USARTs)
//
// You must define in Your project properties Arduino/Compile Options
// -DuseVariantMFP
// because SerialCom uses its own interrupt handlers
// and You have to use file variant.cpp from folder locCore in Your
// core path (or change Your variant.cpp accordingly)
//

SerialCom monitor(DueCom0);
// DueCom0 is the name for TX0 and RX0 at the Arduino Due board
// Use DueCom- for the other serial ports TX- and RX- (1 to 3)
// TX0/RX0 is connected to the USB programming port.
// So in the Arduino-IDE (also with Sloeber) You have the Serial Monitor View
// to communicate via TX0/RX0.
// Thus I called this instance of SerialCom <monitor>.
//

#define SndBufMonitorSize 256
uint8_t sndBufferMonitor[SndBufMonitorSize];
// This buffer holds the bytes to be sent by the interrupt handler

#define RecBufMonitorSize 256
uint8_t recBufferMonitor[RecBufMonitorSize];
// This buffer holds the bytes received by the interrupt handler

// Testing all serial ports of Arduino Due
//
SerialCom com1(DueCom1);
uint8_t sndBufCom1[128];
uint8_t recBufCom1[128];

SerialCom com2(DueCom2);
uint8_t sndBufCom2[128];
uint8_t recBufCom2[128];

SerialCom com3(DueCom3);
uint8_t sndBufCom3[128];
uint8_t recBufCom3[128];

void setup()
{
  monitor.start(115200);
  // Start the communicator with defined speed

  monitor.setWriteBuffer(sndBufferMonitor, SndBufMonitorSize);
  // Give a buffer for the data to transmit

  monitor.setReadBuffer(recBufferMonitor, RecBufMonitorSize);
  // Give a buffer for received data

  monitor.putStr((char *)"Test number 1.\r\n");

  com1.start(9600);
  com1.setWriteBuffer(sndBufCom1, 128);
  com1.setReadBuffer(recBufCom1, 128);

  com2.start(9600);
  com2.setWriteBuffer(sndBufCom2, 128);
  com2.setReadBuffer(recBufCom2, 128);

  com3.start(9600);
  com3.setWriteBuffer(sndBufCom3, 128);
  com3.setReadBuffer(recBufCom3, 128);
}

// The loop function is called in an endless loop
//
void loop()
{
  char  localCharBuffer[128];

  // --------------------------------------------------------------------------
  // echo monitor input
  // --------------------------------------------------------------------------
  //
  delay(50);
  // the echo-routine takes only a few microseconds with Arduino Due
  // and it sends many characters back
  // so the transmit buffer will be overloaded, when you send many characters
  // via Serial Monitor View, because this routine will respond on each
  // receiving character (about every 100 us with speed 115200) with a lot
  // of characters
  // the delay avoids this problem
  // occasionally it may happen, that a received block is split into
  // 2 sub-blocks
  // as an alternative you could increase the SndBufMonitorSize accordingly
  // but then You will have a lot of responses, one with each incoming
  // character
  //

  int nrInChars = monitor.inCount();
  if(nrInChars > 0)
  {
    uint32_t startMics = micros();
    // This routine takes only a short time,
    // because putStr simply fills the transmit buffer <sndBufferMonitor>
    //
    monitor.putStr((char *) "Received Chars = ");
    itoa(nrInChars, localCharBuffer, 10);
    monitor.putStr(localCharBuffer);
    monitor.putStr((char *) "\r\n");

    monitor.getCount((uint8_t *) localCharBuffer, nrInChars);
    // You can use your <localCharBuffer> again, because the content
    // was copied with <putStr> into the interrupt transmit buffer

    localCharBuffer[nrInChars] = 0;
    // Because the communication may contain binary data (uint8_t),
    // there is no string end mark (0) provided with <getCount>
    // we have to add this extra

    monitor.putStr(localCharBuffer);
    monitor.putStr((char *) "\r\n");
    uint32_t diffMics = micros() - startMics;
    monitor.putStr((char *) "Com copy time = ");
    itoa(diffMics,localCharBuffer,10);
    monitor.putStr(localCharBuffer);
    monitor.putStr((char *) " microseconds\r\n\n");
  }

  com1.putStr((char *) "This is COM1");
  com2.putStr((char *) "This is COM2");
  com3.putStr((char *) "This is COM3");

}
