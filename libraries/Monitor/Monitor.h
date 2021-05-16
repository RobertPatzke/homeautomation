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
#define outl(x)   smnSerial.println(x)
#define GoInp     nextState = &Monitor::getKey;
#define GoPrm     nextState = &Monitor::prompt;

#define modeEcho  0x01
#define BufSize   128

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

  char      buffer[BufSize];
  int       wrIdx;
  int       rdIdx;
  bool      blkOut;

  StatePtr  nextState;

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void  prompt();
  void  getKey();
  void  version();

  void  print(char *txt, bool nl);
  void  print(unsigned int iVal, bool nl);

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Monitor(int mode, int cpu);

  // --------------------------------------------------------------------------
  // Konfiguration und Hilfsfunktionen
  // --------------------------------------------------------------------------
  //
  int   putBuf(char c);
  int   putBuf(char *txt);
  char  getBuf();


  // --------------------------------------------------------------------------
  // Anwenderschnittstelle
  // --------------------------------------------------------------------------
  //

  void run();
  void print(char *txt);
  void println(char *txt);
  void print(unsigned int iVal);
  void println(unsigned int iVal);


};

// ----------------------------------------------------------------------------
#endif // Monitor_h
