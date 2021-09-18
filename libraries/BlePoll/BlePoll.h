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
  plmScan,        // Daten aller aktiven Teilnehmer holen
  plmSend         // Daten übertragen (Slave)
} PlMode;

// Grundsätzliche Datenstruktur für die Nutzdaten
//
typedef struct _PlPduBase
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur
  byte  plData[29];   // weitere spezifische Nutzdaten
} PlPduBase, *PlPduBasePtr;

// Erweiterte Datenstruktur für die Nutzdaten
//
typedef struct _PlPduExtd
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur
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
  byte    appId;    // Kennzeichnung für Dateninhalte
  byte    align;    // Wird nicht gesendet, kennzeichnet Alignement
} PlpFullMeas, *PlpFullMeasPtr;

typedef struct _PlpMeas3
{
  byte    appId;    // Kennzeichnung für Dateninhalte
  byte    reserve;  // Für spätere Spezifikation (z.Zt. Alignement)
  word    meas[3];  // Liste von 3 Messwerten
} PlpMeas3, *PlpMeas3Ptr;

typedef struct _PlpMeas6
{
  byte    appId;    // Kennzeichnung für Dateninhalte
  byte    reserve;  // Für spätere Spezifikation (z.Zt. Alignement)
  word    meas[6];  // Liste von 9 Messwerten
} PlpMeas6, *PlpMeas6Ptr;

typedef struct _PlpMeas9
{
  byte    appId;    // Kennzeichnung für Dateninhalte
  byte    reserve;  // Für spätere Spezifikation (z.Zt. Alignement)
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
} Slave, *SlavePtr;


typedef struct _PollInfo
{
  dword   aliens;     // Anzahl der netzfremden Aktivitäten
  dword   wrongs;     // Anzahl ungewünschter Netzaktivitäten
} PollInfo, *PollInfoPtr;

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
    ctSLAVE
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
  IntrfRadio  *radio;
  bcPdu       pduOut;
  bcPdu       pduIn;
  cbVector    nextState;
  MicsecFuPtr micSec;
  dword       toSet;
  dword       toValue;
  dword       cycleMics;
  dword       cycleCnt;

  int         chn;
  int         adr;
  int         area;
  bool        master;
  bool        eadr;
  bool        nak;

  PlMode      plMode;
  PlpType     plpType;

  Slave       slaveList[MAXSLAVE+1];
  Slave       *curSlave;
  int         pollIdx;
  int         maxAdr;
  dword       cntPolling;
  bool        epStop;
  bool        epStopped;

  dword       cntAlien;
  dword       cntWrong;
  dword       cntWaitDisabled;

  dword       bleState;
  dword       runCounter;

  TxStatistics  statistic;



  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void setPduAddress();
  void setTimeOut(dword value);
  bool timeOut();

  // Zustandsmaschine
  // -----------------------------
  void smInit();

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



  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //
  dword debGetDword(int idx);

};


// ----------------------------------------------------------------------------
#endif // BlePoll_h
