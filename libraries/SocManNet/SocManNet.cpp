//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Broadcast Socket Interface
// Datei:   SocManNet.cpp
// Editor:  Igor Farber, Robert Patzke
// URI/URL: www.mfp-portal.de / homeautomation.x-api.de
// Todo:    The initialisation (init/open) has to be revised and improved
//          it is to complex and not transparent (10.02.18, R.P.)
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//
    //-------------------------------------------------------------------------
    // Inkludierte Dateien
    //-------------------------------------------------------------------------
    //
#include "SocManNet.h"

    //-------------------------------------------------------------------------
    // Funktionen
    //-------------------------------------------------------------------------

smnServPtr  nextSrv;

        //---------------------------------------------------------------------
        // Ereignis (Event) der Schnittstelle verarbeiten
        //---------------------------------------------------------------------
        //
#if defined(smnESP32) || defined(smnESP8266)

WiFiUDP         *wifiUdpPtr;
unsigned int    *locUdpPortPtr;
char            *ipAdrPtr;
IPAddress       ipAdr;
byte            macAdr[MAC_ADR_SIZE];
bool            *connectedPtr;
bool            *wifiPendingPtr;
unsigned int    *connectCountPtr;
int             wifiEventCounter = 0;
int             wifiEventIdx = 0;
WiFiEvent_t     wifiEventList[SMNmaxNrIFEvt];
WiFiServer      server(CONFIG_PORT);

#endif

#if defined(smnESP32)

  void smnWiFiEventHandler(WiFiEvent_t wifiEvent)
  {
    String    tmpStr;

    wifiEventList[wifiEventIdx] = wifiEvent;
    wifiEventCounter++;
    wifiEventIdx++;
    if(wifiEventIdx == SMNmaxNrIFEvt)
      wifiEventIdx = 0;

    switch(wifiEvent)
    {
      // Wenn der Aufbau einer Verbindung zum AP gestartet ist:
      //
      case SYSTEM_EVENT_STA_START:              // 2
        break;

      // Wenn nach einer intern definierten Zeit eine funktionierende
      // Verbindung zum AP aufgebaut werden konnte:
      //
      case SYSTEM_EVENT_STA_CONNECTED:          // 4
        *wifiPendingPtr = true;         // Evt warten auf IP-Adresse
        *connectedPtr = false;          // Verbindung noch nicht nutzbar
        break;

      // Wenn nach einer intern definierten Zeit keine funktionierende
      // Verbindung zum AP aufgebaut werden konnte:
      //
      case SYSTEM_EVENT_STA_DISCONNECTED:       // 5
        *wifiPendingPtr = false;
        *connectedPtr = false;
        break;

      // Sobald die Verbindung zum AP aufgebaut und eine Internet-Adresse
      // zugewiesen wurde:
      //
      case SYSTEM_EVENT_STA_GOT_IP:             // 7
        *wifiPendingPtr = false;
        ipAdr = WiFi.localIP();
        WiFi.macAddress(macAdr);
        wifiUdpPtr->begin(*locUdpPortPtr);
        *connectedPtr = true;
        (*connectCountPtr)++;
        break;

      case SYSTEM_EVENT_STA_LOST_IP:             // 8
        *wifiPendingPtr = true;
        *connectedPtr = false;
        break;

      default:
        break;
    }
  }
#endif


#ifdef smnESP8266

  void smnWiFiEventHandler(WiFiEvent_t wifiEvent)
  {
    String    tmpStr;

    wifiEventList[wifiEventIdx] = wifiEvent;
    wifiEventCounter++;
    wifiEventIdx++;
    if(wifiEventIdx == SMNmaxNrIFEvt)
      wifiEventIdx = 0;

    switch(wifiEvent)
    {
      // Wenn nach einer intern definierten Zeit eine funktionierende
      // Verbindung zum AP aufgebaut werden konnte:
      //
      case WIFI_EVENT_STAMODE_CONNECTED:                                        // SYSTEM_EVENT_STA_CONNECTED:          // 4
        *wifiPendingPtr = true;         // Evt warten auf IP-Adresse
        *connectedPtr = false;          // Verbindung noch nicht nutzbar
        break;

      // Wenn nach einer intern definierten Zeit keine funktionierende
      // Verbindung zum AP aufgebaut werden konnte:
      //
      case WIFI_EVENT_STAMODE_DISCONNECTED:                                     // SYSTEM_EVENT_STA_DISCONNECTED:       // 5
        *wifiPendingPtr = false;
        *connectedPtr = false;
        break;

      // Sobald die Verbindung zum AP aufgebaut und eine Internet-Adresse
      // zugewiesen wurde:
      //
      case WIFI_EVENT_STAMODE_GOT_IP:                                           // SYSTEM_EVENT_STA_GOT_IP:             // 7
        *wifiPendingPtr = false;
        ipAdr = WiFi.localIP();
        WiFi.macAddress(macAdr);
        wifiUdpPtr->begin(*locUdpPortPtr);
        *connectedPtr = true;
        (*connectCountPtr)++;
        break;

      case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:                                     // SYSTEM_EVENT_STA_LOST_IP:             // 8
        *wifiPendingPtr = true;
        *connectedPtr = false;
        break;

      default:
        break;
    }
  }
#endif



#ifdef smnSimLinux
  IPAddress       ipAdr;
  byte            macAdr[MAC_ADR_SIZE];

#endif

#ifdef smnArduinoShieldEth
  EthernetServer    server(CONFIG_PORT);
#endif

        //---------------------------------------------------------------------
        // Constructors and initilisations
        //---------------------------------------------------------------------
SocManNet::SocManNet()
{
  initLoc();
}


void SocManNet::initLoc()
{
  cntRecMsg       = 0;
  cntSendMsg      = 0;
  debugOn         = false;
  idxFieldObjData = 0;
  idxFieldObjName = 0;
  portBroadcast   = 4100;
  portLocal       = 4000;
  recParseCounter = 0;
  connected       = false;
  initialised     = false;
  staticInitDone  = false;
  initPending     = false;
  useDHCP         = false;
  error           = smnError_none;
  bcEnable        = 0;
  connectCount    = 0;
  connectMark     = 0;
  runDisabled     = false;
  nextSrv         = srvInit;

#ifdef smnSimLinux
  socketId        = 0;
  socketBcAdrLen  = 0;
  socketRecAdrLen = 0;
  ipLocal         = NULL;
  ipPrimaryDNS    = NULL;
  ipSecondaryDNS  = NULL;
  ipGateway       = NULL;
  ipSubNet        = NULL;
  ipBroadcast     = NULL;
#endif
}

        //---------------------------------------------------------------------
        // Broadcast-Interface initialisieren und Öffnen mit SocManNetUser.h
        //---------------------------------------------------------------------
        //
byte            localMacAdr[MAC_ADR_SIZE];
byte            localIp[4];
unsigned int    localPort;

byte            broadcastIp[4];
unsigned int    broadcastPort;
byte            subNetMask[4];
byte            gatewayIp[4];
byte            primDnsIp[4];
byte            secDnsIp[4];
char            netName[SMNnetNameMaxSize];
char            netPass[SMNnetPassMaxSize];


void SocManNet::setMac(byte *bList)
{
  for(int i = 0; i < 6; i++)
    localMacAdr[i] = bList[i];

  // DNS IP-Adresse
  primDnsIp[0] = PRIMDNS_IP_B0;
  primDnsIp[1] = PRIMDNS_IP_B1;
  primDnsIp[2] = PRIMDNS_IP_B2;
  primDnsIp[3] = PRIMDNS_IP_B3;

  secDnsIp[0] = SECDNS_IP_B0;
  secDnsIp[1] = SECDNS_IP_B1;
  secDnsIp[2] = SECDNS_IP_B2;
  secDnsIp[3] = SECDNS_IP_B3;
}

void SocManNet::setIpAdr(byte *bList)
{
  for(int i = 0; i < 4; i++)
  {
    localIp[i] = broadcastIp[i] = gatewayIp[i] = bList[i];
  }
  subNetMask[0] = subNetMask[1] = subNetMask[2] = 255;

  subNetMask[3]     = 0;
  broadcastIp[3]    = 255;
  gatewayIp[3]      = bList[4];
}

void SocManNet::setPorts(byte *bList)
{
  broadcastPort = (bList[0] << 8) + bList[1];
  localPort     = (bList[2] << 8) + bList[3];
}

void SocManNet::setNetName(byte *bList)
{
  char  c;
  int   i;

  for(i = 0; i < (SMNnetNameMaxSize - 1); i++)
  {
    c = bList[i];
    netName[i] = c;
    if(c == '\0') break;
  }
  if(i == (SMNnetNameMaxSize - 1))
    netName[i] = '\0';
  ssid = netName;
}

void SocManNet::setNetPass(byte *bList)
{
  char  c;
  int   i;

  for(i = 0; i < (SMNnetPassMaxSize - 1); i++)
  {
    c = bList[i];
    netPass[i] = c;
    if(c == '\0') break;
  }
  if(i == (SMNnetPassMaxSize - 1))
    netPass[i] = '\0';
  pass = netPass;
}

enum SocManNetError SocManNet::setInit(bool dhcp)
{
  if(staticInitDone) return(smnError_alreadyInitialised);

  useDHCP       = dhcp;

  //---------------------------------------------------------------------------
  // Broadcast-Interface zuruecksetzen
  //---------------------------------------------------------------------------
  memset(macLocal, 0, sizeof(macLocal));

#if defined(smnESP32) || defined(smnESP8266)
  WiFi.disconnect(true);    // Eine eventuelle alte Verbindung beenden
#endif

  //return(smnError_debugBreak1);

  // --------------------------------------------------------------------------
  // Die Telegramm-Behandlung zuruecksetzen
  // --------------------------------------------------------------------------
  memset(commBufRec, 0, sizeof(commBufRec));
  memset(eventDstList, 0, sizeof(eventDstList));
  idxFieldObjName = 0;
  idxFieldObjData = 0;

  //---------------------------------------------------------------------------
  // Debug-Interface zuruecksetzen
  //---------------------------------------------------------------------------
  debugOn = false;

  //---------------------------------------------------------------------------
  // Statistik zuruecksetzen
  //---------------------------------------------------------------------------
  cntSendMsg    = 0;
  cntRecMsg     = 0;

  initialised   = true;

#if defined(smnESP32) || defined(smnESP8266)

  // Umgebung für den Event initialisieren
  locUdpPortPtr     = &localPort;
  wifiUdpPtr        = &Udp;
  connected         = false;
  connectedPtr      = &connected;
  connectCount      = 0;
  connectCountPtr   = &connectCount;
  wifiPendingPtr    = &initPending;
  nextSrv           = srvInit;

  WiFi.onEvent(smnWiFiEventHandler);

#endif

#ifdef SocManNetDebug

  sprintf(ds,"localMacAdr = %02X-%02X-%02X-%02X-%02X-%02X",
          localMacAdr[0],localMacAdr[1],localMacAdr[2],localMacAdr[3],localMacAdr[4],localMacAdr[5]);
  dpl(ds);

  sprintf(ds,"localIpAdr = %d.%d.%d.%d",
          localIp[0],localIp[1],localIp[2],localIp[3]);
  dpl(ds);

  sprintf(ds,"broadcastIpAdr = %d.%d.%d.%d",
          broadcastIp[0],broadcastIp[1],broadcastIp[2],broadcastIp[3]);
  dpl(ds);

  sprintf(ds,"gatewayIpAdr = %d.%d.%d.%d",
          gatewayIp[0],gatewayIp[1],gatewayIp[2],gatewayIp[3]);
  dpl(ds);

  sprintf(ds,"localPort = %d, broadcastPort = %d",localPort,broadcastPort);
  dpl(ds);

  sprintf(ds,"primDnsIpAdr = %d.%d.%d.%d",
          primDnsIp[0],primDnsIp[1],primDnsIp[2],primDnsIp[3]);
  dpl(ds);

  sprintf(ds,"secDnsIpAdr = %d.%d.%d.%d",
          secDnsIp[0],secDnsIp[1],secDnsIp[2],secDnsIp[3]);
  dpl(ds);

  return(smnError_debugBreak1);

#endif

  // Kommunikationsschnittstelle oeffnen
  error = open  (
                localMacAdr,localIp,localPort,broadcastIp,
                broadcastPort,subNetMask,gatewayIp,primDnsIp,
                secDnsIp, false
                );

  return(error);

}

enum SocManNetError SocManNet::init(bool dhcp)
{
  if(staticInitDone) return(smnError_alreadyInitialised);

  // Lokale MAC-Adresse
  localMacAdr[0] = LOCAL_MAC_ADR_B0;
  localMacAdr[1] = LOCAL_MAC_ADR_B1;
  localMacAdr[2] = LOCAL_MAC_ADR_B2;
  localMacAdr[3] = LOCAL_MAC_ADR_B3;
  localMacAdr[4] = LOCAL_MAC_ADR_B4;
  localMacAdr[5] = LOCAL_MAC_ADR_B5;

  // Lokale IP-Adresse
  localIp[0] = LOCAL_IP_B0;
  localIp[1] = LOCAL_IP_B1;
  localIp[2] = LOCAL_IP_B2;
  localIp[3] = LOCAL_IP_B3;

  // Lokale Portnummer
  localPort = LOCAL_PORT;

  // Broadcast IP-Adresse
  broadcastIp[0] = BROADCAST_IP_B0;
  broadcastIp[1] = BROADCAST_IP_B1;
  broadcastIp[2] = BROADCAST_IP_B2;
  broadcastIp[3] = BROADCAST_IP_B3;

  // Broadcast Portnummer
  broadcastPort = BROADCAST_PORT;

  subNetMask[0] = SUB_NET_MASK_B0;
  subNetMask[1] = SUB_NET_MASK_B1;
  subNetMask[2] = SUB_NET_MASK_B2;
  subNetMask[3] = SUB_NET_MASK_B3;

  // Gateway IP-Adresse
  gatewayIp[0] = GATEWAY_IP_B0;
  gatewayIp[1] = GATEWAY_IP_B1;
  gatewayIp[2] = GATEWAY_IP_B2;
  gatewayIp[3] = GATEWAY_IP_B3;

  // DNS IP-Adresse
  primDnsIp[0] = PRIMDNS_IP_B0;
  primDnsIp[1] = PRIMDNS_IP_B1;
  primDnsIp[2] = PRIMDNS_IP_B2;
  primDnsIp[3] = PRIMDNS_IP_B3;

  secDnsIp[0] = SECDNS_IP_B0;
  secDnsIp[1] = SECDNS_IP_B1;
  secDnsIp[2] = SECDNS_IP_B2;
  secDnsIp[3] = SECDNS_IP_B3;

  // Kommunikationsobjekt initialisieren
  init((char *) SMNSSID,
       (char *) SMNPASS,dhcp);

#if defined(smnESP32) || defined(smnESP8266)

  // Umgebung für den Event initialisieren
  locUdpPortPtr     = &localPort;
  wifiUdpPtr        = &Udp;
  connected         = false;
  connectedPtr      = &connected;
  connectCount      = 0;
  connectCountPtr   = &connectCount;
  wifiPendingPtr    = &initPending;
  nextSrv           = srvInit;

  WiFi.onEvent(smnWiFiEventHandler);

#endif


  // Kommunikationsschnittstelle oeffnen
  error = open  (
                localMacAdr,localIp,localPort,broadcastIp,
                broadcastPort,subNetMask,gatewayIp,primDnsIp,
                secDnsIp, false
                );

  return(error);
}
        //---------------------------------------------------------------------
        // Broadcast-Interface initialisieren mit variablen Werten
        //---------------------------------------------------------------------
        //
void SocManNet::init(char *netName, char *netPass, bool dhcp)
{
  ssid          = netName;
  pass          = netPass;
  useDHCP       = dhcp;

  //---------------------------------------------------------------------------
  // Broadcast-Interface zuruecksetzen
  //---------------------------------------------------------------------------
  memset(macLocal, 0, sizeof(macLocal));

#if defined(smnESP32) || defined(smnESP8266)
  WiFi.disconnect(true);    // Eine eventuelle alte Verbindung beenden
#endif


  // --------------------------------------------------------------------------
  // Die Telegramm-Behandlung zuruecksetzen
  // --------------------------------------------------------------------------
  memset(commBufRec, 0, sizeof(commBufRec));
  memset(eventDstList, 0, sizeof(eventDstList));
  idxFieldObjName = 0;
  idxFieldObjData = 0;

  //---------------------------------------------------------------------------
  // Debug-Interface zuruecksetzen
  //---------------------------------------------------------------------------
  debugOn = false;

  //---------------------------------------------------------------------------
  // Statistik zuruecksetzen
  //---------------------------------------------------------------------------
  cntSendMsg    = 0;
  cntRecMsg     = 0;

  initialised   = true;
}

        //---------------------------------------------------------------------
        // UDP-Socket oeffnen
        //---------------------------------------------------------------------
        //
SocManNetError  SocManNet::open()
{
  error = open  (
                localMacAdr,localIp,localPort,broadcastIp,
                broadcastPort,subNetMask,gatewayIp,primDnsIp,
                secDnsIp, false
                );

  return(error);
}

SocManNetError  SocManNet::reopen()
{
#if defined(smnESP32) || defined(smnESP8266) || defined(smnSimLinux)
  byte ipAdrBytes[4];
  byte *macBytes;

  ipAdrBytes[0] = ipAdr[0];
  ipAdrBytes[1] = ipAdr[1];
  ipAdrBytes[2] = ipAdr[2];
  ipAdrBytes[3] = ipAdr[3];

  macBytes = macAdr;


  error = open  (
                macBytes,ipAdrBytes,localPort,broadcastIp,
                broadcastPort,subNetMask,gatewayIp,primDnsIp,
                secDnsIp, true
                );

#else
  SocManNetError error = smnError_none;
#endif

  return(error);
}

SocManNetError
SocManNet::open(byte * ptrMacLocal,
                uint8_t *    ptrIpLocal,
                unsigned int localPort,
                uint8_t *    ptrIpBroadcast,
                unsigned int broadcastPort,
                uint8_t *    ptrIpSubNet,
                uint8_t *    ptrIpGateway,
                uint8_t *    ptrIpPrimDNS,
                uint8_t *    ptrIpSecDNS,
                bool         repOpen
               )
{
#if defined(smnESP32) || defined(smnESP8266) || defined(ArduinoShieldEth)
  bool  ok;
#endif

#ifdef smnSimLinux
  int   error;
#endif

  //---------------------------------------------------------------------------
  // Pruefen, ob es moeglich ist, Socket zu oeffnen
  //---------------------------------------------------------------------------
  if(connected == true)
  {
    return(smnError_multipleOpen);
  }

  //---------------------------------------------------------------------------
  // Daten uebernehmen
  //---------------------------------------------------------------------------
  // MAC-Adresse
  memcpy(macLocal, ptrMacLocal, MAC_ADR_SIZE);

  // Lokale IP-Adresse
  ipLocal = ptrIpLocal;

  // Lokale Portnummer
  portLocal = localPort;

  // Broadcast IP-Adresse
  ipBroadcast = ptrIpBroadcast;

  // Broadcast Portnummer
  portBroadcast = broadcastPort;

  // Subnetzmaske
  ipSubNet = ptrIpSubNet;

  // Gateway
  ipGateway = ptrIpGateway;

  // DNS
  ipPrimaryDNS = ptrIpPrimDNS;
  ipSecondaryDNS = ptrIpSecDNS;

  //---------------------------------------------------------------------------
  // Start Ethernet oder Wifi und evt. UDP
  //---------------------------------------------------------------------------
#ifdef smnArduinoShieldEth
  Ethernet.begin(macLocal,ipLocal);
#endif

#if defined(smnESP32) || defined(smnESP8266)

  // Bedingte Konfiguration des Netzwerkes
  //
  if(useDHCP == false)
  {
    ok = WiFi.config(ipLocal, ipGateway, ipSubNet, ipPrimaryDNS, ipSecondaryDNS);
    if(ok == false)
      return(smnError_configFailed);
  }

  initPending = true;
  WiFi.begin(ssid,pass);
  //Udp.begin(portLocal);
#endif

#if defined (smnArduinoShieldEth)
  Udp.begin(portLocal);
  connected = true;
  connectCount = 1;
#endif

#ifdef smnSimLinux
  socketId = socket(AF_INET, SOCK_DGRAM, 0);
  if(socketId < 0) return(smnError_socketFailed);

  bcEnable = 1;
  error =
      setsockopt(socketId, SOL_SOCKET, SO_BROADCAST, &bcEnable, sizeof(bcEnable));
  if(error < 0)
    return(smnError_socketConf);

  socketBcAdrLen = sizeof(socketBcAdr);
  memset((char *) &socketBcAdr, 0, socketBcAdrLen);
  socketBcAdr.sin_family = AF_INET;
  socketBcAdr.sin_port = htons(broadcastPort);
  inet_aton(BcAddress,&socketBcAdr.sin_addr);

  socketRecAdrLen = sizeof(socketRecAdr);
  memset((char *) &socketRecAdr, 0, socketRecAdrLen);
  socketRecAdr.sin_family = AF_INET;
  socketRecAdr.sin_port = htons(broadcastPort);
  socketRecAdr.sin_addr.s_addr = INADDR_ANY;

  error =
      bind(socketId, (struct sockaddr *) &socketRecAdr, socketRecAdrLen);
  if(error < 0)
    return(smnError_bind);

  connected = true;
  connectCount = 1;

#endif

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(smnError_none);
}


        //---------------------------------------------------------------------
        // UDP-Socket schliessen
        //---------------------------------------------------------------------
        //
int SocManNet::closeConnection()
{
  //---------------------------------------------------------------------------
  // Pruefen, ob es moeglich/noetig ist, Socket zu schliessen
  //---------------------------------------------------------------------------
  if(connected == false)
  {
    return(smnError_alreadyClosed);
  }

  //---------------------------------------------------------------------------
  // Socket schliessen
  //---------------------------------------------------------------------------

#if defined(smnESP32) || defined(smnESP8266)
  WiFi.disconnect(true);
#endif

#ifdef smnSimLinux
  shutdown(socketId, SHUT_RDWR);
  close(socketId);
#endif

  //---------------------------------------------------------------------------
  // Status vom Socket aktualisieren
  //---------------------------------------------------------------------------
  connected = false;

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(smnError_none);
}

void SocManNet::getIfInfo(SmnIfInfo *memSmnIfInfo)
{
#if defined(smnESP32) || defined(smnESP8266)
  memSmnIfInfo->ipAddress    = ipAdr;
  sprintf(memSmnIfInfo->ipAdrCStr,"%d.%d.%d.%d",ipAdr[0],ipAdr[1],ipAdr[2],ipAdr[3]);

  memSmnIfInfo->macAddress   = macAdr;
  sprintf(memSmnIfInfo->macAdrCStr,"%02X-%02X-%02X-%02X-%02X-%02X",
          macAdr[0],macAdr[1],macAdr[2],macAdr[3],macAdr[4],macAdr[5]);

  memSmnIfInfo->evtCounter   = wifiEventCounter;
  memSmnIfInfo->evtIdx       = wifiEventIdx;
  if(wifiEventCounter >= SMNmaxNrIFEvt)
  {
    for(int i = 0; i < SMNmaxNrIFEvt; i++)
      memSmnIfInfo->evtList[i] = (int) wifiEventList[i];
  }
  else if(wifiEventIdx > 0)
  {
    for(int i = 0; i < wifiEventIdx; i++)
      memSmnIfInfo->evtList[i] = (int) wifiEventList[i];
  }
#endif

#if defined (smnArduinoShieldEth)
  memSmnIfInfo->ipAddress    = ipLocal;
  sprintf(memSmnIfInfo->ipAdrCStr,"%d.%d.%d.%d",ipLocal[0],ipLocal[1],ipLocal[2],ipLocal[3]);

  memSmnIfInfo->macAddress   = macLocal;
  sprintf(memSmnIfInfo->macAdrCStr,"%02X-%02X-%02X-%02X-%02X-%02X",
          macLocal[0],macLocal[1],macLocal[2],macLocal[3],macLocal[4],macLocal[5]);

  memSmnIfInfo->evtCounter   = 0;
  memSmnIfInfo->evtIdx       = 0;
#endif

  return;
}

void SocManNet::getIfStatus(SmnIfStatus *memSmnIfStatus)
{
#if defined(smnESP32) || defined(smnESP8266)
  wl_status_t wifiStatus;

  memSmnIfStatus->changed        = false;

  if(memSmnIfStatus->connected != connected)
  {
    memSmnIfStatus->connected    = connected;
    memSmnIfStatus->changed      = true;
  }

  if(memSmnIfStatus->initPending != initPending)
  {
    memSmnIfStatus->initPending  = initPending;
    memSmnIfStatus->changed      = true;
  }

  if(memSmnIfStatus->connectCount != connectCount)
  {
    memSmnIfStatus->connectCount = connectCount;
    memSmnIfStatus->changed      = true;
  }

  wifiStatus = WiFi.status();
  if(memSmnIfStatus->ifStatus != (int) wifiStatus)
  {
    memSmnIfStatus->ifStatus     = (int) wifiStatus;
    memSmnIfStatus->changed      = true;
  }
#endif

  return;
}

char * SocManNet::getErrorMsg(enum SocManNetError err)
{
  char * retMsg;

  switch(err)
  {
    case smnError_alreadyClosed:
      retMsg = (char *) "Wiederholtes Schließen abgewiesen";
      break;

    case smnError_alreadyInitialised:
      retMsg = (char *) "Wiederholte Initialisierung abgewiesen";
      break;

    case smnError_multipleOpen:
      retMsg = (char *) "Mehrfaches Öffnen abgewiesen";
      break;

    case smnError_notConnected:
      retMsg = (char *) "Auftrag abgewiesen weil nicht verbunden";
      break;

    case smnError_wrongArgument:
      retMsg = (char *) "Übergebenes Argument ungültig";
      break;

    case smnError_configFailed:
      retMsg = (char *) "Konfiguration nicht ausgeführt";
      break;

    default:
      retMsg = (char *) "Anderweitig definierter Fehler";
      break;
  }

  return(retMsg);
}

        //---------------------------------------------------------------------
        // Daten ueber UDP-Socket senden
        //---------------------------------------------------------------------
int SocManNet::send(uint8_t * msg, unsigned int msgLen)
{
  //---------------------------------------------------------------------------
  // Eingabeparameter ueberpruefen
  //---------------------------------------------------------------------------
  if(msg == NULL)
  {
    return(smnError_wrongArgument);
  }

  if(msgLen == 0)
  {
    return(smnError_wrongArgument);
  }

  //---------------------------------------------------------------------------
  // Zustand ueberpruefen
  //---------------------------------------------------------------------------
  //
  if(connected == false)
  {
    return(smnError_notConnected);
  }

  //---------------------------------------------------------------------------
  // Telegramm versenden
  //---------------------------------------------------------------------------

#if defined(smnESP32) || defined(smnESP8266) || defined(smnArduinoShieldEth)
  Udp.beginPacket(ipBroadcast, portBroadcast);

  Udp.write(msg, msgLen);

  Udp.endPacket();
#endif

#ifdef smnSimLinux
  sendto(socketId, msg, msgLen, 0, (struct sockaddr *) &socketBcAdr, socketBcAdrLen);
#endif

  cntSendMsg++;

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(smnError_none);
}

        //---------------------------------------------------------------------
        // receive(...)
        // Daten vom UDP-Socket empfangen
        //---------------------------------------------------------------------
int SocManNet::receive(unsigned char * buf, int bufSize)
{
  int packetSize;
  int msgLen;

  if(connected == false)
    return(smnError_notConnected);

  //---------------------------------------------------------------------------
  // Pruefen, ob Telegramm empfangen wurde
  //---------------------------------------------------------------------------
#if defined(smnESP32) || defined(smnESP8266) || defined(smnArduinoShieldEth)
  packetSize = Udp.parsePacket();
#endif

#ifdef smnSimLinux
  packetSize = (int) recvfrom
      (socketId, buf, (unsigned long) bufSize - 1, MSG_DONTWAIT, NULL, NULL);
#endif

  if(packetSize <= 0)
  {
    return(0);
  }

  //---------------------------------------------------------------------------
  // Telegramm holen (bei besonderen Treibern)
  //---------------------------------------------------------------------------

#if defined(smnESP32) || defined(smnESP8266) || defined(smnArduinoShieldEth)
  Udp.read(buf, bufSize-1);
#endif

  cntRecMsg++;

  //---------------------------------------------------------------------------
  // Telegramm behandeln
  //---------------------------------------------------------------------------
  // Am Ende des Telegramms wird Null-Terminator angehaengt.
  // Damit ist es moeglich, das Telegramm als C-String zu betrachten
  if(packetSize < bufSize)
  {
    buf[packetSize] = 0;

    msgLen = packetSize;
  }
  else
  {
    buf[bufSize-1] = 0;

    msgLen = bufSize;

    //writeDebug((char *) "Message is too long for the debug output");
  }

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(msgLen);
}

        //---------------------------------------------------------------------
        // checkMsg(...)
        // Empfangene Telegramm validieren
        //---------------------------------------------------------------------
int SocManNet::checkSocManNetMsg(char * msg, unsigned int msgLen)
{
  //---------------------------------------------------------------------------
  // Telegramm validieren
  //---------------------------------------------------------------------------
  // Telegrammlaenge ueberpruefen
  if(msgLen < 3)
  {
    return(-1);
  }

  // Anfang des Telegramms ueberpruefen
  if(strncmp(msg, "N10", 3) != 0)
  {
    return(-2);
  }

  // Ende des Telegramms ueberpruefen
  if(msg[msgLen-2] != ':')
  {
    return(-3);
  }

  if(msg[msgLen-1] != 0)
  {
    return(-4);
  }

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(0);
}

        //---------------------------------------------------------------------
        // parseMsg(...)
        // Telegramm parsen
        //---------------------------------------------------------------------

int SocManNet::parseMsg(char * msg, unsigned int msgLen)
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
  idxFieldObjName = 0;
  idxFieldObjData = 0;

  // Zustandsmaschine zuruecksetzen
  parserState = p_msg_st_waitFieldSeparator;
  cntField = 0;
  parserReady = false;
  parserError = false;

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < msgLen; idx++)
  {
    //-------------------------------------------------------------------------
    // Je nach Parserzustand verzweigen
    //-------------------------------------------------------------------------
    switch(parserState)
    {
      // ------------------------------------------------------------------- //
      case p_msg_st_waitFieldSeparator:
      // ------------------------------------------------------------------- //
        if(msg[idx] == ':')
        {
          cntField++;

          if(cntField == 5)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_setObjNameField;

            break;
          }
          else if(cntField == 7)
          {
            // Naechsten Zustand setzen
            parserState = p_msg_st_setDataField;

            break;
          }

          // Sonst im Zustand bleiben
        }

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_setObjNameField:
      // ------------------------------------------------------------------- //
        // Index des Datenfeldes mit dem Objektnamen setzen
        idxFieldObjName = idx;

        // Naechsten Zustand setzen
        parserState = p_msg_st_waitFieldSeparator;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_setDataField:
      // ------------------------------------------------------------------- //
        // Index des Datenfeldes mit objektspezifischen Daten setzen
        idxFieldObjData = idx;

        // Merker 'Parser ist fertig' setzen
        parserReady = true;

        // Naechsten Zustand setzen
        parserState = p_msg_st_parseReady;

        break;

      // ------------------------------------------------------------------- //
      case p_msg_st_parseReady:
      // ------------------------------------------------------------------- //
        // Im Zustand bleiben
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

  return(0);
}

int SocManNet::parseMsg2(char * msg, unsigned int msgLen)
{
  int           cntField;
  unsigned int  idx;

  recParseCounter++;

  //---------------------------------------------------------------------------
  // Parser initialisieren
  //---------------------------------------------------------------------------
  cntField = 0;
  pduHeadIdxField[phiType] = 0;   // Das Telegramm fängt mit dem Typ an

  //---------------------------------------------------------------------------
  // Telegramm parsen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < msgLen; idx++)
  {
    if(msg[idx] == ':')
    {
      cntField++;

      if(msg[idx+1] != ':')
        pduHeadIdxField[cntField] = idx + 1;
      else
        pduHeadIdxField[cntField] = -1;

      if(cntField == phiData)
      {
        idxFieldObjName = pduHeadIdxField[phiName];
        idxFieldObjData = idx + 1;
        return(0);
      }
    }
  }
  return(-1);
}


        //---------------------------------------------------------------------
        // run(...)
        // Staendig laufenden Prozess
        //---------------------------------------------------------------------
void SocManNet::run(void)
{
  unsigned int  msgLen;
  unsigned int  idx;
  int           chkv;
  char        * objName;
  char        * objMsg;
  unsigned int  objMsgLen;

  if(runDisabled) return;
  if(error != smnError_none) return;
  if(initPending) return;
  if(connectCount == 0) return;

#ifdef smnSMNDBG02
  dbgRef->msg("SocManNet.run[1]: connected = ");
  if(connected == true)
    dbgRef->msg("true\r\n");
  else
    dbgRef->msg("false\r\n");
  return;
#endif

  if(connected == false)
  {
    if(connectCount == connectMark) return;
    reopen();
    connectMark = connectCount;
    return;
  }

  //---------------------------------------------------------------------------
  // Telegramme empfangen
  //---------------------------------------------------------------------------
  msgLen = receive((unsigned char *)commBufRec, (sizeof(commBufRec)-1));
  if(msgLen <= 0)
  {
    // Wenn kein Telegramm empfangen wird, dann den Server bedienen
    //
    nextSrv();
    return;
  }

  //---------------------------------------------------------------------------
  // Telegramm validieren
  //---------------------------------------------------------------------------
  if(checkSocManNetMsg((char *)commBufRec, (unsigned int)msgLen) != 0)
  {
    return;
  }

    //-------------------------------------------------------------------------
    // Telegramm parsen
    //-------------------------------------------------------------------------
  chkv = parseMsg2((char *)commBufRec, (unsigned int)msgLen);
  if(chkv != 0)
  {
    return;
  }

    //-------------------------------------------------------------------------
    // Eventuell Telegramm weiterleiten
    //-------------------------------------------------------------------------
  for(idx = 0; idx < COMM_EVT_DST_NUM; idx++)
  {
    if(eventDstList[idx].login != true)
    {
      break;
    }

    objName = (char *)(&commBufRec[idxFieldObjName]);
    objMsg = (char *)(&commBufRec[idxFieldObjData]);
    objMsgLen = msgLen-idxFieldObjData;

    if(strncmp(objName, eventDstList[idx].objName, eventDstList[idx].objNameLen) == 0)
    {
      if(eventDstList[idx].evtFu != NULL)
      {
        (*(eventDstList[idx].evtFu))(eventDstList[idx].evtHnd,
                                    (char *)objMsg,
                                     objMsgLen
                                     );
      }

      break;
    }
  }
}

        //---------------------------------------------------------------------
        // attachEvtRecMsg(...)
        // Eventfunktion anmelden
        //---------------------------------------------------------------------
int SocManNet::attachEvtRecMsg(char * commObjName, void * evtHnd, BROADCAST_EVT evtFu)
{
  unsigned int idx;

  //---------------------------------------------------------------------------
  // Eingangsparameter validieren
  //---------------------------------------------------------------------------
  if(strlen(commObjName) >= (COMM_OBJ_NAME_LEN-1))
  {
    return(-1);
  }

  //---------------------------------------------------------------------------
  // Nach freien Ereignisdeskriptor suchen
  //---------------------------------------------------------------------------
  for(idx = 0; idx < COMM_EVT_DST_NUM; idx++)
  {
    if(eventDstList[idx].login == false)
    {
      break;
    }
  }

  if(idx >= COMM_EVT_DST_NUM)
  {
    return(-1);
  }

  //---------------------------------------------------------------------------
  // Ereignisdeskriptor beschreiben
  //---------------------------------------------------------------------------
  strncpy(eventDstList[idx].objName,
          commObjName,
          sizeof(eventDstList[idx].objName)-2);
  eventDstList[idx].objNameLen = strlen(commObjName);
  eventDstList[idx].evtHnd = evtHnd;
  eventDstList[idx].evtFu = evtFu;
  eventDstList[idx].login = true;

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(0);
}

// ---------------------------------------------------------------------------
// Steuerbarer Server
// ---------------------------------------------------------------------------

// Variables to control server task
//
bool            ctlServerOn;
CtlServerStatus ctlServerStatus;

void srvInit()
{
  if(ctlServerOn == false) return;

  ctlServerStatus = cssInit;
  server.begin();
  ctlServerStatus = cssWaitClient;
  nextSrv = waitClient;
}

#if defined(smnESP32) || defined(smnESP8266)

WiFiClient extClient;

#endif

#ifdef smnArduinoShieldEth

EthernetClient extClient;

#endif

void waitClient()
{
  extClient = server.available();
  if(!extClient) return;
  ctlServerStatus = cssWaitClientMsg;
  nextSrv = waitClientMsg;
}

int     extClientMsgLen;
int     extClientMsgIdx;
bool    srvRecFin;

void waitClientMsg()
{
  if(!extClient.connected())
  {
    nextSrv = waitClient;
    return;
  }

  extClientMsgLen = extClient.available();
  if(extClientMsgLen == 0) return;

  extClientMsgIdx = 0;
  srvRecFin = false;
  nextSrv = readBlockClient;
  ctlServerStatus = cssReadBlockClient;
}

byte    srvReceiveBuffer[SRV_BUF_REC_SIZE];
bool    srvRecBufFilled;

void readBlockClient()
{
  int   readVal;

  for(int i = 0; i < SMNreadExtClientBlockSize; i++)
  {
    readVal = extClient.read();
    if(readVal < 0)
    {
      srvRecFin = true;
      break;
    }

    srvReceiveBuffer[extClientMsgIdx] = (byte) readVal;
    extClientMsgIdx++;
    if(extClientMsgIdx == extClientMsgLen)
    {
      srvRecFin = true;
      break;
    }
  }

  if(srvRecFin)
  {
    srvRecBufFilled = true;
    ctlServerStatus = cssFinClientMsg;
    nextSrv = finClientMsg;
  }
}


void finClientMsg()
{
  if(srvRecFin) return;
  ctlServerStatus = cssWaitClientMsg;
  nextSrv = waitClientMsg;
}


void SocManNet::startServer()
{
  ctlServerOn = true;
  srvRecBufFilled = false;
}

void SocManNet::getCtlSrvResult(CtlServerResPtr srvResPtr)
{
  srvResPtr->recFinished = srvRecFin;
  srvResPtr->nrOfBytes = extClientMsgIdx;
  srvResPtr->byteMem = srvReceiveBuffer;
}

void SocManNet::quitCtlSrvRead()
{
  extClientMsgIdx = 0;
  srvRecFin = false;
}

/*
        //---------------------------------------------------------------------
        // writeDebug(...)
        // Ein String ueber Debug-Schnittstelle ausgeben
        //---------------------------------------------------------------------
void SocManNet::writeDebug(char * str)
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
#ifdef smnDebugArduino
  Serial.println(str);
#endif

#ifdef smnDebugLinuxConsole
  printf("%s", str);
  printf("\n");
#endif
}
*/

        //---------------------------------------------------------------------
        // getStatistic(...)
        // Statistik ausgeben
        //---------------------------------------------------------------------
unsigned int SocManNet::getStatistic(char * strPtr)
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
  msgLen = sprintf(strPtr, "SocManNet: CntSend = %d CntRec = %d\r\n",
                   cntSendMsg,
                   cntRecMsg
                  );

  //---------------------------------------------------------------------------
  // Ergebnis definieren
  //---------------------------------------------------------------------------
  return(msgLen);
}
