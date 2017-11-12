//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Broadcast Socket Interface
// Datei:   SocManNet.h
// Editor:  Igor Farber, Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//
//
#ifndef _SocManNet_h
#define _SocManNet_h
//---------------------------------------------------------------------------//

#ifndef _environment_h
  #include "environment.h"
#endif

#ifndef _socManNetUser_h
  #include "socManNetUser.h"
#endif

#ifdef smnSimLinux
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include "arduinoDefs.h"
  #define SYSMICSEC locMicros()
  #pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#ifdef smnSloeber
#include "Arduino.h"
#endif

#ifdef smnArduinoShieldEth
  #include <Ethernet.h>
  #include <EthernetUdp.h>
#endif

#ifdef smnESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
#endif

#ifdef smnESP32
  #include <WiFi.h>
  #include <WiFiUdp.h>
#endif

    //-------------------------------------------------------------------------
    // Festlegungen für den Betrieb
    //-------------------------------------------------------------------------
#define MAC_ADR_SIZE 6
#define COMM_BUF_MSG_SEND_SIZE 3000
#define COMM_BUF_MSG_REC_SIZE 2048
#define COMM_OBJ_NAME_LEN 32
#define COMM_EVT_DST_NUM 4
#define COMM_MSG_MAX_NUM_DATA_FIELDS 30
#define SMNmaxNrIFEvt  32

typedef void (*BROADCAST_EVT)(void * evtHnd, char * msg, unsigned int msgLen);

typedef struct _COMM_EVT_DST
{
  bool          login;
  char          objName[COMM_OBJ_NAME_LEN];
  unsigned int  objNameLen;
  void *        evtHnd;
  BROADCAST_EVT evtFu;
} COMM_EVT_DST;

typedef struct _SmnIfInfo
{
  IPAddress     ipAddress;
  byte *        macAddress;
  char          ipAdrCStr[16];
  char          macAdrCStr[18];
  int           evtCounter;
  int           evtIdx;
  int           evtList[SMNmaxNrIFEvt];
} SmnIfInfo;

typedef struct  _SmnIfStatus
{
  bool          changed;
  bool          connected;
  bool          initPending;
  unsigned int  connectCount;
  int           ifStatus;
} SmnIfStatus;

enum SocManNetError
{
  smnError_none = 0,
  smnError_multipleOpen = -99,
  smnError_alreadyInitialised,
  smnError_alreadyClosed,
  smnError_wrongArgument,
  smnError_configFailed,
  smnError_socketFailed,
  smnError_socketConf,
  smnError_bind,
  smnError_notConnected,
  smnError_unexpected
};

    //-------------------------------------------------------------------------
    // class SocManNet
    //-------------------------------------------------------------------------
class SocManNet
{
  //-------------------------------------------------------------------------
  // Festlegungen für die Anwendung
  //-------------------------------------------------------------------------
  //
public:

  //---------------------------------------------------------------------------
  // Interne Festlegungen
  //---------------------------------------------------------------------------
private:
  enum PARSER_MSG_STATE
  {
    p_msg_st_unknown = 0,
    p_msg_st_waitFieldSeparator,
    p_msg_st_setObjNameField,
    p_msg_st_setDataField,
    p_msg_st_parseReady
  };
  // Zustaende des Parserprozesses

  enum pduHeadIdx
  {
    phiType,    // Telegrammtyp
    phiMac,     // Mac-Adresse
    phiIp,      // Ip-Adresse
    phiSrv,     // Dienstkennzeichnung
    phiAcc,     // Datenzugriff oder Wiederholungssteuerung
    phiName,    // Objektname
    phiIdx,     // Arrayindex oder Objekttyp
    phiData,    // Datenfeld
    phiCount    // Anzal der Indizes
  };

private:
  // --------------------------------------------------------------------------
  // Interne Variablen fuer die HW-Behandlung
  // --------------------------------------------------------------------------
  byte          macLocal[MAC_ADR_SIZE];
  // Lokale MAC-Adresse

  IPAddress     ipLocal;
  // Lokale IP-Adresse

  unsigned int  portLocal;
  // Lokale Portnummer

  IPAddress     ipBroadcast;
  // Broadcast IP-Adresse

  unsigned int  portBroadcast;
  // Broadcast Portnummer

  IPAddress     ipSubNet;
  // IP-Subnetzmaske

  IPAddress     ipGateway;
  // IP-Gateway

  IPAddress     ipPrimaryDNS;
  IPAddress     ipSecondaryDNS;

#ifdef smnSimLinux
  int                   socketId;
  struct sockaddr_in    socketBcAdr;
  int                   socketBcAdrLen;
  struct sockaddr_in    socketRecAdr;
  int                   socketRecAdrLen;
#endif

#ifdef smnArduinoShieldEth
  EthernetUDP   Udp;	// An EthernetUDP instance to let us send and receive packets over UDP
#endif

#if defined (smnESP8266) || defined (smnArduinoShieldWiFi) || defined (smnESP32)
  WiFiUDP Udp;
#endif


  // --------------------------------------------------------------------------
  // Interne Variablen fuer die Telegramm-Behandlung
  // --------------------------------------------------------------------------
private:
  unsigned char commBufRec[COMM_BUF_MSG_REC_SIZE];
  // Empfangspuffer

  COMM_EVT_DST eventDstList[COMM_EVT_DST_NUM];
  // Zeiger auf CallBack-Funktion fuer das Empfang-Ereignis

  unsigned int idxFieldObjName;
  // Index des Datenfeldes mit dem Objektnamen

  unsigned int idxFieldObjData;
  // Index des Datenfeldes mit objektspezifischen Daten

  int     pduHeadIdxField[phiCount];

  enum SocManNetError error;

  // --------------------------------------------------------------------------
  // Interne Variablen fuer Statistik
  // --------------------------------------------------------------------------
private:
  unsigned int cntSendMsg;
  unsigned int cntRecMsg;

  // --------------------------------------------------------------------------
  // Interne Funktionen fuer Broadcast-Interface
  // --------------------------------------------------------------------------
private:
  int receive(unsigned char * buf, int bufSize);
  int checkSocManNetMsg(char * msgBuf, unsigned int msgLen);
  int parseMsg(char * msg, unsigned int msgLen);
  int parseMsg2(char * msg, unsigned int msgLen);

#ifdef smnESP32
  //void WiFiEventHandler(WiFiEvent_t wifiEvent);
#endif

  // --------------------------------------------------------------------------
  // Interne Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
private:
  void writeDebug(char * str);

public:
  // --------------------------------------------------------------------------
  //  Oeffentliche Variablen fuer Debugzwecke
  // --------------------------------------------------------------------------
  bool debugOn;
  // Verwaltung der Debug-Ausgabe

  int   recParseCounter;
  char  *MacAddress;
  char  *IpAddress;
  char  *ssid;
  char  *pass;
  char  *BcAddress;
  bool  useDHCP;

  bool  connected;      // Status des Socket-Interfaces bzw. der Verbindung
  bool  initialised;    //  "
  bool  staticInitDone; //  "
  bool  initPending;    // Interface-Initialisierung noch aktiv
  int   bcEnable;

  unsigned int  connectCount;   // Zähler für die aufgebauten Verbindungen
  unsigned int  connectMark;    // Nachgeführter Merker als Zustand

public:
  // --------------------------------------------------------------------------
  //  Allgemeine oeffentliche Funktionen
  // --------------------------------------------------------------------------
  SocManNet();
  // Konstruktor

  void init(char *macAdr, char *ipAdr, char *netName, char *netPass, bool dhcp);
  // Broadcast-Interface Initialisierung (dynamische Argumente)

  enum SocManNetError init(bool dhcp);
  // Broadcast-Interface Initialisierung und Vorbereiten zum Betrieb
  // mit den Parametern aus SocManNetUser.h

  // --------------------------------------------------------------------------
  //  Oeffentliche Funktionen fuer Broadcast-Interface
  // --------------------------------------------------------------------------
  SocManNetError    open();
  SocManNetError    reopen();

  SocManNetError
  open(byte *       ptrMacLocal,
       uint8_t *    ptrIpLocal,
       unsigned int localPort,
       uint8_t *    ptrIpBroadcast,
       unsigned int broadcastPort,
       uint8_t *    ptrIpSubNet,
       uint8_t *    ptrIpGateway,
       uint8_t *    ptrIpPrimDNS,
       uint8_t *    ptrIpSecDNS,
       bool         repOpen);
  int closeConnection();
  int send(uint8_t * msg, unsigned int msgLen);
  void run(void);
  int attachEvtRecMsg(char * commObjName, void * evtHnd, BROADCAST_EVT evtFu);

  // --------------------------------------------------------------------------
  //  Oeffentliche Funktionen fuer Debugzwecke und Fehlerauswertung
  // --------------------------------------------------------------------------
  unsigned int      getStatistic(char * strPtr);
  char *            getErrorMsg(enum SocManNetError err);
  void              getIfInfo(SmnIfInfo *);
  void              getIfStatus(SmnIfStatus *);
};
#endif // _SocManNet_h
