//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   BlePoll.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   1. September 2021
//
// Diese Bibliothek (Klassse) enthält diverse Ressourcen zur Kommunikation
// über BLE-Funkmodule auf niedriger Ebene, also dem direkten Telegrammaustausch.
// Darauf aufbauend sollen mehrkanalige Messeinrichtungen mit möglichst
// geringen Latenzzeiten entwickelt werden.
//

#include "BlePoll.h"

// --------------------------------------------------------------------------
// Textmakros zur Vereinfachung der Programmierung
// --------------------------------------------------------------------------

#define next(x) nextState = &BlePoll::x

// --------------------------------------------------------------------------
// Initialisierungen
// --------------------------------------------------------------------------

BlePoll::BlePoll(IntrfRadio *refRadio, dword inCycleMics)
{
  init(refRadio, inCycleMics, NULL);
}

BlePoll::BlePoll(IntrfRadio *refRadio, MicsecFuPtr inMicroFu)
{
  init(refRadio, 0, inMicroFu);
}

void BlePoll::init(IntrfRadio *refRadio, dword inCycleMics, MicsecFuPtr inMicroFu)
{
  radio = refRadio;
  radio->setPacketParms(bptAdv);
  radio->setAccessAddress(AdvAccAddr);
  chn = adr = area = 0;
  master = nak = eadr = false;
  plMode = plmIdle;
  plpType = plptEmpty;
  micSec = inMicroFu;
  cycleMics = inCycleMics;
  cycleCnt = 0;
  toValue = 0;
  toSet = 0;
  nextState = NULL;
  pollIdx = 0;
  maxAdr = MAXSLAVE;
  curSlave = NULL;
  cntAlien = 0;
  cntWrong = 0;
  cntWaitDisabled = 0;
  cntPolling = 0;
  epStopped = false;
  epStop = false;
  runCounter = 0;
}

void BlePoll::begin(ComType typeIn, int adrIn, AppType appType)
{
  if(typeIn == ctMASTER)
    master  = true;
  else
    master = false;

  chn     = 0;          // 1. Bewerbungskanal
  area    = 0;          // Default-Anwendungsbereich
  eadr    = true;       // Start mit leerem Polling

  plMode  = plmEmpty;   // Leeres Polling (Adressenliste)

  if(master)
  {
    nak = false;        // Bei Master wird ohne Nak-Bit gestartet
    maxAdr = adrIn;
    if(maxAdr > MAXSLAVE)
      maxAdr = MAXSLAVE;
    pollIdx = 1;
    adr  = 1;
    next(smInit);
  }
  else
  {
    nak = true;
    adr = adrIn;
    next(smInit);
  }

  if(appType == atSOAAP || appType == atTestSend)
  {
    pduOut.adr5 = 0x53;
    pduOut.adr4 = 0x4F;
    pduOut.adr3 = 0x41;
  }

  if(appType == atTestSend)
    plMode = plmTest;

  pduOut.head = HeadS0B;
}


// --------------------------------------------------------------------------
// Konfiguration
// --------------------------------------------------------------------------
//
void BlePoll::setPollAddress(int chnIn, int adrIn, int areaIn, bool masterIn, bool eadrIn, bool nakIn)
{
  chn     = chnIn;
  adr     = adrIn;
  area    = areaIn;
  master  = masterIn;
  eadr    = eadrIn;
  nak     = nakIn;
}

void BlePoll::setPduAddress()
{
  pduOut.adr0 = (byte) adr;
  pduOut.adr1 = (byte) (area & 0x3F);
  if(nak) pduOut.adr1 |= 0x40;
  if(eadr) pduOut.adr1 |= 0x80;
  pduOut.adr2 = (byte) chn;
  if(master) pduOut.adr2 |= 0x80;
}

// --------------------------------------------------------------------------
// Hilfsfunktionen
// --------------------------------------------------------------------------
//
void BlePoll::setTimeOut(dword value)
{
  if(micSec != NULL)
  {
    toSet = micSec();
    toValue = value;
  }
  else
  {
    if(cycleMics > 1)
      cycleCnt = value / cycleMics;
    else
      cycleCnt = value;
  }
}

bool BlePoll::timeOut()
{
  if(micSec != NULL)
  {
    if((micSec() - toSet) > toValue)
      return(true);
    else
      return(false);
  }
  else
  {
    if(cycleCnt > 0)
      return(false);
    else
      return(true);
  }
}

// --------------------------------------------------------------------------
// Steuerung des Polling
// --------------------------------------------------------------------------
//

// Anhalten des leeren Polling
//
void BlePoll::stopEP()
{
  epStop = true;
}

// Weiterlaufen des leeren Polling
//
void BlePoll::resumeEP()
{
  epStop    = false;
  epStopped = false;
}

// Abfrage, ob gestoppt
//
bool BlePoll::stoppedEP()
{
  return(epStopped);
}


// Eintritt in die Zustandsmaschine
//
void BlePoll::run()
{
  runCounter++;
  if(cycleCnt > 0) cycleCnt--;

  if(nextState != NULL)
    (this->*nextState)();
}

// ****************************************************************************
// Zustandsmaschine
// ****************************************************************************

// ----------------------------------------------------------------------------
// Verzweigung nach Anwendung
// ----------------------------------------------------------------------------
//
void BlePoll::smInit()
{
  bleState = 100;

  switch(plMode)
  {
    case plmIdle:
      break;

    case plmTest:
      next(smStartTest);
      break;

    case plmEmpty:
      next(smStartEP);
      break;

    case plmScan:
      break;

    case plmSend:
      break;
  }
}

// ----------------------------------------------------------------------------
// Low Level Tests
// ----------------------------------------------------------------------------
//
void BlePoll::smStartTest()
{
  bleState = 200;

  if(master)
  {
    nak = false;
    pollIdx = 0;
    adr = 1;
    setTimeOut(500000);
  }
  else
  {
    nak = true;
  }
  pduOut.len  = 6;
  setPduAddress();
  radio->setChannel(0);
  next(smWaitTest);
}

void BlePoll::smWaitTest()
{
  if(!timeOut()) return;
  radio->disable(txmRead);
  next(smLoopTest);
}

void BlePoll::smLoopTest()
{
  pduOut.adr0++;
  radio->send(&pduOut, txmRead);
  setTimeOut(500000);
  next(smWaitTest);
}



// ----------------------------------------------------------------------------
// Leeres Polling
// ----------------------------------------------------------------------------
//
void BlePoll::smStartEP()
{
  bleState = 1000;

  pduOut.len  = 6;
  radio->setChannel(0);

  if(master)
  {
    nak = false;
    pollIdx = 1;
    adr = 1;
    next(smReqEadr);
  }
  else
  {
    nak = true;
    next(smWaitEadr);
  }
}

// ----------------------------------------------------------------------------
// Leeres Polling           M a s t e r
// ----------------------------------------------------------------------------
//
void BlePoll::smReqEadr()
{
  bleState = 1100;

  if(!radio->disabled(txmRead))
  {
    radio->disable(txmRead);
    return;
  }

  curSlave = &slaveList[pollIdx];
  curSlave->adr  = adr;
  curSlave->area = area;
  curSlave->chn  = chn;

  setPduAddress();
  //setTimeOut(2000);
  // Test
  setTimeOut(1000000);
  radio->send(&pduOut, txmRead);
  radio->getStatistics(&statistic);
  cntPolling++;
  next(smWaitNak);
}

void BlePoll::smWaitNak()
{
  if(timeOut())
  {
    radio->disable(txmRead);
    curSlave->cntTo++;
    pollIdx++;
    adr++;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
    return;
  }


  if(radio->fin(txmRead))
  {
    radio->getRecData(&pduIn, 8);

    if(pduIn.adr3 != pduOut.adr3 || pduIn.adr4 != pduOut.adr4 || pduIn.adr5 != pduOut.adr5)
    {
      cntAlien++;
      radio->cont(txmRead);
      return;
    }

    if(pduIn.adr0 != pduOut.adr0 || (pduIn.adr1 & 0x3F) != (pduOut.adr1 & 0x3F))
    {
      cntWrong++;
      radio->cont(txmRead);
      return;
    }

    radio->disable(txmRead);
    curSlave->cntNakEP++;
    pollIdx++;
    adr++;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
  }
  radio->getStatistics(&statistic);
}

void BlePoll::smEndEP()
{
  if(epStop || epStopped)
  {
    epStopped = true;
    return;
  }
  // Von vorne (zur Zeit, Test)
  //
  pollIdx = 1;
  adr = 1;
  next(smReqEadr);
}

// ----------------------------------------------------------------------------
// Leeres Polling           S l a v e
// ----------------------------------------------------------------------------
//
void BlePoll::smWaitEadr()
{
  bleState = 1200;

  if(!radio->disabled(txmRespE))
  {
    radio->disable(txmRespE);
    cntWaitDisabled++;
    return;
  }

  setPduAddress();
  radio->send(&pduOut, txmRespE);
  next(smEvalPoll);
}

void BlePoll::smEvalPoll()
{
  bleState = 1210;

  radio->getStatistics(&statistic);
  if(!radio->fin(txmRespE)) return;
  next(smWaitEadr);
}

// --------------------------------------------------------------------------
// Debugging
// --------------------------------------------------------------------------
//
dword BlePoll::debGetDword(int idx)
{
  dword retv = 0;

  switch(idx)
  {
    case 0:
      retv = statistic.mode;
      break;

    case 1:
      retv = statistic.interrupts;
      break;

    case 2:
      if(master)
        retv = cntPolling;
      else
        retv = statistic.sendings;
      break;

    case 3:
      if(master)
        retv = cntAlien;
      else
        retv = statistic.aliens;
      break;

    case 4:
      retv = statistic.wrongs;
      break;

    case 5:
      retv = bleState;
      break;

    case 6:
      retv = radio->getState();
      break;

    case 7:
      retv = runCounter;
      break;

    case 9:
      retv = 9;
      break;
  }

  return(retv);
}









