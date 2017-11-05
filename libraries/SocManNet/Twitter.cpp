/*
  Twitter.cpp
  Zwitscherer f�r Arduino Due
  Robert Patzke, 08. M�rz 2015
*/

#include 	<string.h>
#include	"Arduino.h"
#include 	"Twitter.h"
//#include 	"rtc_clock.h"																			//RTC

// ----------------------------------------------------------------------------
// Konstruktoren und Initialisierungen
// ----------------------------------------------------------------------------
//

Twitter::Twitter()
{
	baseMode 		= 0;
	baseState 		= 0;
	cntSendMsg 		= 0;
	crPDUStatus 	= cpsHeader;
	debugOn 		= false;
	delayCounter 	= 0;
	deviceKey 		= 0;
	deviceState 	= 0;
	enabled 		= false;
	errorCode 		= 0;
	errorMsg 		= (char *) "noError";
	netHnd 			= 0;
	nrFloatValues   = 0;
	nrIntValues 	= 0;
	nrTextValues 	= 0;
	posX 			= 0;
	posY 			= 0;
	posZ 			= 0;
	resultMsg 		= (char *) "noMsg";
	runError        = rsInit;
	runStatus       = rsInit;
	speed           = normalSpeed;
	speedCounter    = 0;
	refDateTimePdu  = "2017-10-09T18:00:00.000+00:00";
	pduCounter      = 0;
	pduIdx          = 0;
	loopIdx         = 0;
	speedLimit      = 0;
	overflow        = false;
	markOverflow    = false;
	overflowCounter = 0;
}


Twitter::Twitter(SocManNet *    inNetHnd,
                 const char *   ptrDateTimePdu,
                 char *         commObject,
                 int            nrIntVal,
                 int            nrFloatVal,
                 int            nrTextVal,
                 Speed          inSpeed)
{
  init(inNetHnd, ptrDateTimePdu, commObject,
       nrIntVal, nrFloatVal, nrTextVal, inSpeed);
}

Twitter::Twitter(SocManNet * inNetHnd, char *commObject)
{
  init(inNetHnd, refDateTimePdu,
       commObject, 4, 4, 4, normalSpeed);
}

void Twitter::init(SocManNet *  inNetHnd,
                   const char * ptrDateTimePdu,
                   char *       commObject,
                   int          nrInt,
                   int          nrFloat,
                   int          nrText,
                   Speed        inSpeed)
{
  //---------------------------------------------------------------------------
  // Globale Variablen zur Steuerung initialisieren
  //---------------------------------------------------------------------------
  pduCounter = 0;

  errorCode = 0;
  //errorMsg = "";
  //resultMsg = "";

  enabled = false;
  speed = inSpeed;

  posX = 0;
  posY = 0;
  posZ = 0;
  baseState = 0;
  baseMode = 0;

  memset(intValArray, 0, sizeof(intValArray));
  memset(floatValArray, 0, sizeof(floatValArray));
  memset(textValArray, 0, sizeof(textValArray));

  //---------------------------------------------------------------------------
  // Globale Variablen fuer Debugzwecke
  //---------------------------------------------------------------------------
  debugOn = false;

  // --------------------------------------------------------------------------
  // Lokale Variablen zur Steuerung
  // --------------------------------------------------------------------------
  if(nrInt >= MAXNRINT)
  {
    nrIntValues = MAXNRINT;
  }
  else
  {
    nrIntValues = nrInt;
  }

  if(nrFloat >= MAXNRFLOAT)
  {
    nrFloatValues = MAXNRFLOAT;
  }
  else
  {
    nrFloatValues = nrFloat;
  }

  if(nrText >= MAXNRTEXT)
  {
    nrTextValues = MAXNRTEXT;
  }
  else
  {
    nrTextValues = nrText;
  }

  strncpy(objectName, commObject, sizeof(objectName) - 2);

  memset(msgHeader, 0, sizeof(msgHeader));
  memset(pduHeader, 0, sizeof(pduHeader));
  memset(pduTime, 0, sizeof(pduTime));
  memset(pduMsg, 0, sizeof(pduMsg));

  delayCounter  = 0;
  speedCounter  = 0;
  runStatus     = rsInit;
  runError      = rsNrOfStates;

  crPDUStatus = cpsHeader;

  // --------------------------------------------------------------------------
  // lokale Variablen fuer die Kommunikationsschnittstelle
  // --------------------------------------------------------------------------
  netHnd = inNetHnd;
  refDateTimePdu = ptrDateTimePdu;

  // --------------------------------------------------------------------------
  // Interne Variablen fuer Statistik
  // --------------------------------------------------------------------------
  cntSendMsg = 0;

  //---------------------------------------------------------------------------
  // Telegramm-Generator initialisieren
  //---------------------------------------------------------------------------
  createMsgHeader();
}


// ----------------------------------------------------------------------------
// Bedienfunktionen
// ----------------------------------------------------------------------------
//


// ----------------------------------------------------------------------------
// Aufruf der Zustandsmaschine (zyklisch)
// ----------------------------------------------------------------------------
//

void Twitter::run(int SecFactor)
{
  run(SecFactor, 0);
}

void Twitter::run(int secFactor, int delay)
{
  int  fin;
  size_t msgLen;
  unsigned int sendLen;

  //-------------------------------------------------------------------------
  // Pruefen, ob die Ausfuehrung der Zustandsmaschine erlaubt ist
  //-------------------------------------------------------------------------
  if(!enabled) return;
  if(!netHnd->connected) return;

  //-------------------------------------------------------------------------
  // Eventuell die Ausfuehrung der Zustandsmaschine verzoegern
  //-------------------------------------------------------------------------
  if(delay > 0)
  {
    delayCounter++;
    if(delayCounter < delay) return;
  }

  delayCounter = 0;

  //-------------------------------------------------------------------------
  // Je nach Zustand verzweigen
  //-------------------------------------------------------------------------
  switch(runStatus)
  {
    // ------------------------------------------------------------------ //
    case rsInit:
    // ------------------------------------------------------------------ //
      speedCounter = 1;
      if(speed == highSpeed)
        speedLimit = secFactor/10;
      else if(speed == lowSpeed)
        speedLimit = secFactor * 10;
      else
        speedLimit = secFactor;

      runStatus = rsCreate;
      break;

    // ------------------------------------------------------------------ //
    case rsCreate:
    // ------------------------------------------------------------------ //
      // Telegramm aufbauen
      fin = createPDU();
      speedCounter++;

      if(fin == 1)
      {
        runStatus = rsSend;
      }
       break;

    // ------------------------------------------------------------------ //
    case rsSend:
    // ------------------------------------------------------------------ //
      msgLen = strlen(pduMsg);
      if(msgLen > 0)
      {
        sendLen = msgLen + 1; // String-Terminator beruecksichtigen
		  
        netHnd->send((uint8_t *)pduMsg, sendLen);

        cntSendMsg++;
      }

      speedCounter++;

      if(speedCounter > speedLimit)
      {
        overflow = true;
        markOverflow = true;
        overflowCounter++;
      }
      runStatus = rsWait;
      break;

    // ------------------------------------------------------------------ //
    case rsWait:
    // ------------------------------------------------------------------ //
      speedCounter++;

      if(speedCounter >= speedLimit)
      {
        speedCounter    = 1;
        errorCode       = 0;
        overflow        = false;
        runStatus       = rsCreate;
      }

      break;

    // ------------------------------------------------------------------ //
    case rsError:
    // ------------------------------------------------------------------ //
      // Fehlerzustand
      break;

    // ------------------------------------------------------------------ //
    default:
    // ------------------------------------------------------------------ //
      runStatus = rsWait;
      break;
  }
}

int Twitter::createPDU()
{
  int ready;
  size_t len;
  char      tmpStr[32];
  char      *srcPtr;
  double    tmpFloat;
  int       tmpInt;

  //-------------------------------------------------------------------------
  // Lokale Variablen initialisieren
  //-------------------------------------------------------------------------
  ready = 0;
  len = 0;

  //-------------------------------------------------------------------------
  // Je nach Zustand verzweigen
  //-------------------------------------------------------------------------
  switch(crPDUStatus)
  {
    // ------------------------------------------------------------------- //
    case cpsHeader:
    // ------------------------------------------------------------------- //
      // Telegrammheader konstruieren
      pduIdx = 0;
      srcPtr = msgHeader;

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;

      crPDUStatus = cpsCounter;
      break;

    // ------------------------------------------------------------------- //
    case cpsCounter:
    // ------------------------------------------------------------------- //
      pduCounter++;
      srcPtr = itoa(pduCounter,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsApplicationKey;
      break;

    // ------------------------------------------------------------------- //
    case cpsApplicationKey:
    // ------------------------------------------------------------------- //
      srcPtr = pduApplicationKeyStr;

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsDeviceKey;
      break;

    // ------------------------------------------------------------------- //
    case cpsDeviceKey:
    // ------------------------------------------------------------------- //
      srcPtr = pduDeviceKeyStr;

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsDeviceState;
      break;

    // ------------------------------------------------------------------- //
    case cpsDeviceState:
    // ------------------------------------------------------------------- //
      srcPtr = itoa(deviceState,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsDeviceName;
      break;

    // ------------------------------------------------------------------- //
    case cpsDeviceName:
    // ------------------------------------------------------------------- //
      srcPtr = deviceName;

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsTime;
      break;

    // ------------------------------------------------------------------- //
    case cpsTime:
    // ------------------------------------------------------------------- //
      // ISO-Zeitformat mit aktueller Zeit über Referenz
      //
      srcPtr = (char *) refDateTimePdu;

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsPosX;
      break;

    // ------------------------------------------------------------------- //
    case cpsPosX:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(posX,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsPosY;
      break;


    // ------------------------------------------------------------------- //
    case cpsPosY:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(posY,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsPosZ;
      break;

    // ------------------------------------------------------------------- //
    case cpsPosZ:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(posZ,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsBaseState;
      break;

    // ------------------------------------------------------------------- //
    case cpsBaseState:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(baseState,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsBaseMode;
      break;

    // ------------------------------------------------------------------- //
    case cpsBaseMode:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(baseMode,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsNrInt;
      break;


    // ------------------------------------------------------------------- //
    case cpsNrInt:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(nrIntValues,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsNrFloat;
      break;


    // ------------------------------------------------------------------- //
    case cpsNrFloat:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(nrFloatValues,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      crPDUStatus = cpsNrText;
      break;


    // ------------------------------------------------------------------- //
    case cpsNrText:
    // ------------------------------------------------------------------- //
      //
      srcPtr = itoa(nrTextValues,tmpStr,10);

      while(*srcPtr != '\0')
        pduMsg[pduIdx++] = *srcPtr++;
      pduMsg[pduIdx++] = ';';

      loopIdx = 0;
      crPDUStatus = cpsIntValues;
      break;

    // ------------------------------------------------------------------- //
    case cpsIntValues:
    // ------------------------------------------------------------------- //
      // Integerwerte als Telegrammelemente
      // Wenn kein Integerwert vorhanden ist, wird das Datenfeld
      // fuer Integerwerte nicht in das Telegramm eingetragen

      if(loopIdx < nrIntValues)
      {
        srcPtr = itoa(intValArray[loopIdx],tmpStr,10);

        while(*srcPtr != '\0')
          pduMsg[pduIdx++] = *srcPtr++;
        pduMsg[pduIdx++] = ';';
      }
      else
      {
        loopIdx = 0;
        crPDUStatus = cpsFloatValues;
        break;
      }

      loopIdx++;

      if(loopIdx == nrIntValues)
      {
        loopIdx = 0;
        crPDUStatus = cpsFloatValues;
        break;
      }

      break;


    // ------------------------------------------------------------------- //
    case cpsFloatValues:
    // ------------------------------------------------------------------- //
      // Floatwerte als Telegrammelemente
      // Wenn kein Floatwert vorhanden ist, wird das Datenfeld
      // fuer Floatwerte nicht in das Telegramm eingetragen
      if(loopIdx < nrFloatValues)
      {
        tmpFloat = floatValArray[loopIdx];
        tmpInt = (int) tmpFloat;
        srcPtr = itoa(tmpInt,tmpStr,10);

        while(*srcPtr != '\0')
          pduMsg[pduIdx++] = *srcPtr++;

        if(tmpFloat < 0)
        {
          tmpFloat = -tmpFloat;
          tmpInt   = -tmpInt;
        }

        tmpFloat = FLOAT_PREC_FACTOR * (tmpFloat - (double) tmpInt);
        tmpInt = (int) tmpFloat;

        if(tmpInt != 0)
        {
          pduMsg[pduIdx++] = '.';

          srcPtr = itoa(tmpInt,tmpStr,10);
          len = strlen(tmpStr);

          for(int i = 0; i < (FLOAT_PRECISION - len); i++)
            pduMsg[pduIdx++] = '0';

          while(*srcPtr != '\0')
            pduMsg[pduIdx++] = *srcPtr++;

          while(pduMsg[pduIdx - 1] == '0')
            pduIdx--;
        }
        pduMsg[pduIdx++] = ';';
      }
      else
      {
        loopIdx = 0;
        crPDUStatus = cpsTextValues;
        break;
      }

      loopIdx++;

      if(loopIdx == nrFloatValues)
      {
        loopIdx = 0;
        crPDUStatus = cpsTextValues;
        break;
      }

      break;


    // ------------------------------------------------------------------- //
    case cpsTextValues:
    // ------------------------------------------------------------------- //
      // Textwerte als Telegrammelemente
      // Wenn kein Textwert vorhanden ist, wird das Datenfeld
      // fuer Textwerte nicht in das Telegramm eingetragen

      if(loopIdx < nrTextValues)
      {
        if(textValArray != NULL)
        {
          srcPtr = textValArray[loopIdx];

          while(*srcPtr != '\0')
            pduMsg[pduIdx++] = *srcPtr++;
        }

        pduMsg[pduIdx++] = ';';
      }
      else
      {
        goto crPduFin;
      }

      loopIdx++;

      if(loopIdx == nrTextValues)
      {
crPduFin:
        // Das Telegramm abschliessen
        pduMsg[pduIdx - 1]  = ':';
        pduMsg[pduIdx++]    = ':';
        pduMsg[pduIdx]      = '\0';

        // Das Erstellen des Telegramm ist fertig
        ready = 1;

        // Naechsten Zustand setzen
        crPDUStatus = cpsHeader;
        break;
      }

      break;

      break;

    // ------------------------------------------------------------------- //
    default:
    // ------------------------------------------------------------------- //
      // Telegramm-String zuruecksetzen
      pduMsg[0] = 0;

      // Das Erstellen des Telegramm ist fertig
      ready = 1;

      // Naechsten Zustand setzen
      crPDUStatus = cpsHeader;
      break;
  }

  return(ready);
}


void Twitter::createMsgHeader()
{
  SmnIfInfo ifInfo;

  netHnd->getIfInfo(&ifInfo);

  sprintf(msgHeader, "%s:%s:%s:%d:%d:%s:%d:",
          "N10",              // Header Umas-Telegramm
          ifInfo.macAdrCStr,  // Sender, MAC-Adresse
          ifInfo.ipAdrCStr,   // Sender, IP-Adresse
          53,                 // Umas-Kommando (Schreibzugriff)
          0,                  // Kommunikationsvariable, Handle / Wiederholungssteuerung
          objectName,         // Kommunikationsvariable, Name
          0                   // Kommunikationsvariable, Index
         );

  itoa(deviceKey, pduDeviceKeyStr, 10);  // wird bei createDeviceHeader verwendet
  itoa(applicationKey, pduApplicationKeyStr, 10);

  //writeDebug(msgHeader);

  return;
}

void Twitter::createDeviceHeader(void)
{
  pduCounter++;

  itoa(pduCounter, pduCounterStr, 10);
  itoa(deviceState, pduDeviceStateStr, 10);

  strcpy(pduHeader,pduCounterStr);
  strcat(pduHeader,";");
  strcat(pduHeader,pduApplicationKeyStr);
  strcat(pduHeader,";");
  strcat(pduHeader,pduDeviceKeyStr);
  strcat(pduHeader,";");
  strcat(pduHeader,pduDeviceStateStr);
  strcat(pduHeader,";");
  strcat(pduHeader,deviceName);
  strcat(pduHeader,";");

  //writeDebug(pduHeader);

  return;
}

int Twitter::pduFromTime(char * timeStr)
{
  strcpy(timeStr,refDateTimePdu);

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return 0;
}

// ----------------------------------------------------------------------------
// Geräteparameter festlegen / ändern
// ----------------------------------------------------------------------------
//

void Twitter::setDeviceName(char * name)
{
	strncpy(deviceName,name,DEV_NAME_LEN);
	deviceName[DEV_NAME_LEN - 1] = '\0';
}

void Twitter::setDeviceKey(int key)
{
	deviceKey = key;
}

void Twitter::setDeviceState(int state)
{
	deviceState = state;
}

// ----------------------------------------------------------------------------
// Werte eintragen
// ----------------------------------------------------------------------------
//

void Twitter::setIntValue(int idx, int value)
{
  if(idx < 0) return;
  if(idx >= nrIntValues) return;
  if(idx >= MAXNRINT) return;
  intValArray[idx] = value;
}

void Twitter::setFloatValue(int idx, double value)
{
  if(idx < 0) return;
  if(idx >= nrFloatValues) return;
  if(idx >= MAXNRFLOAT) return;
  floatValArray[idx] = value;
}

void Twitter::setTextValue(int idx, char  *value)
{
  if(idx < 0) return;
  if(idx >= nrTextValues) return;
  if(idx >= MAXNRTEXT) return;
  textValArray[idx] = value;
}

// ----------------------------------------------------------------------------
// Debug-Funktionen
// ----------------------------------------------------------------------------
//
void Twitter::writeDebug(char * str)
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
  Serial.println(str);
}

unsigned int Twitter::getStatistic(char * strPtr)
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
  msgLen = sprintf(strPtr, "Twitter: CntSendMsg = %d\r\n",
                   cntSendMsg
                  );

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(msgLen);
}
