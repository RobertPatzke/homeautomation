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

  void Beacon::init01(mbcType type, measId id)
  {
    ((measBasePtr)pdu.data)->type = type;
    ((measBasePtr)pdu.data)->id = id;

  }

  Beacon::Beacon(mbcType type, measId id)
  {
    switch(type)
    {
      case iBeacon:
      case eddy:
        break;

      case mbcBasic:
        pdu.head = HeadS0B;
        init01(type,id);
        break;

      case mbcPlus:
        pdu.head = HeadS0BS;
        init01(type,id);
        break;
    }
  }

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void Beacon::setDevAddress(BD_ADR bdAdr)
  {
    pdu.adr0 = bdAdr[5];
    pdu.adr1 = bdAdr[4];
    pdu.adr2 = bdAdr[3];
    pdu.adr3 = bdAdr[2];
    pdu.adr4 = bdAdr[1];
    pdu.adr5 = bdAdr[0];
  }

  void Beacon::setRadioInterface(IntrfRadio * inRadio)
  {
    radio = inRadio;
    inRadio->sendSync(&pdu);
  }

  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void Beacon::start()
  {

  }

