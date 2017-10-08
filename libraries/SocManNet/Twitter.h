/*
  Twitter.h
  Zwitscherer für Arduino Due
  Robert Patzke, 08. März 2015
*/

#ifndef Twitter_h
#define Twitter_h

#include <WString.h>
#include "Arduino.h"
#include "SocManNet.h"
//#include "rtc_clock.h"

#define MAXNRINT    4
#define MAXNRFLOAT  4
#define MAXNRTEXT   4
#define TWITTER_OBJ_NAME_LEN 32

#define DEV_NAME_LEN   16
#define MSG_HEADER_LEN 256
#define PDU_HEADER_LEN 256
#define PDU_TIME_LEN   32
#define PDU_MSG_LEN    1024

typedef enum _RunStatus
{
  rsWait,
  rsCreate,
  rsSend,
  rsError,
  rsNrOfStates
}
RunStatus;

typedef enum _CrPDUStatus
{
  cpsHeader,
  cpsTime,
  cpsStandard,
  cpsValueHeader,
  cpsIntValues,
  cpsFloatValues,
  cpsTextValues,
  cpsNrOfStates
}
CrPDUStatus;

typedef enum _Speed
{
  normalSpeed,
  highSpeed,
  lowSpeed,
  nrOfSpeeds
}
Speed;


class Twitter
{
  // --------------------------------------------------------------------------
  // Methoden (Funktionen) für den Anwender
  // --------------------------------------------------------------------------
  //
  public:
    // Konstruktoren
    //
    Twitter();
    Twitter(SocManNet * inNetHnd,
            char *      commObject,
            int         nrIntVal,
            int         nrFloatVal,
            int         nrTextVal,
            Speed       inSpeed);

    Twitter(SocManNet * inNetHnd, char *commObject);

    void init(SocManNet * inNetHnd,
              char *      commObject,
              int         nrInt,
              int         nrFloat,
              int         nrString,
              Speed       inSpeed);

    // Zyklischer Einstieg in die ZM
    //
    void      run(int SecFactor);
    void      run(int SecFactor, int delay);

    // Geräteparameter festlegen
    //
    void      setDeviceName(char * name);
    void      setDeviceKey(int key);
    void      setDeviceState(int state);

    // Setzen von Werten
    //
    void      setIntValue(int idx, int value);
    void      setFloatValue(int idx, double value);
    void      setTextValue(int idx, char *value);

  // --------------------------------------------------------------------------
  //  Oeffentliche Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
  unsigned int getStatistic(char * strPtr);

  // --------------------------------------------------------------------------
  // globale Variablen zur Steuerung
  // --------------------------------------------------------------------------
  //
  public:
    static int pduCounter;

    int        errorCode;
    char *     errorMsg;
    char *     resultMsg;

    bool       enabled;
    Speed      speed;

    int        posX;
    int        posY;
    int        posZ;
    int        baseState;
    int        baseMode;

    int        intValArray[MAXNRINT];
    double     floatValArray[MAXNRFLOAT];
    char       *textValArray[MAXNRTEXT];

//    RTC_clock * rtcClockHnd;

  // --------------------------------------------------------------------------
  //  globale Variablen fuer Debugzwecke
  // --------------------------------------------------------------------------
 public:
  bool debugOn;  // Verwaltung der Debug-Ausgabe

  // --------------------------------------------------------------------------
  // lokale Variablen zur Steuerung
  // --------------------------------------------------------------------------
  //
  private:
    int       nrIntValues;
    int       nrFloatValues;
    int       nrTextValues;

    char      objectName[TWITTER_OBJ_NAME_LEN];

    char      msgHeader[MSG_HEADER_LEN];
    char      pduHeader[PDU_HEADER_LEN];
    char      pduTime[PDU_TIME_LEN];
    char      pduMsg[PDU_MSG_LEN];

    int       delayCounter;
    int       speedCounter;

    int       deviceKey;
    int       deviceState;
    char      deviceName[DEV_NAME_LEN];

    RunStatus runStatus;
    RunStatus runError;

    CrPDUStatus crPDUStatus;

  // --------------------------------------------------------------------------
  // lokale Variablen fuer die Kommunikationsschnittstelle
  //---------------------------------------------------------------------------
  //
  private:
    SocManNet * netHnd;

  // --------------------------------------------------------------------------
  // Interne Variablen fuer Statistik
  // --------------------------------------------------------------------------
  unsigned int cntSendMsg;

  // --------------------------------------------------------------------------
  // lokale Funktionen zur Initialisierung und Steuerung
  // --------------------------------------------------------------------------
  //
  private:
    int createPDU(int frequency);
    void createMsgHeader(void);
    void createDeviceHeader(void);
    int pduFromTime(char * timeStr);

  // --------------------------------------------------------------------------
  // lokale Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
  //
  private:
    void writeDebug(char * str);
};

#endif

