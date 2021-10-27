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
  slaveIdx = 0;
  pollIdx = 0;
  pollNr = 0;
  maxAdr = MAXSLAVE;
  curSlave = NULL;
  cntAlien = 0;
  cntWrong = 0;
  cntAllNaks = 0;
  cntWaitDisabled = 0;
  cntPolling = 0;
  pollStopped = false;
  pollStop = false;
  runCounter = 0;
  newValue = false;

  for(int i = 1; i <= MAXSLAVE; i++)
  {
    slaveList[i].cntNakEP = 0;
    slaveList[i].cntTo = 0;
    slaveList[i].pIdx = 0;
    pollList[i].prioCnt = 0;
    pollList[i].slIdx = 0;
    pollList[i].status = 0;
  }
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

  // --------------------------------------------------------------------------
  plMode  = plmEmpty;   // Leeres Polling (Adressenliste)
  // --------------------------------------------------------------------------

  if(master)
  {
    nak = true;        // Nak-Bit vom Master forciert leere Antwort
    maxAdr = adrIn;
    if(maxAdr > MAXSLAVE)
      maxAdr = MAXSLAVE;
    slaveIdx = 1;
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

    pduIn.adr5 = 0x53;
    pduIn.adr4 = 0x4F;
    pduIn.adr3 = 0x41;
  }

  if(appType == atTestSend)
    plMode = plmTest;

  pduOut.head = HeadS0B;
  pduIn.head = HeadS0B;

  pduOut.data[0] = 0;         // Pdu-Zähler (CNT)
  pduIn.data[0] = 0;

  pduOut.data[1] = appType;   // Pdu-Typ (TYPE)
  pduIn.data[1] = appType;

  if(appType == atSOAAP)
  {
    valuePdu.appId = plptMeas6;
    plMode = plmSoaapM;
    fullCycle = true;
  }

  valuePdu.measCnt = 0;
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
  setPduAddress(&pduOut);
}

void BlePoll::setPduAddress(bcPduPtr pduPtr)
{
  pduPtr->adr0 = (byte) adr;
  pduPtr->adr1 = (byte) (area & 0x3F);
  if(nak) pduPtr->adr1 |= 0x40;
  if(eadr) pduPtr->adr1 |= 0x80;
  pduPtr->adr2 = (byte) chn;
  if(master) pduPtr->adr2 |= 0x80;
}

void BlePoll::setEmptyPollParams(int cycleTotal, int cycleRun, dword timeOut)
{
  epCycleTotal  = cycleTotal;
  epCycleRun    = cycleRun;
  epTimeOut     = timeOut;
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

bool BlePoll::getValues(bcPduPtr pduPtr)
{
  bool retv = false;

  pduPtr->len = 28;
  pduPtr->data[2] = valuePdu.appId;
  if(newValue)
  {
    retv = true;
    pduPtr->data[3]++;              // measCnt
  }
  return(retv);
}


// --------------------------------------------------------------------------
// Steuerung des Polling
// --------------------------------------------------------------------------
//

// Aktuellen Betriebszustand einstellen
//
void BlePoll::start(PlMode inPlMode)
{
  oldPlMode = plMode;
  plMode = inPlMode;
  pollStop = true;
}

// Anhalten des leeren Polling
//
void BlePoll::stopEP()
{
  pollStop = true;
}

// Weiterlaufen des leeren Polling
//
void BlePoll::resumeEP()
{
  pollStop    = false;
  pollStopped = false;
}

// Abfrage, ob gestoppt
//
bool BlePoll::stoppedEP()
{
  return(pollStopped);
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

// --------------------------------------------------------------------------
// Zugriff auf Polling-Informationen
// --------------------------------------------------------------------------
//
int BlePoll::getSlaveList(byte *dest, int maxByte)
{
  int           slIdx;

  for(int i = 0; i < pollMaxNr; i++)
  {
    if(i == maxByte) break;
    slIdx = pollList[i+1].slIdx;
    dest[i] = slaveList[slIdx].adr;
  }
  return(pollMaxNr);
}



// ****************************************************************************
// Zustandsmaschine
// ****************************************************************************
//

// ----------------------------------------------------------------------------
// Verzweigung nach Anwendung (nach Anlauf)
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
      epCycleTotal = -1;
      epCycleRun = -1;
      next(smStartEP);
      break;

    case plmScan:
      break;

    case plmSoaapM:
      next(smStartEP);
      break;

    case plmXchg:
      break;
  }
}

// ----------------------------------------------------------------------------
// Verzweigung nach Anwendung (im Betrieb)
// ----------------------------------------------------------------------------
//
void BlePoll::smIdle()
{
  bleState = 200;

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
      if(master)
        next(smReqComS);
      break;

    case plmXchg:
      if(!master)
        next(smStartComES);
      break;
  }
}

// ----------------------------------------------------------------------------
// Low Level Tests
// ----------------------------------------------------------------------------
//
void BlePoll::smStartTest()
{
  bleState = 500;

  if(master)
  {
    nak = false;
    slaveIdx = 0;
    adr = 1;
    setTimeOut(500000);
  }
  else
  {
    nak = true;
  }
  pduOut.len  = 6;
  setPduAddress();
  radio->setChannel(chn);
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
// L e e r e s   P o l l i n g
// ----------------------------------------------------------------------------
//
void BlePoll::smStartEP()
{
  bleState = 1000;

  pduOut.len  = 6;          // Nur Adresse
  radio->setChannel(chn);
  pollMaxNr = 0;

  nak = true;

  if(master)
  {
    slaveIdx = 1;
    pollIdx = 1;
    adr = 1;
    pollNr = 0;
    next(smReqEadr);
  }
  else
  {
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

  // Der Durchlauf startet mit abgeschaltetem Radio
  // ggf. muss es abgeschaltet werden
  //
  if(!radio->disabled(txmRead))
  {
    radio->disable(txmRead);
    return;
  }

  // Datenstruktur für den Slave definieren
  //
  curSlave = &slaveList[slaveIdx];
  curSlave->adr  = adr;
  curSlave->area = area;
  curSlave->chn  = chn;

  curPoll = &pollList[pollIdx];

  setPduAddress();
  setTimeOut(epTimeOut);
  radio->send(&pduOut, txmRead);
  radio->getStatistics(&statistic);
  cntPolling++;
  next(smWaitNak);
}

void BlePoll::smWaitNak()
{
  bleState = 1110;

  if(timeOut())
  {
    // Für die Adresse ist kein Teilnehmer vorhanden, oder die Übertragung ist gestört
    //
    radio->disable(txmRead);

    if(curSlave->pIdx != 0)
    {
      // Wenn der Teilnehmer bereits in die Poll-Liste eingetragen ist
      // dann wird darin seine temporäre Abwesenheit markiert
      pollList[(int) curSlave->pIdx].status &= ~psSlaveIsPresent;
    }

    // Der Time-Out-Zähler macht erkenntlich, wie stark sich Störungen auswirken
    //
    curSlave->cntTo++;

    // Nächste Adresse und zur Anforderung
    //
    slaveIdx++;
    adr++;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
    return;
  }


  if(radio->fin(txmRead))
  {
    // Auf dem Kanal wurde ein Datensatz (BLE-Beacon) empfangen
    // und es werden die ersten 8 Byte (Header, Len und Adresse) geholt
    //
    radio->getRecData(&pduIn, 8);

    if(pduIn.adr3 != pduOut.adr3 || pduIn.adr4 != pduOut.adr4 || pduIn.adr5 != pduOut.adr5)
    {
      // Wenn die höherwertigen 3 Byte der Adresse nicht mit der Anwendungskodierung
      // übereinstimmen, dann ist es ein Fremd-Beacon.
      // Diese werden gezählt und kennzeichnen, wie stark aktiv ein anderes
      // BLE-Netzwerk in Reichweite ist.
      //
      cntAlien++;

      // Der Empfang wird fortgesetzt, weil innerhalb des Time-Out ja noch die richtige
      // Antwort eintreffen könnte.
      //
      radio->cont(txmRead);
      return;
    }

    if(pduIn.adr0 != pduOut.adr0 || (pduIn.adr1 & 0x3F) != (pduOut.adr1 & 0x3F))
    {
      // Wenn die Teinehmernummer oder die Gebietsnummer falsch sind, dann ist
      // möglicherweise ein weiterer Master aktiv, der fehlerhafterweise denselben
      // Kanal verwendet.
      // Auch dieses ereignis wird gezählt und der empfang wird fortgesetzt.
      cntWrong++;
      radio->cont(txmRead);
      return;
    }

    // Alles korrekt, der adressierte Teilnehmer hat rechtzeitig geantwortet
    // Radio wird abgeschaltet
    //
    radio->disable(txmRead);

    if(curSlave->pIdx != 0)
    {
      // Wenn der Teilnehmer bereits in die Poll-Liste eingetragen ist,
      // dann wird er darin als aktuell anwesend markiert
      //
      pollList[(int) curSlave->pIdx].status |= psSlaveIsPresent;
    }
    else
    {
      // Wenn nicht, wird der aktuelle Listeneintrag definiert
      //
      curPoll->status |= psSlaveIsPresent | psSlaveWasPresent;
      curPoll->slIdx = slaveIdx;  // Querverweis zur Liste möglicher Teilnehmer
      curSlave->pIdx = pollIdx;   // Und in der Liste auch ein Verweis zur Poll-Liste
      pollIdx++;                  // Weiter mit nächstem Listenplatz
      pollNr++;                   // Anzahl der beantworteten Pollings
    }

    // Die Nak-Antworten werden gezählt
    //
    curSlave->cntNakEP++; // Teilnehmerspezifisch
    cntAllNaks++;         // und insgesamt

    // Weiter mit nächstem Teilnehmer und nächster Adresse (TN-Nummer)
    //
    slaveIdx++;
    adr++;

    // Wenn die vorgegeben Endadresse erreicht ist
    // dann zum Ende des Polldurchgangs über alle möglichen Teilnehmer,
    // ansonsten nächsten Teilnehmer pollen
    //
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
  }
  radio->getStatistics(&statistic);
}

void BlePoll::smEndEP()
{
  // Nach jedem vollständigen Polldurchlauf kann das Polling
  // abgebrochen werden.
  //
  if(pollStop || pollStopped)
  {
    pollStopped = true;
    next(smIdle);
    return;
  }

  // Es werden die Maximalwerte der rechtzeitigen Antworten gebildet
  //
  if(pollNr > pollMaxNr)
    pollMaxNr = pollNr;

  if(pollMaxNr == 0)
  {
    // Wenn noch kein Teilnehmer angeschlossen ist (oder Kanal total gestört)
    //
    if(epCycleTotal > 0)
    {
      // dann wird der Pollvorgang so oft wie vorgegeben wiederholt
      //
      epCycleTotal--;
      pollNr = 0;
      pollIdx = 1;
      slaveIdx = 1;
      adr = 1;
      next(smReqEadr);
      return;
    }
    else
    {
      // und anschließend der Idle-Zustand angenommen
      next(smIdle);
      return;
    }
  }
  else
  {
    // Wenn wenigstens schon ein Teilnehmer geantwortet hat
    //
    if(epCycleRun > 0)
    {
      // dann wird der Pollvorgang auch so oft wie anderweitig vorgegeben wiederholt
      //
      epCycleRun--;
      pollNr = 0;
      pollIdx = 1;
      slaveIdx = 1;
      adr = 1;
      next(smReqEadr);
      return;
    }
    else
    {
      // und anschließend die Datenübertragung gestartet
      next(smIdle);
      return;
    }
  }

  pollNr = 0;
  pollIdx = 1;
  slaveIdx = 1;
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

// ----------------------------------------------------------------------------
// D a t e n ü b e r t r a g u n g
// ----------------------------------------------------------------------------
//
void BlePoll::smStartCom()    // z.Zt. durch smIdle() ersetzt
{
  bleState = 2000;

  pduOut.len  = 6;
  radio->setChannel(chn);

  if(master)
  {
    nak = false;
    slaveIdx = 1;
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
// Datenübertragung Master         M a s t e r  - >  S l a v e
// ----------------------------------------------------------------------------
//
void BlePoll::smReqComE()
{
  bleState = 1100;

  if(!radio->disabled(txmRead))
  {
    radio->disable(txmRead);
    return;
  }

  curSlave = &slaveList[slaveIdx];
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

void BlePoll::smWaitAckComE()
{
  bleState = 1110;

  if(timeOut())
  {
    radio->disable(txmRead);
    curSlave->cntTo++;
    slaveIdx++;
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
    cntAllNaks++;
    slaveIdx++;
    adr++;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
  }
  radio->getStatistics(&statistic);
}

void BlePoll::smEndComE()
{
  if(pollStop || pollStopped)
  {
    pollStopped = true;
    return;
  }
  // Von vorne (zur Zeit, Test)
  //
  slaveIdx = 1;
  adr = 1;
  next(smReqEadr);
}

// ----------------------------------------------------------------------------
// Datenübertragung Master          S l a v e  - >  M a s t e r
// ----------------------------------------------------------------------------
//
void BlePoll::smReqComS()
{
  bleState = 2100;

  if(!radio->disabled(txmRead))
  {
    radio->disable(txmRead);
    return;
  }

  curSlave = &slaveList[slaveIdx];
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

void BlePoll::smWaitAckComS()
{
  bleState = 1110;

  if(timeOut())
  {
    radio->disable(txmRead);
    curSlave->cntTo++;
    slaveIdx++;
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
    cntAllNaks++;
    slaveIdx++;
    adr++;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
  }
  radio->getStatistics(&statistic);
}

void BlePoll::smEndComS()
{
  if(pollStop || pollStopped)
  {
    pollStopped = true;
    return;
  }
  // Von vorne (zur Zeit, Test)
  //
  slaveIdx = 1;
  adr = 1;
  next(smReqEadr);
}


// ----------------------------------------------------------------------------
// Datenübertragung Slave         M a s t e r  < - >  S l a v e
// ----------------------------------------------------------------------------
//
void BlePoll::smStartComES()
{
  bool  newValues;
  byte  lenValues;

  bleState = 1200;

  if(!radio->disabled(txmResp))
  {
    radio->disable(txmResp);
    cntWaitDisabled++;
    return;
  }

  nak = true;
  eadr = true;
  setPduAddress(&pduIn);
  pduIn.len = 6;

  nak = false;
  eadr = false;
  setPduAddress(&pduOut);

  newValues = getValues(&pduOut);
  radio->send(&pduIn, &pduOut, txmResp, newValues);
  next(smWaitComES);
}

void BlePoll::smWaitComES()
{
  bleState = 1210;

  radio->getStatistics(&statistic);
  if(!radio->fin(txmResp)) return;
  next(smStartComES);
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
      if(master)
        retv = cntWrong;
      else
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

    case 8:
      retv = cntAllNaks;
      break;

    case 9:
      retv = 9;
      break;
  }

  return(retv);
}









