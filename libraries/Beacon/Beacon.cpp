//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Beacon.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   8. Mai 2021
//
// Der Inhalt dieser Datei sind Festlegungen zur Gestaltung eines sog. Beacon.
// Das ist ein relativ kleines Datenpaket (ca. 30 Bytes), das zyklisch per
// Rundruf versendet wird (Leuchtfeuer).
// Die Dateninhalte sind variabel, also es können damit genauso definierte
// Daten (iBeacon, Eddystone, etc.) wie auch Messwerte versendet werden.
//

#include "Beacon.h"

  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Beacon::Beacon(mbcType type, measId id)
  {
    switch(type)
    {
      case iBeacon:
      case eddy:
        break;

      case mbcBasic:
        pdu.head = HeadS0B;
        ((measBasePtr)pdu.data)->type = type;
        break;

      case mbcPlus:
        ((measBasePtr)pdu.data)->type = type;
        pdu.head = HeadS0BS;
        break;
    }
  }

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void Beacon::start()
  {

  }

