//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Timer.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#include "nRF52840Timer.h"

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void  nRF52840Timer::setMilli(ifTimerNumber timNr, int milliSec, int repeat)
  {
    IRQn_Type irq;
    dword     isr;

    elapsed0    = false;
    irqCounter  = 0;
    repCount    = repeat;

    switch(timNr)
    {
      case ifTimer0:
        timerPtr  = NrfTimer0Ptr;
        instPtr0  = this;
        irq = (IRQn_Type) 8;
        isr = (dword) nRF52840Timer::irqHandler0;
        nrCC = 4;
        break;

      case ifTimer1:
        timerPtr  = NrfTimer1Ptr;
        instPtr1  = this;
        irq = (IRQn_Type) 9;
        isr = (dword) nRF52840Timer::irqHandler1;
        nrCC = 4;
        break;

      case ifTimer2:
        timerPtr  = NrfTimer2Ptr;
        instPtr2  = this;
        irq = (IRQn_Type) 10;
        isr = (dword) nRF52840Timer::irqHandler2;
        nrCC = 4;
        break;

      case ifTimer3:
        timerPtr  = NrfTimer3Ptr;
        instPtr3  = this;
        irq = (IRQn_Type) 26;
        isr = (dword) nRF52840Timer::irqHandler3;
        nrCC = 6;
        break;

      case ifTimer4:
        timerPtr  = NrfTimer4Ptr;
        instPtr4  = this;
        irq = (IRQn_Type) 27;
        isr = (dword) nRF52840Timer::irqHandler4;
        nrCC = 6;
        break;

      case ifTimer5:
      case ifTimer6:
      case ifTimer7:
      case ifTimer8:
      case ifTimer9:
        return;
    }

    timerPtr->TASKS_STOP = 1;

    timerPtr->MODE = 0;                 // select timer
    timerPtr->BITMODE = 3;              // select 32 bit

    timerPtr->PRESCALER = 4;            // set to 1 MHz
    timerPtr->CC[0] = 1000 * milliSec;  // capture set to milliseconds

    for(int i = 0; i < nrCC; i++)
      timerPtr->EVENTS_COMPARE[i] = 0;

    timerPtr->SHORTS = 1;               // timer auto restart
    timerPtr->INTENSET = 1 << 16;       // enable intCC0

    NVIC_SetVector(irq, isr);
    NVIC_SetPriority(irq, 8);
    NVIC_EnableIRQ(irq);

    timerPtr->TASKS_START = 1;
  }

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  bool  nRF52840Timer::milli()
  {
    if(!elapsed0) return(false);
    elapsed0 = false;
    return(true);
  }


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  nRF52840Timer *nRF52840Timer::instPtr0 = NULL;
  void nRF52840Timer::irqHandler0()
  {
    if(instPtr0 == NULL) return;
    instPtr0->irqHandler();
  }

  nRF52840Timer *nRF52840Timer::instPtr1 = NULL;
  void nRF52840Timer::irqHandler1()
  {
    if(instPtr1 == NULL) return;
    instPtr1->irqHandler();
  }

  nRF52840Timer *nRF52840Timer::instPtr2 = NULL;
  void nRF52840Timer::irqHandler2()
  {
    if(instPtr2 == NULL) return;
    instPtr2->irqHandler();
  }

  nRF52840Timer *nRF52840Timer::instPtr3 = NULL;
  void nRF52840Timer::irqHandler3()
  {
    if(instPtr3 == NULL) return;
    instPtr3->irqHandler();
  }

  nRF52840Timer *nRF52840Timer::instPtr4 = NULL;
  void nRF52840Timer::irqHandler4()
  {
    if(instPtr4 == NULL) return;
    instPtr4->irqHandler();
  }

  void nRF52840Timer::irqHandler()
  {
    timerPtr->EVENTS_COMPARE[0] = 0;
    elapsed0 = true;
    irqCounter++;
  }
