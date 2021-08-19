//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SensorLSM9DS1.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "SensorLSM9DS1.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

SensorLSM9DS1::SensorLSM9DS1(IntrfTw *refI2C)
{
  twPtr = refI2C;
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
int SensorLSM9DS1::resetAG()
{
  twPtr->writeByteReg(AG_Adr, AG_Ctrl8, 0x05);

  // Get ID
  return(twPtr->readByteReg(AG_Adr, AG_Id));
}

int SensorLSM9DS1::resetM()
{
  twPtr->writeByteReg(M_Adr, M_Ctrl2, 0x0C);

  // Get ID
  return(twPtr->readByteReg(M_Adr, M_Id));
}

int SensorLSM9DS1::reset()
{
  int retv;

  twPtr->writeByteReg(AG_Adr, AG_Ctrl8, 0x05);
  twPtr->writeByteReg(M_Adr, M_Ctrl2, 0x0C);

  retv = twPtr->readByteReg(AG_Adr, AG_Id);
  retv += twPtr->readByteReg(M_Adr, M_Id);
  return(retv);
}

void SensorLSM9DS1::setScanAG(byte scValueAG, byte scValueA, byte scValueG)
{
  twPtr->writeByteReg(AG_Adr, AG_Ctrl6, scValueAG | scValueA);
  twPtr->writeByteReg(AG_Adr, AG_Ctrl1, scValueAG | scValueG);
}

void SensorLSM9DS1::begin()
{
  reset();
  setScanAG(AG_Odr119, A_Fs4g | A_LpAuto, G_Fs2000 | G_LpHH);
}

// ----------------------------------------------------------------------------
// Steuerfunktionen, gezielte Prozessorzugriffe und Hilfsfunktionen
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//





