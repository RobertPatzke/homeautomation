//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "Arduino.h"
#include "nRF52840Radio.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

nRF52840Radio::nRF52840Radio()
{
  NrfRadioPtr->TASKS_DISABLE;   // Sender/Empfänger abgeschaltet
#ifdef nrfPowerDCDCEN
  *nrfPowerDCDCEN = 1;
#endif
#ifdef nrfClockTASKS_HFCLKSTART
  *nrfClockTASKS_HFCLKSTART = 1;
#endif
  NrfRadioPtr->POWER = 0;
  NrfRadioPtr->POWER = 1;

  irqCounter = 0;
  trfMode = txmBase;
  statisticPtr = NULL;
  recMode = true;
  pmPtr = (bcPduPtr) pduMem;
  psPtr = (bcPduPtr) pduSentS;
  pePtr = (bcPduPtr) pduSentE;
  eadM = false;
  nakM = false;
  comFin = false;
  newValues = false;
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------

// Allgemeine Vorbereitungen
//
void  nRF52840Radio::begin()
{
  instPtr0 = this;      // Verzweigung im statischen Bereich setzen

  NrfRadioPtr->INTENCLR = 0xFFFFFFFF;   // Vorsichtshalber keine Interrupts

  // Interruptvektor setzen
  //
  __NVIC_SetVector((IRQn_Type) 1, (dword) nRF52840Radio::irqHandler0);
  __NVIC_SetPriority((IRQn_Type) 1, 1);
  __NVIC_EnableIRQ((IRQn_Type) 1);
}

// Setzen der Zugriffsadresse
//
void  nRF52840Radio::setAccessAddress(dword addr)
{
  dword prefix = addr >> 24;
  dword base = addr << 8;

  cfgData.base0   = NrfRadioPtr->BASE0        = base;
  cfgData.prefix0 = NrfRadioPtr->PREFIX0      = prefix;
  cfgData.txAddr  = NrfRadioPtr->TXADDRESS    = 0;
  cfgData.rxAddr  = NrfRadioPtr->RXADDRESSES  = 0x01;
}

// Telegrammparameter setzen
//
void  nRF52840Radio::setPacketParms(blePduType type)
{
  switch(type)
  {
    case bptAdv:
      cfgData.pCnf0     = NrfRadioPtr->PCNF0        = PCNF0_LFLEN(8) | PCNF0_S0LEN(1) | PCNF0_S1LEN(0);
      cfgData.pCnf1     = NrfRadioPtr->PCNF1        = PCNF1_MAXLEN(42) | PCNF1_BALEN(3) | PCNF1_WHITEEN(1);
      cfgData.modeCnf0  = NrfRadioPtr->MODECNF0     = 1;
      cfgData.crcCnf    = NrfRadioPtr->CRCCNF       = CRCCNF_LEN(3) | CRCCNF_SKIPADDR(1);
      cfgData.crcPoly   = NrfRadioPtr->CRCPOLY      = PolynomCRC;
      cfgData.crcInit   = NrfRadioPtr->CRCINIT      = AdvStartCRC;
      cfgData.packetPtr = NrfRadioPtr->PACKETPTR    = (dword) pduMem;
      cfgData.mode      = NrfRadioPtr->MODE         = 3;
      cfgData.dacnf     = NrfRadioPtr->DACNF        = 0x0FF00;
      break;

    case bptAux:
      break;
  }
}


// ----------------------------------------------------------------------------
// Steuerfunktionen und gezielte Prozessorzugriffe
// ----------------------------------------------------------------------------

// Schalten des Bewerbungskanals
//
void nRF52840Radio::setChannel(int nr)
{
  cfgData.frequency = NrfRadioPtr->FREQUENCY = channelList[nr].freq;
  cfgData.whiteInit = NrfRadioPtr->DATAWHITEIV = channelList[nr].idx;
}

// ----------------------------------------------------------------------------
//                      S e n d e n
// ----------------------------------------------------------------------------

// Einstellen der Sendeleistung
//
void  nRF52840Radio::setPower(int DBm)
{
  cfgData.txPower = NrfRadioPtr->TXPOWER = DBm;
}
// Senden eines Telegramms
// Es wird davon ausgeganen, das der Radio-Zustand = DISABLED ist
//
int nRF52840Radio::sendSync(bcPduPtr inPduPtr, TxStatePtr refState)
{
  int   retv = 0;
  NrfRadioPtr->INTENCLR = 0xFFFFFFFF;
  NrfRadioPtr->EVENTS_READY = 0;
  NrfRadioPtr->EVENTS_END = 0;
  memcpy((void *)pduMem, (void *)inPduPtr, sizeof(bcPdu));  // Daten kopieren
  if(refState != NULL)
    refState->prgLoopPrep = retv = 3 + sizeof(bcPdu);
  NrfRadioPtr->TASKS_TXEN = 1;                  // Starten des Anlaufes
  while(NrfRadioPtr->EVENTS_READY != 1) retv++; // Warten bis angelaufen
  if(refState != NULL)
    refState->evtLoopRampUp = retv - 3;
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Sendevorgangs
  while(NrfRadioPtr->EVENTS_END != 1) retv++;   // Warten bis gesendet
  NrfRadioPtr->TASKS_DISABLE = 1;               // Sender abschalten
  if(refState != NULL)
  {
    refState->evtLoopTrans = retv - refState->evtLoopRampUp;
    refState->txBufferPtr = pduMem;
  }
  return(retv);
}

void  nRF52840Radio::send(bcPduPtr inPduPtr, TxMode txMode)
{
  send(inPduPtr, NULL, txMode, false);
}

void  nRF52840Radio::send(bcPduPtr inPduPtrE, bcPduPtr inPduPtrS, TxMode txMode, bool inNewValues)
{
  NrfRadioPtr->INTENCLR         = 0xFFFFFFFF;
  NrfRadioPtr->EVENTS_READY     = 0;
  NrfRadioPtr->EVENTS_END       = 0;
  NrfRadioPtr->EVENTS_DISABLED  = 0;
  NrfRadioPtr->EVENTS_RXREADY   = 0;
  NrfRadioPtr->EVENTS_TXREADY   = 0;
  NrfRadioPtr->EVENTS_ADDRESS   = 0;

  memcpy((void *)pduMem, (void *)inPduPtrE, sizeof(bcPdu));    // Daten in Funkpuffer kopieren
  memcpy((void *)pduSentE, (void *)inPduPtrE, sizeof(bcPdu));  // Daten in extra Puffer kopieren
  if(inPduPtrS != NULL)
    memcpy((void *)pduSentS, (void *)inPduPtrS, sizeof(bcPdu));// Daten in extra Puffer kopieren

  comFin = false;
  newValues = inNewValues;

  trfMode = txMode;
  statisticPtr = &statList[(int) txMode];
  statisticPtr->mode = txMode;

  switch(txMode)
  {
    case txmBase:
      NrfRadioPtr->SHORTS = NrfScREADY_START | NrfScEND_DISABLE;
      NrfRadioPtr->TASKS_TXEN = 1;
      break;

    case txmRepStart:
      NrfRadioPtr->SHORTS = NrfScREADY_START;
      NrfRadioPtr->TASKS_TXEN = 1;
      break;

    case txmRepCont:
      NrfRadioPtr->SHORTS = 0;
      NrfRadioPtr->TASKS_START = 1;
      break;

    case txmRepEnd:
      NrfRadioPtr->SHORTS = NrfScEND_DISABLE;
      NrfRadioPtr->TASKS_START = 1;
      break;

    case txmReadPrep:
      NrfRadioPtr->SHORTS = NrfScTXREADY_START | NrfScEND_DISABLE | NrfScDISABLED_RXEN;
      NrfRadioPtr->TASKS_TXEN = 1;
      break;

    case txmReadS:
      NrfRadioPtr->SHORTS = NrfScTXREADY_START | NrfScEND_DISABLE | NrfScDISABLED_RXEN | NrfScRXREADY_START;
      NrfRadioPtr->INTENSET = NrfIntADDRESS;
      NrfRadioPtr->TASKS_TXEN = 1;
      break;

    case txmRead:
      NrfRadioPtr->SHORTS = NrfScTXREADY_START | NrfScEND_DISABLE | NrfScDISABLED_RXEN | NrfScRXREADY_START;
      NrfRadioPtr->INTENSET = NrfIntADDRESS | NrfIntEND;
      NrfRadioPtr->TASKS_TXEN = 1;
      break;

    case txmResp:
    case txmRespE:
      recMode = true;
      NrfRadioPtr->SHORTS = NrfScREADY_START;
      NrfRadioPtr->INTENSET = NrfIntEND;
      NrfRadioPtr->TASKS_RXEN = 1;
      break;
  }
}

void nRF52840Radio::disable(TxMode txMode)
{
  switch(txMode)
   {
     case txmBase:
       break;

     case txmRepStart:
       break;

     case txmRepCont:
       break;

     case txmRepEnd:
       break;

     case txmReadPrep:
       break;

     case txmRead:
       NrfRadioPtr->TASKS_DISABLE = 1;
       break;

     case txmResp:
     case txmRespE:
       NrfRadioPtr->TASKS_DISABLE = 1;
       break;
   }
  NrfRadioPtr->SHORTS = 0;
}

bool nRF52840Radio::disabled(TxMode txMode)
{
  bool retv = false;

  switch(txMode)
   {
     case txmBase:
       break;

     case txmRepStart:
       break;

     case txmRepCont:
       break;

     case txmRepEnd:
       break;

     case txmReadPrep:
       break;

     case txmRead:
       if(NrfRadioPtr->EVENTS_DISABLED == 1)
       {
         NrfRadioPtr->EVENTS_DISABLED = 0;
         retv = true;
       }
       else
       {
         if(NrfRadioPtr->STATE == NrfStDISABLED)
           retv = true;
       }
       break;

     case txmResp:
     case txmRespE:
       if(NrfRadioPtr->EVENTS_DISABLED == 1)
       {
         NrfRadioPtr->EVENTS_DISABLED = 0;
         retv = true;
       }
       else
       {
         if(NrfRadioPtr->STATE == NrfStDISABLED)
           retv = true;
       }
       break;
   }
  return(retv);
}

bool nRF52840Radio::fin(TxMode txMode)
{
  bool retv = false;

  switch(txMode)
   {
     case txmBase:
       break;

     case txmRepStart:
       break;

     case txmRepCont:
       break;

     case txmRepEnd:
       break;

     case txmReadPrep:
       break;

     case txmReadS:
       retv = comFin;
       break;

     case txmRead:
       if(NrfRadioPtr->EVENTS_END == 1)
       {
         NrfRadioPtr->EVENTS_END = 0;
         retv = true;
       }
       else
       {
         if(NrfRadioPtr->STATE == NrfStRXIDLE)
           retv = true;
       }
       break;

     case txmResp:
       retv = comFin;
       break;

     case txmRespE:
       if(NrfRadioPtr->STATE == NrfStDISABLED && !recMode)
         retv = true;
       break;
   }
  return(retv);
}

void nRF52840Radio::cont(TxMode txMode)
{
  switch(txMode)
   {
     case txmBase:
       break;

     case txmRepStart:
       break;

     case txmRepCont:
       break;

     case txmRepEnd:
       break;

     case txmReadPrep:
       break;

     case txmRead:
       NrfRadioPtr->TASKS_START = 1;
       break;
   }
}

// ----------------------------------------------------------------------------
//                      E m p f a n g e n
// ----------------------------------------------------------------------------

// Starten des Datenempfangs
//
int nRF52840Radio::startRec()
{
  int   retv;
  NrfRadioPtr->INTENCLR = 0xFFFFFFFF;
  NrfRadioPtr->EVENTS_READY = 0;
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_RXEN = 1;                  // Anlauf Empfänger starten
  retv = 8;
  while(NrfRadioPtr->EVENTS_READY != 1) retv++; // Warten bis angelaufen
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Empfangs
  return(retv + 1);
}

// Fortsetzen des Datenempfangs
//
int nRF52840Radio::contRec()
{
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Empfangs
  return(6);
}

// Beenden des Datenempfangs
//
int nRF52840Radio::endRec()
{
  int   retv;
  NrfRadioPtr->EVENTS_DISABLED = 0;
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_DISABLE = 1;               // Anlauf Empfänger beenden
  retv = 7;
  while(NrfRadioPtr->EVENTS_DISABLED != 1) retv++; // Warten bis abgelaufen
  return(retv);
}

// Empfangszustand abfragen
//
int nRF52840Radio::checkRec()
{
  int retv = 0;

  if(NrfRadioPtr->EVENTS_ADDRESS != 0)
    retv |= RECSTAT_ADDRESS;

  if(NrfRadioPtr->EVENTS_PAYLOAD != 0)
    retv |= RECSTAT_PAYLOAD;

  if(NrfRadioPtr->EVENTS_END != 0)
    retv |= RECSTAT_END;

  if(NrfRadioPtr->CRCSTATUS != 0)
    retv |= RECSTAT_CRCOK;

  if(NrfRadioPtr->EVENTS_DISABLED != 0)
    retv |= RECSTAT_DISABLED;

  return(retv);
}

int   nRF52840Radio::getRecData(bcPduPtr data, int max)
{
  int retv;
  byte *bPtr = (byte *) data;

  data->head = pduMem[0];
  retv = data->len  = pduMem[1];

  for(int i = 2; i < (retv + 2); i++)
  {
    if(i == max) break;
    bPtr[i] = pduMem[i];
  }

  return(retv);
}

// ----------------------------------------------------------------------------
// Interruptverarbeitung
// ----------------------------------------------------------------------------
//

nRF52840Radio *nRF52840Radio::instPtr0 = NULL;

void nRF52840Radio::irqHandler0()
{
  if(instPtr0 == NULL) return;
  instPtr0->irqCounter++;
  instPtr0->irqHandler();
}

void nRF52840Radio::irqHandler()
{
  statisticPtr->interrupts++;

  switch(trfMode)
  {
    // ------------------------------------------------------------------------
    case txmRead:               // Empty Polling für Master
    // ------------------------------------------------------------------------

      if((NrfRadioPtr->STATE & 0x08) != 0)  // Noch im Sendemodus
      { // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_ADDRESS == 1)  // AC-Adr gesendet
        {
          NrfRadioPtr->EVENTS_ADDRESS = 0;    // nur quittieren
        }

        if(NrfRadioPtr->EVENTS_END == 1)      // Senden fertig
        {
          NrfRadioPtr->EVENTS_END = 0;        // nur quittieren
        }
      }
      else                                  // im Empfangsmodus
      { // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_ADDRESS == 1)  // AC-Adr empfangen
        {
          NrfRadioPtr->EVENTS_ADDRESS = 0;    // quittieren
          NrfRadioPtr->SHORTS = 0;            // Direktverbindungen löschen
        }

        if(NrfRadioPtr->EVENTS_END == 1)      // Empfang fertig
        {
          NrfRadioPtr->EVENTS_END = 0;        // nur quittieren
        }
      }

      break;

    // ------------------------------------------------------------------------
    case txmReadS:              // Datenübertragung für Master (Slave->Master)
    // ------------------------------------------------------------------------

      if(NrfRadioPtr->EVENTS_ADDRESS == 1)    // AC-Adr gesendet
      {
        NrfRadioPtr->EVENTS_ADDRESS = 0;      // quittieren

        if((NrfRadioPtr->STATE & 0x08) != 0)  // im Sendezustand
          break;                              // nichts weiter
        // --------------------------------------------------------------------
        else                                  // Im Empfangszustand
        {
          NrfRadioPtr->SHORTS = NrfScEND_DISABLE; // automatisch abschalten
          NrfRadioPtr->INTENCLR = 0xFFFFFFFF;     // und nur noch DISABLED
          NrfRadioPtr->INTENSET = NrfIntDISABLED; // Interrupt freischalten
        }
      }

      if(NrfRadioPtr->EVENTS_DISABLED == 1)   // Übertragung fertig
      {
        NrfRadioPtr->EVENTS_DISABLED = 0;     // quittieren
        comFin = true;
      }


      break;

    // ----------------------------------------------------------------------
    case txmRespE:              // Empty Polling für Slave
    // ----------------------------------------------------------------------

        // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_END == 1)        // Übertragung beendet
        // --------------------------------------------------------------------
        {
          NrfRadioPtr->EVENTS_END = 0;          // Event quittieren

          if(recMode)                           // im Empfangsmodus
          { // ----------------------------------------------------------------
            NrfRadioPtr->SHORTS = 0;            // keine direkte Kopplung mehr

            // ----------------------------------------------------------------
            // Reaktion
            // ----------------------------------------------------------------
            //
            if((pduSentE[5] == pduMem[5]) && (pduSentE[6] == pduMem[6]) && (pduSentE[7] == pduMem[7]))
            {
              // Die richtige Protokollumgebung (z.B. Soaap)
              //
              if(pduSentE[2] != pduMem[2])
              {
                // aber die falsche Adresse
                // Datenempfang fortsetzen
                statisticPtr->wrongs++;
                NrfRadioPtr->TASKS_START = 1;
              }
              else
              { // richtige Adresse, Antwort schicken
                // ------------------------------------------------------------
                statisticPtr->pollNaks++;

                // zunächst alle Funk-Interrupts sperren
                NrfRadioPtr->INTENCLR = 0xFFFFFFFF;

                // Interrupt freigeben für "Abgeschaltet"
                NrfRadioPtr->INTENSET = NrfIntDISABLED;

                // Empfangsbetrieb abschalten
                NrfRadioPtr->TASKS_DISABLE = 1;
              }

            }
            else
            {
              // Fremde Umgebung (nicht akzeptierte PDU)
              // Datenempfang fortsetzen
              statisticPtr->aliens++;
              NrfRadioPtr->TASKS_START = 1;
            }
          }
          else                                      // im Sendemodus
          { // ----------------------------------------------------------------


          }
        }

        // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_DISABLED == 1)       // ausgeschaltet
        // --------------------------------------------------------------------
        {
          NrfRadioPtr->EVENTS_DISABLED = 0;         // quittieren

          if(recMode)
          {
            // zunächst alle Funk-Interrupts sperren
            NrfRadioPtr->INTENCLR = 0xFFFFFFFF;

            // Interrupt freigeben für "Abgeschaltet"
            NrfRadioPtr->INTENSET = NrfIntDISABLED;

            // Kopplung automatisch starten und abschalten nach Ende
            NrfRadioPtr->SHORTS = NrfScREADY_START | NrfScEND_DISABLE;

            NrfRadioPtr->TASKS_TXEN = 1;             // Sender einschalten
            recMode = false;

            // Daten in Funkpuffer kopieren
            memcpy((void *)pduMem, (void *)pduSentE, sizeof(bcPdu));
          }
          else
          {
            NrfRadioPtr->SHORTS = 0;
            statisticPtr->sendings++;
          }
        }

        break;

      // ----------------------------------------------------------------------
      case txmResp:               // Datenübertragung Slave
      // ----------------------------------------------------------------------

        // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_END == 1)        // Übertragung beendet
        // --------------------------------------------------------------------
        {
          NrfRadioPtr->EVENTS_END = 0;          // Event quittieren

          if(recMode)                           // im Empfangsmodus (SADR/EADR)
          { // ----------------------------------------------------------------
            NrfRadioPtr->SHORTS = 0;            // keine direkte Kopplung mehr

            // ----------------------------------------------------------------
            // Reaktion
            // ----------------------------------------------------------------
            //
            if((pduSentE[5] == pduMem[5]) && (pduSentE[6] == pduMem[6]) && (pduSentE[7] == pduMem[7]))
            {
              // Die richtige Protokollumgebung (z.B. Soaap)
              //
              if(pduSentE[2] != pduMem[2])
              {
                // aber die falsche Adresse
                // Datenempfang fortsetzen
                statisticPtr->wrongs++;
                NrfRadioPtr->TASKS_START = 1;
              }
              else
              { // richtige Adresse, Antwort schicken
                // ------------------------------------------------------------
                eadM = ((pduMem[3] & SOAAP_EADR) != 0);
                nakM = ((pduMem[3] & SOAAP_NAK) != 0);

                if(nakM)
                  statisticPtr->pollNaks++;
                else
                  statisticPtr->pollAcks++;

                // zunächst alle Funk-Interrupts sperren
                NrfRadioPtr->INTENCLR = 0xFFFFFFFF;

                // Interrupt freigeben für "Abgeschaltet"
                NrfRadioPtr->INTENSET = NrfIntDISABLED;

                // Empfangsbetrieb abschalten
                NrfRadioPtr->TASKS_DISABLE = 1;
              }

            }
            else
            {
              // Fremde Umgebung (nicht akzeptierte PDU)
              // Datenempfang fortsetzen
              statisticPtr->aliens++;
              NrfRadioPtr->TASKS_START = 1;
            }
          }
          else                                      // im Sendemodus
          { // ----------------------------------------------------------------
            //
            // Das wäre ein Fehler, kein END beim Senden erwartet sondern DISABLED
          }
        }

        // --------------------------------------------------------------------
        if(NrfRadioPtr->EVENTS_DISABLED == 1)       // ausgeschaltet
        // --------------------------------------------------------------------
        {
          NrfRadioPtr->EVENTS_DISABLED = 0;         // quittieren

          if(recMode)   // *** im Empfangsmodus ***
          {
            // zunächst alle Funk-Interrupts sperren
            NrfRadioPtr->INTENCLR = 0xFFFFFFFF;

            // Interrupt freigeben für "Abgeschaltet" (nach dem Senden)
            NrfRadioPtr->INTENSET = NrfIntDISABLED;

            // Kopplung automatisch starten und abschalten nach Ende
            NrfRadioPtr->SHORTS = NrfScREADY_START | NrfScEND_DISABLE;

            // Daten in Funkpuffer kopieren
            if(eadM)
              // ToDo: Hier auch auf Empfangspolling reagieren
              memcpy((void *)pduMem, (void *)pduSentE, sizeof(bcPdu));
            else
            {
              // Sendepolling, Daten je nach Modus manipulieren
              // --------------------------------------------------------------

              if(newValues || !nakM)
              // neue Messwerte oder Polling ohne Nak-Bit
              {
                newValues = false;  // neue Messwerte quittieren
                memcpy((void *)pduMem, (void *)pduSentS, sizeof(bcPdu));
                // ansonsten ist das Telegramm in pduSentS schon das Richtige
              }
              else
              // keine Daten, nur NAK
              {
                memcpy((void *)pduMem, (void *)pduSentS, PollPduSize);
                BLE_LEN = PollAdrSize;
                BLE_ADR1 |= SOAAP_NAK;
              }
            }

            NrfRadioPtr->TASKS_TXEN = 1;            // Sender einschalten
            recMode = false;
          }
          else        // *** im Sendemodus ***
          {
            NrfRadioPtr->SHORTS = 0;
            statisticPtr->sendings++;
            comFin = true;                          // Pollvorgang abgeschlossen
          }
        }

        break;

  }
}

// --------------------------------------------------------------------------
// Datenzugriffe
// --------------------------------------------------------------------------
//
int   nRF52840Radio::getStatistics(TxStatisticsPtr dest)
{
  int retv = 0;

  dest->aliens      = statisticPtr->aliens;
  dest->interrupts  = statisticPtr->interrupts;
  dest->mode        = statisticPtr->mode;
  dest->pollAcks    = statisticPtr->pollAcks;
  dest->pollNaks    = statisticPtr->pollNaks;
  dest->sendings    = statisticPtr->sendings;
  dest->wrongs      = statisticPtr->wrongs;

  return(retv);
}

int nRF52840Radio::getState()
{
  return(NrfRadioPtr->STATE);
}

// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------
//
int   nRF52840Radio::getPduMem(byte *dest, int start, int end)
{
  int i,j;

  j = 0;

  for(i = start; i < end; i++)
  {
    dest[j++] = pduMem[i];
  }
  return(j);
}

int   nRF52840Radio::getPduSent(byte *dest, int start, int end)
{
  int i,j;

  j = 0;

  for(i = start; i < end; i++)
  {
    dest[j++] = pduSentE[i];
  }
  return(j);
}







