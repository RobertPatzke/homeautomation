//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Adc.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   03. Oktober 2022
//

#ifndef NRF52840ADC_H
#define NRF52840ADC_H

#include <stdio.h>
#include "arduinoDefs.h"
#include "IntrfAdc.h"
#include "nrf52840.h"

#ifndef nrfAdcDef
// ----------------------------------------------------------------------------
typedef struct _ChnLimits
{
  dword   Low;
  dword   High;
} ChnLimits;

typedef struct _ChnConfigs
{
  dword   PSELP;
  dword   PSELN;
  dword   CONFIG;
  dword   LIMIT;
} ChnConfigs;

#define ChnConfPullDownInP  0x00000001
#define ChnConfPullUpInP    0x00000002
#define ChnConfV1_2InpP     0x00000003
#define ChnConfPullDownInN  0x00000010
#define ChnConfPullUpInN    0x00000020
#define ChnConfV1_2InpN     0x00000030
#define ChnConfGain1_6      0x00000000
#define ChnConfGain1_5      0x00000100
#define ChnConfGain1_4      0x00000200
#define ChnConfGain1_3      0x00000300
#define ChnConfGain1_2      0x00000400
#define ChnConfGain1        0x00000500
#define ChnConfGain2        0x00000600
#define ChnConfGain4        0x00000700
#define ChnConfRefV1_4      0x00001000
#define ChnConfAcqT3        0x00000000
#define ChnConfAcqT5        0x00010000
#define ChnConfAcqT10       0x00020000
#define ChnConfAcqT15       0x00030000
#define ChnConfAcqT20       0x00040000
#define ChnConfAcqT40       0x00050000
#define ChnConfDiffMode     0x00100000
#define ChnConfBurst        0x01000000


typedef struct _nrfAdc
{
  volatile  dword TASKS_START;                    // 000
  volatile  dword TASKS_SAMPLE;                   // 004
  volatile  dword TASKS_STOP;                     // 008
  volatile  dword TASKS_CALIBRATEOFFSET;          // 00C
  volatile  dword Reserve01[60];                  // 010
  volatile  dword EVENTS_STARTED;                 // 100
  volatile  dword EVENTS_END;                     // 104
  volatile  dword EVENTS_DONE;                    // 108
  volatile  dword EVENTS_RESULTDONE;              // 10C
  volatile  dword EVENTS_CALIBRATEDONE;           // 110
  volatile  dword EVENTS_STOPPED;                 // 114
  volatile  ChnLimits EVENTS_CHNLIMIT[8];         // 118
  volatile  dword Reserve02[106];                 // 158
  volatile  dword INTEN;                          // 300
  volatile  dword INTENSET;                       // 304
  volatile  dword INTENCLR;                       // 308
  volatile  dword Reserve03[61];                  // 30C
  volatile  dword STATUS;                         // 400
  volatile  dword Reserve04[63];                  // 404
  volatile  dword ENABLE;                         // 500
  volatile  dword Reserve05[03];                  // 504
  volatile  ChnConfigs CH[8];                     // 510
  volatile  dword Reserve06[24];                  // 590
  volatile  dword RESOLUTION;                     // 5F0
  volatile  dword OVERSAMPLE;                     // 5F4
  volatile  dword SAMPLERATE;                     // 5F8
  volatile  dword Reserve07[12];                  // 5FC
  volatile  dword RESULT_PTR;                     // 62C
  volatile  dword RESULT_MAXCNT;                  // 630
  volatile  dword RESULT_AMOUNT;                  // 634
} nrfAdc, *nrfAdcPtr;

#define NrfAdcBase    0x40007000
#define NrfAdcPtr     ((nrfAdcPtr) NrfAdcBase)

// Falls noch kein Zugriff auf die Taktfestlegung erfolgte
//
#ifndef NrfClockBase
#define NrfClockBase    0x40000000
#endif

#ifndef nrfClockTASKS_HFCLKSTART
#define nrfClockTASKS_HFCLKSTART  ((dword *) 0x40000000)
#endif

#ifndef nrfClock_HFCLKSTAT
#define nrfClock_HFCLKSTAT  ((dword *) 0x4000040C)
#endif

#ifndef nrfClock_HFCLKisRunning
#define nrfClock_HFCLKisRunning   0x00010000
#endif

#define nrfAdcDef
// ----------------------------------------------------------------------------
#endif

#define P0(x) (x)
#define P1(x) (32+x)

#ifdef smnNANOBLE33
// ----------------------------------------------------------------------------
#define ArdA0Bit    4
#define ArdA1Bit    5
#define ArdA2Bit    30
#define ArdA3Bit    29
#define ArdA4Bit    31
#define ArdA5Bit    2
#define ArdA6Bit    28
#define ArdA7Bit    3

#define ArdA0   P0(4)
#define ArdA1   P0(5)
#define ArdA2   P0(30)
#define ArdA3   P0(29)
#define ArdA4   P0(31)
#define ArdA5   P0(2)
#define ArdA6   P0(28)
#define ArdA7   P0(3)

#define ArdA0Mask   (1 << 4)
#define ArdA1Mask   (1 << 5)
#define ArdA2Mask   (1 << 30)
#define ArdA3Mask   (1 << 29)
#define ArdA4Mask   (1 << 31)
#define ArdA5Mask   (1 << 2)
#define ArdA6Mask   (1 << 28)
#define ArdA7Mask   (1 << 3)

enum  AdcChnPin
{
  seC1P = 4,
  seC2P = 5,
  seC3P = 30,
  seC4P = 29,
  seC5P = 31,
  seC6P = 2,
  seC7P = 28,
  seC8P = 3
};

// ----------------------------------------------------------------------------
#endif

class nRF52840Adc : IntrfAdc
{
  // --------------------------------------------------------------------------
  // Spezifische Datentypen
  // --------------------------------------------------------------------------
  //
  class Channels
  {
  public:
    int     unsList[8];
    int     srtList[8];
    int     nrOfChannels;
    short   seResult[8];
    short   diResult[4];

    void clrLists();
    int  add(int chnNr);
    void sort();
  };

private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  Channels    channels;
  Statistic   statistic;

  // --------------------------------------------------------------------------
  // lokale Funktionen
  // --------------------------------------------------------------------------
  //
  dword cnfChn(int chnIdx, ChnConf conf, AcqTime acqt, PreGain pg);
  dword cnfPin(PinNr pinNr);

public:
  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  nRF52840Adc();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void config(PinNr pinNrP, PinNr pinNrN, ChnNr chnNr, ChnConf conf, AcqTime acqt, PreGain pg);
  void begin();

  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //

  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
  static  nRF52840Adc *instPtr0;
  static  void irqHandler0();
  void    irqHandler();

  // --------------------------------------------------------------------------
  // Debugging und globale Variablen
  // --------------------------------------------------------------------------
  //

};

#endif //NRF52840ADC_H
