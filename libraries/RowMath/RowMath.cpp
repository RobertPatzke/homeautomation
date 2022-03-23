//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   RowMath.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   30. November 2021
//
// Berechnungen mit Messwertreihen.
//

#include "RowMath.h"

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

void RowMath::begin(int size, short *bufPtr)
{
  bufSize = size;
  calcBuffer = bufPtr;
  for(int i = 0; i < MaxNrOfFilter; i++)
  {
    calcIdx[i] = bufSize + 1;
    sum[i] = 0;
    count[i] = bufSize;
    intI[i] = 0;
    intII[i] = 0;
  }
  writeIdx = 0;
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
void RowMath::setEdge(int idx, int inCount)
{
  count[idx] = inCount;
  int anzBuf = writeIdx - inCount;
  if(anzBuf < 0) anzBuf += bufSize;
  calcIdx[idx] = anzBuf;
}


// ----------------------------------------------------------------------------
// Steuerung und Datenaustausch
// ----------------------------------------------------------------------------
//
void RowMath::enterValue(short val)
{
  oldValue = lastValue;

  for(int i = 0; i < MaxNrOfFilter; i++)
  {
    if(calcIdx[i] > bufSize) continue;

    sum[i] += val - calcBuffer[calcIdx[i]++];
    if(calcIdx[i] >= bufSize) calcIdx[i] = 0;
  }
  calcBuffer[writeIdx++] = lastValue = val;
  if(writeIdx >= bufSize) writeIdx = 0;
}

void  RowMath::filter(int idx, FilterResultPtr result)
{
  result->sum   = sum[idx];
  result->avg   = sum[idx] / count[idx];
  //result->hp    = lastValue - result->avg;
  //result->hp    = ((result->hp1 + lastValue - oldValue) * 9) / 10;
  //result->hp    = (9 * result->hp1 + 10 * (lastValue - oldValue)) / 11;
  //result->hp    = (5915 * result->hp1 + 15915 * (lastValue - oldValue)) / 25915;
  result->hp    = (14915 * result->hp1 + 15915 * (lastValue - oldValue)) / 16915;
  result->hp1   = result->hp;
  intI[idx]     += result->hp;
  intII[idx]    += intI[idx];
  result->intI  = intI[idx];
  result->intII = intII[idx];
}

void  RowMath::resetInts(int idx)
{
  intI[idx]   = 0;
  intII[idx]  = 0;
}

// ----------------------------------------------------------------------------
// Debugging
// ----------------------------------------------------------------------------
//









