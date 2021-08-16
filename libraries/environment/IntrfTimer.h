//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfTimer.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//
// Eine Schnittstelle zu den unterschiedlichen Timern in Mikrocontrollern
//

#ifndef IntrfTimer_h
#define IntrfTimer_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

typedef enum _ifTimerNumber
{
  ifTimer0,
  ifTimer1,
  ifTimer2,
  ifTimer3,
  ifTimer4,
  ifTimer5,
  ifTimer6,
  ifTimer7,
  ifTimer8,
  ifTimer9
} ifTimerNumber;

class IntrfTimer
{
public:

  //virtual ~IntrfTimer();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  virtual void  setMilli(ifTimerNumber timNr, int milliSec, int repeat);
  //virtual void  setMilli(ifTimerNumber timNr, int milliSec, int repeat, dword ISR);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  virtual bool  milli();        // Abfrage des Timer, <true> wenn abgelaufen

};

// ----------------------------------------------------------------------------
#endif //IntrfTimer_h
