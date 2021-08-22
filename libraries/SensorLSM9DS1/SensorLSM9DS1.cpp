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
  twPtr               = refI2C;
  runState            = rsInit;
  twiByteSeq.len      = 12;
  twiByteSeq.valueRef = byteArray;

  fullScaleA  = 4;
  fullScaleG  = 2000;
  fullScaleM  = 4;

  newValue    = false;
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

void SensorLSM9DS1::setScanM(byte scValue1, byte scValue2, byte scValue3, byte scValue4)
{
  twPtr->writeByteReg(M_Adr, M_Ctrl1, scValue1);
  twPtr->writeByteReg(M_Adr, M_Ctrl2, scValue2);
  twPtr->writeByteReg(M_Adr, M_Ctrl3, scValue3);
  twPtr->writeByteReg(M_Adr, M_Ctrl4, scValue4);
}


void SensorLSM9DS1::begin()
{
  //reset();

  setScanAG(AG_Odr119, A_Fs4g | A_LpAuto, G_Fs2000 | G_LpHH);
  setScanM(M_Odr40 | Mxy_PmMed | M_TmpOn, M_Fs4G, M_Contin, Mz_PmMed);

  delay(10);
}

// ----------------------------------------------------------------------------
// Steuerfunktionen, gezielte Prozessorzugriffe und Hilfsfunktionen
// ----------------------------------------------------------------------------
//
void SensorLSM9DS1::run()
{
  switch(runState)
  {
    case rsInit:

      runState = rsScanReq;
      break;

    case rsScanReq:
      twPtr->recByteReg(AG_Adr, AG_Status, &twiByte);
      runState = rsScanChk;
      break;

    case rsScanChk:
      if(twiByte.twiStatus != TwStFin)
        break;
      if((twiByte.value & 0x03) == 0)
      {
        runState = rsScanReq;
        break;
      }

      twPtr->recByteRegSeq(AG_Adr, G_Out, &twiByteSeq);
      runState = rsFetch;
      break;

    case rsFetch:
      if(twiByteSeq.twiStatus != TwStFin)
        break;

      for(int i = 0; i < 12; i++)
        valueArray[i] = byteArray[i];
      newValue = true;
      runState = rsScanReq;
      break;
  }
}

bool  SensorLSM9DS1::getValues(RawDataPtr rdptr)
{
  if(!newValue) return(false);
  for(int i = 0; i < 12; i++)
    rdptr->byteArray[i] = valueArray[i];
  newValue = false;
  return(true);
}

bool  SensorLSM9DS1::getValues(CalValueAGPtr calPtr)
{
  if(!newValue) return(false);

  union
  {
    byte  b[2];
    short val;
  };

  b[0] = valueArray[0];
  b[1] = valueArray[1];
  calPtr->G.x = (float) fullScaleG * (float) val / (float) 32767;

  b[0] = valueArray[2];
  b[1] = valueArray[3];
  calPtr->G.y = (float) fullScaleG * (float) val / (float) 32767;

  b[0] = valueArray[4];
  b[1] = valueArray[5];
  calPtr->G.z = (float) fullScaleG * (float) val / (float) 32767;

  b[0] = valueArray[6];
  b[1] = valueArray[7];
  calPtr->A.x = (float) fullScaleA * (float) val / (float) 32767;

  b[0] = valueArray[8];
  b[1] = valueArray[9];
  calPtr->A.y = (float) fullScaleA * (float) val / (float) 32767;

  b[0] = valueArray[10];
  b[1] = valueArray[11];
  calPtr->A.z = (float) fullScaleA * (float) val / (float) 32767;

  newValue = false;
  return(true);
}



// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//





