//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfTw.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   3. Juni 2021
//
// Eine Schnittstelle zu den unterschiedlichen I2C Schnittstellen
// oder Boards mit geeigneten Mikrocontrollern
//

#ifndef IntrfTw_h
#define IntrfTw_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

typedef enum _TwiDevType
{
  TwiMaster,
  TwiSlave
} TwiDevType;

typedef enum _TwiSpeed
{
  Twi100k,      // Stadard, sollte unterstützt werden
  Twi250k,
  Twi400k,      // Schnell, sollte unterstützt werden
  Twi1000k,
  Twi3400k,
  Twi5000k
} TwiSpeed;

typedef struct _TwiParams
{
  int         inst;           // Nummer (Index) der Twi-Instanz
  TwiDevType  type;           // Auswahl Master/Slave
  int         clkPort;        // Nummer (Index) des Port fuer Takt
  int         clkPin;         // Nummer (Index) des Pin fuer Takt
  int         dataPort;       // Nummer (Index) des Port fuer Daten
  int         dataPin;        // Nummer (Index) des Pin fuer Daten
  TwiSpeed    speed;
} TwiParams, *TwiParamsPtr;

typedef enum _TwiError
{
  TEnoError
} TwiError;

typedef enum _TwiStatus
{
  TwStUnborn,
  TwStRdReq,
  TwStWrReq,
  TwStSent,
  TwStRecvd,
  TwStFin     = 0x0080,
  TwStError   = 0x0100,
  TwStOverrun = 0x0101,
  TwStAdrNak  = 0x0102,
  TwStDataNak = 0x0104
} TwiStatus;

typedef struct _TwiByte
{
  TwiStatus   twiStatus;
  byte        value;
} TwiByte, *TwiBytePtr;

typedef struct _TwiWord
{
  TwiStatus   twiStatus;
  word        value;
} TwiWord, *TwiWordPtr;

typedef struct _TwiByteSeq
{
  TwiStatus   twiStatus;
  int         len;
  byte        *valueRef;
} TwiByteSeq, *TwiByteSeqPtr;


class IntrfTw
{
public:
  // --------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // --------------------------------------------------------------------------
  //
  virtual TwiError begin(TwiParamsPtr inParPtr);

  //virtual ~IntrfTw();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Datenaustausch
  // --------------------------------------------------------------------------
  //
  // asynchrone Kommunikation, Zustand in TwiByte.twiStatus
  //
  virtual TwiError sendByte(int adr, TwiBytePtr refByte);
  virtual TwiError sendByteReg(int adr, int reg, TwiBytePtr refByte);
  virtual TwiError recByteReg(int adr, int reg, TwiBytePtr refByte);
  virtual TwiError recByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq);

  // synchrone Kommunikation
  //
  virtual TwiStatus writeByteReg(int adr, int reg, byte value);
  virtual int       readByteReg(int adr, int reg);

};

// ----------------------------------------------------------------------------
#endif  // IntrfTw_h
