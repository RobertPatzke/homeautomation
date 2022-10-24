//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Gpio.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#include "nRF52840Adc.h"

// --------------------------------------------------------------------------
// Konstruktoren
// --------------------------------------------------------------------------
//
  nRF52840Adc::nRF52840Adc()
  {
#ifdef nrfClockTASKS_HFCLKSTART
    if((*nrfClock_HFCLKSTAT & nrfClock_HFCLKisRunning) == 0)
      *nrfClockTASKS_HFCLKSTART = 1;
#endif

  }

// --------------------------------------------------------------------------
// Unterklasse Channels
// --------------------------------------------------------------------------
//
void nRF52840Adc::Channels::clrLists()
{
  for(int i = 0; i < 8; i++)
  {
    unsList[i] = 0;
    srtList[i] = 0;
    nrOfChannels = 0;
  }
}

int nRF52840Adc::Channels::add(int chnNr)
{
  for(int i = 0; i < 8; i++)
  {
    if(unsList[i] == 0)
    {
      unsList[i] = chnNr;
      nrOfChannels++;
      return(i);
    }
  }
  return(-1);
}

void nRF52840Adc::Channels::sort()
{
  int minPos;
  int cnt;
  int tmp;

  for(cnt = 0; cnt < 8; cnt++)
  {
    if(unsList[cnt] == 0) break;
    srtList[cnt] = unsList[cnt];
  }

  for(int i = 0; i < cnt; i++)
  {
    minPos = i; // Position des kleinsten Elementes auf Durchgang

    // Position minPos des kleinsten Elementes bestimmen
    for(int j = i+1; j < cnt; j++)
      if(srtList[j] < srtList[minPos])
        minPos = j;
    // minPos zeigt jetzt auf das kleinste Element im unsortierten Bereich

    if(minPos != i) // Wenn ein späteres Element kleiner ist
    { // Vertauschen der Felder von i und minPos
      tmp = srtList[i];
      srtList[i] = srtList[minPos];
      srtList[minPos] = tmp;
    }
  }
}



// --------------------------------------------------------------------------
// Konfigurationen
// --------------------------------------------------------------------------
//

// Einstellen der Kanalparameter (CH[].CONFIG)
//
dword nRF52840Adc::cnfChn(int chnIdx, ChnConf conf, AcqTime acqt, PreGain pg)
{
  dword confVal = 0;

  if(conf.inpResistorP)   // Beschaltung am Pluseingang
  {
    if(conf.inpPullUpP)
      confVal |= ChnConfPullUpInP;
    else
      confVal |= ChnConfPullDownInP;
  }
  else if(conf.inpVoltP)
    confVal |= ChnConfV1_2InpP;

  if(conf.inpResistorN)   // Beschaltung am Minuseingang
  {
    if(conf.inpPullUpN)
      confVal |= ChnConfPullUpInN;
    else
      confVal |= ChnConfPullDownInN;
  }
  else if(conf.inpVoltN)
    confVal |= ChnConfV1_2InpN;

  // Einstellen des Vorverstärkers
  //
  if(pg == pg1_5)
    confVal |= ChnConfGain1_5;
  else if(pg == pg1_4)
    confVal |= ChnConfGain1_4;
  else if(pg == pg1_3)
    confVal |= ChnConfGain1_3;
  else if(pg == pg1_2)
    confVal |= ChnConfGain1_2;
  else if(pg == pg1)
    confVal |= ChnConfGain1;
  else if(pg == pg2)
    confVal |= ChnConfGain2;
  else if(pg == pg4)
    confVal |= ChnConfGain4;

  // Auswahl der Referenzspannung
  //
  if(conf.externRef)
    confVal |= ChnConfRefV1_4;

  // Einstellen der Wartezeit vor Wandlung (Ladezeit, Erfassungszeit)
  //
  if(acqt == acqt5us)
    confVal |= ChnConfAcqT5;
  else if (acqt == acqt10us)
    confVal |= ChnConfAcqT10;
  else if (acqt == acqt15us)
    confVal |= ChnConfAcqT15;
  else if (acqt == acqt20us)
    confVal |= ChnConfAcqT20;
  else if (acqt == acqt40us)
    confVal |= ChnConfAcqT40;

  // Differenzmessung einstellen
  //
  if(conf.diffMode)
    confVal |= ChnConfDiffMode;

  // Burst (Oversampling) aktivieren
  //
  if(conf.burst)
    confVal |= ChnConfBurst;

  return(confVal);
}

// Zuordnen der Eingänge
//
dword nRF52840Adc::cnfPin(PinNr pinNr)
{
  if(pinNr < aInpSp1)
    return((dword) pinNr);
  if(pinNr == aInpSp1)
    return(9);
  if(pinNr == aInpSp2)
    return(0x0D);
  return(0);
}


void nRF52840Adc::config(PinNr pinNrP, PinNr pinNrN, ChnNr chnNr,
                          ChnConf conf, AcqTime acqt, PreGain pg)
{
  int chnIdx;
  volatile ChnConfigs  *chnCfgPtr;

  if(chnNr < dc1)     // Single-Ended Kanäle
  {
    // Zeiger auf die Konfigurationsregister festlegen
    chnIdx = chnNr;
    chnCfgPtr = &(NrfAdcPtr->CH[chnIdx]);

    // Konfigurationsdaten (vom Anwender) eintragen
    conf.diffMode = false;    // Mögliche Fehler vermeiden
    chnCfgPtr->CONFIG = cnfChn(chnIdx,conf,acqt,pg);

    // Eingänge zuordnen
    chnCfgPtr->PSELP = cnfPin(pinNrP);
    chnCfgPtr->PSELN = 0;   // Negativeingang nicht benutzt

    // Vorerst Einzelabtastungen über Task-Anweisung
    NrfAdcPtr->RESULT_MAXCNT = 1;
    NrfAdcPtr->SAMPLERATE = 0;

    // Speicher zuweisen
    NrfAdcPtr->RESULT_PTR = (dword) &channels.seResult;

    // Kanalliste aufbauen
    channels.add(chnIdx+1);
  }

}

// ----------------------------------------------------------------------------
// Anwendungsfunktionen
// ----------------------------------------------------------------------------
//
void nRF52840Adc::begin()
{
  // Auflösung vorerst fest eingestellt
  NrfAdcPtr->RESOLUTION = 2;  // 12 Bit

  // Einschalten des ADC
  NrfAdcPtr->ENABLE = 1;

  // Temperatur-Kalibrierung wird später gemacht
  //

  // Kanäle sortieren in der Liste
  //
  channels.sort();

  // Interrupt-Handling
  // ----------------------------------------------------------------------
  //
  NrfAdcPtr->INTENCLR = 0xFFFFFFFF;   // Vorsichtshalber keine Interrupts

  instPtr0 = this;    // Statischen Zeiger auf diese Instanz setzen

  // Interruptvektor setzen
  //
  __NVIC_SetVector((IRQn_Type) 7, (dword) nRF52840Adc::irqHandler0);
  __NVIC_SetPriority((IRQn_Type) 7, 1);
  __NVIC_EnableIRQ((IRQn_Type) 7);

}


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

nRF52840Adc *nRF52840Adc::instPtr0 = NULL;

void nRF52840Adc::irqHandler0()
{
  if(instPtr0 == NULL) return;
  instPtr0->irqHandler();
}

void nRF52840Adc::irqHandler()
{
  statistic.interrupts++;

}
