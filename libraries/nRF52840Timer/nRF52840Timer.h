//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Timer.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#ifndef NRF52840TIMER_H
#define NRF52840TIMER_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfTimer.h"

typedef struct _NRF_TIMER_Type
{
  volatile  dword  TASKS_START;
  volatile  dword  TASKS_STOP;
  volatile  dword  TASKS_COUNT;
  volatile  dword  TASKS_CLEAR;
  volatile  dword  ShutDownDepr;
  volatile  dword  Reserved0[11];
  volatile  dword  TASKS_CAPTURE[6];
  volatile  dword  Reserved1[58];
  volatile  dword  EVENTS_COMPARE[6];
  volatile  dword  Reserved2[42];
  volatile  dword  SHORTS;
  volatile  dword  Reserved3[64];
  volatile  dword  INTENSET;
  volatile  dword  INTENCLR;
  volatile  dword  Reserved4[126];
  volatile  dword  MODE;
  volatile  dword  BITMODE;
  volatile  dword  PRESCALER;
  volatile  dword  Reserved5[12];
  volatile  dword  CC[6];
} *nrfTimerPtr;

#define NrfTimer0Base   0x40008000
#define NrfTimer1Base   0x40009000
#define NrfTimer2Base   0x4000A000
#define NrfTimer3Base   0x4001A000
#define NrfTimer4Base   0x4001B000
#define NrfTimer0Ptr    ((nrfTimerPtr) NrfTimer0Base)
#define NrfTimer1Ptr    ((nrfTimerPtr) NrfTimer1Base)
#define NrfTimer2Ptr    ((nrfTimerPtr) NrfTimer2Base)
#define NrfTimer3Ptr    ((nrfTimerPtr) NrfTimer3Base)
#define NrfTimer4Ptr    ((nrfTimerPtr) NrfTimer4Base)

class nRF52840Timer : IntrfTimer
{
private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  bool        elapsed0;
  nrfTimerPtr timerPtr;
  int         nrCC;
  int         repCount;

public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void  setMilli(ifTimerNumber timNr, int milliSec, int repeat);
  //void  setMilli(ifTimerNumber timNr, int milliSec, int repeat, dword ISR);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  bool  milli();        // Abfrage des Timer, <true> wenn abgelaufen

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  static  nRF52840Timer *instPtr0;
  static  void irqHandler0();

  static  nRF52840Timer *instPtr1;
  static  void irqHandler1();

  static  nRF52840Timer *instPtr2;
  static  void irqHandler2();

  static  nRF52840Timer *instPtr3;
  static  void irqHandler3();

  static  nRF52840Timer *instPtr4;
  static  void irqHandler4();

  void    irqHandler();

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //
  int   irqCounter;

};

#endif //NRF52840TIMER_H
