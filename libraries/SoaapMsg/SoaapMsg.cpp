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

// Erstellen eines ASCII-Telegramms zum Übertragen der Messwerte
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

    case saiDefaultMeasCtrl:
      measLen = 18;
      break;

   case saiMaximalMeas:
      measLen = 26;
      break;
  }

  for(measIdx = 0; measIdx < measLen; measIdx++)
  {
    measByte = meas[measIdx];

    // Erst das niederwertige Nibble als Hex-Ascii eintragen
    //
    measChar = (measByte & 0x0F) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;

    // dann das höherwertige Nibble
    //
    measChar = (measByte >> 4) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;
  }

  if(appId == saiDefaultMeasCtrl)
  {
    measByte = meas[20];

    // Erst das niederwertige Nibble als Hex-Ascii eintragen
    //
    measChar = (measByte & 0x0F) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;

    // dann das höherwertige Nibble
    //
    measChar = (measByte >> 4) | 0x30;
    if (measChar > 0x39) measChar += 7;
    dest[msgIdx++] = measChar;
  }

  dest[msgIdx] = '\0';
  return(msgIdx);
}

// Umwandeln eines Messwert aus ASCII-Telegramm in Integer
//
short  SoaapMsg::asc2meas(byte *ascList)
{
  unsigned short retv;

  retv = getVal(ascList[0]);
  retv += getVal(ascList[1]) << 4;
  retv += getVal(ascList[2]) << 8;
  retv += getVal(ascList[3]) << 12;

  return((short) retv);
}

// Auflösung der Messwerte in Zeichen (Bytes)
//
int   SoaapMsg::measRes(SoaapApId appId)
{
  int retv = 0;

  switch(appId)
  {
    case saiDefaultMeas:
      retv = 4;
      break;

    case saiDefaultMeasCtrl:
      retv = 4;
      break;

    case saiMaximalMeas:
      retv = 4;
      break;
  }

  return(retv);
}

// Anzahl der Messwerte im Telegramm
//
int   SoaapMsg::measCnt(SoaapApId appId)
{
  int retv = 0;

  switch(appId)
  {
    case saiDefaultMeas:
      retv = 9;
      break;

    case saiDefaultMeasCtrl:
      retv = 9;
      break;

    case saiMaximalMeas:
      retv = 13;
      break;
  }

  return(retv);
}

