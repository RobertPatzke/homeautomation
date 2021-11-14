//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SensorLSM9DS1.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef SENSORLSM9DS1_H
#define SENSORLSM9DS1_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfTw.h"

// ----------------------------------------------------------------------------

// ------------------------ Acceleration and Gyroscope -------
#define AG_Adr    0x6B
// ------------------------ Acceleration and Gyroscope -------
#define AG_Id     0x0F
#define AG_Ctrl1  0x10
#define G_Out     0x18
#define AG_Ctrl6  0x20
#define AG_Ctrl8  0x22
#define AG_Status 0x27

#define AG_Rate(x)      (x << 5)
#define AG_Odr14_9      0x20
#define AG_Odr59_5      0x40
#define AG_Odr119       0x60
#define AG_Odr238       0x80
#define AG_Odr476       0xA0
#define AG_Odr952       0xC0

typedef enum _FreqAG
{
  FreqAG_OFF  = 0xFF,
  FreqAG14_9  = AG_Odr14_9,
  FreqAG59_5  = AG_Odr59_5,
  FreqAG119   = AG_Odr119,
  FreqAG238   = AG_Odr238,
  FreqAG476   = AG_Odr476,
  FreqAG952   = AG_Odr952
} FreqAG;

#define AG_FullScale(x) (x << 3)
#define A_Fs2g          0x00
#define A_Fs4g          0x10
#define A_Fs8g          0x18
#define A_Fs16g         0x08
#define G_Fs245         0x00
#define G_Fs2000        0x18
#define G_Fs500         0x08

typedef enum _MaxA
{
  MaxAcc2g    = A_Fs2g,
  MaxAcc4g    = A_Fs4g,
  MaxAcc8g    = A_Fs8g,
  MaxAcc16g   = A_Fs16g
} MaxA;

typedef enum _MaxG
{
  MaxGyro245dps   = G_Fs245,
  MaxGyro500dps   = G_Fs500,
  MaxGyro2000dps  = G_Fs2000
} MaxG;

#define AG_LowPass(x)   (x)
#define A_LpAuto        0x00
#define A_Lp50          0x07
#define A_Lp105         0x06
#define A_Lp211         0x05
#define A_Lp408         0x04
#define G_LpLL          0x00
#define G_LpLH          0x01
#define G_LpHL          0x02
#define G_LpHH          0x03

// ------------------------ Magnetic Field -------
#define M_Adr     0x1E
// ------------------------ Magnetic Field -------
#define M_Id      0x0F
#define M_Ctrl1   0x20
#define M_Ctrl2   0x21
#define M_Ctrl3   0x23
#define M_Ctrl4   0x24
#define M_Status  0x27
#define M_Out     0x28

// Control 1
#define M_Rate(x)       (x << 2)
#define M_Odr0_625      0x00
#define M_Odr1_25       0x04
#define M_Odr2_5        0x08
#define M_Odr5          0x0C
#define M_Odr10         0x10
#define M_Odr20         0x14
#define M_Odr40         0x18
#define M_Odr80         0x1C

typedef enum _FreqM
{
  FreqM_OFF   = 0xFF,
  FreqM0_625  = M_Odr0_625,
  FreqM1_25   = M_Odr1_25,
  FreqM2_5    = M_Odr2_5,
  FreqM5      = M_Odr5,
  FreqM10     = M_Odr10,
  FreqM20     = M_Odr20,
  FreqM40     = M_Odr40,
  FreqM80     = M_Odr80
} FreqM;

#define M_Temp(x)       (x << 7)
#define M_TmpOn         0x80
#define M_TmpOff        0x00

#define Mxy_Power(x)    (x << 6)
#define Mxy_PmLow       0x00
#define Mxy_PmMed       0x20
#define Mxy_PmHigh      0x40
#define Mxy_PmUhigh     0x60

// Control 2
#define M_FullScale(x)  (x << 5)
#define M_Fs4G          0x00
#define M_Fs8G          0x20
#define M_Fs12G         0x40
#define M_Fs16G         0x60

typedef enum _MaxM
{
  MaxMag4G      = M_Fs4G,
  MaxMag8G      = M_Fs8G,
  MaxMag12G     = M_Fs12G,
  MaxMag16G     = M_Fs16G
} MaxM;


// Control 3
#define M_OpMode(x)     (x)
#define M_Contin        0x00
#define M_Single        0x01
#define M_Down          0x10

// Control 4
#define Mz_Power(x)    (x << 2)
#define Mz_PmLow       0x00
#define Mz_PmMed       0x04
#define Mz_PmHigh      0x08
#define Mz_PmUhigh     0x0C


typedef enum _RunState
{
  rsInit,
  rsScanAGReq,
  rsWaitAG,
  rsScanAGChk,
  rsFetchAG,
  rsScanMReq,
  rsScanMChk,
  rsFetchM
} RunState;

#define NrOfRunStates 8

typedef struct _RawValue
{
  short int   x;
  short int   y;
  short int   z;
} RawValue;

typedef struct _SumValue
{
  int   x;
  int   y;
  int   z;
} SumValue, *SumValuePtr;

typedef struct _RawValueAG
{
  RawValue  G;
  RawValue  A;
} RawValueAG;

typedef union _RawDataAG
{
  byte        byteArray[12];
  RawValueAG  valueAG;
} RawDataAG, *RawDataAGPtr;

typedef union _RawDataM
{
  byte        byteArray[6];
  RawValue    valueM;
} RawDataM, *RawDataMPtr;

typedef struct _CalValue
{
  float   x;
  float   y;
  float   z;
} CalValue, *CalValuePtr;

typedef struct _CalValueAG
{
  CalValue  G;
  CalValue  A;
} CalValueAG, *CalValueAGPtr;

typedef struct _SensorErrors
{

} SensorErrors, *SensorErrorsPtr;

class SensorLSM9DS1
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten und Funktionen
  // --------------------------------------------------------------------------
  //
  IntrfTw     *twPtr;
  TwiByte     twiByte;
  TwiByteSeq  twiByteSeq;
  byte        byteArray[12];

  bool        enableMeasAG;
  bool        newValueAG;
  RawDataAG   rawDataAG;
  RawDataAG   comDataAG;

  bool        enableMeasM;
  bool        newValueM;
  RawDataM    rawDataM;

  int         fullScaleA;
  int         fullScaleG;
  int         fullScaleM;

  SumValue    sumA;
  SumValue    sumG;
  int         avgSetAG;
  int         avgCntAG;

  SumValue    sumM;
  int         avgSetM;
  int         avgCntM;

  dword       timeOutTwiStatus;
  dword       timeOutTwiDataAG;
  dword       timeOutTwiDataM;

  dword       timeOutStatusAG;
  dword       toValueStatusAG;
  dword       timeOutStatusM;
  dword       toValueStatusM;

  int         twiCycle;
  int         twiStatusCycle;
  int         twiDataCycleAG;
  int         twiDataCycleM;

  RunState    runState;
  int         waitCnt;
  int         runCycle;

  void setTimeOutValues(FreqAG fAG, FreqM fM);

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  SensorLSM9DS1(IntrfTw *refI2C, int inRunCycle);

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  int resetAG();
  int resetM();
  int reset();

  void setScanAG(byte scValueAG, byte scValueA, byte scValueG);
  // Messparameter für Accel und Gyro
  // scValueAG = Abtastrate
  // scValueA  = Vollausschlag und Tiefpass für Beschleunigung
  // scValueB  = Vollausschlag und Tiefpass für Gyrometer

  void setScanM(byte scValue1, byte scValue2, byte scValue3, byte scValue4);
  // Messparameter für Magnetfeld
  // scValue1 = Abtastrate, Temperaturkompensation und XY-Powermode
  // scValue2 = Vollausschlag
  // scValue3 = Betriebsart
  // scValue4 = Z-Powermode


  void begin(FreqAG freqAG, int avgAG, MaxA maxA, MaxG maxG, FreqM freqM, int avgM, MaxM maxM);
  void begin();

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  void run();
  void run0();
  void run1();

  // --------------------------------------------------------------------------
  // Datenaustausch
  // --------------------------------------------------------------------------
  //
  dword       errorCntOverAG;
  dword       errorCntAdrNakAG;
  dword       errorCntDataNakAG;

  dword       errorCntOverM;
  dword       errorCntAdrNakM;
  dword       errorCntDataNakM;

  dword       toCntTwiStatusAG;
  dword       toCntTwiStatusM;
  dword       toCntTwiDataAG;
  dword       toCntTwiDataM;
  dword       toCntStatusAG;
  dword       toCntStatusM;

  void  syncValuesAG();
  bool  getValuesAG(RawDataAGPtr rdptr);
  bool  getValuesAG(CalValueAGPtr calPtr);
  bool  getAvgValuesAG(CalValueAGPtr calPtr);
  bool  getValuesM(RawDataMPtr rdptr);
  bool  getValuesM(CalValuePtr calPtr);

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

  // ----------------------------------------------------------------------------
  //                      D e b u g - H i l f e n
  // ----------------------------------------------------------------------------
  //
  dword runStateCntArray[NrOfRunStates];
  dword runStateCntTotal;
  dword debGetDword(int code);

};

#endif // SENSORLSM9DS1_H

