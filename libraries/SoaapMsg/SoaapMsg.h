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

#ifndef byte
#define byte unsigned char
#endif

typedef enum
{
  saiDefaultMeas = 0x68,
  saiMaximalMeas = 0x69
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

public:
  // --------------------------------------------------------------------------
  // Anwenderschnittstelle (Funktionen)
  // --------------------------------------------------------------------------
  //
  int   getMsgA(int area, int slvNr, SoaapApId appId, char *dest, byte *meas);

};

#endif // SoaapMsg_h
