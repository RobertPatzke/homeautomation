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
    radio->setPacketParms(bptAdv);
    radio->setAccessAddress(AdvAccAddr);
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
  void Scanner::setDevAddress(int sel, BD_ADR bdAdr)
  {
    /*
    pdu.adr0 = bdAdr[5];
    pdu.adr1 = bdAdr[4];
    pdu.adr2 = bdAdr[3];
    pdu.adr3 = bdAdr[2];
    pdu.adr4 = bdAdr[1];
    pdu.adr5 = bdAdr[0]; */

    // TODO
    // Hier müssen die Register für den Adressenvergleich gesetzte werden
    // Beim nRF52840 sind das: DAB[n], DAP[n] und DACNF
  }

  void Scanner::setDevAddress(int sel, word head, dword body)
  {
    BD_ADR  bdAdr;

    bdAdr[0] = (octet) (head >> 8);
    bdAdr[1] = (octet) (head);

    bdAdr[2] = (octet) (body >> 24);
    bdAdr[3] = (octet) (body >> 16);
    bdAdr[4] = (octet) (body >> 8);
    bdAdr[5] = (octet) (body);

    setDevAddress(sel, bdAdr);
  }


  // --------------------------------------------------------------------------
  // Steuerung des Scanners
  // --------------------------------------------------------------------------
  //
  void Scanner::start()
  {

  }

  void Scanner::receive(RecMode mode)
  {
    radio->advChannel(0);
    if(mode == StartRec)
      radio->startRec();
    else if(mode == ContinueRec)
      radio->contRec();
    else if(mode == EndRec)
      radio->endRec();
  }

  int Scanner::check()
  {
    int retv = radio->checkRec();
    if(retv & RECSTAT_END)
      radio->getRecData(&pdu, sizeof(bcPdu));
    return(retv);
  }

  int Scanner::getPdu(bcPdu *dest, int nr)
  {
    int   retv;
    byte *dPtr = (byte *) dest;
    byte *sPtr = (byte *) &pdu;

    dest->head = pdu.head;
    retv = dest->len  = pdu.len;

    for(int i = 2; i < retv; i++)
    {
      if(i == nr) break;
      dPtr[i] = sPtr[i];
    }

    return(retv);
  }
