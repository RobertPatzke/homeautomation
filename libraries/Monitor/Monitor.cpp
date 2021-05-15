//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Monitor.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   15. Mai 2021
//
// Der Monitor dient zum direkten Zugriff auf die Ressourcen eines
// Mikrocontrollers über die serielle Schnittstelle.
//

#include "Monitor.h"

//-----------------------------------------------------------------------------
// Initialisierungen
//-----------------------------------------------------------------------------

Monitor::Monitor(int inMode, int inCpu)
{
  mode  = inMode;
  cpu   = inCpu;

  nextState =
      &Monitor::getKey;
}

//-----------------------------------------------------------------------------
// Konfiguration
//-----------------------------------------------------------------------------
//

//-----------------------------------------------------------------------------
// Lokale Abläufe
//-----------------------------------------------------------------------------
//

void Monitor::getKey()
{
  char  c;

  if(!keyHit()) return;
  c = keyIn();
  if(mode & modeEcho)
    out(c);
}



//-----------------------------------------------------------------------------
// Anwenderschnittstelle
//-----------------------------------------------------------------------------
//

void Monitor::run()
{
  (this->*nextState)();
}

void Monitor::version()
{

}


