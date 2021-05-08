//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "nRF52840Radio.h"

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

nRF52840Radio::nRF52840Radio()
{
  ;
}


// ----------------------------------------------------------------------------
// Steuerfunktionen und Prozessorzugriffe
// ----------------------------------------------------------------------------

void nRF52840Radio::advChannel(int idx)
{
  int deltaF = 0;

  switch(idx)
  {
    case 0:
      deltaF = FrqOffsAdvChn1;
      break;

    case 1:
      deltaF = FrqOffsADvChn2;
      break;

    case 2:
      deltaF = FrqOffsAdvChn3;
      break;
  }
  NrfRadioPtr->FREQUENCY = deltaF;
}
