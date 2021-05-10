//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfRadio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   9. Mai 2021
//
// Eine Schnittstelle zu den unterschiedlichen Transceivern in Mikrocontrollern
// oder Boards mit Mikrocontrollern und Radio-Transceivern
//

#ifndef IntrfRadio_h
#define IntrfRadio_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"
#include "bleSpec.h"
#include "Beacon.h"

class IntrfRadio
{
public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  virtual void  setAccessAddress(dword addr);
  virtual void  setPacketParms(blePduType type);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  virtual void  advChannel(int idx);          // Schalten Bewerbungskanal (advertizing)
  virtual int   sendSync(bcPduPtr inPduPtr);  // Senden eines Telegramms (und warten)
  virtual void  setPower(int DBm);            // Leistung des Senders in DBm


};

// ----------------------------------------------------------------------------
#endif  // IntrfRadio_h
