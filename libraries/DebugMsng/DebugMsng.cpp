//-----------------------------------------------------------------------------
// Topic:   Debug Messenger
// File:    DebugMsng.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#include "DebugMsng.h"

// ---------------------------------------------------------------------------
// constructors and initialisations
// ---------------------------------------------------------------------------
//
DebugMsng::DebugMsng()
{
  delay = 0;
}

// ----------------------------------------------------------------------------
// Output
// ----------------------------------------------------------------------------
//

void DebugMsng::print(char *msg)
{
#ifdef smnArduino
  Serial.print(msg);
#endif
}

void DebugMsng::print(int intVal)
{
#ifdef smnArduino
  Serial.print(intVal);
#endif
}

void DebugMsng::print(unsigned int intVal)
{
#ifdef smnArduino
  Serial.print(intVal);
#endif
}

void DebugMsng::println()
{
#ifdef smnArduino
  Serial.println();
#endif
}

// ----------------------------------------------------------------------------
// User functions
// ----------------------------------------------------------------------------
//
void DebugMsng::cyclicMsg(unsigned int intVal, int inDelay)
{
  if(delay <= 0)
  {
    print(intVal);
    println();
    delay = inDelay;
  }
  else
    delay--;
}



