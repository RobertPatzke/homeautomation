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
  cntAllRecs = 0;
  cntAllTo = 0;
  pollStopped = false;
  pollStop = false;
  recStopped = false;
  recStop = false;
  runCounter = 0;
  newValue = false;
  cbData = NULL;
  lenPollCtrl = 0;

  for(int i = 1; i <= MAXSLAVE; i++)
  {
    slaveList[i].cntNakEP = 0;
    slaveList[i].cntTo = 0;
    slaveList[i].pIdx = 0;
    slaveList[i].delayCnt = 5;
    slaveList[i].newPdu = false;
    slaveList[i].newMeas = false;
    pollList[i].prioCnt = 0;
    pollList[i].slIdx = 0;
    pollList[i].status = 0;
  }

  DataExchange = false;
}

// ----------------------------------------------------------------------------
void BlePoll::begin(ComType typeIn, int adrIn, AppType appType, dword watchDog)
{
  // TODO
  // --------------------------------------------------------------------------
  // Das muss nochmal völlig neu überarbeitet werden.
  // Zur Zeit sind viele Redundanzen und teilweise Mehrdeutigkeiten enthalten,
  // weil für jede Testanwendung spezifische Vorbereitungen gemacht wurden.
  // --------------------------------------------------------------------------
  //
  PlpCtrl25Ptr ctrlPtr;

  wdTimeOut = watchDog;   // WatchDog-Time-Out in Mikrosekunden

  if(typeIn == ctMASTER)
    master  = true;
  else
    master = false;  void  resetPollCounters();


  chn     = 0;          // 1. Bewerbungskanal
  area    = 0;          // Default-Anwendungsbereich
  eadr    = true;       // Start mit leerem Polling

  // --------------------------------------------------------------------------
  plMode  = plmEmpty;   // Leeres Polling (Adressenliste)
  // --------------------------------------------------------------------------

  if(master)
  {
    nak = true;         // Nak-Bit vom Master forciert leere Antwort
    maxAdr = adrIn;
    if(maxAdr > MAXSLAVE)
      maxAdr = MAXSLAVE;
    adr  = 1;
    slaveIdx = adr;     // Reserve für getrennte Verwaltung von adr und slaveIdx
    next(smInit);
  }
  else // Slave
  {
    nak = true;
    adr = adrIn;
    next(smInit);
  }

  if
    (
        appType == atSOAAP1
     || appType == atTestSend
     || appType == atDevSOAAP
     || appType == atSOAAP2
    )
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

  if(appType == atSOAAP1)
  {
    if(master)
    {
      valuePdu.appId = plptMeas9Ctrl4;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapM;
      fullCycle = true;
    }
    else
    {
      valuePdu.appId = plptMeas9Ctrl4;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapS;
    }
  }
  else if(appType == atSOAAP2)
  {
    if(master)
    {
      // Bei atSOAAP2 wird die Übermittlung von Daten beim Sendeaufruf implementiert.
      // Das erfordert eine spezifische Behandlung für jeden Slave.
      // Die globalen Anweisungen sind obsolet und sollen demnächst eliminiert werden.
      // (Vorher Auswertung kontrollieren).
      //
      valuePdu.appId = plptIMU3F4Ctrl4;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapM;
      fullCycle = true;

      for(int i = 1; i <= MAXSLAVE; i++)
      {
        ctrlPtr = (PlpCtrl25Ptr) &slaveList[i].control;
        ctrlPtr->counter = 0;
        ctrlPtr->type = appType;
        ctrlPtr->appId = plptCtrl2;
        ctrlPtr->ctrlPath = 0;
        ctrlPtr->ctrlCnt = 0;
        ctrlPtr->reqAppId = plptIMU3F4Ctrl4;
        ctrlPtr->ctrl[0] = 0;
        ctrlPtr->ctrl[1] = 0;
      }

    }
    else
    {
      valuePdu.appId = plptIMU3F4Ctrl4;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapS;
    }
  }
  else if (appType == atDevSOAAP)
  {
    if(master)
    {
      valuePdu.appId = plptMeas13;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapM;
      fullCycle = true;
    }
    else
    {
      valuePdu.appId = plptMeas13;
      ctrlPdu.appId = plptCtrl2;
      plMode = plmSoaapS;
    }
  }

  gAppId = (PlpType) valuePdu.appId;

  valuePdu.measCnt = 0;
  valuePdu.counter = 0;
  valuePdu.type = appType;
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

void BlePoll::setDataPollParams(int slAdr, int prio, int minPrio, dword timeOut)
{
  if(slAdr > MAXSLAVE) return;
  slaveList[slAdr].prioSet = prio;
  slaveList[slAdr].minPrio = minPrio;
  slaveList[slAdr].timeOut = timeOut;
}

void BlePoll::setCbDataPtr(cbDataPtr cbPtr)
{
  cbData = cbPtr;
}

void BlePoll::setCbCtrlPtr(cbCtrlPtr cbPtr)
{
  cbCtrl = cbPtr;
}


dword smStartComESCnt;

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

bool BlePoll::getValues(bcPduPtr pduPtr, PlpType appId)
{
  bool  retv = false;

  switch (appId)
  {
    case plptMeas6:
      pduPtr->len = sizeof(PlpMeas6) + 6;
      break;

    case plptMeas9Ctrl4:
      pduPtr->len = sizeof(PlpM9C4) + 6;
      break;

    case plptIMU3F4Ctrl4:
      pduPtr->len = sizeof(PlpI3S4C4) + 6;
      break;
  }
  pduPtr->data[0]++;      // Pdu-Counter
  pduPtr->data[1] = valuePdu.type;
  pduPtr->data[2] = appId;

  newValue = cbData(appId, &pduPtr->data[4]);

  if(newValue)
  {
    retv = true;
    pduPtr->data[3]++;              // measCnt
  }
  return(retv);
}


bool BlePoll::getCtrls(bcPduPtr pduPtr, PlpType appId)
{
  int   ctrlLen;
  PlpI3S4C4Ptr  locPtr = (PlpI3S4C4Ptr) &pduPtr->data[0];

  if(pollCtrl.len > 6)
    ctrlLen = pollCtrl.len - 6;
  else
    ctrlLen = 4;

  newCtrl = cbCtrl((PlpType) appId, &locPtr->ctrlPath, &pollCtrl.data[0], ctrlLen);

  return(newCtrl);
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

// Anhalten des Empfangs beim Slave
//
void BlePoll::stopSR()
{
  recStop = true;
}

// Weiterlaufen des Empfangs beim Slave
//
void BlePoll::resumeSR()
{
  recStop     = false;
  recStopped  = false;
}

// Abfrage, ob Slaveempfang gestoppt
//
bool BlePoll::stoppedSR()
{
  return(recStopped);
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

  for(int i = 1; i <= pollMaxNr; i++)
  {
    if(i == maxByte) break;
    slIdx = pollList[i].slIdx;
    dest[i-1] = slaveList[slIdx].adr;
  }
  return(pollMaxNr);
}

void  BlePoll::resetPollCounters()
{
  int           slIdx;
  SlavePtr      slPtr;

  for(int i = 1; i <= pollMaxNr; i++)
  {
    slIdx = pollList[i].slIdx;
    slPtr = &slaveList[slIdx];
    slPtr->cntAckDP = 0;
    slPtr->cntErrCrc = 0;
    slPtr->cntLostMeas = 0;
    slPtr->cntLostPdu = 0;
    slPtr->cntNakEP = 0;
    slPtr->cntTo = 0;
  }
}




// ****************************************************************************
// Zustandsmaschine
// ****************************************************************************
//

// ----------------------------------------------------------------------------
// Verzweigung nach Anwendung (nach Anlauf)
// ----------------------------------------------------------------------------
//
dword smInitCnt;

void BlePoll::smInit()
{
  bleState = 100;
  smInitCnt++;

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
      if(pollStop)
        pollStopped = true;
      if(!pollStopped)
        next(smStartEP);
      break;

    case plmSoaapS:
      next(smStartComES);
      break;

    case plmXchg:
      break;
  }
}

// ----------------------------------------------------------------------------
// Verzweigung nach Anwendung (im Betrieb)
// ----------------------------------------------------------------------------
//
dword smIdleCnt;

void BlePoll::smIdle()
{
  bleState = 200;
  smIdleCnt++;

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

    case plmSoaapM:
      if(!pollStopped)
        next(smStartEP);
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
    adr = 1;
    slaveIdx = adr;
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
    adr = 1;
    slaveIdx = adr;         // Slave-Array[0] reserviert
    pollIdx = 1;            // Poll-Array[0] reserviert
    pollNr = 0;             // Anzahl in der Poll-Liste
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

  // Datenstruktur für den Slave definieren
  //
  curSlave = &slaveList[slaveIdx];    // Zeiger auf zu pollenden Slave
  curSlave->adr  = adr;
  curSlave->area = area;
  curSlave->chn  = chn;

  curPoll = &pollList[pollIdx];       // Zeiger auf freien Platz in Poll-Liste

  setPduAddress();
  setTimeOut(epTimeOut);

  radio->getStatistics(&statistic);

  radio->send(&pduOut, txmPoll);
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
    radio->disable(txmPoll);

    if(curSlave->pIdx != 0)
    {
      // Wenn der Teilnehmer bereits in die Poll-Liste eingetragen ist
      // dann wird darin seine temporäre Abwesenheit markiert
      pollList[(int) curSlave->pIdx].status &= ~psSlaveIsPresent;
    }

    // Der Time-Out-Zähler macht erkenntlich, wie stark sich Störungen auswirken
    //
    curSlave->cntTo++;
    cntAllTo++;

    // Nächste Adresse und zur Anforderung
    //
    adr++;
    slaveIdx = adr;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
    return;
  }


  if(radio->fin(txmPoll, &crcError))
  {
    // Auf dem Kanal wurde ein Datensatz (BLE-Beacon) empfangen
    // und es werden die ersten 8 Byte (Header, Len und Adresse) geholt
    //
    cntAllRecs++;
    radio->getRecData(&pduIn, 8);

    if(pduIn.adr3 != pduOut.adr3 || pduIn.adr4 != pduOut.adr4 || pduIn.adr5 != pduOut.adr5)
    {
      // Wenn die höherwertigen 3 Byte der Adresse nicht mit der Anwendungskodierung
      // übereinstimmen, dann ist es ein Fremd-Beacon.
      // Diese werden gezählt und kennzeichnen, wie stark aktiv ein anderes
      // BLE-Netzwerk in Reichweite ist.
      //
      cntAlien++;

      // Nächste Adresse und zur Anforderung, nicht auf Time-Out warten
      //
      adr++;
      slaveIdx = adr;
      if(adr > maxAdr)
        next(smEndEP);
      else
        next(smReqEadr);
      return;
    }

    if(pduIn.adr0 != pduOut.adr0 || (pduIn.adr1 & 0x3F) != (pduOut.adr1 & 0x3F))
    {
      // Wenn die Teinehmernummer oder die Gebietsnummer falsch sind, dann ist
      // möglicherweise ein weiterer Master aktiv, der fehlerhafterweise denselben
      // Kanal verwendet.
      // Auch dieses Ereignis wird gezäht.
      cntWrong++;

      // Nächste Adresse und zur Anforderung, nicht auf Time-Out warten
      //
      adr++;
      slaveIdx = adr;
      if(adr > maxAdr)
        next(smEndEP);
      else
        next(smReqEadr);
      return;
    }

    // Alles korrekt, der adressierte Teilnehmer hat rechtzeitig geantwortet
    // Radio ist abgeschaltet
    //

    if(curSlave->pIdx != 0)
    {
      // Wenn der Teilnehmer bereits in die Poll-Liste eingetragen ist,
      // dann wird er darin als aktuell anwesend markiert
      //
      pollList[(int) curSlave->pIdx].status |= psSlaveIsPresent;
      pollNr++;
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
    adr++;
    slaveIdx = adr;

    // Wenn die vorgegeben Endadresse erreicht ist
    // dann zum Ende des Polldurchgangs über alle möglichen Teilnehmer,
    // ansonsten nächsten Teilnehmer pollen
    //
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
  }
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
      adr = 1;
      slaveIdx = adr;
      next(smReqEadr);
      return;
    }
    else if(epCycleTotal == 0)
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
      //pollIdx = 1; falsch, pollIdx zeigt auf nächsten freien Platz
      adr = 1;
      slaveIdx = adr;
      next(smReqEadr);
      return;
    }
    else if(epCycleRun == 0)
    {
      // und anschließend die Datenübertragung gestartet
      // oder das Polling ganz beendet
      //
      if(fullCycle)
        next(smStartCom);
      else
        next(smIdle);
      return;
    }
  }

  // Nächster Poll-Lauf, wenn epCycleXXX noch nicht abgelaufen ist
  // oder auf einen wert kleiner 0 gestellt wurde
  //
  pollNr = 0;
  //pollIdx = 1; falsch, pollIdx zeigt auf nächsten freien Platz
  adr = 1;
  slaveIdx = adr;
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
  if(!radio->fin(txmRespE, &crcError)) return;
  next(smWaitEadr);
}

// ----------------------------------------------------------------------------
// D a t e n ü b e r t r a g u n g
// ----------------------------------------------------------------------------
//

// Vorbereitung der Datenübertragung
//
void BlePoll::smStartCom()
{
  bleState = 2000;

  if(pollStop || pollStopped) // Der Start der Datenübertragung kann
  {                           // verzögert werden
    pollStopped = true;
    return;
  }

  // --------------------------------------------
  // Auswahl des Funkkanals (Frequenz)
  // --------------------------------------------
  //
  radio->setChannel(chn);

  // --------------------------------------------
  // Voreinstellungen für den Master
  // --------------------------------------------
  //
  if(master)
  {
    // Aufbau der Polling-Liste
    //
    for(int i = 1; i <= pollMaxNr; i++)
    {
      int slIdx = pollList[i].slIdx;
      pollList[i].prioCnt = slaveList[slIdx].prioSet;
    }
    pollIdx = 1;

    next(smReqComS);
  }
  // --------------------------------------------
  // Voreinstellungen für den Slave
  // --------------------------------------------
  //
  else
  {
    nak = true;
    next(smWaitEadr);
  }

  DataExchange = true;
}

// ----------------------------------------------------------------------------
// Datenübertragung Master          S l a v e  - >  M a s t e r
// ----------------------------------------------------------------------------
//

// M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M
// Polling : Anfordern von Daten beim Slave
// ----------------------------------------------------------------------------
//
void BlePoll::smReqComS()
{
  bleState = 2100;

  if(pollStop || pollStopped) // Das Polling kann
  {                           // angehalten werden
    pollStopped = true;
    return;
  }

  // Es ist von einem vorherigen Funkempfang auszugehen
  // Die Hardware muss erst ausgeschaltet werden
  //
  if(!radio->disabled(txmPoll))
  {
    radio->disable(txmPoll);
    return;
  }

  // Der aufzufordernde Teilnehmer wird der Poll-Liste entnommen
  //
  curPoll = &pollList[pollIdx];

  // Ein Aufruf erfolgt nur, wenn der Prioritätszähler auf 0 steht
  // ansonsten wird er dekrementiert und der nächste Teilnehmer
  // im nächsten Zustandslauf ausgewählt.
  //
  if(curPoll->prioCnt > 0)
  {
    curPoll->prioCnt--;
    pollIdx++;
    if(pollIdx > pollMaxNr)
      pollIdx = 1;
    return;
  }

  // Zugriff auf den Slave aus der Poll-Liste vorbereiten
  //
  slaveIdx = curPoll->slIdx;
  curSlave = &slaveList[slaveIdx];

  // Slave-spezifische Parameter setzen
  //
  eadr = false;         // Ist true, wenn Daten nur zum Slave übertragen werden
  nak = false;          // Ist true, wenn keine Daten übertragen werden (empty poll)
  adr = curSlave->adr;
  area = curSlave->area;
  setPduAddress(&pduOut);
  setTimeOut(curSlave->timeOut);

  // Vorbereitung der mit dem Polling übermittelten Daten
  //
  //ctrlPdu.counter = 0;        // Zähler im Steuertelegramm
  //ctrlPdu.appId = gAppId;     // Info für Slave zur Antwort


  pduOut.len  = 14;           // Adresse (6) + 7 Byte Steuerung
  ctrlPduPtr = (PlpCtrl25Ptr) &curSlave->control;

  int dIdx = 0;
  pduOut.data[dIdx++] = ctrlPduPtr->counter++;
  pduOut.data[dIdx++] = ctrlPduPtr->type;
  pduOut.data[dIdx++] = ctrlPduPtr->appId;
  pduOut.data[dIdx++] = ctrlPduPtr->ctrlPath;
  pduOut.data[dIdx++] = ctrlPduPtr->ctrlCnt;
  pduOut.data[dIdx++] = ctrlPduPtr->reqAppId;
  pduOut.data[dIdx++] = ctrlPduPtr->ctrl[0];
  pduOut.data[dIdx++] = ctrlPduPtr->ctrl[1];

  // Statistic-Daten einholen für evt. Auswertung
  //
  radio->getStatistics(&statistic);

  // Aufruf des Slave starten
  //
  radio->send(&pduOut, txmPoll);

  cntPolling++;
  next(smWaitAckComS);
}

// M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M
// Warten auf die Antwort vom Slave
// ----------------------------------------------------------------------------
//
void BlePoll::smWaitAckComS()
{
  byte    tmpByte;
  short   tmpShort;

  // Zeiger zur spezifischen Betrachtung von Empfangsdaten
  PlPduMeasPtr resPtr;

  bleState = 2110;

  if(timeOut())
  {
    // Wenn der Slave nicht antwortet (kann auch eine Störung sein),
    // dann wird seine Priorität heruntergesetzt (Zählwert erhöht)
    // und der nächste Slave aus der Poll-Liste angefragt
    //  byte      oldPduCount;

    curSlave->prioSet++;
    if(curSlave->prioSet > curSlave->minPrio)
      curSlave->prioSet = curSlave->minPrio;

    curPoll->prioCnt = curSlave->prioSet;

    curSlave->cntTo++;
    pollIdx++;
    if(pollIdx > pollMaxNr)
      pollIdx = 1;

    radio->disable(txmPoll);
    next(smReqComS);
    return;
  }

  if(radio->fin(txmPoll, &crcError))
  {
    cntAllRecs++;

    // Wenn (irgend-) ein Beacon eingegangen ist,
    // wird die maximale (BLE-Standard) Anzahl von Bytes kopiert
    //
    radio->getRecData(&pduIn, 39);

    if(pduIn.adr3 != pduOut.adr3 || pduIn.adr4 != pduOut.adr4 || pduIn.adr5 != pduOut.adr5)
    {
      // Beacons aus fremdem Netzen werden nur gezählt und es wird weiter gewartet
      //
      cntAlien++;
      pollIdx++;
      if(pollIdx > pollMaxNr)
        pollIdx = 1;

      next(smReqComS);
      return;
    }

    if(pduIn.adr0 != pduOut.adr0 || (pduIn.adr1 & 0x3F) != (pduOut.adr1 & 0x3F))
    {
      // Beacons mit falscher Slaveadresse werden ebenfalls nur gezählt
      // Hier wird später die Rundrufübertragung implementiert
      //
      cntWrong++;
      pollIdx++;
      if(pollIdx > pollMaxNr)
        pollIdx = 1;

      next(smReqComS);
      return;
    }

    // Antwort vom richtigen Teilnehmer ist eingegangen
    //

    if(crcError)
    {
      // Die Daten werden bei einem CRC-Fehler verworfen.
      // Der Fehler wird gezählt und ist ein Hinweis auf fremde
      // Funkaktivitäten
      //
      curSlave->cntErrCrc++;
      pollIdx++;
      if(pollIdx > pollMaxNr)
        pollIdx = 1;

      next(smReqComS);
      return;
    }

    // Die Daten werden in der Slave-Struktur abgelegt
    //
    resPtr = (PlPduMeasPtr) &curSlave->result;

    resPtr->counter  = pduIn.data[0];
    resPtr->type     = pduIn.data[1];
    resPtr->appId    = pduIn.data[2];
    resPtr->measCnt  = pduIn.data[3];

    // Die Inhalte sind abhängig von der <appId>
    //
    switch(resPtr->appId)
    {
      case plptMeas9Ctrl4:
        ((PlpM9C4Ptr) resPtr)->meas[0]  = *(word *) &pduIn.data[4];
        ((PlpM9C4Ptr) resPtr)->meas[1]  = *(word *) &pduIn.data[6];
        ((PlpM9C4Ptr) resPtr)->meas[2]  = *(word *) &pduIn.data[8];
        ((PlpM9C4Ptr) resPtr)->meas[3]  = *(word *) &pduIn.data[10];
        ((PlpM9C4Ptr) resPtr)->meas[4]  = *(word *) &pduIn.data[12];
        ((PlpM9C4Ptr) resPtr)->meas[5]  = *(word *) &pduIn.data[14];
        ((PlpM9C4Ptr) resPtr)->meas[6]  = *(word *) &pduIn.data[16];
        ((PlpM9C4Ptr) resPtr)->meas[7]  = *(word *) &pduIn.data[18];
        ((PlpM9C4Ptr) resPtr)->meas[8]  = *(word *) &pduIn.data[20];
        ((PlpM9C4Ptr) resPtr)->ctrlPath = pduIn.data[22];
        ((PlpM9C4Ptr) resPtr)->procCnt  = pduIn.data[23];
        ((PlpM9C4Ptr) resPtr)->ctrl[0]  = pduIn.data[24];
        ((PlpM9C4Ptr) resPtr)->ctrl[1]  = pduIn.data[25];
        break;

      case plptIMU3F4Ctrl4:
        ((PlpI3S4C4Ptr) resPtr)->meas[0]  = *(float *) &pduIn.data[4];
        ((PlpI3S4C4Ptr) resPtr)->meas[1]  = *(float *) &pduIn.data[8];
        ((PlpI3S4C4Ptr) resPtr)->meas[2]  = *(float *) &pduIn.data[12];
        ((PlpI3S4C4Ptr) resPtr)->state[0] = pduIn.data[16];
        ((PlpI3S4C4Ptr) resPtr)->state[1] = pduIn.data[17];
        ((PlpI3S4C4Ptr) resPtr)->state[2] = pduIn.data[18];
        ((PlpI3S4C4Ptr) resPtr)->state[3] = pduIn.data[19];
        ((PlpI3S4C4Ptr) resPtr)->ctrlPath = pduIn.data[20];
        ((PlpI3S4C4Ptr) resPtr)->procCnt  = pduIn.data[21];
        ((PlpI3S4C4Ptr) resPtr)->ctrl[0]  = pduIn.data[22];
        ((PlpI3S4C4Ptr) resPtr)->ctrl[1]  = pduIn.data[23];
        break;

      case plptMeas6:
        ((PlpM9C4Ptr) resPtr)->meas[0]  = *(word *) &pduIn.data[4];
        ((PlpM9C4Ptr) resPtr)->meas[1]  = *(word *) &pduIn.data[6];
        ((PlpM9C4Ptr) resPtr)->meas[2]  = *(word *) &pduIn.data[8];
        ((PlpM9C4Ptr) resPtr)->meas[3]  = *(word *) &pduIn.data[10];
        ((PlpM9C4Ptr) resPtr)->meas[4]  = *(word *) &pduIn.data[12];
        ((PlpM9C4Ptr) resPtr)->meas[5]  = *(word *) &pduIn.data[14];
        ((PlpM9C4Ptr) resPtr)->meas[6]  = *(word *) &pduIn.data[16];
        break;
    }


    // Zählen der verlorenen Telegramme und Messwerte
    // beginnt um <delayCnt> Pollzyklen verzögert
    //
    if(curSlave->delayCnt == 0)
    {
      tmpByte = curSlave->result.counter - curSlave->oldPduCount;
      if(tmpByte > 1)
        curSlave->cntLostPdu += tmpByte - 1;

      tmpByte = resPtr->measCnt - curSlave->oldMeasCount;
      if(tmpByte != 0)
        curSlave->newMeas = true;
      if(tmpByte > 1)
        curSlave->cntLostMeas += tmpByte - 1;
    }
    else curSlave->delayCnt--;

    curSlave->oldPduCount = curSlave->result.counter;
    curSlave->oldMeasCount = resPtr->measCnt;

    curSlave->newPdu  = true;
    curSlave->cntAckDP++;
    curPoll->prioCnt = curSlave->prioSet;

    pollIdx++;
    if(pollIdx > pollMaxNr)
      pollIdx = 1;

    next(smReqComS);
    return;
  }
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
  adr = 1;
  slaveIdx = adr;
  next(smReqEadr);
}

// ----------------------------------------------------------------------------
// Datenübertragung Master         M a s t e r  - >  S l a v e
// ----------------------------------------------------------------------------
//
void BlePoll::smReqComE()
{
  bleState = 4100;

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
  bleState = 4110;

  if(timeOut())
  {
    radio->disable(txmRead);
    curSlave->cntTo++;
    adr++;
    slaveIdx = adr;
    if(adr > maxAdr)
      next(smEndEP);
    else
      next(smReqEadr);
    return;
  }


  if(radio->fin(txmRead, &crcError))
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
    adr++;
    slaveIdx = adr;
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
  adr = 1;
  slaveIdx = adr;
  next(smReqEadr);
}


// S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S
// ----------------------------------------------------------------------------
// Datenübertragung Slave         M a s t e r  < - >  S l a v e
// ----------------------------------------------------------------------------
//

// S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S
// Vorbereitungen für den Empfang (Polling durch Master)
// ----------------------------------------------------------------------------
//
void BlePoll::smStartComES()
{
  bool  newValues;
  bool  newCtrl;
  byte  lenValues;
  byte  appId;

  bleState = 1310;
  smStartComESCnt++;

  if(recStop || recStopped)
  {
    recStopped = true;
    return;
  }

  // Wenn keine Daten von der Anwendung zur Verfügung gestellt werden,
  // dann macht der Betrieb hier keinen Sinn und der Slave geht in IDLE
  //
  if(cbData == NULL)
  {
    next(smIdle);
    return;
  }

  // Falls der Sender noch nicht ausgeschaltet ist, muss gewartet werden
  //
  if(!radio->disabled(txmResp))
  {
    radio->disable(txmResp);
    cntWaitDisabled++;
    return;
  }

  // Vorbereiten des erwarteten Inhalts beim Polling durch den Master
  //
  nak = true;
  eadr = true;
  setPduAddress(&pduIn);
  pduIn.len = 6;


  // Vorbereiten des zu sendenden Inhalts als Antwort auf das Polling
  //
  nak = false;
  eadr = false;
  setPduAddress(&pduOut);

  // Eintragen der Messwerte in das Sendetelegramm
  //
  if(lenPollCtrl == 0)          // Wenn noch kein Empfangszyklus vorliegt
    appId = valuePdu.appId;     // dann wird der voreingestellte Satz gewählt
  else
    appId = pollCtrl.data[5];   // ansonsten der speziell angeforderte

  newValues = getValues(&pduOut, (PlpType) appId);

  if((appId == plptMeas9Ctrl4 || appId == plptIMU3F4Ctrl4) && (cbCtrl != NULL))
    getCtrls(&pduOut, (PlpType) appId);

  radio->setChannel(chn);
  radio->send(&pduIn, &pduOut, txmResp, newValues);

  setTimeOut(wdTimeOut);
  next(smWaitComES);
}

// S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S
void BlePoll::smWaitComES()
{
  bleState = 1320;

  radio->getStatistics(&statistic);
  if(timeOut())
  {
    //TEST
    lenPollCtrl = 2;
    next(smStartComES);
    return;
  }

  if(!radio->fin(txmResp, &crcError)) return;
  //
  // Übertragung beendet, Daten empfangen (polling) und versendet (response)
  //
  lenPollCtrl = radio->getRecData(&pollCtrl, txmResp, sizeof(pollCtrl));

  next(smStartComES);
}

// M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M
// --------------------------------------------------------------------------
// Anwenderfunktionen als Master
// --------------------------------------------------------------------------
//

// neue Steuerungsdaten für einen Slave
//
void BlePoll::updControl(int adr, byte *ctrlList, int nr)
{
  if(adr <= 0) return;
  if(adr >= MAXSLAVE) return;
  if(nr <= 1) return;
  if(nr > 26) return;

  SlavePtr  slavePtr = &slaveList[adr];
  PlpCtrl25Ptr ctrlPtr = (PlpCtrl25Ptr) &slavePtr->control;
  for(int i = 0; i < nr; i++)
    ctrlPtr->ctrl[i] = ctrlList[i];
  ctrlPtr->ctrlCnt++;
  slavePtr->rspOk = false;
}

// Feststellenn, ob Übertragung der Steuerungsdaten erfolgt ist
//
bool BlePoll::ackTrans(int adr)
{
  if(adr <= 0) return(false);
  if(adr >= MAXSLAVE) return(false);

  SlavePtr  slavePtr = &slaveList[adr];
  return(slavePtr->rspOk);
}

// Feststellen, ob Steuerungsdaten beim Slave verarbeitet sind
//
bool BlePoll::ackControl(int adr)
{
  if(adr <= 0) return(false);
  if(adr >= MAXSLAVE) return(false);

  SlavePtr  slavePtr = &slaveList[adr];
  PlpCtrl25Ptr ctrlPtr = (PlpCtrl25Ptr) &slavePtr->control;
  if(ctrlPtr->ctrlCnt == slavePtr->rspCtrlCount)
    return(true);
  else
    return(false);
}

// ----------------------------------------------------------------------------
// Zugriff auf Slavedaten über die Adresse
// ----------------------------------------------------------------------------
//

// Feststellen, ob ein Slave neue Messwerte hat
//
bool  BlePoll::measAvail(int slAdr)
{
  if(slAdr < 1) return(false);
  if(slAdr >= MAXSLAVE) return(false);

  SlavePtr  slavePtr = &slaveList[slAdr];

  if(!slavePtr->newMeas)
    return(false);

  slavePtr->newMeas = false;
  return(true);
}

// Auslesen der Netzwerk-Area
//
int BlePoll::getArea(int slAdr)
{
  if(slAdr < 1) return(false);
  if(slAdr >= MAXSLAVE) return(false);

  SlavePtr  slavePtr = &slaveList[slAdr];

  return(slavePtr->area);
}

// Auslesen der AppId aus Sicht der Klasse BlePoll
//
PlpType BlePoll::getAppId(int slAdr)
{
  if(slAdr < 1) return(plptError);
  if(slAdr >= MAXSLAVE) return(plptError);

  SlavePtr  slavePtr = &slaveList[slAdr];

  return((PlpType) slavePtr->result.plData[0]);
}

// Auslesen der Messwerte
//
int BlePoll::getMeas(int slAdr, byte *dest)
{
  int     anzByte;
  PlpType appId;

  if(slAdr < 1) return(false);
  if(slAdr >= MAXSLAVE) return(false);

  SlavePtr  slavePtr = &slaveList[slAdr];

  appId = (PlpType) slavePtr->result.plData[0];

  switch(appId)
  {
    case plptMeas6:
      anzByte = 12;
      break;

    case plptMeas9:
      anzByte = 18;
      break;

    case plptMeas9Ctrl4:
      anzByte = 22;
      break;

    case plptIMU3F4Ctrl4:
      anzByte = 20;
      break;

    case plptMeas13:
      anzByte = 26;
      break;

    default:
      anzByte = 18;
      break;
  }

  for (int i = 0; i < anzByte; i++)
  {
    dest[i] = slavePtr->result.plData[i+2];
  }

  return(anzByte);
}

/*
// Auslesen der Steuerwerte/Antwort
//
CtrlData2Ptr BlePoll::getCtrl(int slAdr)
{

  if(slAdr < 1) return(NULL);
  if(slAdr >= MAXSLAVE) return(NULL);

  SlavePtr  slavePtr = &slaveList[slAdr];
  PlpI3S4C4Ptr dptr = (PlpI3S4C4Ptr) &slavePtr->result;
  return((CtrlData2Ptr) &dptr->ctrlPath);
}
*/


// S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S
// --------------------------------------------------------------------------
// Anwenderfunktionen als Slave
// --------------------------------------------------------------------------
//
bool BlePoll::getCtrlResp(int adr, CtrlResp2Ptr ctlRspPtr)
{
  PlpType   appId;
  bool      retv = false;
  SlavePtr  slPtr = &slaveList[adr];

  appId = (PlpType) slPtr->result.plData[0];

  switch(appId)
  {
    case plptMeas6:
      break;

    case plptMeas9:
      break;

    case plptMeas9Ctrl4:
      ctlRspPtr->ctrlPath = ((PlpM9C4Ptr) &slPtr->result)->ctrlPath;
      ctlRspPtr->procCnt  = ((PlpM9C4Ptr) &slPtr->result)->procCnt;
      ctlRspPtr->ctrl[0]  = ((PlpM9C4Ptr) &slPtr->result)->ctrl[0];
      ctlRspPtr->ctrl[1]  = ((PlpM9C4Ptr) &slPtr->result)->ctrl[1];
      retv = true;
      break;

    case plptIMU3F4Ctrl4:
      ctlRspPtr->ctrlPath = ((PlpI3S4C4Ptr) &slPtr->result)->ctrlPath;
      ctlRspPtr->procCnt  = ((PlpI3S4C4Ptr) &slPtr->result)->procCnt;
      ctlRspPtr->ctrl[0]  = ((PlpI3S4C4Ptr) &slPtr->result)->ctrl[0];
      ctlRspPtr->ctrl[1]  = ((PlpI3S4C4Ptr) &slPtr->result)->ctrl[1];
      retv = true;
      break;

    case plptMeas13:
      break;

    default:
      break;
  }

  return(retv);
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
      retv = plMode;
      break;

    case 1:
      retv = cntAllRecs;
      break;

    case 2:
      retv = cntAllNaks;
      break;

    case 3:
      retv = cntAllTo;
      break;

    case 4:
      retv = cntAlien;
      break;

    case 5:
      retv = cntWrong;
      break;

    case 6:
      retv = statistic.pollAcks;
      break;

    case 7:
      retv = statistic.pollNaks;
      break;

    case 8:
      retv = bleState;
      break;

    case 9:
      retv = radio->getState();
      break;
  }

  return(retv);
}

dword BlePoll::getStatistics(TxStatisticsPtr dest)
{
  *dest = statistic;
  return(bleState);
}


SlavePtr      BlePoll::getSlavePtr(int idx)
{
  return(&slaveList[idx]);
}

PollStatePtr  BlePoll::getPollPtr(int idx)
{
  return(&pollList[idx]);
}

int BlePoll::getCtrlData(byte *dest)
{
  byte *src = (byte *) &pollCtrl;
  for(int i = 0; i < lenPollCtrl+2; i++)
    dest[i] = src[i];
  return(lenPollCtrl+2);
}






