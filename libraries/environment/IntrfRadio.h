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

typedef struct  _Channel
{
  int   idx;
  int   freq;
}Channel, *ChannelPtr;

// Zustand des Datenempfangs (Bit)
//
#define RECSTAT_ADDRESS   0x0001
#define RECSTAT_PAYLOAD   0x0002
#define RECSTAT_END       0x0004
#define RECSTAT_CRCOK     0x0010

class IntrfRadio
{
public:
  // Kanalfrequenzen (Offsets zur Basisfrequenz) und Whitening-Preset
  //
  Channel channelList[40] =
  {
      {37, 2} , {38,26} , {39,80} , { 1, 6} , { 3,10} , { 5,14} , { 7,18} , { 9,22} ,
      {12,30} , {14,34} , {16,38} , {18,42} , {20,46} , {22,50} , {24,54} , {26,58} ,
      {28,62} , {30,66} , {32,70} , {34,74} , {35,76} , { 2, 8} , { 4,12} , { 6,16} ,
      { 8,20} , {33,72} , {31,68} , {13,32} , {15,36} , {17,40} , {19,44} , {21,48} ,
      {23,52} , {25,56} , {27,60} , {29,64} , {11,28} , {10,24} , { 0, 4} , {36,78}
  };

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
  virtual void  setChannel(int chnr);         // Schalten physikalischer Kanal
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
