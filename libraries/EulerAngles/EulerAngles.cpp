//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Calculations with Euler Angles
// Datei:   EulerAngles.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "EulerAngles.h"


// ---------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ---------------------------------------------------------------------------
//
EulerAngles::EulerAngles()
{
  minTanDenom = 0.0001;
}

// ---------------------------------------------------------------------------
// Anwenderfunktionen
// ---------------------------------------------------------------------------
//
float EulerAngles::getRollFromGravity(float accY, float accZ)
{
  if(accZ < 0)
  {
    if(accZ > - minTanDenom)
      accZ = - minTanDenom;
  }
  else
  {
    if(accZ < minTanDenom)
      accZ = minTanDenom;
  }
  return(atan2(accY,accZ));
}

float EulerAngles::getPitchFromGravity(float accX, float accY, float accZ)
{
  float denom;

  denom = sqrt(accY * accY + accZ * accZ);
  if(denom < minTanDenom)
    denom = minTanDenom;

  if(accZ < 0)
    denom = - denom;

  return(atan2(- accX, denom));
}
