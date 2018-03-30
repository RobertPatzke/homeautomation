// ---------------------------------------------------------------------------
// File:        MY92Led.cpp
// Editors:     Robert Patzke,
// Start:       19. March 2018
// Last change: 19. March 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
// This library is based on the work of Xose Pérez in
// https://github.com/xoseperez/my92xx
// and a datasheet of MY9221 which I found somewhere in the Internet.
// I could not find any datasheet for the used MY9231, only overview pages.
// The reason for creating a new library is to understand the interface
// to MY9231 based on the datasheet of MY9221
// With respect to the work of [XP], the first step is to wrap the library
// of [XP] as is, i.e. including <my92xx>, for SonOff B1 (MY9231)
//

#include "MY92Led.h"

// -------------------------------------------------------------------------
// Constructors and initialisations
// -------------------------------------------------------------------------
//
MY92Led::MY92Led(ChipType chip, uint8_t nrChips, uint8_t dataPin, uint8_t clockPin, uint16_t cmd)
{
  chipType  = chip;
  chipCnt   = nrChips;
  chipCmd   = cmd;
  datPin    = dataPin;
  clkPin    = clockPin;

  if(chip == MY92XX)
  {
    return;
  }

  switch(chip)
  {
    case MY9221:
      channelCnt = 12 * nrChips;
      break;

    case MY9231:
      channelCnt = 3 * nrChips;
      break;

    case MY9291:
      channelCnt = 4 * nrChips;
      break;
  }

  dutyCycles = new word[channelCnt];    // allocate PWM memory
  for(int i = 0; i < channelCnt; i++)   // did not use memset because not
    dutyCycles[i] = 0;                  // sure about alignment

  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
}

// -------------------------------------------------------------------------
// User functions
// -------------------------------------------------------------------------
//
void MY92Led::begin()
{
  if(chipType == MY92XX)
  {
    my92x = new my92xx(MY92XX_MODEL_MY9231, chipCnt, datPin, clkPin, MY92XX_COMMAND_DEFAULT);
    my92x->setState(true);
  }
}

void MY92Led::setLight(byte cold, byte warm, byte red, byte green, byte blue)
{
  if(chipType == MY92XX)
  {
    my92x->setChannel(0, cold);
    my92x->setChannel(1, warm);
    my92x->setChannel(3, green);
    my92x->setChannel(4, red);
    my92x->setChannel(5, blue);
    my92x->update();
  }
}


