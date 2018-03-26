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
  char top;

  delay = 0;
  stepCount = -1;
  enable = false;
  freeMem = &top - reinterpret_cast<char *>(sbrk(0));
}

// ----------------------------------------------------------------------------
// Output
// ----------------------------------------------------------------------------
//

void DebugMsng::print(char *msg)
{
  if(!enable) return;

#ifdef smnArduino
  Serial.print(msg);
#endif
}

void DebugMsng::print(int intVal)
{
  if(!enable) return;

#ifdef smnArduino
  Serial.print(intVal);
#endif
}

void DebugMsng::print(unsigned int intVal)
{
  if(!enable) return;

#ifdef smnArduino
  Serial.print(intVal);
#endif
}

void DebugMsng::println()
{
  if(!enable) return;

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
  if(!enable) return;

  if(delay <= 0)
  {
    print(intVal);
    println();
    delay = inDelay;
  }
  else
    delay--;
}

void DebugMsng::msg(char * txt)
{
  if(stepCount == 0) return;
  if(stepCount > 0) stepCount--;
  print(txt);
}

void DebugMsng::val(int intVal)
{
  print(intVal);
}

void DebugMsng::nl()
{
  println();
}


