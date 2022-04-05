//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Gpio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#ifndef NRF52840GPIO_H
#define NRF52840GPIO_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfGpio.h"

typedef struct _NRF_GPIO_Type
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
} *nrfGpioPtr;

#define NrfGpio0Base   0x40008000
#define NrfGpio1Base   0x40009000
#define NrfGpio2Base   0x4000A000
#define NrfGpio3Base   0x4001A000
#define NrfGpio4Base   0x4001B000
#define NrfGpio0Ptr    ((nrfGpioPtr) NrfGpio0Base)
#define NrfGpio1Ptr    ((nrfGpioPtr) NrfGpio1Base)
#define NrfGpio2Ptr    ((nrfGpioPtr) NrfGpio2Base)
#define NrfGpio3Ptr    ((nrfGpioPtr) NrfGpio3Base)
#define NrfGpio4Ptr    ((nrfGpioPtr) NrfGpio4Base)

class nRF52840Gpio : IntrfGpio
{
private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  bool        elapsed0;
  nrfGpioPtr timerPtr;
  int         nrCC;
  int         repCount;

public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void  setMilli(ifGpioNumber timNr, int milliSec, int repeat);
  //void  setMilli(ifGpioNumber timNr, int milliSec, int repeat, dword ISR);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  bool  milli();        // Abfrage des Gpio, <true> wenn abgelaufen

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  static  nRF52840Gpio *instPtr0;
  static  void irqHandler0();

  static  nRF52840Gpio *instPtr1;
  static  void irqHandler1();

  static  nRF52840Gpio *instPtr2;
  static  void irqHandler2();

  static  nRF52840Gpio *instPtr3;
  static  void irqHandler3();

  static  nRF52840Gpio *instPtr4;
  static  void irqHandler4();

  void    irqHandler();

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //
  int   irqCounter;

};

#endif //NRF52840GPIO_H
