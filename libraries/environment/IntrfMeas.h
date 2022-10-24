//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfMeas.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   23. September 2022
//
// Eine Schnittstelle zur Messwererfassung
//

#ifndef IntrfMeas_h
#define IntrfMeas_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

#define signX         0x01
#define deltaSignX    0x02
#define signY         0x04
#define deltaSignY    0x08
#define signZ         0x10
#define deltaSignZ    0x20
#define signAll       0x40
#define deltaSignAll  0x80
#define chkSignAll    0x15

typedef struct _TriFloat
{
  float   x;
  float   y;
  float   z;
} TriFloat, *TriFloatPtr;

typedef struct _TriSenseSigns
{
  byte    signsAcc;
  byte    signsGyro;
  byte    signsMag;
} TriSenseSigns, *TriSenseSignsPtr;

class IntrfMeas
{
public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //


  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //
  virtual void sync(int type, int code);
  virtual bool available(int type, int code);
  virtual int  getValues(int type, int code, void *dataPtr);
  virtual byte getSigns(int type, int code);

};

// ----------------------------------------------------------------------------
#endif  // IntrfMeas_h
