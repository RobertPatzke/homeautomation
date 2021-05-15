//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Monitor.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   15. Mai 2021
//
// Der Monitor dient zum direkten Zugriff auf die Ressourcen eines
// Mikrocontrollers über die serielle Schnittstelle.
//

#include  "Arduino.h"
#include  "environment.h"

#ifndef Monitor_h
#define Monitor_h
// ----------------------------------------------------------------------------

#define keyHit()  smnSerial.available()
#define keyIn()   smnSerial.read()
#define out(x)    smnSerial.print(x)

#define modeEcho  0x01

class Monitor
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //
  typedef void (Monitor::*StatePtr)(void);



private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  int       cpu;
  int       mode;

  StatePtr  nextState;

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void  getKey();
  void  version();

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Monitor(int mode, int cpu);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //


  // --------------------------------------------------------------------------
  // Anwenderschnittstelle
  // --------------------------------------------------------------------------
  //

  void run();


};

// ----------------------------------------------------------------------------
#endif // Monitor_h
