//-----------------------------------------------------------------------------
// Thema:   Steuerung optischer und akustischer Ausgaben für Perfomer
// Datei:   SoaapMsg.h
// Editor:  Robert Patzke
// URI/URL: www.hs-hannover.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   18. März 2022
//
// Diese Bibliothek (Klassse) enthält diverse Ressourcen zur Generierung
// von Meldungen und Telegrammen, die im Rahmen des SOAAP-Projektes
// eingesetzt werden.
//

#include "SoaapMsg.h"

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//
SoaapMsg::SoaapMsg()
{

}

// ----------------------------------------------------------------------------
// Anwenderschnittstelle (Funktionen)
// ----------------------------------------------------------------------------
//

int   SoaapMsg::getMsgA(int area, int slvNr, SoaapApId appId, char *dest, byte *meas)
{
  int   msgIdx = 0;
  int   measIdx;
  int   measLen;
  byte  measByte;
  char  measChar;

  dest[msgIdx++]  = (char) (area | 0x10);
  dest[msgIdx++]  = (char) (slvNr | 0x60);
  dest[msgIdx++]  = (char) (appId);

  switch(appId)
  {
    case saiDefaultMeas:
      measLen = 18;
      break;

    case saiMaximalMeas:
      measLen = 26;
      break;
  }

  for(measIdx = 0; measIdx < measLen; measIdx++)
  {
    measByte = meas[measIdx];

    measChar = (measByte >> 4) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;

    measChar = (measByte & 0x0F) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;
  }

  dest[msgIdx] = '\0';
  return(msgIdx);
}


