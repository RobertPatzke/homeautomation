//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfGpio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   04. April 2022
//
// Eine Schnittstelle zu den unterschiedlichen Ports in Mikrocontrollern
//

#ifndef IntrfGpio_h
#define IntrfGpio_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

typedef enum _ifPortNumber
{
  ifPort0,
  ifPort1,
  ifPort2,
  ifPort3,
  ifPort4,
  ifPort5,
  ifPort6,
  ifPort7,
  ifPort8,
  ifPort9
} ifPortNumber;

typedef enum
{
  GEnoError,
  GEcdictPar
} GpioError;

typedef struct _GpioMask
{
  dword       port;
  dword       pins;
} GpioMask, *GpioMaskPtr;

typedef struct _GpioExtMask
{
  dword         port;
  dword         pins;
  _GpioExtMask  *next;
} GpioExtMask, *GpioExtMaskPtr;

typedef struct _GpioRef
{
  dword     *ioPtr;
  dword     pins;
} GpioRef, *GpioRefPtr;

typedef struct _GpioExtRef
{
  dword       *ioPtr;
  dword       pins;
  _GpioExtRef *next;
} GpioExtRef, *GpioExtRefPtr;

typedef struct _GpioExtVal
{
  dword       value;
  _GpioExtVal *next;
} GpioExtVal, *GpioExtValPtr;

// Spezifikation der Schnittstellentreiber
//
#define IfDrvInput          0x0000
#define IfDrvOutput         0x0001
#define IfDrvStrongHigh     0x0002
#define IfDrvStrongLow      0x0004
#define IfDrvOpenDrain      0x0008
#define IfDrvOpenSource     0x0010
#define IfDrvPullUp         0x0020
#define IfDrvPullDown       0x0040
#define IfDrvPullStrong     0x0080

typedef enum
{
  ArdD2D5,
  ArdA0A3,
  ArdA4A5,
  ArdA0A5,
  ArdA4A7,
  ArdA0A7
} ArdMask;

class IntrfGpio
{
public:

  //virtual ~IntrfGpio();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  virtual GpioError config(int nr, unsigned int cnfBits, GpioExtRefPtr refPtr);
  virtual GpioError config(int nrFrom, int nrTo, unsigned int cnfBits, GpioExtRefPtr refPtr);
  virtual GpioError config(GpioExtMask mask, unsigned int cnfBits, GpioExtRefPtr refPtr);
  virtual GpioError configArd(ArdMask ardMask, unsigned int cnfBits);

  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
  virtual void      read(GpioExtRefPtr refPtr, GpioExtValPtr valPtr);
  virtual dword     readArd(ArdMask ardMask);

  virtual void      write(GpioExtRefPtr refPtr, GpioExtValPtr valPtr);
  virtual void      writeArd(ArdMask ardMask, dword value);
};

// ----------------------------------------------------------------------------
#endif //IntrfGpio_h
