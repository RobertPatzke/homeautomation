//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    KeyMatrix.h
// Editor:  Robert Patzke
// Created: 18. February 2019
// Changed: 18. February 2019
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _KeyMatrix_h
#define _KeyMatrix_h
//-----------------------------------------------------------------------------

#ifndef smnNoArduinoLibs
  #define smnArduino
#endif

#ifdef smnArduino
  #include "Arduino.h"
#endif

// Test for type definitions of SAM3X
//
#ifndef PIO_PER_P0
  #define Pio void
#endif

#include "environment.h"

#define ArduinoFunc


typedef struct
{
  char    value;
  bool    down;
  int     timeDown;
  int     pastTimeDown;
  int     timeUp;
  int     pastTimeUp;
  bool    edgeRise;
  bool    edgeFall;
} Key, *KeyPtr;

// ---------------------------------------------------------------------------
// class KeyMatrix
// ---------------------------------------------------------------------------
//

class KeyMatrix
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //

private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  int   *rowList;
  int   *colList;
  int   rowCount;
  int   colCount;
  Key   *keyList;

  int   rowIdx;

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //


public:
  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //

  // -------------------------------------------------------------------------
  // constructors and initializations
  // -------------------------------------------------------------------------
  //
  KeyMatrix(int rowArray[], int nrRows, int colArray[], int nrCols, Key matrix[]);

  void initPins();

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void  run();                      // has to be cyclic called

};



//-----------------------------------------------------------------------------
#endif // _KeyMatrix_h
