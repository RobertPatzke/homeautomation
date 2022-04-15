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

#ifndef nrfGpioDef
// ----------------------------------------------------------------------------
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

#define nrfGpioDef
// ----------------------------------------------------------------------------
#endif

#define P0(x) (x)
#define P1(x) (32+x)

#ifdef smnNANOBLE33
// ----------------------------------------------------------------------------
#define ArdA0Bit    4
#define ArdA1Bit    5
#define ArdA2Bit    30
#define ArdA3Bit    29
#define ArdA4Bit    31
#define ArdA5Bit    2
#define ArdA6Bit    28
#define ArdA7Bit    3

#define ArdA0   P0(4)
#define ArdA1   P0(5)
#define ArdA2   P0(30)
#define ArdA3   P0(29)
#define ArdA4   P0(31)
#define ArdA5   P0(2)
#define ArdA6   P0(28)
#define ArdA7   P0(3)

#define ArdA0Mask   (1 << 4)
#define ArdA1Mask   (1 << 5)
#define ArdA2Mask   (1 << 30)
#define ArdA3Mask   (1 << 29)
#define ArdA4Mask   (1 << 31)
#define ArdA5Mask   (1 << 2)
#define ArdA6Mask   (1 << 28)
#define ArdA7Mask   (1 << 3)


#define ArdD2       P1(11)
#define ArdD3       P1(12)
#define ArdD4       P1(15)
#define ArdD5       P1(13)
#define ArdD6       P0(14)
#define ArdD7       P0(23)
#define ArdD8       P1(21)
#define ArdD9       P0(27)
#define ArdD10      P1(2)
#define ArdD11      P1(1)
#define ArdD12      P1(8)
#define ArdD13      P0(13)

#define ArdD2Mask   (1 << 11)
#define ArdD3Mask   (1 << 12)
#define ArdD4Mask   (1 << 15)
#define ArdD5Mask   (1 << 13)
#define ArdD6Mask   (1 << 14)
#define ArdD7Mask   (1 << 23)
#define ArdD8Mask   (1 << 21)
#define ArdD9Mask   (1 << 27)
#define ArdD10Mask  (1 << 2)
#define ArdD11Mask  (1 << 1)
#define ArdD12Mask  (1 << 8)
#define ArdD13Mask  (1 << 13)

typedef enum
{
  ArdA0A3,
  ArdA4A7,
  ArdA0A7,
  ArdD2D5
} ArdMask;

// ----------------------------------------------------------------------------
#endif

class nRF52840Gpio : IntrfGpio
{
private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  nrfGpioPtr  gpioPtr;

public:
  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  nRF52840Gpio();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  dword     getCnfValue(unsigned int cnfBits);
  GpioError config(int nr, unsigned int cnfBits, GpioRefPtr refPtr);
  GpioError config(int nrFrom, int nrTo, unsigned int cnfBits, GpioRefPtr refPtr);
  GpioError config(GpioMask mask, unsigned int cnfBits, GpioRefPtr refPtr);

  GpioError configArd(ArdMask ardMask, unsigned int cnfBits, GpioRefPtr refPtr);

  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
  dword     read(GpioRef ioRef);
  dword     readArd(ArdMask ardMask, GpioRef ioRef);

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //

};

#endif //NRF52840GPIO_H
