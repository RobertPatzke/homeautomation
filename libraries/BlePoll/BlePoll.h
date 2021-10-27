//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   BlePoll.h
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

#ifndef BlePoll_h
#define BlePoll_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "arduinoDefs.h"
#include "bleSpec.h"
#include "IntrfRadio.h"

#define MAXSLAVE  100

// Betriebsmodus (Polling und Slave)
//
typedef enum _PlMode
{
  plmIdle,        // Ruhezustand, Gerät nicht im Netz aktiv
  plmTest,        // Low Level Tests
  plmEmpty,       // Leeres Polling (Aufbau Adressliste)
  plmScan,        // Daten aller aktiven Teilnehmer holen (Master)
  plmSoaapM,      // Vollständiger Betrieb SOAAP (Master)
  plmXchg         // Daten übertragen (Slave, beide Richtungen)
} PlMode;

// Grundsätzliche Datenstruktur für die Nutzdaten
//
typedef struct _PlPduBase
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur (AppType)
  byte  plData[29];   // weitere spezifische Nutzdaten
} PlPduBase, *PlPduBasePtr;

// Erweiterte Datenstruktur für die Nutzdaten
//
typedef struct _PlPduExtd
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur (AppType)
  byte  plData[247];  // weitere spezifische Nutzdaten
} PlPduExtd, *PlPduExtdPtr;

// Datentypen (type in plPduBase)
//
typedef enum _PlpType
{
  plptEmpty,        // Leeres Telegramm (nur adresse)
  plptBasic,        // Grundlegende Bytestruktur
  plptFullMeas,     // Maximale Belegung mit 16-Bit Messwerten (word)
  plptMeas3,        // 3 Messwerte (1 Raumsensor)
  plptMeas6,        // 6 Messwerte (2 Raumsensoren)
  plptMeas9         // 9 Messwerte (3 Raumsensoren)
} PlpType, *PlpTypePtr;

// Spezifische Datenstrukturen
//
typedef struct _PlpFullMeas
{
  word    meas[12]; // Liste von 12 Messwerten
  byte    appId;    // Kennzeichnung für Dateninhalte (PlpType)
  byte    align;    // Wird nicht gesendet, kennzeichnet Alignement
} PlpFullMeas, *PlpFullMeasPtr;

typedef struct _PlpMeas3
{
  byte    appId;    // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;  // Zähler für Messwertaktualisierung
  word    meas[3];  // Liste von 3 Messwerten
} PlpMeas3, *PlpMeas3Ptr;

typedef struct _PlpMeas6
{
  byte    appId;    // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;  // Zähler für Messwertaktualisierung
  word    meas[6];  // Liste von 6 Messwerten
} PlpMeas6, *PlpMeas6Ptr;

typedef struct _PlpMeas9
{
  byte    appId;    // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;  // Zähler für Messwertaktualisierung
  word    meas[9];  // Liste von 9 Messwerten
} PlpMeas9, *PlpMeas9Ptr;

// Identifikator für die Art der Daten
//
typedef enum _MeasId
{
  app               // Gestaltung/Bedeutung der Daten aus Anwendung
} MeasId, *MeasIdPtr;

typedef struct _Slave
{
  dword   cntTo;
  dword   cntNakEP;
  byte    adr;
  byte    area;
  byte    chn;
  byte    pIdx;
} Slave, *SlavePtr;


typedef struct _PollInfo
{
  dword   aliens;     // Anzahl der netzfremden Aktivitäten
  dword   wrongs;     // Anzahl ungewünschter Netzaktivitäten
} PollInfo, *PollInfoPtr;

typedef struct _PollState
{
  byte    slIdx;      // Index in der Slave-Liste
  byte    prioCnt;    // Prioritätszähler
  byte    prioSet;    // Priorität
  byte    status;     // Zustand
} PollState, *PollStatePtr;

#define psSlaveWasPresent 0x01
#define psSlaveIsPresent  0x02

// ----------------------------------------------------------------------------
//                            B l e P o l l
// ----------------------------------------------------------------------------
class BlePoll
{
public:
  // -------------------------------------------------------------------------
  // Öffentliche Datentypen
  // -------------------------------------------------------------------------
  //
  typedef enum _ComType
  {
    ctMASTER,
    ctSLAVE,
    ctHybrid
  } ComType;

  // Identifikator für die Anwendung
  //
  typedef enum _AppType
  {
    atDefault,        // Standard-Default-Anwendung
    atTestSend,       // einfacher Sendetest (Soaap)
    atSOAAP,          // Steuerung optischer und akustischer Ausgaben für Performance-Künstler
    atDHA             // Dezentrale Hausautomatisierung
  } AppType;

private:
  // -------------------------------------------------------------------------
  // Private Datentypen
  // -------------------------------------------------------------------------
  //

  typedef void (BlePoll::*cbVector)(void);
  typedef dword (*MicsecFuPtr)(void);

  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  IntrfRadio    *radio;
  bcPdu         pduOut;
  bcPdu         pduIn;
  cbVector      nextState;
  MicsecFuPtr   micSec;
  dword         toSet;
  dword         toValue;
  dword         cycleMics;
  dword         cycleCnt;

  int           chn;
  int           adr;
  int           area;
  bool          master;
  bool          eadr;
  bool          nak;

  PlMode        plMode;
  PlMode        oldPlMode;
  PlpType       plpType;

  Slave         slaveList[MAXSLAVE+1];
  SlavePtr      curSlave;
  int           slaveIdx;
  PollState     pollList[MAXSLAVE+1];
  PollStatePtr  curPoll;
  int           pollIdx;
  int           pollNr;
  int           pollMaxNr;

  int           maxAdr;
  dword         cntPolling;
  bool          pollStop;
  bool          pollStopped;

  dword         cntAllNaks;
  dword         cntAlien;
  dword         cntWrong;
  dword         cntWaitDisabled;

  dword         bleState;
  dword         runCounter;

  TxStatistics  statistic;
  PlpMeas9      valuePdu;
  bool          newValue;

  // Einstellungen für den Anwendungsbetrieb
  //
  bool          fullCycle;      // Vollständige Anwendung (EP & Data)
  int           epCycleTotal;   // Anzahl der leeren Pollings gesamt
  int           epCycleRun;     // Anzahl der leeren Pollings nach Kontakt
  dword         epTimeOut;      // Time-Out in Mikrosekunden


  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void setPduAddress();
  void setPduAddress(bcPduPtr pduPtr);
  void setTimeOut(dword value);
  bool timeOut();
  bool getValues(bcPduPtr pduPtr);

  // Zustandsmaschine
  // -----------------------------
  void smInit();
  void smIdle();

  // Leeres Polling Master
  //
  void smStartEP();
  void smReqEadr();
  void smWaitNak();
  void smEndEP();

  // Leeres Polling Slave
  //
  void smWaitEadr();
  void smEvalPoll();

  // Datenübertragung
  //
  void smStartCom();

  // Master: Master -> Slave
  //
  void smReqComE();
  void smWaitAckComE();
  void smEndComE();

  // Master: Slave -> Master
  //
  void smReqComS();
  void smWaitAckComS();
  void smEndComS();

  // Slave: Master <-> Slave
  //
  void smStartComES();
  void smWaitComES();
  void smEvalComES();

  // Test
  //
  void smStartTest();
  void smWaitTest();
  void smLoopTest();


public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------
  BlePoll(IntrfRadio *refRadio, dword inCycleMics);
  BlePoll(IntrfRadio *refRadio, MicsecFuPtr inMicroFu);
  void init(IntrfRadio *refRadio, dword inCycleMics, MicsecFuPtr inMicroFu);

  void begin(ComType typeIn, int adrIn, AppType appType);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setPollAddress(int chnIn, int adrIn, int areaIn, bool masterIn, bool eadrIn, bool nakIn);

  void setEmptyPollParams(int cycleTotal, int cycleRun, dword timeOut);

  // --------------------------------------------------------------------------
  // Steuerung des Telegrammaustausches (Polling)
  // --------------------------------------------------------------------------
  //

  void run();

  // Test
  //

  // Leeres Polling
  //
  void stopEP();
  void resumeEP();
  bool stoppedEP();

  // Laufender Betrieb
  //
  void start(PlMode inPlMode);

  // --------------------------------------------------------------------------
  // Zugriff auf Polling-Informationen
  // --------------------------------------------------------------------------
  //
  int getSlaveList(byte *dest, int maxByte);


  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //
  dword debGetDword(int idx);

};


// ----------------------------------------------------------------------------
#endif // BlePoll_h
