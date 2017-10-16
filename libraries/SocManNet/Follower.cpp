/*
  Follower.cpp
  Lauschen  Twitter-Telegramme f�r Arduino Due
  I.Farber, 07.04.2015
*/
#include "Follower.h"

// ----------------------------------------------------------------------------
// Statische Variablen, Initialisierungen
// ----------------------------------------------------------------------------
char strDbg[100];
char * Follower::defaultObject = (char *) "TestTwitter";

// ----------------------------------------------------------------------------
// Ereignis-Funktion fuer den Empfang des Telegramms
// ----------------------------------------------------------------------------
void evtRecMsgDistributor(void * evtHnd, char * msg, unsigned int msgLen)
{
  Follower * instPtr;

  //---------------------------------------------------------------------------
  // Eingabeparameter ueberpruefen
  //---------------------------------------------------------------------------
  if(evtHnd == NULL)
  {
    return;
  }

  if(msgLen == 0)
  {
    return;
  }

  //---------------------------------------------------------------------------
  // Ereignis weiterleiten
  //---------------------------------------------------------------------------
  instPtr = (Follower *) evtHnd;

  instPtr->evtRecMsg(msg, msgLen);
}

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//
Follower::Follower()
{
	busyMsgAnalysis 		= false;
	cntRecMsg 				= 0;
	debugOn 				= false;
	enabled 				= false;
	floatCount 				= 0;
	intCount 				= 0;
	netHnd 					= 0;
	pduCount 				= 0;
	recParseCounter 		= 0;
	textCount 				= 0;

  idxFieldPduCount          = 0;
  idxFieldDeviceKey         = 0;
  idxFieldDeviceState       = 0;
  idxFieldDeviceName        = 0;
  idxFieldDeviceTime        = 0;
  idxFieldDevicePosX        = 0;
  idxFieldDevicePosY        = 0;
  idxFieldDevicePosZ        = 0;
  idxFieldDeviceAppState    = 0;
  idxFieldDeviceAppMode     = 0;

  idxFieldIntCount          = 0;
  idxFieldFloatCount        = 0;
  idxFieldTextCount         = 0;
  idxFieldValue             = 0;

  baseMode                  = 0;
  baseState                 = 0;
  deviceKey                 = 0;
  deviceState               = 0;
  posX                      = 0;
  posY                      = 0;
  posZ                      = 0;
}

Follower::Follower(SocManNet * inNetHnd)
{
  init(inNetHnd, defaultObject);
}

Follower::Follower(SocManNet * inNetHnd, char * commObject)
{
  init(inNetHnd, commObject);
}

void Follower::init(SocManNet * inNetHnd, char * commObject)
{
  //---------------------------------------------------------------------------
  // Globale Variablen f�r die Anwendung initialisieren
  //---------------------------------------------------------------------------
  enabled = false;

  //---------------------------------------------------------------------------
  // Variablen fuer den Inhalt des eingegangenen Telegramms initialisieren
  //---------------------------------------------------------------------------
  pduCount = 0;
  intCount = 0;
  floatCount = 0;
  textCount = 0;

  memset(intArray, 0, sizeof(intArray));
  memset(floatArray, 0, sizeof(floatArray));
  memset(textArray, 0, sizeof(textArray));

  //---------------------------------------------------------------------------
  //Variablen fuer die Verarbeitung des eingegangenen Telegramms initialisieren
  //---------------------------------------------------------------------------
  idxFieldPduCount = 0;
  idxFieldIntCount = 0;
  idxFieldFloatCount = 0;
  idxFieldTextCount = 0;
  idxFieldValue = 0;

  busyMsgAnalysis = false;

  //---------------------------------------------------------------------------
  // Globale Variablen fuer Debugzwecke initialisieren
  //---------------------------------------------------------------------------
  debugOn = false;

  //---------------------------------------------------------------------------
  // Lokale Variablen fuer die Kommunikationsschnittstelle initialisieren
  //---------------------------------------------------------------------------
  netHnd = inNetHnd;

  //---------------------------------------------------------------------------
  // Interne Variablen fuer Statistik
  //---------------------------------------------------------------------------
  cntRecMsg = 0;

  //---------------------------------------------------------------------------
  // Die Verbindung mit der Kommunikationsschnittstelle erstellen
  //---------------------------------------------------------------------------
  inNetHnd->attachEvtRecMsg(commObject, this, evtRecMsgDistributor);
}

// ----------------------------------------------------------------------------
// Werte und Status der Werte lesen
// ----------------------------------------------------------------------------
//
bool Follower::getIntStatus(ReceivedValue * intVal)
{
  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(intCount < 1)
  {
    intVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  if(intVal->pduCount == pduCount)
  {
    intVal->status = STATUSVAL_BM_UNBORN;
    return true;
  }

  if(intVal->idx < 0 || intVal->idx >= MAXNRINT)
  {
    intVal->status = STATUSVAL_BM_NEWPDU;
    intVal->status |= STATUSVAL_BM_IDX;
    return true;
  }

  //---------------------------------------------------------------------------
  // Status bestimmen und ausgeben
  //---------------------------------------------------------------------------
  intVal->status = STATUSVAL_BM_NEWPDU;
  if((pduCount - intVal->pduCount) > 1)
  {
    intVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  intVal->pduCount = pduCount;
  return false;
}

bool Follower::getFloatStatus(ReceivedValue * floatVal)
{
  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(floatCount < 1)
  {
    floatVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  if(floatVal->pduCount == pduCount)
  {
    floatVal->status = STATUSVAL_BM_UNBORN;
    return true;
  }

  if(floatVal->idx < 0 || floatVal->idx >= MAXNRFLOAT)
  {
    floatVal->status = STATUSVAL_BM_NEWPDU;
    floatVal->status |= STATUSVAL_BM_IDX;
    return true;
  }

  //---------------------------------------------------------------------------
  // Status bestimmen und ausgeben
  //---------------------------------------------------------------------------
  floatVal->status = STATUSVAL_BM_NEWPDU;
  if((pduCount - floatVal->pduCount) > 1)
  {
    floatVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  floatVal->pduCount = pduCount;
  return false;
}

bool Follower::getTextStatus(ReceivedValue * textVal)
{
  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(textCount < 1)
  {
    textVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  if(textVal->pduCount == pduCount)
  {
    textVal->status = STATUSVAL_BM_UNBORN;
    return true;
  }

  if(textVal->idx < 0 || textVal->idx >= MAXNRTEXT)
  {
    textVal->status = STATUSVAL_BM_NEWPDU;
    textVal->status |= STATUSVAL_BM_IDX;

    return true;
  }

  //---------------------------------------------------------------------------
  // Status bestimmen und ausgeben
  //---------------------------------------------------------------------------
  textVal->status = STATUSVAL_BM_NEWPDU;
  if((pduCount - textVal->pduCount) > 1)
  {
    textVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  textVal->pduCount = pduCount;
  return false;
}

void Follower::getValue(IntegerValue * intVal)
{
  bool  fin;
  int   idx;

  intVal->newPdu = false;
  intVal->newValue = false;

  //---------------------------------------------------------------------------
  // Pruefen, ob es moeglich ist Wert zu lesen
  //---------------------------------------------------------------------------
  fin = getIntStatus(&(intVal->recDsc));
  if(fin) return;

  intVal->newPdu = true;

  //---------------------------------------------------------------------------
  // Wert lesen
  //---------------------------------------------------------------------------
  idx = intVal->recDsc.idx;
  int value = intArray[idx];

  if(value != intVal->value)
  {
    intVal->value = value;
    intVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    intVal->newValue = true;
  }
}

void Follower::getValue(FloatValue * floatVal)
{
  bool  fin;
  int   idx;

  floatVal->newPdu = false;
  floatVal->newValue = false;

  //---------------------------------------------------------------------------
  // Pruefen, ob es moeglich ist Wert zu lesen
  //---------------------------------------------------------------------------
  fin = getFloatStatus(&(floatVal->recDsc));
  if(fin) return;

  floatVal->newPdu = true;

  //---------------------------------------------------------------------------
  // Wert lesen
  //---------------------------------------------------------------------------
  idx = floatVal->recDsc.idx;

  float value = floatArray[idx];

  if(value != floatVal->value)
  {
    floatVal->value = value;
    floatVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    floatVal->newValue = true;
  }
}

void Follower::getValue(TextValue * textVal)
{
  bool  fin;
  int   idx;

  textVal->newPdu   = false;
  textVal->newValue = false;

  //---------------------------------------------------------------------------
  // Pruefen, ob es moeglich ist Wert zu lesen
  //---------------------------------------------------------------------------
  fin = getTextStatus(&(textVal->recDsc));
  if(fin) return;

  textVal->newPdu = true;

  //---------------------------------------------------------------------------
  // Wert lesen
  //---------------------------------------------------------------------------
  idx = textVal->recDsc.idx;

  if(strcmp(textVal->value, &textArray[idx][0]))
  {
    strncpy(textVal->value, &textArray[idx][0], sizeof(textVal->value));
    textVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    textVal->newValue = true;
  }
}

// ----------------------------------------------------------------------------
// Funktionen zur Verarbeitung des eingegangenen Telegramms
// ----------------------------------------------------------------------------
//
void Follower::evtRecMsg(char * msg, unsigned int msgLen)
{
  int chkv;

  //---------------------------------------------------------------------------
  // Pruefen, ob die Auswertung des Telegramms freigeben ist
  //---------------------------------------------------------------------------
  if(busyMsgAnalysis == true)
  {
    return;
  }

  //---------------------------------------------------------------------------
  // Auswertung des Telegramms sperren
  //---------------------------------------------------------------------------
  busyMsgAnalysis = true;

  //---------------------------------------------------------------------------
  // Telegramm validieren
  //---------------------------------------------------------------------------
  if(msgLen < 3)
  {
    busyMsgAnalysis = false;
    return;
  }

  // Telegrammende ueberpruefen
  if(msg[msgLen-2] != ':')
  {
    busyMsgAnalysis = false;
    return;
  }

  //---------------------------------------------------------------------------
  // Statistik aktualisieren
  //---------------------------------------------------------------------------
  cntRecMsg++;

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  chkv = parseMsg2(msg, msgLen);
  if(chkv)
  {
    busyMsgAnalysis = false;
    return;
  }

  //---------------------------------------------------------------------------
  // Inhalt des Telegramms speichern
  //---------------------------------------------------------------------------
  storeDataMsg2(msg, msgLen);

  //---------------------------------------------------------------------------
  // Auswertung des Telegramms freigeben
  //---------------------------------------------------------------------------
  busyMsgAnalysis = false;
}

int Follower::parseMsg(char * msg, unsigned int msgLen)
{
  unsigned int cntField;
  unsigned int idx;
  PARSER_MSG_STATE parserState;
  bool parserReady;
  bool parserError;

  recParseCounter++;

  //---------------------------------------------------------------------------
  // Parser initialisieren
  //---------------------------------------------------------------------------
  // Ergebnisdaten zuruecksetzen
  idxFieldPduCount        = 0;
  idxFieldDeviceKey       = 0;
  idxFieldDeviceState     = 0;
  idxFieldDeviceName      = 0;
  idxFieldDeviceTime      = 0;
  idxFieldDevicePosX      = 0;
  idxFieldDevicePosY      = 0;
  idxFieldDevicePosZ      = 0;
  idxFieldDeviceAppState  = 0;
  idxFieldDeviceAppMode   = 0;

  idxFieldIntCount 		    = 0;
  idxFieldFloatCount 	    = 0;
  idxFieldTextCount       = 0;
  idxFieldValue           = 0;

  // Zustandsmaschine zuruecksetzen
  cntField    = 0;
  parserState = p_msg_st_FieldPduCount;
  parserReady = false;
  parserError = false;

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < msgLen; idx++)
  {
    //-------------------------------------------------------------------------
    // Je nach Zustand verzweigen
    //-------------------------------------------------------------------------
    switch(parserState)
    {
      // ------------------------------------------------------------------- //
      case p_msg_st_waitFieldSeparator:
      // ------------------------------------------------------------------- //
        if(msg[idx] == ';')
        {
          cntField++;


          if(cntField == 10)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_FieldNumInt;

            break;
          }
          else if(cntField == 11)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_FieldNumFloat;

            break;
          }
          else if(cntField == 12)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_FieldNumText;

            break;
          }
          else if(cntField == 13)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_FieldValue;

            break;
          }

          // Sonst im Zustand bleiben
        }

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_FieldPduCount:
      // ------------------------------------------------------------------- //
        idxFieldPduCount = idx;

        // Naechsten Zustand setzen
        parserState = p_msg_st_waitFieldSeparator;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_FieldNumInt:
      // ------------------------------------------------------------------- //
        idxFieldIntCount = idx;

        // Naechsten Zustand setzen
        parserState = p_msg_st_waitFieldSeparator;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_FieldNumFloat:
      // ------------------------------------------------------------------- //
        idxFieldFloatCount = idx;

        // Naechsten Zustand setzen
        parserState = p_msg_st_waitFieldSeparator;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_FieldNumText:
      // ------------------------------------------------------------------- //
        idxFieldTextCount = idx;

        // Naechsten Zustand setzen
        parserState = p_msg_st_waitFieldSeparator;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_FieldValue:
      // ------------------------------------------------------------------- //
        idxFieldValue = idx;

        // Merker 'Parser ist fertig' setzen
        parserReady = true;

        // Naechsten Zustand setzen
        parserState = p_msg_st_Ready;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_Ready:
      // ------------------------------------------------------------------- //
        // Im  Zustand bleiben

      break;

      // ------------------------------------------------------------------- //
      default:
      // ------------------------------------------------------------------- //
        parserError = true;

        break;
    }

    //-------------------------------------------------------------------------
    // Eventuell das Parsen beenden
    //-------------------------------------------------------------------------
    if(parserReady == true)
    {
      break;
    }

    if(parserError == true)
    {
      break;
    }
  }

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  if(parserReady != true)
  {
    return(-1);
  }

  /*
  writeDebug("->");
  writeDebug(&msg[idxFieldPduCount]);
  writeDebug(&msg[idxFieldIntCount]);
  writeDebug(&msg[idxFieldFloatCount]);
  writeDebug(&msg[idxFieldTextCount]);
  writeDebug(&msg[idxFieldValue]);
  writeDebug("<-");
  */

  return(0);
}

int Follower::parseMsg2(char * msg, unsigned int msgLen)
{
  unsigned int cntField;
  unsigned int idx;

  recParseCounter++;

  //---------------------------------------------------------------------------
  // Parser initialisieren
  //---------------------------------------------------------------------------
  // Ergebnisdaten zuruecksetzen

  // Merker zuruecksetzen
  cntField    = 0;

  pduDataIdxField[pdiPduCount] = 0; // Der Msg-Index des PDU-Counter ist 0
                                    // Das Telegramm f�ngt hier beim PDU-Counter an

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < msgLen; idx++)
  {
    if(msg[idx] == ';')
    {
      // Das Datenfeld ist beendet
      // und der Feldz�hler wird auf das n�chste Feld gesetzt
      cntField++;

      // Falls Felder leer sind, m�ssen die Merker auf -1 gesetzt werden
      if(msg[idx+1] == ';')
        pduDataIdxField[cntField] = -1;
      else
        pduDataIdxField[cntField] = idx + 1;
    }

    if(cntField >= pdiValueList)
    {
      return(0);
    }
  }

  return(-1);
}

int Follower::storeDataMsg(char * msg, unsigned int msgLen)
{
  int locIntCount;
  int locFloatCount;
  int locTextCount;

  int     intValue;
  double  floatValue;

  unsigned int idxValue;
  unsigned int idxValueStr;
  unsigned int idxValueChar;

  char          valueBuf[MAXVALCHRLEN];
  unsigned int  idxBuf;
  char chr;

  //bool storeReady;

  //---------------------------------------------------------------------------
  // Parser Ergebnis ueberpruefen
  //---------------------------------------------------------------------------
  if(idxFieldPduCount < 0 || idxFieldPduCount >= msgLen)
  {
    return(-1);
  }

  if(idxFieldIntCount < 1 || idxFieldIntCount >= msgLen)
  {
    return(-1);
  }

  if(idxFieldFloatCount < 1 || idxFieldFloatCount >= msgLen)
  {
    return(-1);
  }

  if(idxFieldTextCount < 1 || idxFieldTextCount >= msgLen)
  {
    return(-1);
  }

  if(idxFieldValue < 1 || idxFieldValue >= msgLen)
  {
    return(-1);
  }

  //---------------------------------------------------------------------------
  // Einzelne Datenelemente speichern
  //---------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // PduCount
    //-------------------------------------------------------------------------
  pduCount = atoi(&msg[idxFieldPduCount]);

    //-------------------------------------------------------------------------
    // intCount, floatCount, textCount erstmal lokal speichern.
    // Erst mal werden alle Werte gespeichert,
    // dann werden Zaehler uebernommen
    //-------------------------------------------------------------------------
  intCount    = locIntCount   = atoi(&msg[idxFieldIntCount]);
  floatCount  = locFloatCount = atoi(&msg[idxFieldFloatCount]);
  textCount   = locTextCount  = atoi(&msg[idxFieldTextCount]);

    //-------------------------------------------------------------------------
    // Werte intArray, floatArray, textArray
    //-------------------------------------------------------------------------
      //-----------------------------------------------------------------------
      // Speichern initialisieren
      //-----------------------------------------------------------------------
  // Index fuer die Position von der Wert-Strings setzen
  idxValueStr = idxFieldValue;

      //-----------------------------------------------------------------------
      // intArray
      //-----------------------------------------------------------------------
  if(locIntCount != 0)
  {  
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Einzelne Zeichen auswerten
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      /*
      switch(chr)
      {
        
        // ----------------------------------------------------------------- //
        case ',':
        // ----------------------------------------------------------------- //
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          intValue = atoi(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRINT)
          {
            intArray[idxValue] = intValue;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          break;
          

        // ----------------------------------------------------------------- //
        case ';':
        case ':':
        // ----------------------------------------------------------------- //
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          intValue = atoi(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRINT)
          {
            intArray[idxValue] = intValue;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          // Merker 'Speichern ist fertig' setzen
          storeReady = true;

          break;

        // ----------------------------------------------------------------- //
        default:
        // ----------------------------------------------------------------- //
          if(idxBuf <= sizeof(idxBuf) - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }
      

      //-----------------------------------------------------------------------
      // Pruefen, ob das Speichern fertig  ist
      //-----------------------------------------------------------------------
      if(storeReady == true)
      {
        // Anzahl der Werte uebernehmen
        // Es wird nicht locIntCount uebernommen,
        // weil es moeglich ist nur eine begrenzte Anzahl der Werte
        // zu uebernehmen.
        // locIntCount - entspricht der Anzahl empfangene Werte
        // idxValue - entspricht der Anzahl uebernommene Werte
        intCount = idxValue;

        // Index fuer die Position weitere Wert-Strings aktualisieren
        idxValueStr = idxValueChar + 1;

        break;
      }
      */

      if(chr != ';' && chr != ':')
      {
          if(idxBuf <= MAXVALCHRLEN - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          intValue = atoi(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRINT)
          {
            intArray[idxValue] = intValue;
            idxValue++;
          }

          // Das Speichern weiterer Werte initialisieren
          idxBuf = 0;
      
          locIntCount--;
          if(locIntCount == 0)
            break;
      }

    } // for

    // idxValueChar zeigt jetzt auf ;
    idxValueStr = idxValueChar + 1;   // idxValueStr auf n�chstes Wertfeld

  } // if(locIntCount > 0)

      //-----------------------------------------------------------------------
      // floatArray
      //-----------------------------------------------------------------------
  if(locFloatCount != 0)
  {
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Zeichen auswerten
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      /*
      switch(chr)
      {
        // ----------------------------------------------------------------- //
        case ',':
        // ----------------------------------------------------------------- //
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          floatValue = atof(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRFLOAT)
          {
            floatArray[idxValue] = floatValue;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          break;

        // ----------------------------------------------------------------- //
        case ';':
        case ':':
        // ----------------------------------------------------------------- //
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          floatValue = atof(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRFLOAT)
          {
            floatArray[idxValue] = floatValue;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          // Merker 'Speichern ist fertig' setzen
          storeReady = true;

          break;

        // ----------------------------------------------------------------- //
        default:
        // ----------------------------------------------------------------- //
          if(idxBuf <= sizeof(idxBuf) - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }

      //-----------------------------------------------------------------------
      // Pruefen, ob das Speichern fertig  ist
      //-----------------------------------------------------------------------
      if(storeReady == true)
      {
        // Anzahl der Werte uebernehmen
        // Es wird nicht locFloatCount uebernommen,
        // weil es moeglich ist nur eine begrenzte Anzahl der Werte
        // zu uebernehmen.
        // locFloatCount - entspricht der Anzahl empfangene Werte
        // idxValue - entspricht der Anzahl uebernommene Werte
        floatCount = idxValue;

        // Index fuer die Position weitere Wert-Strings aktualisieren
        idxValueStr = idxValueChar + 1;

        break;
      }
      */

      if(chr != ';' && chr != ':')
      {
          if(idxBuf <= MAXVALCHRLEN - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->FLOAT konvertieren
          floatValue = atof(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRFLOAT)
          {
            floatArray[idxValue] = floatValue;
            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          locFloatCount--;
          if(locFloatCount == 0)
            break;
      }

    } // for
        
    // idxValueChar zeigt jetzt auf ;
    idxValueStr = idxValueChar + 1;   // idxValueStr auf n�chstes Wertfeld
  }

      //-----------------------------------------------------------------------
      // textArray
      //-----------------------------------------------------------------------
  if(locTextCount != 0)
  {
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Je nach Zeichen verzweigen
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      /*
      switch(chr)
      {
        // ----------------------------------------------------------------- //
        case ',':
        // ----------------------------------------------------------------- //
          // Speichern des Sring-Wertes abschliessen
          if(idxValue < MAXNRTEXT)
          {
            textArray[idxValue][idxBuf] = 0;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          break;

        // ----------------------------------------------------------------- //
        case ';':
        case ':':
        // ----------------------------------------------------------------- //
          // Speichern des Sring-Wertes abschliessen
          if(idxValue < MAXNRTEXT)
          {
            textArray[idxValue][idxBuf] = 0;

            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          // Merker 'Speichern ist fertig' setzen
          storeReady = true;

          break;

        // ----------------------------------------------------------------- //
        default:
        // ----------------------------------------------------------------- //
          if(idxValue < MAXNRTEXT && idxBuf < (TEXTVAL_LEN_MAX - 2))
          {
            textArray[idxValue][idxBuf] = chr;

            idxBuf++;
          }
      }

      //-----------------------------------------------------------------------
      // Pruefen, ob das Speichern fertig  ist
      //-----------------------------------------------------------------------
      if(storeReady == true)
      {
        // Anzahl der Werte uebernehmen
        // Es wird nicht locTextCount uebernommen,
        // weil es moeglich ist nur eine begrenzte Anzahl der Werte
        // zu uebernehmen.
        // locTextCount - entspricht der Anzahl empfangene Werte
        // idxValue - entspricht der Anzahl uebernommene Werte
        textCount = idxValue;

        // Index fuer die Position weitere Wert-Strings aktualisieren
        idxValueStr = idxValueChar + 1;

        break;
      }
      */
            
      if(chr != ';' && chr != ':')
      {
          if(idxValue < MAXNRTEXT && idxBuf < (TEXTVAL_LEN_MAX - 2))
          {
            textArray[idxValue][idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          if(idxValue < MAXNRTEXT)
          {
            // String abschliessen
            textArray[idxValue][idxBuf] = 0;
            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;
      
          locTextCount--;
          if(locTextCount == 0)
            break;
      }

    } // for

  }
  return(0);
}

int Follower::storeDataMsg2(char * msg, unsigned int msgLen)
{
  int   msgIdx;

  int   locIntCount;
  int   locFloatCount;
  int   locTextCount;

  int     intValue;
  double  floatValue;

  unsigned int idxValue;
  unsigned int idxValueStr;
  unsigned int idxValueChar;

  char          valueBuf[MAXVALCHRLEN];
  unsigned int  idxBuf;
  char chr;

  //---------------------------------------------------------------------------
  // Einzelne Datenelemente speichern
  //---------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // PduCount
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPduCount];
  if(msgIdx < 0)
    pduCount = 0;
  else
    pduCount = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // DeviceKey
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceKey];
  if(msgIdx < 0)
    deviceKey = 0;
  else
    deviceKey = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // DeviceState
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceState];
  if(msgIdx < 0)
    deviceState = 0;
  else
    deviceState = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // DeviceName
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceName];
  if(msgIdx < 0)
    strcpy(deviceName,"empty");
  else
    for(int i = 0; i < DATA_OBJ_NAME_LEN; i++)
    {
      deviceName[i] = msg[msgIdx+i];
      if(deviceName[i] == ';')
      {
        deviceName[i] = '\0';
        break;
      }
    }

    //-------------------------------------------------------------------------
    // posX
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosX];
  if(msgIdx < 0)
    posX = 0;
  else
    posX = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // posY
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosY];
  if(msgIdx < 0)
    posY = 0;
  else
    posY = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // posZ
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosZ];
  if(msgIdx < 0)
    posZ = 0;
  else
    posZ = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // baseState
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiBaseState];
  if(msgIdx < 0)
    baseState = 0;
  else
    baseState = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // baseMode
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiBaseMode];
  if(msgIdx < 0)
    baseMode = 0;
  else
    baseMode = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // intCount, floatCount, textCount erstmal lokal speichern.
    // Erst mal werden alle Werte gespeichert,
    // dann werden Zaehler uebernommen
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiIntCount];
  intCount    = locIntCount   = atoi(&msg[msgIdx]);

  msgIdx = pduDataIdxField[pdiFloatCount];
  floatCount  = locFloatCount = atoi(&msg[msgIdx]);

  msgIdx = pduDataIdxField[pdiTextCount];
  textCount   = locTextCount  = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // Werte intArray, floatArray, textArray
    //-------------------------------------------------------------------------
      //-----------------------------------------------------------------------
      // Speichern initialisieren
      //-----------------------------------------------------------------------
  // Index fuer die Position von der Wert-Strings setzen
  idxValueStr = pduDataIdxField[pdiValueList];

      //-----------------------------------------------------------------------
      // intArray
      //-----------------------------------------------------------------------
  if(locIntCount != 0)
  {
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Einzelne Zeichen auswerten
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      if(chr != ';' && chr != ':')
      {
          if(idxBuf <= MAXVALCHRLEN - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->INT konvertieren
          intValue = atoi(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRINT)
          {
            intArray[idxValue] = intValue;
            idxValue++;
          }

          // Das Speichern weiterer Werte initialisieren
          idxBuf = 0;

          locIntCount--;
          if(locIntCount == 0)
            break;
      }

    } // for

    // idxValueChar zeigt jetzt auf ;
    idxValueStr = idxValueChar + 1;   // idxValueStr auf n�chstes Wertfeld

  } // if(locIntCount > 0)

      //-----------------------------------------------------------------------
      // floatArray
      //-----------------------------------------------------------------------
  if(locFloatCount != 0)
  {
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Zeichen auswerten
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      if(chr != ';' && chr != ':')
      {
          if(idxBuf <= MAXVALCHRLEN - 2)
          {
            valueBuf[idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          // String abschliessen
          valueBuf[idxBuf] = 0;

          // Wert ASCII->FLOAT konvertieren
          floatValue = atof(valueBuf);

          // Wert speichern
          if(idxValue < MAXNRFLOAT)
          {
            floatArray[idxValue] = floatValue;
            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          locFloatCount--;
          if(locFloatCount == 0)
            break;
      }

    } // for

    // idxValueChar zeigt jetzt auf ;
    idxValueStr = idxValueChar + 1;   // idxValueStr auf n�chstes Wertfeld
  }

      //-----------------------------------------------------------------------
      // textArray
      //-----------------------------------------------------------------------
  if(locTextCount != 0)
  {
    idxValue = 0;
    idxBuf = 0;
    //storeReady = false;

    for(idxValueChar = idxValueStr; idxValueChar < msgLen; idxValueChar++)
    {
      //-----------------------------------------------------------------------
      // Je nach Zeichen verzweigen
      //-----------------------------------------------------------------------
      chr = msg[idxValueChar];

      if(chr != ';' && chr != ':')
      {
          if(idxValue < MAXNRTEXT && idxBuf < (TEXTVAL_LEN_MAX - 2))
          {
            textArray[idxValue][idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          if(idxValue < MAXNRTEXT)
          {
            // String abschliessen
            textArray[idxValue][idxBuf] = 0;
            idxValue++;
          }

          // Das Speichern weitere Werte initialisieren
          idxBuf = 0;

          locTextCount--;
          if(locTextCount == 0)
            break;
      }

    } // for

  }
  return(0);
}

// ----------------------------------------------------------------------------
// Debug-Funktionen
// ----------------------------------------------------------------------------
//
void Follower::writeDebug(char * str)
{
  //---------------------------------------------------------------------------
  // Pruefen, ob Debug-Schnittstelle bedient werden muss
  //---------------------------------------------------------------------------
  if(debugOn != true)
  {
    return;
  }

  //---------------------------------------------------------------------------
  // Eingabeparameter ueberpruefen
  //---------------------------------------------------------------------------
  if(str == NULL)
  {
    return;
  }

  //---------------------------------------------------------------------------
  // An Debug-Schnittstelle Nachricht weiterleiten
  //---------------------------------------------------------------------------
#ifdef smnSloeber
  Serial.println(str);
#endif

#ifdef smnSimLinux
  printf("%s\n",str);
#endif
}

unsigned int Follower::getStatistic(char * strPtr)
{
  unsigned int msgLen;

  //---------------------------------------------------------------------------
  // Eingabeparameter ueberpruefen
  //---------------------------------------------------------------------------
  if(strPtr == NULL)
  {
    return(0);
  }

  //---------------------------------------------------------------------------
  // Statistik ausgeben
  //---------------------------------------------------------------------------
  msgLen = sprintf(strPtr, "Follower: CntRecMsg=%d PduCount=%d IntCount=%d FloatCount=%d TextCount=%d\r\n",
                   cntRecMsg,
                   pduCount,
                   intCount,
                   floatCount,
                   textCount
                  );

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(msgLen);
}
