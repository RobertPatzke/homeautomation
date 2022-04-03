//-----------------------------------------------------------------------------
// Thema:   Steuerung optischer und akustischer Ausgaben für Perfomer
// Datei:   SoaapComDue.h
// Editor:  Robert Patzke
// URI/URL: www.hs-hannover.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   01.April 2022
//
// Diese Bibliothek (Klassse) enthält diverse Ressourcen zum Zugriff auf den
// SOAAP-BLE-Master über eine serielle Schnittstelle des Arduino DUE.
//

#ifndef SoaapComDue_h
#define SoaapComDue_h

#define ScdNrOfSlaves     6

#include "USARTClass.h"
#include "SoaapMsg.h"

#ifndef byte
#define byte unsigned char
#endif


class SoaapComDue
{
public:
  // --------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // --------------------------------------------------------------------------
  //
  SoaapComDue(USARTClass *serial, SoaapMsg *soaMsg);

  // --------------------------------------------------------------------------
  // öffentliche Datentypen
  // --------------------------------------------------------------------------
  //
  typedef union
  {
    short   defShort[9];
    short   maxShort[13];
    float   maxFloat[6];
  } MeasValues, *pMeasValues;

private:
  // --------------------------------------------------------------------------
  // lokale Datentypen
  // --------------------------------------------------------------------------
  //

  typedef void (SoaapComDue::*CbVector)(void);
  typedef struct
  {
    int         slaveArea;
    int         slaveAdr;
    SoaapApId   apId;
    bool        newVal;
    short       measList[13];
  } SlaveValues, *pSlaveValues;

private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  USARTClass *pCom;
  SoaapMsg   *pMsg;
  CbVector    nextState;

  int         nrBytesIn;          // Antahl aktuell empfangener Zeichen
  int         serInCount;         // Zähler für empfangene Zeichen
  int         serInIdx;           // Index für besonderes Zeichen
  byte        serInChar;          // Einzelnes empfangenes Zeichen
  int         slaveAdr;           // Adresse des Soaap-Slave
  int         slaveArea;          // Quellennetzwerk-Info, Bereich, o.ä.
  SoaapApId   appId;              // Anwendungskennung, Datentyp, o.ä.

  int         measIdx;            // Index für den aktuellen Messwert
  int         nrMeas;             // Anzahl der Messwerte im Telegramm
  int         resMeas;            // Auflösung der Messwerte in Zeichen
  bool        anyNewVal;          // Neuer Wert von beliebigem Slave

  SlaveValues slValList[ScdNrOfSlaves + 1]; // Vorläufige Liste aller Messwerte
  int         slaveIdx;           // Index für lokale Slaveverwaltung

  byte        tmpBuffer[128];     // Zwischenspeicher für empfangene Zeichen

  // --------------------------------------------------------------------------
  // Inline-Methoden
  // --------------------------------------------------------------------------
  //

  // Index von Slave best. Adresse für Datenliste
  //
  int  getSlIdxAdr(int slAdr)
  {
    // Zur Zeit sind Index und Slaveadresse identisch (1-6)
    // Das wir später bei freier Zuordnung angepasst
    return(slAdr);
  }

  // --------------------------------------------------------------------------
  // lokale Methoden (Zustandsmaschine)
  // --------------------------------------------------------------------------
  //
  void runInit();
  void runWaitMsg();
  void runHeader();
  void runValues();

  // --------------------------------------------------------------------------
  // lokale Methoden (Hilfsfunktionen)
  // --------------------------------------------------------------------------
  //
  int   getSlDataAvail();

public:
  // --------------------------------------------------------------------------
  // Anwenderschnittstelle (Funktionen)
  // --------------------------------------------------------------------------
  //
  void run();                     // (zyklischer) Aufruf zum Ablauf
  int   anyDataAvail();           // Daten von irgendeinem Slave verfügbar
  bool  slDataAvail(int slAdr);   // Daten von bestimmtem Slave verfügbar
  int   getData(int slAdr, pMeasValues pMeas);
  // Daten von einem bestimmten Slave abholen

};

#endif // SoaapComDue_h
