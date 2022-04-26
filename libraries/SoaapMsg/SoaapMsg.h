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

#ifndef SoaapMsg_h
#define SoaapMsg_h

#include "arduinoDefs.h"

typedef enum
{
  saiDefaultMeas = 0x68,
  saiMaximalMeas = 0x69,
  saiDefaultMeasCtrl = 0x6A
} SoaapApId;

class SoaapMsg
{
public:
  // --------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // --------------------------------------------------------------------------
  //
  SoaapMsg();

private:
  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Inline-Methoden
  // --------------------------------------------------------------------------
  //
  int getVal(char hexAsc)
  {
    if(hexAsc < 0x39) return(hexAsc - 0x30);
    else return(hexAsc - 0x37);
  }


public:
  // --------------------------------------------------------------------------
  // Anwenderschnittstelle (Funktionen)
  // --------------------------------------------------------------------------
  //

  int   getMsgA(int area, int slvNr, SoaapApId appId, char *dest, byte *meas);
  // Erstellen eines ASCII-Telegramms zum Übertragen der Messwerte

  short asc2meas(byte *ascList);
  // Umwandeln eines Messwert aus ASCII-Telegramm in Integer

  int   measRes(SoaapApId appId);
  // Auflösung der Messwerte in Zeichen (Bytes)

  int   measCnt(SoaapApId appId);
  // Anzahl der Messwerte

};

#endif // SoaapMsg_h
