//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   MeasMuse.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
// Datum:   14. November 2022
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "MeasMuse.h"

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//
MeasMuse::MeasMuse()
{
}

// ----------------------------------------------------------------------------
// Hilfsfunktionen
// ----------------------------------------------------------------------------
//
void MeasMuse::Posture2Midi::setKoeffRoll(MeasMap map)
{
  switch(map)
  {
    case BiLinear:
      koeffRoll = (midiArea[aimRoll].high - midiArea[aimRoll].low)
                / (borderHighRoll - borderLowRoll);
      midiArea[aimRoll].offset = midiArea[aimRoll].low - (byte) koeffRoll * borderLowRoll;
      break;

    default:
      break;
  }
}

void MeasMuse::Posture2Midi::setKoeffPitch(MeasMap map)
{
  switch(map)
  {
    case BiLinear:
      koeffPitch = (midiArea[aimPitch].high - midiArea[aimPitch].low)
                / (borderHighPitch - borderLowPitch);
      midiArea[aimPitch].offset = midiArea[aimPitch].low - (byte) koeffPitch * borderLowPitch;
      break;

    default:
      break;
  }
}

void MeasMuse::Posture2Midi::setKoeffYaw(MeasMap map)
{
  switch(map)
  {
    case BiLinear:
      koeffYaw = (midiArea[aimYaw].high - midiArea[aimYaw].low)
                / (borderHighYaw - borderLowYaw);
      midiArea[aimYaw].offset = midiArea[aimYaw].low - (byte) koeffYaw * borderLowYaw;
      break;

    default:
      break;
  }
}

int MeasMuse::Posture2Midi::getResultRoll(MidiResultPtr refResult, float measValue)
{
  float inVal = measValue + offsetRoll;
#ifdef MeasMuseDebug
  debInValRoll = inVal;
#endif
  if(inVal < borderLowRoll) return(-1);
  if(inVal > borderHighRoll) return(1);
  byte value = midiArea[aimRoll].offset + (byte) (koeffRoll * inVal);
  if(refResult->value != value)
  {
    refResult->value  = value;
    refResult->newVal = true;
  }
  refResult->type   = aimRoll;
#ifdef MeasMuseDebug
  debResVal = value;
#endif
  return(0);
}

int MeasMuse::Posture2Midi::getResultPitch(MidiResultPtr refResult, float measValue)
{
  float inVal = measValue + offsetPitch;
  if(inVal < borderLowPitch) return(-1);
  if(inVal > borderHighPitch) return(1);
  byte value = midiArea[aimPitch].offset + (byte) (koeffPitch * inVal);
  if(refResult->value != value)
  {
    refResult->value  = value;
    refResult->newVal = true;
  }
  refResult->type   = aimPitch;
  return(0);
}

int MeasMuse::Posture2Midi::getResultYaw(MidiResultPtr refResult, float measValue)
{
  float inVal = measValue + offsetYaw;
  if(inVal < borderLowYaw) return(-1);
  if(inVal > borderHighYaw) return(1);
  byte value = midiArea[aimYaw].offset + (byte) (koeffYaw * inVal);
  if(refResult->value != value)
  {
    refResult->value  = value;
    refResult->newVal = true;
  }
  refResult->type   = aimYaw;
  return(0);
}


// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
void MeasMuse::setRollArea(int channel, float offset, float min, float max)
{
  config[channel].offsetRoll      = offset;
  config[channel].borderLowRoll   = min;
  config[channel].borderHighRoll  = max;
}

void MeasMuse::setPitchArea(int channel, float offset, float min, float max)
{
  config[channel].offsetPitch      = offset;
  config[channel].borderLowPitch   = min;
  config[channel].borderHighPitch  = max;
}

void MeasMuse::setYawArea(int channel, float offset, float min, float max)
{
  config[channel].offsetYaw      = offset;
  config[channel].borderLowYaw   = min;
  config[channel].borderHighYaw  = max;
}

void MeasMuse::setAims(int channel, Meas2Midi aimRoll, Meas2Midi aimPitch, Meas2Midi aimYaw)
{
  config[channel].aimRoll   = aimRoll;
  config[channel].aimPitch  = aimPitch;
  config[channel].aimYaw    = aimYaw;
}

void MeasMuse::setMidiArea(int channel, Meas2Midi midi, byte low, byte high)
{
  config[channel].midiArea[midi].low  = low;
  config[channel].midiArea[midi].high = high;
}

void MeasMuse::setMapping(int channel, MeasMap mapRoll, MeasMap mapPitch, MeasMap mapYaw)
{
  config[channel].setKoeffRoll(mapRoll);
  config[channel].setKoeffPitch(mapPitch);
  config[channel].setKoeffYaw(mapYaw);
}

void MeasMuse::setOpMode(int channel, int opMode)
{
  config[channel].midiOpMode = opMode;
}

// ----------------------------------------------------------------------------
// Anwenderfunktionen
// ----------------------------------------------------------------------------
//
int MeasMuse::resultRoll(int channel, MidiResultPtr refResult, float measValue)
{
  return(config[channel].getResultRoll(refResult, measValue));
}

int MeasMuse::resultPitch(int channel, MidiResultPtr refResult, float measValue)
{
  return(config[channel].getResultPitch(refResult, measValue));
}

int MeasMuse::resultYaw(int channel, MidiResultPtr refResult, float measValue)
{
  return(config[channel].getResultYaw(refResult, measValue));
}

int MeasMuse::getOpMode(int channel)
{
  return(config[channel].midiOpMode);
}

