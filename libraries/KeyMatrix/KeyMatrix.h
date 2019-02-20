//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    KeyMatrix.h
// Editor:  Robert Patzke
// Created: 18. February 2019
// Changed: 20. February 2019
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _KeyMatrix_h
#define _KeyMatrix_h
//-----------------------------------------------------------------------------

#define ArduinoFunc
#define kmVersion "Version 1.0.0"
// Version 1.0 only supports key clicks (key down followed by key up)
// and fixed time gaps to ignore bouncing

#ifdef ArduinoFunc
  #include "Arduino.h"
#endif

#define DefaultFinUpTime    500
#define DefaultMaxUpTime    60000
#define DefaultMinUpTime    100
#define DefaultMinDownTime  100

#define MaxSavedKeyEvents   16



typedef struct
{
  char    value;
  bool    down;
  int     event;
  int     downCount;
  int     pastDownCount;
  int     upCount;
  int     pastUpCount;
  bool    edgeRise;
  bool    edgeFall;
} Key, *KeyPtr;

#define KeyEvtUp        0x0001
#define KeyEvtDown      0x0002
#define KeyEvtEdgeRise  0x0004
#define KeyEvtEdgeFall  0x0008

typedef struct
{
  int     event;
  KeyPtr  keyRef;
} KeyEvent, *KeyEventPtr;

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

  KeyEvent  eventList[MaxSavedKeyEvents];   // List of key events

  int   keWrIdx;          // write index of cyclic buffer
  int   keRdIdx;          // read index of cyclic buffer
  int   keyEvtMask;       // key event mask (bits)


  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //
  void setKeyEvent(KeyPtr keyPtr, int keyEvent);


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
  void    run();                      // has to be cyclic called
  KeyPtr  checkKeyPressed();          // check if a key is still pressed
  KeyPtr  waitKeyClick();             // return key if clicked, otherwise NULL

};



//-----------------------------------------------------------------------------
#endif // _KeyMatrix_h
