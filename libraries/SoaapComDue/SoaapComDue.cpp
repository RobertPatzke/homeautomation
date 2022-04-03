//-----------------------------------------------------------------------------
// Thema:   Steuerung optischer und akustischer Ausgaben für Perfomer
// Datei:   SoaapComDue.cpp
// Editor:  Robert Patzke
// URI/URL: www.hs-hannover.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   01.April 2022
//
// Diese Bibliothek (Klassse) enthält diverse Ressourcen zum Zugriff auf den
// SOAAP-Master über eine serielle Schnittstelle des Arduino DUE.
//

#include "SoaapComDue.h"

#define next(x) nextState = &SoaapComDue::x

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//
SoaapComDue::SoaapComDue(USARTClass *serial, SoaapMsg *soaMsg)
{
  pCom = serial;
  pMsg = soaMsg;
  nextState = &SoaapComDue::runInit;

  nrBytesIn = 0;
  serInIdx = 0;
  serInChar = 0;
  serInCount = 0;
  slaveAdr = 0;
  slaveArea = 0;
  appId = (SoaapApId) 0;

  measIdx = 0;
  nrMeas = 0;
  resMeas = 0;
  slaveIdx = 0;
  anyNewVal = false;
}

// --------------------------------------------------------------------------
// lokale Methoden (Zustandsmaschine)
// --------------------------------------------------------------------------
//
void SoaapComDue::runInit()
{
  next(runWaitMsg);
}

void SoaapComDue::runWaitMsg()
{
  nrBytesIn = pCom->available();
  // Anzahl der über <serial> eingetroffenen Bytes (Zeichen)

  if(nrBytesIn < 1) return;
  // Beim nächsten Takt wieder in diesen Zustand, wenn kein Zeichen da

  for(serInIdx = 0; serInIdx < nrBytesIn; serInIdx++)
  {                               // Suchen nach Startzeichen
    serInChar = pCom->read();
    if(serInChar < 0x20) break;
  }

  if(serInIdx == nrBytesIn) return;
  // Beim nächsten Takt wieder in diesen Zustand, wenn Startzeichen nicht dabei

  slaveArea = serInChar & 0x1F;   // Bereich auskodieren

  serInIdx = 0;     // Index neu setzen für Inhaltszuordnung
  next(runHeader);
  // Beim nächsten Takt zum Zustand <runHeader>

}

void SoaapComDue::runHeader()
{
    nrBytesIn = pCom->available();
    // Anzahl der über Serial1 eingetroffenen Bytes (Zeichen)

    if(nrBytesIn < 1) return;
    // Beim nächsten Takt wieder in diesen Zustand, wenn kein Zeichen da

    serInChar = pCom->read();
    // einzelnes Zeichen lesen

    if(serInIdx == 0)             // nach der Area folgt die Slaveadresse
    {
      slaveAdr = serInChar & 0x1F;               // 1 - 31
      slaveIdx = slaveAdr;        // vorläufig lineare Adress/Index-Zuordnung
      serInIdx++;
      // Beim nächsten Takt wieder in diesen Zustand
    }
    else                     // und dann die Anwendungskennung
    {
      appId = (SoaapApId) serInChar;
      measIdx = 0;                      // Index für Messwertunterscheidung
      serInIdx = 0;                     // Index für Messwertaufbau
      nrMeas = pMsg->measCnt(appId);   // Anzahl Messwerte im Telegramm
      resMeas = pMsg->measRes(appId);  // Auflösung der Messwerte in Zeichen
      next(runValues);
      // Beim nächsten Takt zum Zustand <runValues>
    }
}

void SoaapComDue::runValues()
{
  int i;

  do
  {
    nrBytesIn = pCom->available();
    // Anzahl der über Serial1 eingetroffenen Bytes (Zeichen)

    if(nrBytesIn < 1) return;
    // Beim nächsten Takt wieder in diesen Zustand, wenn kein Zeichen da

    for(i = 0; i < nrBytesIn; i++)
    {
      tmpBuffer[serInIdx++] = pCom->read();
      // einzelnes Zeichen lesen

      if(serInIdx == resMeas) break;
      // Alle Zeichen vom Messwert da, also raus
    }

    if(serInIdx < resMeas) return;
    // Wenn noch nicht ale Zeichen vom Messwert erfasst, dann von vorn

    slValList[slaveIdx].measList[measIdx++] = pMsg->asc2meas(tmpBuffer);
    // Zeichenkette in Messwert wandeln und speichern

    if(measIdx == nrMeas) break;
    // Falls mehr als ein Telegramm eingetroffen ist
    // muss hier ein Ausstieg erfolgen

    serInIdx = 0;   // Nächste Zeichenfolge
  }
  while (pCom->available() > 0);
  // Die Taktzeit der Zustandsmaschine ist größer, als die
  // Übertragungszeit von einem Zeichen.
  // Deshalb werden in einem Zustandstakt alle inzwischen eingetroffenen
  // Zeichen bearbeitet.

  if(measIdx < nrMeas) return;
  // Im Zustand bleiben, bis alle Messwerte gewandelt sind

  slValList[slaveIdx].newVal = true;
  anyNewVal = true;

  next(runWaitMsg);
}

// --------------------------------------------------------------------------
// lokale Methoden (Hilfsfunktionen)
// --------------------------------------------------------------------------
//

// Erster Slave (kleinste Adresse) mit Daten
//
int   SoaapComDue::getSlDataAvail()
{
  for(int i = 1; i <= ScdNrOfSlaves; i++)
    if(slValList[i].newVal)
      return(i);
  return(0);
}

// ----------------------------------------------------------------------------
// Anwenderschnittstelle (Funktionen/Methoden)
// ----------------------------------------------------------------------------
//

// (zyklischer) Aufruf zum Ablauf
//
void SoaapComDue::run()
{
  if(nextState != NULL)
    (this->*nextState)();
}

// Daten von irgendeinem Slave verfügbar
//
int   SoaapComDue::anyDataAvail()
{
  if(!anyNewVal) return(0);
  else return(getSlDataAvail());
}

// Daten von bestimmtem Slave verfügbar
//
bool  SoaapComDue::slDataAvail(int slAdr)
{
  return(slValList[getSlIdxAdr(slAdr)].newVal);
}

// Daten von einem bestimmten Slave abholen
//
int   SoaapComDue::getData(int slAdr, pMeasValues pMeas)
{
  int slIdx = getSlIdxAdr(slAdr);
  for(int i = 0; i < 8; i++)
    pMeas->defShort[i] = slValList[slIdx].measList[i];
  slValList[slIdx].newVal = false;
  anyNewVal = false;
  for(int i = 1; i <= ScdNrOfSlaves; i++)
    if(slValList[i].newVal) anyNewVal = true;
  return(0);
}


