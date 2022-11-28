//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   MeasMuse.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
// Datum:   14. November 2022
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//
#ifndef _MeasMuse_h
#define _MeasMuse_h
//-----------------------------------------------------------------------------

#include "arduinoDefs.h"

//#define MeasMuseDebug

#define NrOfChannelsMM  16

// ----------------------------------------------------------------------------
// Datentypen
// ----------------------------------------------------------------------------
//
typedef struct _Value2Midi
{
  short borderLowAz;
  short borderHighAz;
  short borderLowAy;
  short borderHighAy;
  short borderLowAx;
  short borderHighAx;

  byte  lowNote;
  byte  highNote;

} Value2Midi, *Value2MidiPtr;

enum Meas2Midi
{
  Nothing,      // keine Zielzuweisung
  NoteType,     // Zuweisung Notentyp (Länge)
  NoteVal,      // Zuweisung Notenwert (Höhe)
  NoteVel,      // Zuweisung Anschagstärke
  NrM2M         // Anzahl für Array
};

enum MeasMap
{
  OneToOne,     // Direkte Wertübertragung
  BiLinear,     // Linear auf Linear
  NrMM
};

typedef struct _MidiResult
{
  Meas2Midi type;
  byte      value;
  bool      newVal;
} MidiResult, *MidiResultPtr;

typedef struct _MidiArea
{
  byte  low;
  byte  high;
  byte  offset;
} MidiArea, *MidiAreaPtr;


// ---------------------------------------------------------------------------
// class MeasMuse
// ---------------------------------------------------------------------------
// Abbildung von Messwerten auf musikalische und andere künstlerische
// Umgebung inklusive der Konfiguration

class MeasMuse
{
  // -------------------------------------------------------------------------
  // class Posture2Midi, eingebettet
  // -------------------------------------------------------------------------
  // Details der Konfiguration für einen Kanal
  //
public:
  class Posture2Midi
  {
  public:
    // Konfigurationsdaten
    // -----------------------------------------------------------------------
    float       offsetRoll;
    float       borderLowRoll;
    float       borderHighRoll;
    float       koeffRoll;
    float       offsetPitch;
    float       borderLowPitch;
    float       borderHighPitch;
    float       koeffPitch;
    float       offsetYaw;
    float       borderLowYaw;
    float       borderHighYaw;
    float       koeffYaw;
    Meas2Midi   aimRoll;
    Meas2Midi   aimPitch;
    Meas2Midi   aimYaw;
    byte        signAreaValue[4];
    byte        signAreaCtrl[4];
    MidiArea    midiArea[NrM2M];

    // Hilfsfunktionen
    // -----------------------------------------------------------------------
    //
  public:
    void setKoeffRoll(MeasMap map);
    void setKoeffPitch(MeasMap map);
    void setKoeffYaw(MeasMap map);
    int  getResultRoll(MidiResultPtr refResult, float measValue);
    int  getResultPitch(MidiResultPtr refResult, float measValue);
    int  getResultYaw(MidiResultPtr refResult, float measValue);

#ifdef MeasMuseDebug

    // Debugging
    // -----------------------------------------------------------------------
    //
    float   debInValRoll;
    byte    debResVal;

#endif
  };


public:
  // -------------------------------------------------------------------------
  // globale klassenspezifische Datentypen
  // -------------------------------------------------------------------------
  //

private:
  // -------------------------------------------------------------------------
  // lokale Datentypen
  // -------------------------------------------------------------------------
  //

  // -------------------------------------------------------------------------
  // Hilfsfunktionen
  // -------------------------------------------------------------------------
  //

private:
#ifdef MeasMuseDebug
public:
#endif
  // -------------------------------------------------------------------------
  // lokale Variablen
  // -------------------------------------------------------------------------
  //
  Posture2Midi  config[NrOfChannelsMM];       // Konfiguration der Abbildung

public:
  // -------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // -------------------------------------------------------------------------
  //
  MeasMuse();

  // -------------------------------------------------------------------------
  // Konfiguration
  // -------------------------------------------------------------------------
  //
  void setRollArea(int channel, float offset, float min, float max);
  void setPitchArea(int channel, float offset, float min, float max);
  void setYawArea(int channel, float offset, float min, float max);
  void setAims(int channel, Meas2Midi aimRoll, Meas2Midi aimPitch, Meas2Midi aimYaw);
  void setMidiArea(int channel, Meas2Midi midi, byte low, byte high);
  void setMapping(int channel, MeasMap mapRoll, MeasMap mapPitch, MeasMap mapYaw);

  // -------------------------------------------------------------------------
  // Anwenderfunktionen
  // -------------------------------------------------------------------------
  //
  int resultRoll(int channel, MidiResultPtr refResult, float measValue);
  int resultPitch(int channel, MidiResultPtr refResult, float measValue);
  int resultYaw(int channel, MidiResultPtr refResult, float measValue);

};

//-----------------------------------------------------------------------------
#endif // _MeasMuse_h
