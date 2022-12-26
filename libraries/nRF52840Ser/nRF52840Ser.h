//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Ser.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef NRF52840SER_H
#define NRF52840SER_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfBuf.h"
#include "IntrfSerial.h"

// ----------------------------------------------------------------------------

typedef struct _nrfSer
{
  volatile  dword  TASKS_STARTRX;           // 000
  volatile  dword  TASKS_STOPRX;            // 004
  volatile  dword  TASKS_STARTTX;           // 008
  volatile  dword  TASKS_STOPTX;            // 00C
  volatile  dword  Reserve01[3];            // 010
  volatile  dword  TASKS_SUSPEND;           // 01C
  volatile  dword  Reserve01a[3];           // 020
  volatile  dword  TASKS_FLUSHRX;           // 02C
  volatile  dword  Reserve02[52];           // 030
  volatile  dword  EVENTS_CTS;              // 100
  volatile  dword  EVENTS_NCTS;             // 104
  volatile  dword  EVENTS_RXDRDY;           // 108
  volatile  dword  Reserve03;               // 10C
  volatile  dword  EVENTS_ENDRX;            // 110
  volatile  dword  Reserve04[2];            // 114
  volatile  dword  EVENTS_TXDRDY;           // 11C
  volatile  dword  EVENTS_ENDTX;            // 120
  volatile  dword  EVENTS_ERROR;            // 124
  volatile  dword  Reserve05[7];            // 128
  volatile  dword  EVENTS_RXTO;             // 144
  volatile  dword  Reserve05a;              // 148
  volatile  dword  EVENTS_RXSTARTED;        // 14C
  volatile  dword  EVENTS_TXSTARTED;        // 150
  volatile  dword  Reserve05b;              // 154
  volatile  dword  EVENTS_TXSTOPPED;        // 158
  volatile  dword  Reserve06[41];           // 15C
  volatile  dword  SHORTS;                  // 200
  volatile  dword  Reserve07[63];           // 204
  volatile  dword  INTEN;                   // 300
  volatile  dword  INTENSET;                // 304
  volatile  dword  INTENCLR;                // 308
  volatile  dword  Reserve08[93];           // 30C
  volatile  dword  ERRORSRC;                // 480
  volatile  dword  Reserve09[31];           // 484
  volatile  dword  ENABLE;                  // 500
  volatile  dword  Reserve10;               // 504
  volatile  dword  PSEL_RTS;                // 508
  volatile  dword  PSEL_TXD;                // 50C
  volatile  dword  PSEL_CTS;                // 510
  volatile  dword  PSEL_RXD;                // 514
  volatile  dword  RXD;                     // 518
  volatile  dword  TXD;                     // 51C
  volatile  dword  Reserve11;               // 520
  volatile  dword  BAUDRATE;                // 524
  volatile  dword  Reserve12[3];            // 528
  volatile  dword  RXD_PTR;                 // 534
  volatile  dword  RXD_MAXCNT;              // 538
  volatile  dword  RXD_AMOUNT;              // 53C
  volatile  dword  Reserve13;               // 540
  volatile  dword  TXD_PTR;                 // 544
  volatile  dword  TXD_MAXCNT;              // 548
  volatile  dword  TXD_AMOUNT;              // 54C
  volatile  dword  Reserve14[7];            // 550
  volatile  dword  CONFIG;                  // 56C
} nrfSer, *nrfSerPtr;

#define NrfSerBase0   0x40002000
#define NrfSerPtr0    ((nrfSerPtr) NrfSerBase0)
#define NrfSerBase1   0x40028000
#define NrfSerPtr1    ((nrfSerPtr) NrfSerBase1)

#ifndef nrfGpioDef

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

#define GpioPinCnf_DIROUT     ((dword) 0x00000001)

#define GpioPinCnf_DISBUF     ((dword) 0x00000002)

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

#define nrfGpioDef
#endif

// Festlegungen für die Paketkonfigurationsregister
//

#define SerInt_RXDRDY     ((dword) 0x00000001 << 2)
// Interrupt für Event RXDRDY

#define SerInt_TXDRDY     ((dword) 0x00000001 << 7)
// Interrupt für Event TXDRDY

#define SerInt_ERROR      ((dword) 0x00000001 << 9)
// Interrupt für Event ERROR


#define SerEnable   4
#define SerDisable  0

// Bit-Masken fuer Kommunikationsbedingungen
//
#define BM_REC_NOT_COND 0x00
// Keine Bedingungen beim Empfang
#define BM_REC_END_CHR  0x01
// Empfang Stoppen beim Eintreffen des vorgegebenen Zeichens
#define BM_REC_RINGBUF  0x02
// Receive characters in ring buffer
#define BM_SND_RINGBUF  0x04
// Transmit characters via ring buffer



// ----------------------------------------------------------------------------

class nRF52840Ser : IntrfSerial
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten und Funktionen
  // --------------------------------------------------------------------------
  //
  nrfSerPtr     serPtr;
  dword         irqCounter;

  int           lastError;
  int           anyError;
  dword         cntError;

  int           curIRQ;
  dword         curIntEn;

  IntrfBuf      *bufIf;
  bool          txdFin;       // TRUE = Sendevorgang beendet

  void clrAllEvents();

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  nRF52840Ser();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void begin(SerParamsPtr serParPtr, IntrfBuf *bufferIf);


  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  void resuSend();    // Fortsetzen des Interrupt-Sendebetriebs
  void startSend();   // Starten des Sendebetriebs
  void stopSend();    // Anhalten des Sendebetriebs

  void startRec();    // Starten des Empfangsbetriebs
  void stopRec();     // Anhalten des Empfangsbetriebs


  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //
  bool condSend(byte c);  // Bedingtes Senden eines Zeichens

  int   getLastError();   // Letzten Fehler lesen (Bits)
  int   getAnyError();    // Alle vorgekommenen Fehlerbits
  dword getErrCount();    // Anzahl der Fehler lesen


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  static  nRF52840Ser *instPtr0;
  static  void irqHandler0();

  void    irqHandler();

  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //


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

#endif // NRF52840SER_H

