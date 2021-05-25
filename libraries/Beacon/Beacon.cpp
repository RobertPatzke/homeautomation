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
    radio->setPacketParms(bptAdv);
    radio->setAccessAddress(AdvAccAddr);
    //pdu.len = sizeof(bcPdu) - 2;
    // TEST
    pdu.len = sizeof(bcPdu) - 2;
    ((measBasePtr)pdu.data)->type = type;
    ((measBasePtr)pdu.data)->id = id;
    ((measBasePtr)pdu.data)->counter = 0;
  }

  Beacon::Beacon(IntrfRadio *inRadio, mbcType type, measId id)
  {
    radio = inRadio;

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

  void Beacon::setDevAddress(word head, dword body)
  {
    BD_ADR  bdAdr;

    bdAdr[0] = (octet) (head >> 8);
    bdAdr[1] = (octet) (head);

    bdAdr[2] = (octet) (body >> 24);
    bdAdr[3] = (octet) (body >> 16);
    bdAdr[4] = (octet) (body >> 8);
    bdAdr[5] = (octet) (body);

    setDevAddress(bdAdr);
  }


  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void Beacon::start()
  {

  }

  int  Beacon::send()
  {
    return(send(NULL, 0));
  }

  int Beacon::send(TxStatePtr refState, int chnr)
  {
    ((measBasePtr)pdu.data)->counter++;
    ((measBasePtr)pdu.data)->meas[0] = 0x1111;
    ((measBasePtr)pdu.data)->meas[1] = 0x2222;
    ((measBasePtr)pdu.data)->meas[2] = 0x3333;
    ((measBasePtr)pdu.data)->meas[3] = 0x4444;
    ((measBasePtr)pdu.data)->meas[4] = 0x5555;
    ((measBasePtr)pdu.data)->meas[5] = 0x6666;
    radio->setChannel(chnr);
    return(radio->sendSync(&pdu, refState));
  }










