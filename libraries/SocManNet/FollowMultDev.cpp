//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Follower for multiple devices
// Datei:   FollowMultDev.cpp
// Editor:  Igor Farber, Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "FollowMultDev.h"

// ----------------------------------------------------------------------------
// Statische Variablen, Initialisierungen
// ----------------------------------------------------------------------------
//char strDbg[100];
char * FollowMultDev::defaultObject = (char *) "TestTwitter";

// ----------------------------------------------------------------------------
// Ereignis-Funktion fuer den Empfang des Telegramms
// ----------------------------------------------------------------------------
void evtMultRecMsgDistributor(void * evtHnd, char * msg, unsigned int msgLen)
{
  FollowMultDev * instPtr;

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
  instPtr = (FollowMultDev *) evtHnd;

  instPtr->evtRecMsg(msg, msgLen);
}

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//
FollowMultDev::FollowMultDev()
{
	busyMsgAnalysis 		= false;
	cntRecMsg 				= 0;
	debugOn 				= false;
	enabled 				= false;
	floatCount 				= 0;
	intCount 				= 0;
	netHnd 					= 0;
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

  maxDeviceIdx              = 0;
}

FollowMultDev::FollowMultDev(SocManNet * inNetHnd)
{
  init(inNetHnd, defaultObject);
}

FollowMultDev::FollowMultDev(SocManNet * inNetHnd, char * commObject)
{
  init(inNetHnd, commObject);
}

void FollowMultDev::init(SocManNet * inNetHnd, char * commObject)
{
  //---------------------------------------------------------------------------
  // Globale Variablen f�r die Anwendung initialisieren
  //---------------------------------------------------------------------------
  enabled = false;

  //---------------------------------------------------------------------------
  // Variablen fuer den Inhalt des eingegangenen Telegramms initialisieren
  //---------------------------------------------------------------------------
  for (int i = 0; i < MAXNRSRC; i++)
    pduCount[i] = 0;
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
  inNetHnd->attachEvtRecMsg(commObject, this, evtMultRecMsgDistributor);

  //---------------------------------------------------------------------------
  // Initialisierung der Geräteschlüsselliste
  //---------------------------------------------------------------------------
  //
  for(int i = 0; i < MAXNRSRC; i++)
    deviceKey[i] = -1;
}

// ----------------------------------------------------------------------------
// Werte und Status der Werte lesen
// ----------------------------------------------------------------------------
//
bool FollowMultDev::getIntStatus(ReceivedValueM * intVal)
{
  int   devIdx;

  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(intCount < 1)
  {
    intVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  devIdx = intVal->deviceIdx;

  if(intVal->pduCount == pduCount[devIdx])
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
  if((pduCount[devIdx] - intVal->pduCount) > 1)
  {
    intVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  intVal->pduCount = pduCount[devIdx];
  return false;
}

bool FollowMultDev::getFloatStatus(ReceivedValueM * floatVal)
{
  int devIdx;

  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(floatCount < 1)
  {
    floatVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  devIdx = floatVal->deviceIdx;

  if(floatVal->pduCount == pduCount[devIdx])
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
  if((pduCount[devIdx] - floatVal->pduCount) > 1)
  {
    floatVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  floatVal->pduCount = pduCount[devIdx];
  return false;
}

bool FollowMultDev::getTextStatus(ReceivedValueM * textVal)
{
  int devIdx;

  //---------------------------------------------------------------------------
  // Falls Daten fuer die Statusabfrage nicht gueltig sind, dann
  //---------------------------------------------------------------------------
  if(textCount < 1)
  {
    textVal->status = STATUSVAL_BM_NONE;
    return true;
  }

  devIdx = textVal->deviceIdx;

  if(textVal->pduCount == pduCount[devIdx])
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
  if((pduCount[devIdx] - textVal->pduCount) > 1)
  {
    textVal->status |= STATUSVAL_BM_LOSTPDU;
  }

  textVal->pduCount = pduCount[devIdx];
  return false;
}

void FollowMultDev::getValue(IntegerValueMD * intVal)
{
  bool  fin;
  int   idx;
  int   devIdx;

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
  devIdx = intVal->recDsc.deviceIdx;

  int value = intArray[devIdx][idx];

  if(value != intVal->value)
  {
    intVal->value = value;
    intVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    intVal->newValue = true;
  }

  intVal->recDsc.deviceInfo.baseMode = baseMode[devIdx];
  intVal->recDsc.deviceInfo.baseState = baseState[devIdx];
  intVal->recDsc.deviceInfo.deviceKey = deviceKey[devIdx];
  intVal->recDsc.deviceInfo.deviceName = deviceName[devIdx];
  intVal->recDsc.deviceInfo.deviceState = deviceState[devIdx];
  intVal->recDsc.deviceInfo.posX = posX[devIdx];
  intVal->recDsc.deviceInfo.posY = posY[devIdx];
  intVal->recDsc.deviceInfo.posZ = posZ[devIdx];
}

void FollowMultDev::getValue(FloatValueMD * floatVal)
{
  bool  fin;
  int   idx;
  int   devIdx;

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
  devIdx = floatVal->recDsc.deviceIdx;
  float value = floatArray[devIdx][idx];

  if(value != floatVal->value)
  {
    floatVal->value = value;
    floatVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    floatVal->newValue = true;
  }

  floatVal->recDsc.deviceInfo.baseMode = baseMode[devIdx];
  floatVal->recDsc.deviceInfo.baseState = baseState[devIdx];
  floatVal->recDsc.deviceInfo.deviceKey = deviceKey[devIdx];
  floatVal->recDsc.deviceInfo.deviceName = deviceName[devIdx];
  floatVal->recDsc.deviceInfo.deviceState = deviceState[devIdx];
  floatVal->recDsc.deviceInfo.posX = posX[devIdx];
  floatVal->recDsc.deviceInfo.posY = posY[devIdx];
  floatVal->recDsc.deviceInfo.posZ = posZ[devIdx];
}

void FollowMultDev::getValue(TextValueMD * textVal)
{
  bool  fin;
  int   idx;
  int   devIdx;

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
  devIdx = textVal->recDsc.deviceIdx;
  if(strcmp(textVal->value, &textArray[devIdx][idx][0]))
  {
    strncpy(textVal->value, &textArray[devIdx][idx][0], sizeof(textVal->value));
    textVal->recDsc.status |= STATUSVAL_BM_NEWVAL;
    textVal->newValue = true;
  }

  textVal->recDsc.deviceInfo.baseMode = baseMode[devIdx];
  textVal->recDsc.deviceInfo.baseState = baseState[devIdx];
  textVal->recDsc.deviceInfo.deviceKey = deviceKey[devIdx];
  textVal->recDsc.deviceInfo.deviceName = deviceName[devIdx];
  textVal->recDsc.deviceInfo.deviceState = deviceState[devIdx];
  textVal->recDsc.deviceInfo.posX = posX[devIdx];
  textVal->recDsc.deviceInfo.posY = posY[devIdx];
  textVal->recDsc.deviceInfo.posZ = posZ[devIdx];
}


// ----------------------------------------------------------------------------
// Funktionen zur Verarbeitung des eingegangenen Telegramms
// ----------------------------------------------------------------------------
//
void FollowMultDev::evtRecMsg(char * msg, unsigned int msgLen)
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

int FollowMultDev::parseMsg(char * msg, unsigned int msgLen)
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

int FollowMultDev::parseMsg2(char * msg, unsigned int msgLen)
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
                                    // Das Telegramm fängt hier beim PDU-Counter an

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < msgLen; idx++)
  {
    if(msg[idx] == ';')
    {
      // Das Datenfeld ist beendet
      // und der Feldzähler wird auf das nächste Feld gesetzt
      cntField++;

      // Falls Felder leer sind, müssen die Merker auf -1 gesetzt werden
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

int FollowMultDev::storeDataMsg2(char * msg, unsigned int msgLen)
{
  int   msgIdx;

  int       locIntCount;
  int       locFloatCount;
  int       locTextCount;

  int       inDeviceKey;
  int       deviceIdx;
  int       intValue;
  double    floatValue;

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
  // DeviceKey
  //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceKey];
  if(msgIdx < 0)
    inDeviceKey = 0;
  else
    inDeviceKey = atoi(&msg[msgIdx]);

  // Wenn der Geräteschlüssel neu ist, wird er in die Liste eingetragen
  // und ein Index zugewiesen. Wenn er bereits in der Liste ist,
  // wird sein Index ermittelt.
  //
  for(deviceIdx = 0; deviceIdx < MAXNRSRC; deviceIdx++)
  {
    if(deviceKey[deviceIdx] < 0)
    {
      deviceKey[deviceIdx] = inDeviceKey;
      break;
    }
    if(deviceKey[deviceIdx] == inDeviceKey)
      break;
  }

  // Geräteschlüssel nicht gefunden und kein Platz mehr in der Liste
  // führt zum Verwerfen des Telegramms
  //
  if(deviceIdx == MAXNRSRC) return(-1);

  // Merker für Anwender setzen, wieviel Geräte das Objekt twittern
  //
  if(deviceIdx > maxDeviceIdx)
    maxDeviceIdx = deviceIdx;

    //-------------------------------------------------------------------------
    // PduCount
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPduCount];
  if(msgIdx < 0)
    pduCount[deviceIdx] = 0;
  else
    pduCount[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // DeviceState
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceState];
  if(msgIdx < 0)
    deviceState[deviceIdx] = 0;
  else
    deviceState[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // DeviceName
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiDeviceName];
  if(msgIdx < 0)
    strcpy(deviceName[deviceIdx],"empty");
  else
    for(int i = 0; i < DATA_OBJ_NAME_LEN; i++)
    {
      deviceName[deviceIdx][i] = msg[msgIdx+i];
      if(deviceName[deviceIdx][i] == ';')
      {
        deviceName[deviceIdx][i] = '\0';
        break;
      }
    }

    //-------------------------------------------------------------------------
    // posX
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosX];
  if(msgIdx < 0)
    posX[deviceIdx] = 0;
  else
    posX[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // posY
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosY];
  if(msgIdx < 0)
    posY[deviceIdx] = 0;
  else
    posY[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // posZ
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiPosZ];
  if(msgIdx < 0)
    posZ[deviceIdx] = 0;
  else
    posZ[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // baseState
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiBaseState];
  if(msgIdx < 0)
    baseState[deviceIdx] = 0;
  else
    baseState[deviceIdx] = atoi(&msg[msgIdx]);

    //-------------------------------------------------------------------------
    // baseMode
    //-------------------------------------------------------------------------
  msgIdx = pduDataIdxField[pdiBaseMode];
  if(msgIdx < 0)
    baseMode[deviceIdx] = 0;
  else
    baseMode[deviceIdx] = atoi(&msg[msgIdx]);

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

  // Index fuer die Position der Wert-Strings setzen
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
            intArray[deviceIdx][idxValue] = intValue;
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
            floatArray[deviceIdx][idxValue] = floatValue;
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
            textArray[deviceIdx][idxValue][idxBuf] = chr;
            idxBuf++;
          }
      }
      else
      {
          if(idxValue < MAXNRTEXT)
          {
            // String abschliessen
            textArray[deviceIdx][idxValue][idxBuf] = '\0';
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
void FollowMultDev::writeDebug(char * str)
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

unsigned int FollowMultDev::getStatistic(char * strPtr, int devIdx)
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
                   pduCount[devIdx],
                   intCount,
                   floatCount,
                   textCount
                  );

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(msgLen);
}
