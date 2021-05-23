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

typedef struct _TxState
{
  unsigned int  prgLoopPrep;
  unsigned int  evtLoopRampUp;
  unsigned int  evtLoopTrans;
  byte  *       txBufferPtr;
}TxState, *TxStatePtr;

// Zustand des Datenempfangs (Bit)
//
#define RECSTAT_ADDRESS   0x0001
#define RECSTAT_PAYLOAD   0x0002
#define RECSTAT_END       0x0004
#define RECSTAT_CRCOK     0x0010

class IntrfRadio
{
public:

  //virtual ~IntrfRadio();

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
  virtual int   sendSync(bcPduPtr inPduPtr, TxStatePtr refState);
                                              // Senden eines Telegramms (und warten)
  virtual int   startRec();                   // Datenempfang starten
  virtual int   contRec();                    // Datenempfang fortsetzen
  virtual int   endRec();                     // Datenempfang beenden
  virtual int   checkRec();                   // Zustand Datenempfang feststellen
  virtual int   getRecData(bcPduPtr data, int max);  // Empfangene Daten lesen

  virtual void  setPower(int DBm);            // Leistung des Senders in DBm


};

// ----------------------------------------------------------------------------
#endif  // IntrfRadio_h
