//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "nRF52840Radio.h"
#include <string.h>

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

// Schalten des Bewerbungskanals
//
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

// Senden eines Telegramms
//
void nRF52840Radio::send(bcPduPtr inPduPtr)
{
  memcpy((void *)pduMem, (const void *)inPduPtr, (unsigned int) sizeof(bcPdu));
}
