//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    KeyMatrix.cpp
// Editor:  Robert Patzke
// Created: 18. February 2019
// Changed: 18. February 2019
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#include "KeyMatrix.h"

// -------------------------------------------------------------------------
// constructors and initializations
// -------------------------------------------------------------------------
//
KeyMatrix::KeyMatrix(int rowArray[], int nrRows, int colArray[], int nrCols, Key matrix[])
{
  rowList   = rowArray;
  colList   = colArray;
  rowCount  = nrRows;
  colCount  = nrCols;
  keyList   = matrix;
  initPins();
}

void KeyMatrix::initPins()
{
#ifdef ArduinoFunc

  int   i;

  for(i = 0; i < rowCount; i++)
  {
    pinMode(rowList[i], OUTPUT);
    if(i == 0)
      digitalWrite(rowList[i], LOW);
    else
      digitalWrite(rowList[i], HIGH);
  }

  for(i = 0; i < colCount; i++)
  {
    pinMode(colList[i], INPUT_PULLUP);
  }

#else
  realize direct port access for different microcontrollers
#endif
}

// -------------------------------------------------------------------------
// user functions
// -------------------------------------------------------------------------
//
void  KeyMatrix::run()
{
  int  col;
  int  keyIdx;
  int  readVal;

  for(col = 0; col < colCount; col++)
  {
#ifdef ArduinoFunc

    readVal = digitalRead(colList[col]);

#else
    realize direct port access for different microcontrollers
#endif

    keyIdx = rowIdx * colCount + col;

    if(readVal == LOW)
    { // --------------------------------------------------------------
      // key is down (hit)
      // --------------------------------------------------------------
      if(keyList[keyIdx].down)          // if key was down before
        keyList[keyIdx].timeDown++;     // count key down time
      else
      {                                     // if key was up before
        keyList[keyIdx].down = true;        // mark key down now
        keyList[keyIdx].edgeFall = true;    // and mark as falling edge
        keyList[keyIdx].edgeRise = false;   // clear rising edge
      }
    }
    else
    { // --------------------------------------------------------------
      // key is up (released)
      // --------------------------------------------------------------
      if(!keyList[keyIdx].down)         // if key was up before
        keyList[keyIdx].timeUp++;       // count key up time
      else
      {                                     // if key was down before
        keyList[keyIdx].down = false;       // mark key up now
        keyList[keyIdx].edgeFall = false;   // and clear as falling edge
        keyList[keyIdx].edgeRise = true;    // mark rising edge
      }
    }
  }

#ifdef ArduinoFunc


#else
    realize direct port access for different microcontrollers
#endif


}
