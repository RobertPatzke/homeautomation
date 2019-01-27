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

// Forward references
//
void  comSub1(int nrInChars);
void  comSub2();

// The loop function is called in an endless loop
//
int   loopCtrl = 0;
int   loopCount = 0;
char  localCharBuffer[128];

void loop()
{
  int   nrInChars;

  loopCount++;

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

  nrInChars = monitor.inCount();

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

    if((char) localCharBuffer[0] == '/')
    {
      loopCtrl = localCharBuffer[1] & 0x07;
      com1.clrRecBuf();
      com2.clrRecBuf();
      com3.clrRecBuf();
    }

    monitor.putStr(localCharBuffer);
    monitor.putStr((char *) "\r\n");
    uint32_t diffMics = micros() - startMics;
    monitor.putStr((char *) "Com copy time = ");
    itoa(diffMics,localCharBuffer,10);
    monitor.putStr(localCharBuffer);
    monitor.putStr((char *) " microseconds\r\n\n");
  }

  // --------------------------------------------------------------------------
  // Test 1: Enter /1 and send with Serial Monitor View
  // --------------------------------------------------------------------------
  // Repeated sending of small strings via TX1, TX2 and TX3
  // If you connect TXn and RXn, you will see the sent and received strings
  // also on Serial Monitor View
  //

  if(loopCtrl == 1)
  {
    com1.putStr((char *) "This is COM1");
    com2.putStr((char *) "This is COM2");
    com3.putStr((char *) "This is COM3");
    loopCtrl = 101;
    return;
  }

  if(loopCtrl == 101)
  {
    nrInChars = com1.inCount();
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM1: [");
      comSub1(nrInChars);
      com1.getCount(localCharBuffer, nrInChars);
      comSub2();
    }

    nrInChars = com2.inCount();
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM2: [");
      comSub1(nrInChars);
      com2.getCount(localCharBuffer, nrInChars);
      comSub2();
    }

    nrInChars = com3.inCount();
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM3: [");
      comSub1(nrInChars);
      com3.getCount(localCharBuffer, nrInChars);
      comSub2();
   }

    loopCtrl = 1;
  }

  // --------------------------------------------------------------------------
  // Test 2: Enter /2 and send with Serial Monitor View
  // --------------------------------------------------------------------------
  // Repeated sending of a small string via TX3
  // If you connect TX3 and RX1, TX1 and RX2, TX2 and RX1,
  // you will see the sent and received string passed through
  // all 3 serials also on Serial Monitor View
  //

  if(loopCtrl == 2)
  {
    com3.putStr((char *) "Passing through all Serials.");
    loopCtrl = 201;
    return;
  }

  if(loopCtrl == 201)
  {
    nrInChars = com1.inCount();
    if(nrInChars > 0)
    {
      com1.getCount(localCharBuffer, nrInChars);
      com1.putStr(localCharBuffer, nrInChars);
    }

    nrInChars = com2.inCount();
    if(nrInChars > 0)
    {
      com2.getCount(localCharBuffer, nrInChars);
      com2.putStr(localCharBuffer, nrInChars);
    }

    nrInChars = com3.inCount();
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM3: [");
      comSub1(nrInChars);
      com3.getCount(localCharBuffer, nrInChars);
      comSub2();
   }

    loopCtrl = 2;
  }

  // --------------------------------------------------------------------------
  // Test 3: Enter /3 and send with Serial Monitor View
  // --------------------------------------------------------------------------
  // Repeated sending of small strings via TXn with \r and/or \n (line)
  // If you connect TXn and RXn, you will see the sent and received strings
  // also on Serial Monitor View
  //

  if(loopCtrl == 3)
  {
    com1.putStr((char *) "This is a line with CR/LF\r\n");
    com2.putStr((char *) "This is a line with LF\n");
    com3.putStr((char *) "This is a line with CR/LF\r\nand more... ");
    loopCtrl = 301;
    return;
  }

  if(loopCtrl == 301)
  {
    nrInChars = com1.getLine(localCharBuffer);
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM1: [");
      comSub1(nrInChars);
      comSub2();
    }

    nrInChars = com2.getLine(localCharBuffer);
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM2: [");
      comSub1(nrInChars);
      comSub2();
    }

    nrInChars = com3.getLine(localCharBuffer);
    if(nrInChars > 0)
    {
      monitor.putStr((char *) "\r\nCOM3: [");
      comSub1(nrInChars);
      comSub2();
   }

    loopCtrl = 3;
  }



}

void comSub1(int nrInChars)
{
  char convert[32];

  itoa(nrInChars, convert, 10);
  monitor.putStr(convert);
  monitor.putStr((char *) "] = ");
}

void comSub2()
{
  monitor.putStr(localCharBuffer);
  monitor.putStr((char *) "  Cycle:");
  itoa(loopCount,localCharBuffer,10);
  monitor.putStr(localCharBuffer);
  monitor.putStr((char *) "\r\n");
}
