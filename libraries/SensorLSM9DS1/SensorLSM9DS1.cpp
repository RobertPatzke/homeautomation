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

SensorLSM9DS1::SensorLSM9DS1(IntrfTw *refI2C, int inRunCycle)
{
  TwiParams twiParams;

  twPtr               = refI2C;
  runState            = rsInit;
  twiByteSeq.len      = 12;
  twiByteSeq.valueRef = byteArray;
  runCycle            = inRunCycle;

  fullScaleA  = 4;
  fullScaleG  = 2000;
  fullScaleM  = 4;

  newValueAG  = false;
  newValueM   = false;

  avgSetAG    = 0;
  avgCntAG    = 0;
  avgSetM     = 0;
  avgCntM     = 0;

  errorCntAdrNakAG    = 0;
  errorCntDataNakAG   = 0;
  errorCntOverAG      = 0;

  errorCntAdrNakM     = 0;
  errorCntDataNakM    = 0;
  errorCntOverM       = 0;

  timeOutTwiStatus    = 0;
  timeOutTwiDataAG    = 0;
  timeOutTwiDataM     = 0;

  timeOutStatusAG     = 0;
  toValueStatusAG     = 0;
  timeOutStatusM      = 0;
  toValueStatusM      = 0;

  toCntTwiStatusAG  = 0;
  toCntTwiStatusM   = 0;
  toCntTwiDataAG    = 0;
  toCntTwiDataM     = 0;
  toCntStatusAG     = 0;
  toCntStatusM      = 0;

  sumA.x = sumA.y = sumA.z = sumG.x = sumG.y = sumG.z = 0;
  waitCnt = 2;

  refI2C->getParams(&twiParams);
  switch(twiParams.speed)
  {
    case Twi100k:
      twiCycle = 10;
      break;

    case Twi250k:
      twiCycle = 4;
      break;

    case Twi400k:
      twiCycle = 2;
      break;
  }

  twiStatusCycle  = 40 * twiCycle;
  twiDataCycleAG  = 160 * twiCycle;
  twiDataCycleM   = 100 * twiCycle;

  enableMeasAG  = false;
  enableMeasM   = false;

  runStateCntTotal = 0;
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

void SensorLSM9DS1::setTimeOutValues(FreqAG fAG, FreqM fM)
{
  int freqA = 1190, freqM = 40000;
  int cycleA, cycleM;

  enableMeasAG = true;

  switch(fAG)
  {
    case FreqAG14_9:
      freqA = 149;
      break;

    case FreqAG59_5:
      freqA = 595;
      break;

    case FreqAG119:
      freqA = 1190;
      break;

    case FreqAG238:
      freqA = 2380;
      break;

    case FreqAG476:
      freqA = 4760;
      break;

    case FreqAG952:
      freqA = 9520;
      break;

    case FreqAG_OFF:
      freqA = 1190;
      enableMeasAG = false;
      break;
  }

  cycleA = (freqA * runCycle) / 10;     // Zyklusfrequenz
  toValueStatusAG = 1200000 / cycleA;   // Mikrosekunden + 20% Verlängerung

  // Test
  //toValueStatusAG += 2;

  enableMeasM = true;

  switch(fM)
  {
    case FreqM0_625:
      freqM = 625;
      break;

    case FreqM1_25:
      freqM = 1250;
      break;

    case FreqM2_5:
      freqM = 2500;
      break;

    case FreqM5:
      freqM = 5000;
      break;

    case FreqM10:
      freqM = 10000;
      break;

    case FreqM20:
      freqM = 20000;
      break;

    case FreqM40:
      freqM = 40000;
      break;

    case FreqM80:
      freqM = 80000;
      break;

    case FreqM_OFF:
      freqM = 40000;
      enableMeasM = false;
      break;
  }

  cycleM = (freqM * runCycle) / 1000;   // Zyklusfrequenz
  toValueStatusM = 1200000 / cycleM;    // Mikrosekunden + 20% Verlängerung

  // Test
  //toValueStatusM += 3;

}

void SensorLSM9DS1::begin(FreqAG freqAG, int avgAG, MaxA maxA, MaxG maxG, FreqM freqM, int avgM, MaxM maxM)
{
  setScanAG((byte) freqAG, (byte) maxA | A_LpAuto, (byte) maxG | G_LpHH);
  setScanM((byte) freqM | Mxy_PmMed | M_TmpOn, (byte) maxM, M_Contin, Mz_PmMed);

  setTimeOutValues(freqAG, freqM);

  if(avgAG == 1)
  {
    avgSetAG = 0;
    avgCntAG = 0;
  }
  else
  {
    avgSetAG = avgAG;
    avgCntAG = avgAG;
  }

  if(avgM == 1)
  {
    avgSetM = 0;
    avgCntM = 0;
  }
  else
  {
    avgSetM = avgM;
    avgCntM = avgM;
  }

  delay(10);
}

void SensorLSM9DS1::begin()
{
  //reset();

  delay(10);

  setScanAG(AG_Odr119, A_Fs4g | A_LpAuto, G_Fs2000 | G_LpHH);
  setScanM(M_Odr40 | Mxy_PmMed | M_TmpOn, M_Fs4G, M_Contin, Mz_PmMed);

  avgSetAG  = 6;
  avgCntAG  = 6;

  avgSetM   = 0;
  avgCntM   = 0;


  delay(10);
}

// ----------------------------------------------------------------------------
// Steuerfunktionen, gezielte Prozessorzugriffe und Hilfsfunktionen
// ----------------------------------------------------------------------------
//
/*
void SensorLSM9DS1::run0()
{
  switch(runState)
  {
    case rsInit:
      runState = rsScanAGReq;
      break;

      // ------------ Accel & Gyro ------------

    case rsScanAGReq:
      twPtr->recByteReg(AG_Adr, AG_Status, &twiByte);
      runState = rsScanAGChk;
      break;

    case rsScanAGChk:
      if(twiByte.twiStatus != TwStFin)
        break;

      if((twiByte.value & 0x03) == 0)
      {
        waitCnt = 2;
        runState = rsWaitAG;
        break;
      }

      twiByteSeq.len = 12;
      twPtr->recByteRegSeq(AG_Adr, G_Out, &twiByteSeq);
      runState = rsFetchAG;
      break;

    case rsWaitAG:
      if(waitCnt > 0)
      {
        waitCnt--;
        break;
      }
      else
      {
        runState = rsScanAGReq;
      }
      break;

    case rsFetchAG:
      if(twiByte.twiStatus != TwStFin)
        break;

      for(int i = 0; i < 12; i++)
        tmpDataAG.byteArray[i] = byteArray[i];

      if(avgSetAG > 0)
      {
        sumA.x += (int) tmpDataAG.valueAG.A.x;
        sumA.y += (int) tmpDataAG.valueAG.A.y;
        sumA.z += (int) tmpDataAG.valueAG.A.z;
        sumG.x += (int) tmpDataAG.valueAG.G.x;
        sumG.y += (int) tmpDataAG.valueAG.G.y;
        sumG.z += (int) tmpDataAG.valueAG.G.z;
        avgCntAG--;

        if(avgCntAG == 0)
        {
          rawDataAG.valueAG.A.x = short (sumA.x / avgSetAG);
          rawDataAG.valueAG.A.y = short (sumA.y / avgSetAG);
          rawDataAG.valueAG.A.z = short (sumA.z / avgSetAG);
          rawDataAG.valueAG.G.x = short (sumG.x / avgSetAG);
          rawDataAG.valueAG.G.y = short (sumG.y / avgSetAG);
          rawDataAG.valueAG.G.z = short (sumG.z / avgSetAG);

          sumA.x = sumA.y = sumA.z = sumG.x = sumG.y = sumG.z = 0;
          avgCntAG = avgSetAG;

          newValueAG = true;
        }
      }
      else
      {
        rawDataAG.valueAG.A.x = tmpDataAG.valueAG.A.x;
        rawDataAG.valueAG.A.y = tmpDataAG.valueAG.A.x;
        rawDataAG.valueAG.A.z = tmpDataAG.valueAG.A.x;
        rawDataAG.valueAG.G.x = tmpDataAG.valueAG.A.x;
        rawDataAG.valueAG.G.y = tmpDataAG.valueAG.A.x;
        rawDataAG.valueAG.G.z = tmpDataAG.valueAG.A.x;
        newValueAG = true;
      }

      runState = rsScanAGReq;
      break;
  }
}

void SensorLSM9DS1::run1()
{
  switch(runState)
  {
    case rsInit:
      runState = rsScanAGReq;
      break;

      // ------------ Accel & Gyro ------------

    case rsScanAGReq:
      twPtr->recByteReg(AG_Adr, AG_Status, &twiByte);
      runState = rsScanAGChk;
      break;

    case rsScanAGChk:
      if(twiByte.twiStatus != TwStFin)
        break;

      if((twiByte.value & 0x03) == 0)
      {
        waitCnt = 2;
        runState = rsWaitAG;
        break;
      }

      twiByteSeq.len = 12;
      twPtr->recByteRegSeq(AG_Adr, G_Out, &twiByteSeq);
      runState = rsFetchAG;
      break;

    case rsWaitAG:
      if(waitCnt > 0)
      {
        waitCnt--;
        break;
      }
      else
      {
        runState = rsScanAGReq;
      }
      break;

    case rsFetchAG:
      if(twiByteSeq.twiStatus != TwStFin)
        break;

      for(int i = 0; i < 12; i++)
        rawDataAG.byteArray[i] = byteArray[i];

      newValueAG = true;

      runState = rsScanAGReq;
      break;
  }
}

*/

void SensorLSM9DS1::run()
{
  runStateCntTotal++;

  switch(runState)
  {
    // ------------------------------------------------------------------------
    case rsInit:
    // ------------------------------------------------------------------------
      runStateCntArray[rsInit]++;
      runState = rsScanAGReq;
      timeOutStatusAG = toValueStatusAG;
      timeOutStatusM  = toValueStatusM;
      break;

      // ------------ Accel & Gyro ------------

    // ------------------------------------------------------------------------
    case rsScanAGReq:
    // ------------------------------------------------------------------------
      runStateCntArray[rsScanAGReq]++;
      if(!enableMeasAG)
      {
        runState = rsScanMReq;
        break;
      }

      twPtr->recByteReg(AG_Adr, AG_Status, &twiByte);
      timeOutTwiStatus = twiStatusCycle / runCycle + 1;
      runState = rsScanAGChk;
      break;

    // ------------------------------------------------------------------------
    case rsWaitAG:
    // ------------------------------------------------------------------------
      runStateCntArray[rsWaitAG]++;
      break;

    // ------------------------------------------------------------------------
    case rsScanAGChk:
    // ------------------------------------------------------------------------
      runStateCntArray[rsScanAGChk]++;
      if((twiByte.twiStatus != TwStFin) && ((twiByte.twiStatus & TwStError) == 0))
      {
        if(timeOutTwiStatus > 0)
          timeOutTwiStatus--;
        else
        {
          toCntTwiStatusAG++;
          runState = rsScanAGReq;
        }
        break;
      }

      if((twiByte.twiStatus & TwStError) != 0)
      {
        if(twiByte.twiStatus == TwStAdrNak)
          errorCntAdrNakAG++;
        else if(twiByte.twiStatus == TwStDataNak)
          errorCntDataNakAG++;
        else
          errorCntOverAG++;

        runState = rsScanAGReq;
        break;
      }

      if((twiByte.value & 0x03) == 0)
      {
        if(timeOutStatusAG > 0)
          timeOutStatusAG--;
        else
        {
          timeOutStatusAG = toValueStatusAG;
          toCntStatusAG++;
        }
        runState = rsScanMReq;    // -> Magnet
        break;
      }

      timeOutStatusAG = toValueStatusAG;
      twiByteSeq.len = 12;
      twPtr->recByteRegSeq(AG_Adr, G_Out, &twiByteSeq);
      timeOutTwiDataAG = twiDataCycleAG / runCycle + 1;
      runState = rsFetchAG;
      break;

    // ------------------------------------------------------------------------
    case rsFetchAG:
    // ------------------------------------------------------------------------
      runStateCntArray[rsFetchAG]++;
      if((twiByteSeq.twiStatus != TwStFin) && ((twiByte.twiStatus & TwStError) == 0))
      {
        if(timeOutTwiDataAG > 0)
        {
          timeOutTwiDataAG--;
          break;
        }
      }

      if(((twiByteSeq.twiStatus & TwStError) != 0) || (timeOutTwiDataAG == 0))
      {
        if(twiByteSeq.twiStatus == TwStAdrNak)
          errorCntAdrNakAG++;
        else if(twiByteSeq.twiStatus == TwStDataNak)
          errorCntDataNakAG++;
        else if(twiByteSeq.twiStatus == TwStOverrun)
          errorCntOverAG++;
        else
          toCntTwiDataAG++;

        twiByteSeq.len = 12;
        twPtr->recByteRegSeq(AG_Adr, G_Out, &twiByteSeq);
        timeOutTwiDataAG = twiDataCycleAG / runCycle + 1;
        break;
      }

      for(int i = 0; i < 12; i++)
        rawDataAG.byteArray[i] = byteArray[i];

      if(avgSetAG > 0)
      {
        sumA.x += rawDataAG.valueAG.A.x;
        sumA.y += rawDataAG.valueAG.A.y;
        sumA.z += rawDataAG.valueAG.A.z;
        sumG.x += rawDataAG.valueAG.G.x;
        sumG.y += rawDataAG.valueAG.G.y;
        sumG.z += rawDataAG.valueAG.G.z;
        avgCntAG--;

        if(avgCntAG == 0)
        {
          rawDataAG.valueAG.A.x = sumA.x / avgSetAG;
          rawDataAG.valueAG.A.y = sumA.y / avgSetAG;
          rawDataAG.valueAG.A.z = sumA.z / avgSetAG;
          rawDataAG.valueAG.G.x = sumG.x / avgSetAG;
          rawDataAG.valueAG.G.y = sumG.y / avgSetAG;
          rawDataAG.valueAG.G.z = sumG.z / avgSetAG;
          sumA.x = sumA.y = sumA.z = sumG.x = sumG.y = sumG.z = 0;
          avgCntAG = avgSetAG;
          newValueAG = true;
        }
      }
      else
        newValueAG = true;

      runState = rsScanAGReq;
      break;

      // ------------ Magnet ------------

    // ------------------------------------------------------------------------
    case rsScanMReq:
    // ------------------------------------------------------------------------
      runStateCntArray[rsScanMReq]++;
      if(!enableMeasM)
      {
        runState = rsScanAGReq;
        break;
      }

      twPtr->recByteReg(M_Adr, M_Status, &twiByte);
      timeOutTwiStatus = twiStatusCycle / runCycle + 1;
      runState = rsScanMChk;
      break;

    // ------------------------------------------------------------------------
    case rsScanMChk:
    // ------------------------------------------------------------------------
      runStateCntArray[rsScanMChk]++;
      if((twiByte.twiStatus != TwStFin) && ((twiByte.twiStatus & TwStError) == 0))
      {
        if(timeOutTwiStatus > 0)
          timeOutTwiStatus--;
        else
        {
          toCntTwiStatusM++;
          runState = rsScanMReq;
        }
        break;
      }

      if((twiByte.twiStatus & TwStError) != 0)
      {
        if(twiByte.twiStatus == TwStAdrNak)
          errorCntAdrNakM++;
        else if(twiByte.twiStatus == TwStDataNak)
          errorCntDataNakM++;
        else
          errorCntOverM++;

        runState = rsScanAGReq;
        break;
      }

      if((twiByte.value & 0x08) == 0)
      {
        if(timeOutStatusM > 0)
          timeOutStatusM--;
        else
        {
          timeOutStatusM = toValueStatusM;
          toCntStatusM++;
        }
        runState = rsScanAGReq;    // -> Accel,Gyro
        break;
      }

      timeOutStatusM = toValueStatusM;
      twiByteSeq.len = 6;
      twPtr->recByteRegSeq(M_Adr, M_Out, &twiByteSeq);
      timeOutTwiDataM = twiDataCycleM / runCycle + 1;
      runState = rsFetchM;
      break;

    // ------------------------------------------------------------------------
    case rsFetchM:
    // ------------------------------------------------------------------------
      runStateCntArray[rsFetchM]++;
      if((twiByteSeq.twiStatus != TwStFin) && ((twiByte.twiStatus & TwStError) == 0))
      {
        if(timeOutTwiDataM > 0)
        {
          timeOutTwiDataM--;
          break;
        }
      }

      if( ((twiByteSeq.twiStatus & TwStError) != 0) || (timeOutTwiDataM == 0) )
      {
        if(twiByteSeq.twiStatus == TwStAdrNak)
          errorCntAdrNakM++;
        else if(twiByteSeq.twiStatus == TwStDataNak)
          errorCntDataNakM++;
        else if(twiByteSeq.twiStatus == TwStOverrun)
          errorCntOverM++;
        else
          toCntTwiDataM++;

        twiByteSeq.len = 6;
        twPtr->recByteRegSeq(AG_Adr, M_Out, &twiByteSeq);
        timeOutTwiDataM = twiDataCycleM / runCycle + 1;
        break;
      }

      for(int i = 0; i < 6; i++)
        rawDataM.byteArray[i] = byteArray[i];

      if(avgSetM > 0)
      {
        sumM.x += rawDataM.valueM.x;
        sumM.y += rawDataM.valueM.y;
        sumM.z += rawDataM.valueM.z;
        avgCntM--;

        if(avgCntM == 0)
        {
          rawDataM.valueM.x = sumM.x / avgSetM;
          rawDataM.valueM.y = sumM.y / avgSetM;
          rawDataM.valueM.z = sumM.z / avgSetM;
          sumM.x = sumM.y = sumM.z = 0;
          avgCntM = avgSetM;
          newValueM = true;
        }
      }
      else
      {
        newValueM = true;
      }

      runState = rsScanAGReq;
      break;
  }
}


bool  SensorLSM9DS1::getValuesM(RawDataMPtr rdptr)
{
  if(!newValueM) return(false);
  for(int i = 0; i < 6; i++)
    rdptr->byteArray[i] = rawDataM.byteArray[i];
  newValueM = false;
  return(true);
}

bool  SensorLSM9DS1::getValuesM(CalValuePtr calPtr)
{
  if(!newValueM) return(false);

  calPtr->x = (float) fullScaleM * (float) rawDataM.valueM.x / (float) 32767;
  calPtr->y = (float) fullScaleM * (float) rawDataM.valueM.y / (float) 32767;
  calPtr->z = (float) fullScaleM * (float) rawDataM.valueM.z / (float) 32767;

  newValueM = false;
  return(true);
}

void  SensorLSM9DS1::syncValuesAG()
{
  newValueAG = false;
}

bool  SensorLSM9DS1::getValuesAG(RawDataAGPtr rdptr)
{
  if(!newValueAG) return(false);
  for(int i = 0; i < 12; i++)
    rdptr->byteArray[i] = rawDataAG.byteArray[i];
  newValueAG = false;
  return(true);
}

bool  SensorLSM9DS1::getValuesAG(CalValueAGPtr calPtr)
{
  if(!newValueAG) return(false);

  calPtr->G.x = (float) fullScaleG * (float) rawDataAG.valueAG.G.x / (float) 32767;
  calPtr->G.y = (float) fullScaleG * (float) rawDataAG.valueAG.G.y / (float) 32767;
  calPtr->G.z = (float) fullScaleG * (float) rawDataAG.valueAG.G.z / (float) 32767;

  calPtr->A.x = (float) fullScaleA * (float) rawDataAG.valueAG.A.x / (float) 32767;
  calPtr->A.y = (float) fullScaleA * (float) rawDataAG.valueAG.A.y / (float) 32767;
  calPtr->A.z = (float) fullScaleA * (float) rawDataAG.valueAG.A.z / (float) 32767;

  newValueAG = false;
  return(true);
}

bool  SensorLSM9DS1::getAvgValuesAG(CalValueAGPtr calPtr)
{
  if(!newValueAG) return(false);
  newValueAG = false;

  if(avgSetAG > 0)
  {
    sumA.x += rawDataAG.valueAG.A.x;
    sumA.y += rawDataAG.valueAG.A.y;
    sumA.z += rawDataAG.valueAG.A.z;
    sumG.x += rawDataAG.valueAG.G.x;
    sumG.y += rawDataAG.valueAG.G.y;
    sumG.z += rawDataAG.valueAG.G.z;
    avgCntAG--;

    if(avgCntAG > 0) return(false);

    calPtr->G.x = (float) fullScaleG * (float) (sumG.x / avgSetAG) / (float) 32767;
    calPtr->G.y = (float) fullScaleG * (float) (sumG.y / avgSetAG) / (float) 32767;
    calPtr->G.z = (float) fullScaleG * (float) (sumG.z / avgSetAG) / (float) 32767;

    calPtr->A.x = (float) fullScaleA * (float) (sumA.x / avgSetAG) / (float) 32767;
    calPtr->A.y = (float) fullScaleA * (float) (sumA.y / avgSetAG) / (float) 32767;
    calPtr->A.z = (float) fullScaleA * (float) (sumA.z / avgSetAG) / (float) 32767;

    avgCntAG = avgSetAG;
    return(true);
  }

  calPtr->G.x = (float) fullScaleG * (float) rawDataAG.valueAG.G.x / (float) 32767;
  calPtr->G.y = (float) fullScaleG * (float) rawDataAG.valueAG.G.y / (float) 32767;
  calPtr->G.z = (float) fullScaleG * (float) rawDataAG.valueAG.G.z / (float) 32767;

  calPtr->A.x = (float) fullScaleA * (float) rawDataAG.valueAG.A.x / (float) 32767;
  calPtr->A.y = (float) fullScaleA * (float) rawDataAG.valueAG.A.y / (float) 32767;
  calPtr->A.z = (float) fullScaleA * (float) rawDataAG.valueAG.A.z / (float) 32767;

  return(true);
}




// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------
//
dword SensorLSM9DS1::debGetDword(int code)
{
  dword retv;


  switch(code)
  {
    case 1:
      retv = toValueStatusAG;
      break;

    case 2:
      retv = toValueStatusM;
      break;

    default:
      retv = 0;
      break;
  }
  return(retv);
}





