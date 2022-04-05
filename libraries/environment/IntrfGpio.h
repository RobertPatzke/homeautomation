//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfGpio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   04. April 2022
//
// Eine Schnittstelle zu den unterschiedlichen Ports in Mikrocontrollern
//

#ifndef IntrfGpio_h
#define IntrfGpio_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

typedef enum _ifPortNumber
{
  ifPort0,
  ifPort1,
  ifPort2,
  ifPort3,
  ifPort4,
  ifPort5,
  ifPort6,
  ifPort7,
  ifPort8,
  ifPort9
} ifPortNumber;

class IntrfGpio
{
public:

  //virtual ~IntrfGpio();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //

};

// ----------------------------------------------------------------------------
#endif //IntrfGpio_h
