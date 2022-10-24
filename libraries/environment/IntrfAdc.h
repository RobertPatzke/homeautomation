//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfAdc.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   03. Oktober 2022
//
// Eine Schnittstelle zur A/D-Wandlung
//

#ifndef IntrfAdc_h
#define IntrfAdc_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"

class IntrfAdc
{
public:
  // --------------------------------------------------------------------------
  // Spezifische Datentypen und Zuordnungen
  // --------------------------------------------------------------------------
  // ACHTUNG!
  // Dies ist eine Übermenge für alle eingesetzten Mikrocontroller.
  // Bei Bedarf werden diese Festlegungen erweitert.

typedef struct _Statistic
{
  dword   interrupts;
} Statistic, *StatisticPtr;

// Konfiguration des ADC-Eingangs
//
typedef struct _chnConf
{
  dword inpResistorP  : 1;    // Widerstand zugeschaltet an P-Eingang
  dword inpPullUpP    : 1;    // P-Eingangswiderstand als Pull-Up
  dword inpVoltP      : 1;    // Spannung am P-Eingang
  dword inpResistorN  : 1;    // Widerstand zugeschaltet an N-Eingang
  dword inpPullUpN    : 1;    // N-Eingangswiderstand als Pull-Up
  dword inpVoltN      : 1;    // Spannung am N-Eingang
  dword diffMode      : 1;    // Differenzmodus
  dword burst         : 1;    // Oversampling aktiviert
  dword externRef     : 1;    // externe Referenz
} ChnConf;

// Kennzeichnung des ADC-Kanals (PIN-Zuweisung extra)
//
enum  ChnNr
{
  sc1,      // Single-ended Kanal 1
  sc2,
  sc3,
  sc4,
  sc5,
  sc6,
  sc7,
  sc8,
  dc1,      // Differenzkanal 1
  dc2,
  dc3,
  dc4,
  sAll,     // Alle SE-Kanäle (gleiche Konfiguration)
  dAll,     // Alle Differenzkanäle (gleiche Konfiguration)
  mc1,      // Mix-Messung, 1xDiff, 2xSE, Kanal 1
  mc2,
  mc3,
  mc4
};

// Kennzeichnung der Eingansanschlüsse (Pins)
// Das orientiert sich an durchnummerierten festen Pins für Analogeingänge
// Wenn mehr Pins nutzbar sind, als ADC-Kanäle existieren, dann muss ein
// spezifisches Mapping erfolgen
// Besondere Eingänge (z.B. Versorgungsspannung, Temperatur, etc.) sind
// spezifisch für den jeweiligen Mikrocontroller auszuweisen
//
enum PinNr
{
  noConn,   // nicht angeschlossen
  aInp1,    // Analogeingang 1
  aInp2,
  aInp3,
  aInp4,
  aInp5,
  aInp6,
  aInp7,
  aInp8,
  aInpSp1,  // Spezialeingang 1 (z.B. VDD)
  aInpSp2,
  aInpSp3,
  aInpChn   // Direkte Kanalzuordnung
};

// Einstellung der Haltezeit (Kondensator) am Eingang des ADC
//
enum  AcqTime
{
  acqtUndefined,
  acqt1us,  // Haltezeit 1 Mikrosekunde
  acqt2us,
  acqt3us,
  acqt5us,
  acqt10us,
  acqt15us,
  acqt20us,
  acqt30us,
  acqt40us,
  acqt50us,
  acqt100us
};

// Einstellung der Vorverstärkung
//
enum  PreGain
{
  pgUndefined,
  pg1_6,    // 1/6
  pg1_5,
  pg1_4,
  pg1_3,
  pg1_2,    // 1/2
  pg1,
  pg2,
  pg4,      // 4
  pg8,
  pg16
};

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  virtual void config       // Konfigurieren eines Adc-Kanals
  (
    PinNr pinNrP,           // Anschlusspin + (Analogeingang)
    PinNr pinNrN,           // Anschlusspin - (noConn bei Single-Ended)
    ChnNr chnNr,            // Kanalnummer
    ChnConf conf,           // Konfigurationsdetails (Auswahl)
    AcqTime acqt,           // Haltezeit
    PreGain pg              // Vorverstärkung
  );

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  virtual void begin();     // Starten des Betriebs nach der Konfiguration


  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //

};

// ----------------------------------------------------------------------------
#endif  // IntrfAdc_h
