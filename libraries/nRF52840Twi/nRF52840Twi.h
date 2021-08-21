//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Twi.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef NRF52840TWI_H
#define NRF52840TWI_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfTw.h"

// ----------------------------------------------------------------------------

typedef struct _nrfTwi
{
  volatile  dword  TASKS_STARTRX;           // 000
  volatile  dword  Reserve01;               // 004
  volatile  dword  TASKS_STARTTX;           // 008
  volatile  dword  Reserve02[2];            // 00C
  volatile  dword  TASKS_STOP;              // 014
  volatile  dword  Reserve03;               // 018
  volatile  dword  TASKS_SUSPEND;           // 01C
  volatile  dword  TASKS_RESUME;            // 020
  volatile  dword  Reserve04[56];           // 024
  volatile  dword  EVENTS_STOPPED;          // 104
  volatile  dword  EVENTS_RXDREADY;         // 108
  volatile  dword  Reserve05[4];            // 118
  volatile  dword  EVENTS_TXDSENT;          // 11C
  volatile  dword  Reserve06;               // 120
  volatile  dword  EVENTS_ERROR;            // 124
  volatile  dword  Reserve07[4];            // 128
  volatile  dword  EVENTS_BB;               // 138
  volatile  dword  Reserve08[3];            // 13C
  volatile  dword  EVENTS_SUSPENDED;        // 148
  volatile  dword  Reserve09[45];           // 14C
  volatile  dword  SHORTS;                  // 200
  volatile  dword  Reserve10[64];           // 204
  volatile  dword  INTENSET;                // 304
  volatile  dword  INTENCLR;                // 308
  volatile  dword  Reserve11[110];          // 30C
  volatile  dword  ERRORSRC;                // 4C4
  volatile  dword  Reserve12[14];           // 4C8
  volatile  dword  ENABLE;                  // 500
  volatile  dword  Reserve13;               // 504
  volatile  dword  PSEL_SCL;                // 508
  volatile  dword  PSEL_SDA;                // 50C
  volatile  dword  Reserve14[2];            // 510
  volatile  dword  RXD;                     // 518
  volatile  dword  TXD;                     // 51C
  volatile  dword  Reserve15;               // 520
  volatile  dword  FREQUENCY;               // 524
  volatile  dword  Reserve16[24];           // 528
  volatile  dword  ADDRESS;                 // 588
} nrfTwi, *nrfTwiPtr;

#define NrfTwiBase0   0x40003000
#define NrfTwiPtr0    ((nrfTwiPtr) NrfTwiBase0)
#define NrfTwiBase1   0x40004000
#define NrfTwiPtr1    ((nrfTwiPtr) NrfTwiBase1)

#define NrfTwi100k    0x01980000
#define NrfTwi250k    0x04000000
#define NrfTwi400k    0x06680000

typedef enum _TwiTrfMode
{
  ttmWriteByte = 1,
  ttmWriteByteReg,
  ttmReadByteReg,
  ttmReadByteRegSeq
} TwiTrfMode;

#ifndef nrfGpio

typedef struct _nrfGpio
{
  volatile  dword Reserve01;                // 000
  volatile  dword OUT;                      // 004
  volatile  dword OUTSET;                   // 008
  volatile  dword OUTCLR;                   // 00C
  volatile  dword IN;                       // 010
  volatile  dword DIR;                      // 014
  volatile  dword DIRSET;                   // 018
  volatile  dword DIRCLR;                   // 01C
  volatile  dword LATCH;                    // 020
  volatile  dword DETECTMODE;               // 024
  volatile  dword Reserve02[118];           // 026
  volatile  dword PIN_CNF[32];              // 200
} nrfGpio, *nrfGpioPtr;

#define NrfGpioBase   0x50000000
#define NrfGpioBase0  0x50000500
#define NrfGpioPtr0   ((nrfGpioPtr) NrfGpioBase0)
#define NrfGpioBase1  0x50000800
#define NrfGpioPtr1   ((nrfGpioPtr) NrfGpioBase1)

#define GpioPinCnf_DIR        ((dword) 0x00000001)

#define GpioPinCnf_INPUT      ((dword) 0x00000001 << 1)

#define GpioPinCnf_PULL(x)    ((dword) x << 2)
#define GpioPullDown          1
#define GpioPullUp            3

#define GpioPinCnf_DRIVE(x)   ((dword) x << 8)
#define GpioDriveS0S1         0
#define GpioDriveH0S1         1
#define GpioDriveS0H1         2
#define GpioDriveH0H1         3
#define GpioDriveD0S1         4
#define GpioDriveD0H1         5
#define GpioDriveS0D1         6
#define GpioDriveH0D1         7

#define GpioPinCnf_SENSE(x)   ((dword) x << 16)
#define GpioSenseHigh         2
#define GpioSenseLow          3

#endif

// Festlegungen für die Paketkonfigurationsregister
//

#define TwiInt_STOPPED    ((dword) 0x00000001 << 1)
// Interrupt für Event STOPPED

#define TwiInt_RXDREADY   ((dword) 0x00000001 << 2)
// Interrupt für Event RXDREADY

#define TwiInt_TXDSENT    ((dword) 0x00000001 << 7)
// Interrupt für Event TXDSENT

#define TwiInt_ERROR      ((dword) 0x00000001 << 9)
// Interrupt für Event ERROR

#define TwiInt_BB         ((dword) 0x00000001 << 14)
// Interrupt für Event BB

#define TwiInt_SUSPENDED  ((dword) 0x00000001 << 18)
// Interrupt für Event SUSPENDED

#define TwiEnable   5
#define TwiDisable  0




// ----------------------------------------------------------------------------

class nRF52840Twi : IntrfTw
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten und Funktionen
  // --------------------------------------------------------------------------
  //
  nrfTwiPtr     twiPtr;
  dword         irqCounter;

  TwiBytePtr    byteStruPtr;
  TwiWordPtr    wordStruPtr;
  TwiByteSeqPtr byteSeqPtr;

  TwiTrfMode    trfMode;
  dword         lastError;
  int           comIdx;

  int           curIRQ;
  dword         curIntEn;

  TwiByte       tmpByte;

  //void (nRF52840Twi::* dynHand)();

  void clrAllEvents();

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  nRF52840Twi();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  TwiError begin(TwiParamsPtr inParPtr);

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
  TwiError sendByte(int adr, TwiBytePtr refByte);
  TwiError sendByteReg(int addr, int reg, TwiBytePtr refByte);
  TwiError recByteReg(int addr, int reg, TwiBytePtr refByte);
  TwiError recByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq);

  // synchrone Kommunikation
  //
  TwiStatus writeByteReg(int adr, int reg, byte value);
  int       readByteReg(int adr, int reg);
  TwiStatus readByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq);

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  static  nRF52840Twi *instPtr0;
  static  void irqHandler0();

  static  nRF52840Twi *instPtr1;
  static  void irqHandler1();

  void    irqHandler();

  // ----------------------------------------------------------------------------
  //                      D e b u g - H i l f e n
  // ----------------------------------------------------------------------------
  //
  int           irqIdx;
  int           irqList[8];

  byte          extraValue;
  bool          firstRead;

  dword   getIrqCount();
  void    resetIrqList();
  void    getIrqList(char *dest);

};

#endif // NRF52840RADIO_H

