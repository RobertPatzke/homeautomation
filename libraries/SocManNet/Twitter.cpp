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
	runError        = rsWait;
	runStatus       = rsWait;
	speed           = normalSpeed;
	speedCounter    = 0;
	refDateTimePdu  = "2017-10-09T18:00:00.000+00:00";
	pduCounter      = 0;
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

  delayCounter = 0;
  speedCounter = 8;
  runStatus    = rsWait;
  runError     = rsNrOfStates;

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
  int  speedLimit;
  size_t msgLen;
	unsigned int sendLen;

  //-------------------------------------------------------------------------
  // Pruefen, ob die Ausfuehrung der Zustandsmaschine erlaubt ist
  //-------------------------------------------------------------------------
  if(!enabled) return;

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
    case rsWait:
    // ------------------------------------------------------------------ //
      speedCounter++;
      if(speed == highSpeed)
        speedLimit = secFactor/10;
      else if(speed == lowSpeed)
        speedLimit = secFactor * 10;
      else
        speedLimit = secFactor;

      if(speedCounter >= speedLimit)
      {
        speedCounter  = 8;
        runStatus   = rsCreate;
        errorCode   = 0;
      }

      break;

    // ------------------------------------------------------------------ //
    case rsCreate:
    // ------------------------------------------------------------------ //
      // Telegramm aufbauen
      fin = createPDU();
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

      // Telegramm senden
      runStatus = rsWait;
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
      // Telegramm-String zuruecksetzen
      pduMsg[0] = 0;

      // Telegrammheader konstruieren
      strcat(pduMsg, msgHeader);

      createDeviceHeader();
      strcat(pduMsg, pduHeader);

      // Naechsten Zustand setzen
      crPDUStatus = cpsTime;
      break;

    // ------------------------------------------------------------------- //
    case cpsTime:
    // ------------------------------------------------------------------- //
      // ISO-Zeitformat aus aktueller Zeit
      pduFromTime(pduTime);
      strcat(pduMsg, pduTime);

      // Naechsten Zustand setzen
      crPDUStatus = cpsStandard;
      break;

    // ------------------------------------------------------------------- //
    case cpsStandard:
    // ------------------------------------------------------------------- //
      // Standard-Telegrammanfang
      len = strlen(pduMsg);

      sprintf(&pduMsg[len], ";%d;%d;%d;%d;%d",
              posX,
              posY,
              posZ,
              baseState,
              baseMode
             );

      // Naechsten Zustand setzen
      crPDUStatus = cpsValueHeader;
      break;

    // ------------------------------------------------------------------- //
    case cpsValueHeader:
    // ------------------------------------------------------------------- //
      // Die Anzahl von Int/Float/Text-Werten
      len = strlen(pduMsg);

      sprintf(&pduMsg[len], ";%d;%d;%d",
              nrIntValues,
              nrFloatValues,
              nrTextValues
             );

      // Naechsten Zustand setzen
      crPDUStatus = cpsIntValues;
      break;

    // ------------------------------------------------------------------- //
    case cpsIntValues:
    // ------------------------------------------------------------------- //
      // Integerwerte als Telegrammelemente
      // Wenn kein Integerwert vorhanden ist, wird das Datenfeld
      // fuer Integerwerte nicht in das Telegramm eingetragen
      for(int idx = 0; idx < nrIntValues; idx++)
      {
        len = strlen(pduMsg);
        sprintf(&pduMsg[len], ";%d", intValArray[idx]);
      }

      // Naechsten Zustand setzen
      crPDUStatus = cpsFloatValues;
      break;

    // ------------------------------------------------------------------- //
    case cpsFloatValues:
    // ------------------------------------------------------------------- //
      // Floatwerte als Telegrammelemente
      // Wenn kein Floatwert vorhanden ist, wird das Datenfeld
      // fuer Floatwerte nicht in das Telegramm eingetragen
      for(int idx = 0; idx < nrFloatValues; idx++)
      {
        len = strlen(pduMsg);
        sprintf(&pduMsg[len], ";%f", floatValArray[idx]);
      }

      // Naechsten Zustand setzen
      crPDUStatus = cpsTextValues;
      break;

    // ------------------------------------------------------------------- //
    case cpsTextValues:
    // ------------------------------------------------------------------- //
      // Textwerte als Telegrammelemente
      // Wenn kein Textwert vorhanden ist, wird das Datenfeld
      // fuer Textwerte nicht in das Telegramm eingetragen
      for(int idx = 0; idx < nrTextValues; idx ++)
      {
        len = strlen(pduMsg);

        if(textValArray != NULL)
        {
          sprintf(&pduMsg[len], ";%s", textValArray[idx]);
        }
        else
        {
          sprintf(&pduMsg[len], ";");
        }
      }

      // Das Telegramm abschliessen
      strcat(pduMsg, "::");

      // Das Erstellen des Telegramm ist fertig
      ready = 1;

      // Naechsten Zustand setzen
      crPDUStatus = cpsHeader;
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
