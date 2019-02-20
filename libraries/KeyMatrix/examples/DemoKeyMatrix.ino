//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    DemoKeyMatrix.ino
// Editor:  Robert Patzke
// Created: 20. February 2019
// Changed: 20. February 2019
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#include "Arduino.h"

#include "LoopCheck.h"
#include "KeyMatrix.h"


LoopCheck loopCheck;
// We need an environment for cyclic calling.
// This may be any timer, or a delay (not recommended)
// LoopCheck was developed for having many real time timers based
// on synchronising with the CPU clock (Arduino method micsec())

// ----------------------------------------------------------------------------
// Define your keypad (matrix) here
// ----------------------------------------------------------------------------
//
#define NrOfRows    4
int   kmRowArray[NrOfRows] = {38,40,42,44};
// Number of rows (or columns, may be exchanged)
// and list of DIO-numbers (Arduino pin number)

#define NrOfCols    4
int   kmColArray[NrOfCols] = {46,48,50,52};
// Number of columns (or rows, may be exchanged)
// and list of DIO-numbers (Arduino pin number)

#define NrOfKeys    NrOfRows * NrOfCols

#define KMCycleTime 1
// KeyMatrix is developed for being cyclic called with KeyMatrix.run()
// The cycle time has to be known for calculating time distances
// (bouncing gap, click behaviour, etc.)

Key   keyBoard[NrOfKeys];
// List of key descriptions with data used for measurement

// ----------------------------------------------------------------------------
KeyMatrix keyMatrix(kmRowArray, NrOfRows, kmColArray, NrOfCols, keyBoard, KMCycleTime);
// Instantiation of KeyMatrix
// (you can use many instances for many keypads if enough ports an RAM)

//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(115200);

  Serial.println((char *)"Test Key Matrix.");

  // Set up ASCII values for the keys of your keypad
  // if You do not know the relation of keys tow row and column,
  // put a simple list like a,b,c,d,e,f,..... here
  // then run the program, hit all keys an note the character a,b,c,...
  //
  keyBoard[0].value   = '1';
  keyBoard[1].value   = '4';
  keyBoard[2].value   = '7';
  keyBoard[3].value   = '*';
  keyBoard[4].value   = '2';
  keyBoard[5].value   = '5';
  keyBoard[6].value   = '8';
  keyBoard[7].value   = '0';
  keyBoard[8].value   = '3';
  keyBoard[9].value   = '6';
  keyBoard[10].value  = '9';
  keyBoard[11].value  = '#';
  keyBoard[12].value  = 'A';
  keyBoard[13].value  = 'B';
  keyBoard[14].value  = 'C';
  keyBoard[15].value  = 'D';
}


// The loop function is called in an endless loop
void loop()
{
  loopCheck.begin();
  // --------------------------------------------------------------------------

  if(loopCheck.timerMilli(lcTimer0, KMCycleTime, 0))
    keyMatrix.run();

  KeyPtr kp = keyMatrix.waitKeyClick();
  if(kp != NULL)
  {
    Serial.print("Key Value = ");
    Serial.println(kp->value);
  }

  // --------------------------------------------------------------------------
  loopCheck.end();
}
