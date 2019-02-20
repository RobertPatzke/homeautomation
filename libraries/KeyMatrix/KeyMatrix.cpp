//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    KeyMatrix.cpp
// Editor:  Robert Patzke
// Created: 18. February 2019
// Changed: 20. February 2019
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

  keyEvtMask  = KeyEvtDown | KeyEvtUp;

  initPins();
  initMeasures(cycleTime);
}

void KeyMatrix::initMeasures(int cycleTime)
{
  int  calcCycle = cycleTime * rowCount;

  rowIdx    = 0;
  keRdIdx   = 0;
  keWrIdx   = 0;

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
    if(i == 0)
    {
      pinMode(rowList[i], OUTPUT);
      digitalWrite(rowList[i], LOW);
    }
    else
      pinMode(rowList[i], INPUT_PULLUP);
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
// local functions/methods
// -------------------------------------------------------------------------
//

void KeyMatrix::setKeyEvent(KeyPtr keyPtr, int keyEvent)
{
  // Do not store the same event twice
  //
  if(keyPtr->event == keyEvent) return;

  int nrIn = keWrIdx - keRdIdx;             // Calculate number of free
  if(nrIn < 0) nrIn += MaxSavedKeyEvents;   // places in event buffer
  int free = MaxSavedKeyEvents - nrIn;      // for use below

  eventList[keWrIdx].event  = keyEvent;     // store event
  eventList[keWrIdx].keyRef = keyPtr;       // and object pointer

  keyPtr->event = keyEvent;

  keWrIdx++;                                // switch to next storage
  if(keWrIdx >= MaxSavedKeyEvents)          // respect wrap around length
    keWrIdx = 0;                            // of cycle buffer

  if(free < 1)              // if buffer is full
  {                         // the oldest contained event
    keRdIdx++;              // is deleted
    if(keRdIdx >= MaxSavedKeyEvents)
      keRdIdx = 0;
  }
}

// ----------------------------------------------------------------------------
// user functions
// ----------------------------------------------------------------------------
//

// -----------------------------------------------
// run
// -----------------------------------------------
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
      {
        keyList[keyIdx].downCount++;    // count key down time
        if(keyList[keyIdx].downCount >= minDownCount)
        {
          if(keyEvtMask & KeyEvtDown)
            setKeyEvent(&keyList[keyIdx], KeyEvtDown);
        }
      }
      else
      {                                     // if key was up before
        keyList[keyIdx].down = true;        // mark key down now
        if(keyList[keyIdx].upCount >= minUpCount)
        {                                     // if upCount was beyond
          keyList[keyIdx].pastUpCount =       // minUpCount (bouncing)
            keyList[keyIdx].upCount;          // save up count value
          keyList[keyIdx].edgeFall = true;    // and mark falling edge
          keyList[keyIdx].edgeRise = false;   // and clear rising edge

          if(keyEvtMask & KeyEvtEdgeFall)
            setKeyEvent(&keyList[keyIdx], KeyEvtEdgeRise);  // EVENT
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
        if(keyList[keyIdx].upCount >= minUpCount)
        {
          if(keyEvtMask & KeyEvtUp)
            setKeyEvent(&keyList[keyIdx], KeyEvtUp);
        }
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

          if(keyEvtMask & KeyEvtEdgeRise)
              setKeyEvent(&keyList[keyIdx], KeyEvtEdgeRise);  // EVENT
        }
        else                              // too short down times before
          keyList[keyIdx].downCount = 0;  // are seen as bouncing
      }
    }
  } // for

#ifdef ArduinoFunc

  pinMode(rowList[rowIdx], INPUT_PULLUP); // set current row inactive
  rowIdx++;                               // switch to next row
  if(rowIdx >= rowCount)                  // loop index around length
    rowIdx = 0;
  pinMode(rowList[rowIdx], OUTPUT);       //
  digitalWrite(rowList[rowIdx], LOW);     // set next row active

#else
    realize direct port access for different microcontrollers
#endif

}  // run


// -----------------------------------------------
// check the key event list
// -----------------------------------------------
//

KeyPtr KeyMatrix::checkKeyPressed()
{
  int nrIn = keWrIdx - keRdIdx;
  if(nrIn < 0) nrIn += MaxSavedKeyEvents;
  if(nrIn != 1) return (NULL);

  KeyEvent ke = eventList[keRdIdx];       // get oldest key event
  if(ke.event != KeyEvtDown) return (NULL);
  return(ke.keyRef);
}

KeyPtr KeyMatrix::waitKeyClick()
{
  int nrIn = keWrIdx - keRdIdx;
  if(nrIn < 0) nrIn += MaxSavedKeyEvents;
  if(nrIn < 2) return (NULL);

  KeyEvent ke1 = eventList[keRdIdx];       // get oldest key event
  keRdIdx++;
  if(keRdIdx >= MaxSavedKeyEvents)
    keRdIdx = 0;

  KeyEvent ke2 = eventList[keRdIdx];       // get next younger key event
  keRdIdx++;
  if(keRdIdx >= MaxSavedKeyEvents)
    keRdIdx = 0;

  if(ke1.keyRef != ke2.keyRef) return (NULL);

  if(ke1.event != KeyEvtDown) return (NULL);
  if(ke2.event != KeyEvtUp)   return (NULL);
  return(ke1.keyRef);
}


