// ---------------------------------------------------------------------------
// File:        MY92Led.h
// Editors:     Robert Patzke,
// Start:       19. March 2018
// Last change: 19. March 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
// This library is based on the work of Xose Pérez in
// https://github.com/xoseperez/my92xx, referenced with [XP],
// and a datasheet of MY9221 which I found somewhere in the Internet.
// I could not find any datasheet for the used MY9231, only overview pages.
// The reason for creating a new library is to understand the interface
// to MY9231 based on the datasheet of MY9221
// With respect to the work of [XP], the first step is to wrap the library
// of [XP] as is, i.e. including <my92xx>, for SonOff B1 (MY9231)


#ifndef _MY92Led_h
#define _MY92Led_h
// ---------------------------------------------------------------------------

#include "Arduino.h"
#include "my92xx.h"

enum ChipType
{
  MY9221,
  MY9231,
  MY9291,
  MY92XX
};

typedef struct _MY9221Cmd
{
  uint16_t  onest:1;    // One-shot 0= frame cycle repeat, 1= one-shot
  uint16_t  cntset:1;   // Counter reset 0=free runnung, 1=reset mode
  uint16_t  pol:1;      // Output polarity 0=LED driver, 1=Generator
  uint16_t  osc:1;      // Grayscale clock 1=external GCKI
  uint16_t  sep:1;      // Output waveform 0=PWM, 1=APDM
  uint16_t  gck:3;      // Internal oscillator frequency 8.6 Mhz / 2^gck
  uint16_t  bs:2;       // Grayscale resoulution 8/12/14/16 value 0-3
  uint16_t  hspd:1;     // Iout Tr/Tf select, 1 = fast mode
  uint16_t  temp:5;     // not used
}
MY9221Cmd, *MY9221CmdPtr;

// Setting only sep to 1 (ADPM)
//
#define MY9221DefCmd    0x0010

// the following typedef is derived from [XP],
// but compared to datasheet of MY9221, the value for one-shot is
// in the sequence at an unexspected position.
// And the meaning of several bits is just inverted.
// There will be testing necessary.
// The following comments on bits are from datasheet MY9221
//
typedef struct _MY9231Cmd
{
  uint8_t   sep:1;      // Output waveform 0=PWM, 1=APDM
  uint8_t   gck:2;      // Internal oscillator frequency 8.6 Mhz / 2^gck
  uint8_t   bs:2;       // Grayscale resoulution 8/12/14/16 value 0-3
  uint8_t   hspd:1;     // Iout Tr/Tf select, 1 = fast mode
  uint8_t   onest:1;    // One-shot 0= frame cycle repeat, 1= one-shot
  uint8_t   temp:1;     // not used
}
MY9231Cmd, *MY9231CmdPtr;

// Setting only sep to 1 (ADPM)
//
#define MY9231DefCmd    0x01

// Default value from [XP], which means gray resolution 8 bits, ADPM and fast mode
// as defined in enumerations of [XP].
// But compared to datasheet of MY9221 the bits are inverted.
// So there will be testing necessary to examine the bit meanings
//
#define MY9231XpCmd     0x18


class MY92Led
{
private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  ChipType  chipType;       // Type of used chip
  word      chipCmd;        // Chip specific command
  byte      clkPin;         // PIO for the clock
  byte      datPin;         // PIO for data
  byte      chipCnt;        // Number of cascaded chips
  byte      channelCnt;     // Number of channels incl. cascading
  word      *dutyCycles;    // Pointer to list of duty cycles (all channels)
  my92xx    *my92x;         // Pointer to my92xx-object from [XP]

public:
  // -------------------------------------------------------------------------
  // Constructors and initialisations
  // -------------------------------------------------------------------------
  //
  MY92Led(ChipType chip, uint8_t nrChips, uint8_t dataPin, uint8_t clockPin, uint16_t cmd);

  // -------------------------------------------------------------------------
  // User functions
  // -------------------------------------------------------------------------
  //
  void begin();
  void setLight(byte cold, byte warm, byte red, byte green, byte blue);

};

// ---------------------------------------------------------------------------
#endif // _MY92Led_h

