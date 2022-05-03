//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SamD21Gpio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   12. April 2022
//

#ifndef SAMD21GPIO_H
#define SAMD21GPIO_H

#include "Arduino.h"
#include "environment.h"
#include "arduinoDefs.h"
#include "IntrfGpio.h"

#ifndef sd21GpioDef
// ----------------------------------------------------------------------------
typedef struct _nrfGpio
{
  volatile  dword DIR;                      // 000
  volatile  dword DIRCLR;                   // 004
  volatile  dword DIRSET;                   // 008
  volatile  dword DIRTGL;                   // 00C
  volatile  dword OUT;                      // 010
  volatile  dword OUTCLR;                   // 014
  volatile  dword OUTSET;                   // 018
  volatile  dword OUTTGL;                   // 01C
  volatile  dword IN;                       // 020
  volatile  dword CTRL;                     // 024
  volatile  dword WRCONFIG;                 // 028
  volatile  dword Reserve0;                 // 02C
  volatile  byte  PMUX[16];                 // 030
  volatile  byte  PINCFG[32];               // 040
} sd21Gpio, *sd21GpioPtr;

#define SD21GpioBase  0x41004400
#define SD21GpioBase0 0x41004400
#define SD21GpioPtr0  ((sd21GpioPtr) SD21GpioBase0)
#define SD21GpioBase1 0x41004480
#define SD21GpioPtr1  ((sd21GpioPtr) SD21GpioBase1)

#define GpioPinCnf_DRVSTR     ((byte) 0x40)
#define GpioPinCnf_INEN       ((byte) 0x02)
#define GpioPinCnf_PULLEN     ((byte) 0x04)


#define sd21GpioDef
// ----------------------------------------------------------------------------
#endif

#define P0(x) (x)
#define P1(x) (32+x)

#ifdef smnSD21MINI
// ----------------------------------------------------------------------------
#define ArdA0Bit    2
#define ArdA1Bit    8
#define ArdA2Bit    9
#define ArdA3Bit    4
#define ArdA4Bit    5
#define ArdA5Bit    2

#define ArdA0Port   0
#define ArdA1Port   1
#define ArdA2Port   1
#define ArdA3Port   0
#define ArdA4Port   0
#define ArdA5Port   1

#define ArdA0   P0(2)
#define ArdA1   P1(8)
#define ArdA2   P1(9)
#define ArdA3   P0(4)
#define ArdA4   P0(5)
#define ArdA5   P1(2)

#define ArdA0Mask   (1 << 2)
#define ArdA1Mask   (1 << 8)
#define ArdA2Mask   (1 << 9)
#define ArdA3Mask   (1 << 4)
#define ArdA4Mask   (1 << 5)
#define ArdA5Mask   (1 << 2)

#define ArdD2       P0(14)
#define ArdD3       P0(9)
#define ArdD4       P0(8)
#define ArdD5       P0(15)
#define ArdD6       P0(20)
#define ArdD7       P0(21)
#define ArdD8       P0(6)
#define ArdD9       P0(7)
#define ArdD10      P0(18)
#define ArdD11      P0(16)
#define ArdD12      P0(19)
#define ArdD13      P0(17)

#define ArdD2Mask   (1 << 8)
#define ArdD3Mask   (1 << 9)
#define ArdD4Mask   (1 << 14)
#define ArdD5Mask   (1 << 15)
#define ArdD6Mask   (1 << 20)
#define ArdD7Mask   (1 << 21)
#define ArdD8Mask   (1 << 6)
#define ArdD9Mask   (1 << 7)
#define ArdD10Mask  (1 << 18)
#define ArdD11Mask  (1 << 16)
#define ArdD12Mask  (1 << 19)
#define ArdD13Mask  (1 << 17)

// ----------------------------------------------------------------------------
#endif

class SamD21Gpio : IntrfGpio
{
private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  sd21GpioPtr  gpioPtr;

public:
  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  SamD21Gpio();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  byte      getCnfValue(unsigned int cnfBits);
  void      setCnf(unsigned int cnfBits, GpioExtRefPtr refPtr);
  GpioError config(int nr, unsigned int cnfBits, GpioExtRefPtr refPtr);
  GpioError config(int nrFrom, int nrTo, unsigned int cnfBits, GpioExtRefPtr refPtr);
  GpioError config(GpioExtMask mask, unsigned int cnfBits, GpioExtRefPtr refPtr);

  GpioError configArd(ArdMask ardMask, unsigned int cnfBits);

  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
  void      read(GpioExtRefPtr refPtr, GpioExtValPtr valPtr);
  dword     readArd(ArdMask ardMask);

  void      write(GpioExtRefPtr refPtr, GpioExtValPtr valPtr);
  void      writeArd(ArdMask ardMask, dword value);
  void      set(GpioExtRefPtr refPtr);
  void      clr(GpioExtRefPtr refPtr);


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //

};

#endif //SAMD21GPIO_H
