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
  chipType      = chip;
  chipCnt       = nrChips;
  chipCmd       = cmd;
  datPin        = dataPin;
  clkPin        = clockPin;
  ctrlIdx       = 0;

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
// Operation functions
// -------------------------------------------------------------------------
//
void MY92Led::run()
{
  ChannelCtrlPtr    ctrlPtr;
  ChannelLinkPtr    linkPtr;

  // Update (change of light) is done after all value changes happened.
  // Cycle time is (nrCtrl+1)/frequency
  //
  if(ctrlIdx >= nrCtrl)
  {
    ctrlIdx = 0;
    if(doUpdate)
    {
      doUpdate = false;
      my92x->update();
    }
    return;
  }

  ctrlPtr = &ctrlArrPtr[ctrlIdx];

  // Do blinking of light but with several features
  //
  if(ctrlPtr->blink)
  {
    // Waiting ON or OFF time via counter
    //
    if(ctrlPtr->count > 0)
      ctrlPtr->count--;
    else
    {
      // Waiting time elapsed, change light
      //
      if(ctrlPtr->onToggle)
      {
        // light was ON while waiting
        // set value now to OFF
        //
        ctrlPtr->onToggle = false;
        ctrlPtr->count = ctrlPtr->timeOff;
        my92x->setChannel(ctrlPtr->nrChn, ctrlPtr->minValue);

        // if other channels are linked to this channel
        // then set their values also
        //
        linkPtr = ctrlPtr->chnLink;
        while(linkPtr != NULL)
        {
          if(linkPtr->invert)
            my92x->setChannel(linkPtr->nrChn, linkPtr->chnCtrl->maxValue);
          else
            my92x->setChannel(linkPtr->nrChn, linkPtr->chnCtrl->minValue);
          linkPtr = linkPtr->next;
        }
        doUpdate = true;
      }
      else
      {
        // light was OFF while waiting
        // set value now to ON
        //
        ctrlPtr->onToggle = true;
        ctrlPtr->count = ctrlPtr->timeOn;
        my92x->setChannel(ctrlPtr->nrChn, ctrlPtr->maxValue);

        // if other channels are linked to this channel
        // then set their values also
        //
        linkPtr = ctrlPtr->chnLink;
        while(linkPtr != NULL)
        {
          if(linkPtr->invert)
            my92x->setChannel(linkPtr->nrChn, linkPtr->chnCtrl->minValue);
          else
            my92x->setChannel(linkPtr->nrChn, linkPtr->chnCtrl->maxValue);
          linkPtr = linkPtr->next;
        }
        doUpdate = true;
      }
    }
  }

  ctrlIdx++;

}

// -------------------------------------------------------------------------
// User functions
// -------------------------------------------------------------------------
//
void MY92Led::begin(int cycleTime, int nrCtrlChn)
{
  nrCtrl        = nrCtrlChn;
  ctrlArrPtr    = new ChannelCtrl[nrCtrl];

  frequency = 1000 / cycleTime;
  baseBlinkTime = cycleTime * (nrCtrlChn + 1);

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

int MY92Led::driverChannel(LightColor color)
{
  switch(color)
  {
    case ColdWhite:
      return(0);

    case WarmWhite:
      return(1);

    case Red:
      return(4);

    case Green:
      return(3);

    case Blue:
      return(5);
  }
}


void MY92Led::setBlink(LightColor color, int minVal, int minTime, int maxVal, int maxTime)
{
  ChannelCtrlPtr    ctrlPtr;
  int               colorIdx;

  colorIdx  = (int) color;
  ctrlPtr   = &ctrlArrPtr[colorIdx];
  ctrlPtr->nrChn = driverChannel(color);

  ctrlPtr->maxValue = maxVal;
  ctrlPtr->timeOn   = maxTime / baseBlinkTime;
  ctrlPtr->minValue = minVal;
  ctrlPtr->timeOff  = minTime / baseBlinkTime;

  ctrlPtr->onToggle = false;
  ctrlPtr->count    = 0;
  ctrlPtr->blink    = true;
  ctrlPtr->chnLink  = NULL;
}

void MY92Led::linkBlink(LightColor link, LightColor color, int minVal, int maxVal, bool inv)
{
  ChannelLinkPtr    linkPtr;
  ChannelCtrlPtr    ctrlPtr;
  int               colorIdx;

  colorIdx  = (int) link;
  ctrlPtr   = &ctrlArrPtr[colorIdx];


  if(ctrlPtr->chnLink == NULL)
  {
    ctrlPtr->chnLink            = new ChannelLink;
    ctrlPtr->chnLink->nrChn     = driverChannel(color);
    ctrlPtr->chnLink->invert    = inv;
    ctrlPtr->chnLink->chnCtrl   = &ctrlArrPtr[(int) color];
    ctrlPtr->chnLink->next      = NULL;

    ctrlPtr->chnLink->chnCtrl->maxValue = maxVal;
    ctrlPtr->chnLink->chnCtrl->minValue = minVal;
    return;
  }

  linkPtr = ctrlPtr->chnLink;

  for(int i = 0; i < (nrCtrl - 1); i++)
  {
    if(linkPtr->next != NULL)
    {
      linkPtr = linkPtr->next;
      continue;
    }

    linkPtr->next           = new ChannelLink;
    linkPtr->next->nrChn    = driverChannel(color);
    linkPtr->next->invert   = inv;
    linkPtr->next->chnCtrl  = &ctrlArrPtr[(int) color];
    linkPtr->next->next     = NULL;

    linkPtr->next->chnCtrl->maxValue = maxVal;
    linkPtr->next->chnCtrl->minValue = minVal;
    return;
  }
}

