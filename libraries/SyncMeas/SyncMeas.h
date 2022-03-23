//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SyncMeas.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
// Datum:   13. März 2022
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//
#ifndef _SyncMeas_h
#define _SyncMeas_h
//-----------------------------------------------------------------------------

#include <stdlib.h>

// ---------------------------------------------------------------------------
// class SyncMeas
// ---------------------------------------------------------------------------
// Zyklische Ausgabe von Messwerten (Messages) mit Kompensation von fehlenden
// Eingaben (stete Ausgabe mit fester Frequenz)

class SyncMeas
{
public:
  // -------------------------------------------------------------------------
  // globale klassenspezifische Datentypen
  // -------------------------------------------------------------------------
  //

private:
  // -------------------------------------------------------------------------
  // lokale Datentypen
  // -------------------------------------------------------------------------
  //
  typedef enum
  {
    init,
    chkData
  } State;


private:
  // -------------------------------------------------------------------------
  // lokale Variablen
  // -------------------------------------------------------------------------
  //
  int   rCycle;       // Zykluszeit von run() in Mikrosekunden
  int   mCycle;       // Zykluszeit der Ausgabe (alle Kanäle) in Millisekunden
  int   *idxList;     // Liste der Indizes verschiedener Kanäle
  int   listLen;      // Anzahl der Kanäle
  int   listIdx;      // Index auf den Listeneintrag
  int   curIdx;       // Aktuell behandelter Kanal

  State state;        // Zustand (des Ablaufs)

public:
  // -------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // -------------------------------------------------------------------------
  //
  SyncMeas(int runCycle);

  // -------------------------------------------------------------------------
  // Anwenderfunktionen
  // -------------------------------------------------------------------------
  //
  void run();   // Zyklische CPU-Übergabe

  int setChannels(int *iList, int iCnt);  // Setzen der Kanalliste
  int setMeasCycle(int measCycle);        // Mess/Ausgabe-Zyklus in Millisekunden


};

//-----------------------------------------------------------------------------
#endif // _SyncMeas_h
