/*
  Twitter.h
  Zwitscherer f�r Arduino Due
  Robert Patzke, 08. M�rz 2015
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

#define FLOAT_PRECISION     6
#define FLOAT_PREC_FACTOR   1000000

typedef enum _RunStatus
{
  rsInit,
  rsCreate,
  rsSend,
  rsBurst,
  rsWait,
  rsError,
  rsNrOfStates
}
RunStatus;

typedef enum _CrPDUStatus
{
  cpsUpdate,
  cpsHeader,
  cpsCounter,
  cpsApplicationKey,
  cpsDeviceKey,
  cpsDeviceState,
  cpsDeviceName,
  cpsTime,
  cpsPosX,
  cpsPosY,
  cpsPosZ,
  cpsBaseState,
  cpsBaseMode,
  cpsNrInt,
  cpsNrFloat,
  cpsNrText,
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

typedef enum _BurstMode
{
  noBurst = 0,
  burst2  = 2,
  burst3  = 3,
  burst5  = 5
}
BurstMode;


class Twitter
{

  // --------------------------------------------------------------------------
  // Hilfsklassen
  // --------------------------------------------------------------------------
  //
public:
  struct  Int
  {
    int       index;
    int       value;
    Twitter   *twitPtr;

    Int(int idx);
    Int(int idx,Twitter twPtr);
    void  set(int);
    void  update();
    void  operator=(int);
  };

  struct  Float
  {
    int       index;
    double    value;
    Twitter   *twitPtr;

    Float(int idx);
    Float(int idx,Twitter twPtr);
    void  set(double);
    void  update();
    void  operator=(double);
  };

  struct  Text
  {
    int       index;
    char      value[32];
    Twitter   *twitPtr;

    Text(int idx);
    Text(int idx,Twitter twPtr);
    void  set(const char *);
    void  update();
    void  operator=(const char *);
  };


  // --------------------------------------------------------------------------
  // Methoden (Funktionen) für den Anwender
  // --------------------------------------------------------------------------
  //
  public:
    // Konstruktoren
    //
    Twitter();
    Twitter(SocManNet *     inNetHnd,
            const char *    ptrDateTimePdu,
            char *          commObject,
            int             nrIntVal,
            int             nrFloatVal,
            int             nrTextVal,
            Speed           inSpeed);

    Twitter(SocManNet * inNetHnd, char *commObject);

    void init(SocManNet *   inNetHnd,
              const char *  ptrDateTimePdu,
              const char *  commObject,
              int           nrInt,
              int           nrFloat,
              int           nrString,
              Speed         inSpeed);

    // Zyklischer Einstieg in die ZM
    //
    void      run(int SecFactor);
    void      run(int SecFactor, int delay);

    // Geräteparameter festlegen
    //
    void      setDeviceName(char * name);
    void      setDeviceKey(int key);
    void      setDeviceKey();
    int       getDeviceKey();
    void      setApplicationKey(int key);
    void      setDeviceState(int state);

    // Setzen von Werten
    //
    void      setIntValue(int idx, int value);
    void      setFloatValue(int idx, double value);
    void      setTextValue(int idx, char *value);

    // Besonderheiten und Laufzeitparameter
    //
    void      setBurst(BurstMode bm, int delay);

    // Statistische und Betriebsinformationen
    //
    bool      pduSent(unsigned int *refCounter);
    void      getName(char *dest);

    void      pinVar(Int   *intVar);
    void      pinVar(Float *floatVar);
    void      pinVar(Text  *textVar);

  // --------------------------------------------------------------------------
  //  Oeffentliche Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
  unsigned int getStatistic(char * strPtr);

  // --------------------------------------------------------------------------
  // globale Variablen zur Steuerung
  // --------------------------------------------------------------------------
  //
  public:
    int         pduCounter;

    int         errorCode;
    char *      errorMsg;
    char *      resultMsg;

    const char *    refDateTimePdu;
    // Zeiger auf den aktuellen String von Datum/Uhrzeit

    bool       enabled;
    Speed      speed;

    bool       markOverflow;
    bool       overflow;
    // Aufruffrequenz zu gering, Zeitrahmen nicht haltbar

    int         overflowCounter;

    int         posX;
    int         posY;
    int         posZ;
    int         deviceState;
    int         baseState;
    int         baseMode;

    int         intValArray[MAXNRINT];
    double      floatValArray[MAXNRFLOAT];
    char        *textValArray[MAXNRTEXT];

    int         intValStore[MAXNRINT];
    double      floatValStore[MAXNRFLOAT];

//    RTC_clock * rtcClockHnd;

  // --------------------------------------------------------------------------
  //  globale Variablen fuer Debugzwecke
  // --------------------------------------------------------------------------
  public:
    bool          debugOn;  // Verwaltung der Debug-Ausgabe
    unsigned int  cntRun;

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
    char      pduHeader[DEV_NAME_LEN + 16 + 16 + 16 + 8];
    char      pduCounterStr[16];
    char      pduApplicationKeyStr[16];
    char      pduDeviceKeyStr[16];
    char      pduDeviceStateStr[16];
    char      pduTime[PDU_TIME_LEN];
    char      pduMsg[PDU_MSG_LEN];

    int       delayCounter;
    int       speedCounter;

    bool      keySetByApp;
    int       applicationKey;
    int       deviceKey;
    char      deviceName[DEV_NAME_LEN];

    int       pduIdx;
    int       loopIdx;
    int       speedLimit;

    BurstMode bMode;
    int       bDelay;
    int       burstCounter;
    int       burstLimit;
    int       burstLoop;


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
    void  setup();
    int   createPDU();
    void  createMsgHeader(void);
    void  createDeviceHeader(void);
    int   pduFromTime(char * timeStr);

  // --------------------------------------------------------------------------
  // lokale Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
  //
  private:
    void writeDebug(char * str);

};

#endif

