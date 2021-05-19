///-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Scanner.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   8. Mai 2021
//
// Der Inhalt dieser Datei sind Festlegungen zum Empfangen und Vorauswerten
// von BLE-Telegrammen
//

#include "Scanner.h"

  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  void Scanner::init01()
  {
  }

  Scanner::Scanner(IntrfRadio *inRadio)
  {
    radio = inRadio;
    init01();
  }

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void Scanner::setDevAddress(BD_ADR bdAdr)
  {
  }

  void Scanner::setDevAddress(word head, dword body)
  {
  }


  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void Scanner::start()
  {

  }



