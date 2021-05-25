//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Scanner.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   8. Mai 2021
//
// Der Inhalt dieser Datei sind Festlegungen zum Empfangen und Vorauswerten
// von BLE-Telegrammen
//

#ifndef Scanner_h
#define Scanner_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "arduinoDefs.h"
#include "bleSpec.h"
#include "IntrfRadio.h"

typedef enum _RecMode
{
  StartRec,
  ContinueRec,
  EndRec
}RecMode;

// ----------------------------------------------------------------------------
//                       S c a n n e r
// ----------------------------------------------------------------------------
class Scanner
{
private:
  // ----------------------------------------------------------------------
  // Lokale Daten
  // ----------------------------------------------------------------------
  //
  IntrfRadio  *radio;
  bcPdu       pdu;


  // ----------------------------------------------------------------------
  // Lokale Funktionen
  // ----------------------------------------------------------------------
  //
  void  init01();


public:
  // ----------------------------------------------------------------------
  // Initialisierungen
  // ----------------------------------------------------------------------

  Scanner(IntrfRadio *inRadio);

  // ----------------------------------------------------------------------
  // Konfiguration
  // ----------------------------------------------------------------------
  void setDevAddress(int sel, BD_ADR bdAdr);
  void setDevAddress(int sel, word head, dword body);
  void setRadioInterface(IntrfRadio * inRadio);


  // ----------------------------------------------------------------------
  // Steuerung
  // ----------------------------------------------------------------------
  //
  void  start();
  void  receive(RecMode mode, int chn);
  int   check();
  int   getPdu(bcPdu *dest, int nr);
};


// ----------------------------------------------------------------------------
#endif // Scanner_h
