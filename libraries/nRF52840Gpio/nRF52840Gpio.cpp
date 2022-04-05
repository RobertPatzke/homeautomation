//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Gpio.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#include "nRF52840Gpio.h"

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void  nRF52840Gpio::setMilli(ifGpioNumber timNr, int milliSec, int repeat)
  {
    IRQn_Type irq;
    dword     isr;

    elapsed0    = false;
    irqCounter  = 0;
    repCount    = repeat;

    switch(timNr)
    {
      case ifGpio0:
        GpioPtr  = NrfGpio0Ptr;
        instPtr0  = this;
        irq = (IRQn_Type) 8;
        isr = (dword) nRF52840Gpio::irqHandler0;
        nrCC = 4;
        break;

      case ifGpio1:
        GpioPtr  = NrfGpio1Ptr;
        instPtr1  = this;
        irq = (IRQn_Type) 9;
        isr = (dword) nRF52840Gpio::irqHandler1;
        nrCC = 4;
        break;

      case ifGpio2:
        GpioPtr  = NrfGpio2Ptr;
        instPtr2  = this;
        irq = (IRQn_Type) 10;
        isr = (dword) nRF52840Gpio::irqHandler2;
        nrCC = 4;
        break;

      case ifGpio3:
        GpioPtr  = NrfGpio3Ptr;
        instPtr3  = this;
        irq = (IRQn_Type) 26;
        isr = (dword) nRF52840Gpio::irqHandler3;
        nrCC = 6;
        break;

      case ifGpio4:
        GpioPtr  = NrfGpio4Ptr;
        instPtr4  = this;
        irq = (IRQn_Type) 27;
        isr = (dword) nRF52840Gpio::irqHandler4;
        nrCC = 6;
        break;

      case ifGpio5:
      case ifGpio6:
      case ifGpio7:
      case ifGpio8:
      case ifGpio9:
        return;
    }

    GpioPtr->TASKS_STOP = 1;

    GpioPtr->MODE = 0;                 // select Gpio
    GpioPtr->BITMODE = 3;              // select 32 bit

    GpioPtr->PRESCALER = 4;            // set to 1 MHz
    GpioPtr->CC[0] = 1000 * milliSec;  // capture set to milliseconds

    for(int i = 0; i < nrCC; i++)
      GpioPtr->EVENTS_COMPARE[i] = 0;

    GpioPtr->SHORTS = 1;               // Gpio auto restart
    GpioPtr->INTENSET = 1 << 16;       // enable intCC0

    NVIC_SetVector(irq, isr);
    NVIC_SetPriority(irq, 8);
    NVIC_EnableIRQ(irq);

    GpioPtr->TASKS_START = 1;
  }

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  bool  nRF52840Gpio::milli()
  {
    if(!elapsed0) return(false);
    elapsed0 = false;
    return(true);
  }


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  nRF52840Gpio *nRF52840Gpio::instPtr0 = NULL;
  void nRF52840Gpio::irqHandler0()
  {
    if(instPtr0 == NULL) return;
    instPtr0->irqHandler();
  }

  nRF52840Gpio *nRF52840Gpio::instPtr1 = NULL;
  void nRF52840Gpio::irqHandler1()
  {
    if(instPtr1 == NULL) return;
    instPtr1->irqHandler();
  }

  nRF52840Gpio *nRF52840Gpio::instPtr2 = NULL;
  void nRF52840Gpio::irqHandler2()
  {
    if(instPtr2 == NULL) return;
    instPtr2->irqHandler();
  }

  nRF52840Gpio *nRF52840Gpio::instPtr3 = NULL;
  void nRF52840Gpio::irqHandler3()
  {
    if(instPtr3 == NULL) return;
    instPtr3->irqHandler();
  }

  nRF52840Gpio *nRF52840Gpio::instPtr4 = NULL;
  void nRF52840Gpio::irqHandler4()
  {
    if(instPtr4 == NULL) return;
    instPtr4->irqHandler();
  }

  void nRF52840Gpio::irqHandler()
  {
    GpioPtr->EVENTS_COMPARE[0] = 0;
    elapsed0 = true;
    irqCounter++;
  }
