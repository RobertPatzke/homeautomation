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
// constructors and initialisations
// -------------------------------------------------------------------------
//
KeyMatrix::KeyMatrix(int rowArray[], int nrRows,
                     int colArray[], int nrCols,
                     Key matrix[], int cycleTime)
{
  rowList   = rowArray;
  colList   = colArray;
  rowCount  = nrRows;
  colCount  = nrCols;
  keyList   = matrix;
  initPins();
  initMeasures(cycleTime);
}

void KeyMatrix::initMeasures(int cycleTime)
{
  int  calcCycle = cycleTime * rowCount;

  rowIdx = 0;
  maxUpCount    = DefaultMaxUpTime / calcCycle;
  minUpCount    = DefaultMinUpTime / calcCycle;
  finUpCount    = DefaultFinUpTime / calcCycle;
  minDownCount  = DefaultMinDownTime / calcCycle;
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
        keyList[keyIdx].downCount++;    // count key down time
      else
      {                                     // if key was up before
        keyList[keyIdx].down = true;        // mark key down now
        if(keyList[keyIdx].upCount >= minUpCount)
        {                                     // if upCount was beyond
          keyList[keyIdx].pastUpCount =       // minUpCount (bouncing)
            keyList[keyIdx].upCount;          // save up count value
          keyList[keyIdx].edgeFall = true;    // and mark falling edge
          keyList[keyIdx].edgeRise = false;   // and clear rising edge
        }
        else                              // too short up times before
          keyList[keyIdx].upCount = 0;    // are seen as bouncing
      }
    }
    else
    { // --------------------------------------------------------------
      // key is up (released)
      // --------------------------------------------------------------
      if(!keyList[keyIdx].down)         // if key was up before
      {
        if(keyList[keyIdx].upCount < maxUpCount)  // respect limit
          keyList[keyIdx].upCount++;        // count key up time
      }
      else
      {                                     // if key was down before
        keyList[keyIdx].down = false;       // mark key up now
        if(keyList[keyIdx].downCount >= minDownCount)
        {                                     // if downCount was beyond
          keyList[keyIdx].pastDownCount =     // minDownCount (bouncing)
            keyList[keyIdx].downCount;        // save down count value
          keyList[keyIdx].edgeFall = false;   // and clear falling edge
          keyList[keyIdx].edgeRise = true;    // and mark rising edge
        }
        else                              // too short down times before
          keyList[keyIdx].downCount = 0;  // are seen as bouncing
      }
    }
  } // for

#ifdef ArduinoFunc

  digitalWrite(rowList[rowIdx], HIGH);
  rowIdx++;
  if(rowIdx >= rowCount)
    rowIdx = 0;
  digitalWrite(rowList[rowIdx], LOW);

#else
    realize direct port access for different microcontrollers
#endif

}
