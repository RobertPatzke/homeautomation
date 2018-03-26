//-----------------------------------------------------------------------------
// Topic:   Debug Messenger
// File:    DebugMsng.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _DebugMsng_h
#define _DebugMsng_h
// ---------------------------------------------------------------------------

#ifndef smnNoArduinoLibs
  #define smnArduino
#endif

#ifdef smnArduino
  #include "Arduino.h"
#endif

extern "C" char* sbrk(int incr);

#define DBMSG(x)  if(dbgRef != NULL) dbgRef->msg(x);

// ---------------------------------------------------------------------------
// class DebugMsng
// ---------------------------------------------------------------------------
//

class DebugMsng
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
  int   delay;

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //
  void print(char * msg);
  void print(int intVal);
  void print(unsigned int intVal);
  void println();

public:
  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //
  bool  enable;
  int   stepCount;
  int   freeMem;

  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  DebugMsng();

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void cyclicMsg(unsigned int intVal, int delay);
  void msg(char * txt);
  void val(int intVal);
  void nl();

};









// ---------------------------------------------------------------------------
#endif
