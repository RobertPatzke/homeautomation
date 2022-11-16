//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Process Measurements
// Datei:   ProcMeas.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "ProcMeas.h"

#include "arduinoDefs.h"

// ---------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ---------------------------------------------------------------------------
//
ProcMeas::ProcMeas(IntrfMeas *measPtr)
{ 
  pMeas = measPtr;
  gravAnglesAvail = false;
  pmState = pmInit;
  gravSigns = 0;
#ifdef  ProcMeasDebug
  memset(&statistics,0,sizeof(Statistics));
#endif
};

// ---------------------------------------------------------------------------
// Anwenderfunktionen
// ---------------------------------------------------------------------------
//

void ProcMeas::run()
{
  if(pMeas == NULL) return;

  switch(pmState)
  {
    case pmInit:
#ifdef  ProcMeasDebug
      statistics.pmInitCnt++;
#endif
      pmState = pmWait;
      break;

    case pmWait:
#ifdef  ProcMeasDebug
      statistics.pmWaitCnt++;
#endif
      if(pMeas->available(1, 1))
      {
        pMeas->getValues(1, 1, &gravAngles);
        gravSigns = pMeas->getSigns(1, 1);
        pmState = pmCalc;
      }
      break;

    case pmCalc:
#ifdef  ProcMeasDebug
      statistics.pmCalcCnt++;
#endif
      posture.pitch = euler.getPitchFromGravity(gravAngles.x, gravAngles.y, gravAngles.z) * smnRAD_TO_DEG;
      posture.roll  = euler.getRollFromGravity(gravAngles.y, gravAngles.z) * smnRAD_TO_DEG;
      posture.yaw   = 0.0;
      pMeas->sync(1, 1);
      gravAnglesAvail = true;
      pmState = pmWait;
      break;
  }
}

bool ProcMeas::availAngles(bool reset)
{
  if(!gravAnglesAvail) return(false);
  if(reset) gravAnglesAvail = false;
  return(true);
}

float ProcMeas::getRollValue()
{
  return(posture.roll);
}

float ProcMeas::getPitchValue()
{
  return(posture.pitch);
}

float ProcMeas::getYawValue()
{
  return(posture.yaw);
}

byte  ProcMeas::getGravSigns()
{
  return(gravSigns);
}

