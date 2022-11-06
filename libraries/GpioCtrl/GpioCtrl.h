//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   GpioCtrl.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//
#ifndef GpioCtrl_h
#define GpioCtrl_h
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "IntrfGpio.h"

#define MaxGpioElements   4

class GpioCtrl
{
  // --------------------------------------------------------------------------
  // spezifische Datentypen
  // --------------------------------------------------------------------------
  //
  typedef struct _Blink
  {
    int           blinkLen;
    int           blinkLenSet;
    int           blinkPause;
    int           blinkPauseSet;
    bool          doBlink;
    bool          blinked;
    bool          portSet;
    GpioExtRefPtr portRef;
  } Blink, *BlinkPtr;

private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  IntrfGpio *gpioPtr;                 // Referenz auf Pin-Treiber
  Blink     blinkEl[MaxGpioElements]; // Liste der Blinkelemente
  int       period;                   // Aufrufperiode in Mikrosekunden

  // --------------------------------------------------------------------------
  // lokale Funktionen
  // --------------------------------------------------------------------------
  //

public:
  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  GpioCtrl(IntrfGpio *inGpioPtr, int inPeriod);

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
  void run();
  void  blink(GpioExtRefPtr portRef, int chn, int len, int pause);


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //

};

//-----------------------------------------------------------------------------
#endif // GpioCtrl_h
