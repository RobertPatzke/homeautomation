//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SyncMeas.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
// Datum:   13. März 2022
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "SyncMeas.h"


// -------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// -------------------------------------------------------------------------
//
SyncMeas::SyncMeas(int runCycle)
{
  rCycle  = runCycle;
  curIdx  = 0;
  listIdx = 0;
  listLen = 1;
  idxList = &listLen;
  state   = init;
  mCycle  = 1;
}

// -------------------------------------------------------------------------
// Anwenderfunktionen
// -------------------------------------------------------------------------
//
int SyncMeas::setChannels(int *iList, int iCnt)
{
  idxList = iList;
  listLen = iCnt;

  return(1);
}

int SyncMeas::setMeasCycle(int measCycle)
{
  mCycle = measCycle;
  return(1);
}


void SyncMeas::run()
{
  // Aktuell zu bearbeitender Kanal (Index)
  //
  curIdx = idxList[listIdx];

  switch(state)
  {
  case init:
    break;

  case chkData:
    break;
  }

  listIdx++;
  if(listIdx >= listLen)
    listIdx = 0;
}

