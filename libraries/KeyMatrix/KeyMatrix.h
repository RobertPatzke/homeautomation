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

#define DefaultFinUpTime    500
#define DefaultMaxUpTime    60000
#define DefaultMinUpTime    100
#define DefaultMinDownTime  100

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
  int     downCount;
  int     pastDownCount;
  int     upCount;
  int     pastUpCount;
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
  int   *rowList;         // Array of DIO-Numbers (Arduino)
  int   *colList;         // Array of DIO-Numbers (Arduino)
  int   rowCount;         // Number of rows
  int   colCount;         // Number of columns
  Key   *keyList;         // Array of key descriptors

  int   rowIdx;           // current row under test

  int   maxUpCount;       // maximum value for key up (limit)
  int   minUpCount;       // minimum value to validate up
  int   finUpCount;       // value for key action finish
  int   minDownCount;     // minimum value to validate down

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
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  KeyMatrix(int rowArray[], int nrRows,
            int colArray[], int nrCols,
            Key matrix[], int cycleTime);

  void initPins();
  void initMeasures(int cycleTime);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void  run();                      // has to be cyclic called

};



//-----------------------------------------------------------------------------
#endif // _KeyMatrix_h
