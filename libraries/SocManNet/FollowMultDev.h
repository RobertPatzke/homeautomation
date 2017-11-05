//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Follower for multiple devices
// Datei:   FollowMultDev.h
// Editor:  Igor Farber, Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#ifndef FollowMultDev_h
#define FollowMultDev_h

#ifndef _environment_h
  #include "environment.h"
#endif

#ifdef smnSimLinux
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "arduinoDefs.h"
  #include "SocManNet.h"
  #pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#ifdef smnSloeber
  #include <WString.h>
  #include "Arduino.h"
  #include "SocManNet.h"
#endif

#define MAXNRSRC    8

#define MAXNRINT    4
#define MAXNRFLOAT  4
#define MAXNRTEXT   4

#define MAXVALCHRLEN  128

#define COMM_OBJ_NAME_LEN 32
#define DATA_OBJ_NAME_LEN 32

#define TEXTVAL_LEN_MAX 256

typedef struct _DeviceInfo
{
  int         applicationKey;
  int         deviceKey;
  int         deviceState;
  char        *deviceName;
  int         posX;
  int         posY;
  int         posZ;
  int         baseState;
  int         baseMode;
} DeviceInfo;

typedef struct _ReceivedValueM
{
  int           idx;
  unsigned int  status;
  int           pduCount;
  DeviceInfo    deviceInfo;
  int           deviceIdx;
} ReceivedValueM;

typedef struct _IntegerValueMD
{
  int               value;
  ReceivedValueM    recDsc;
  bool              newValue;
  bool              newPdu;
} IntegerValueMD;

typedef struct _FloatValueMD
{
  double            value;
  ReceivedValueM    recDsc;
  bool              newValue;
  bool              newPdu;
} FloatValueMD;

typedef struct _TextValueMD
{
  char              value[TEXTVAL_LEN_MAX];
  ReceivedValueM    recDsc;
  bool              newValue;
  bool              newPdu;
} TextValueMD;

#define STATUSVAL_BM_UNBORN  0x00000000
#define STATUSVAL_BM_NEWVAL  0x00000001 // Es liegt ein neuer Wert vor
#define STATUSVAL_BM_EMPTY   0x00000002 // Noch kein Telegramm oder neuer Speicher
#define STATUSVAL_BM_NONE    0x00000004 // Sender liefert keinen entsprechenden Wert
#define STATUSVAL_BM_NEWPDU  0x00000008 // Ein neues Telegramm eingetroffen
#define STATUSVAL_BM_IDX     0x00000010 // F�r den Index liegt kein Wert vor
#define STATUSVAL_BM_LOSTPDU 0x00000020 // Mehrere Telegramme nach letztem Zugriff

enum pduDataIdx
{
  pdiPduCount,
  pdiApplicationKey,
  pdiDeviceKey,
  pdiDeviceState,
  pdiDeviceName,
  pdiTime,
  pdiPosX,
  pdiPosY,
  pdiPosZ,
  pdiBaseState,
  pdiBaseMode,
  pdiIntCount,
  pdiFloatCount,
  pdiTextCount,
  pdiValueList,
  pdiCount
};

class FollowMultDev
{
  // --------------------------------------------------------------------------
  // Interne Festlegungen
  // --------------------------------------------------------------------------
  //
  enum PARSER_MSG_STATE
  {
    p_msg_st_unknown = 0,
    p_msg_st_waitFieldSeparator,
    p_msg_st_FieldPduCount,
    p_msg_st_FieldNumInt,
    p_msg_st_FieldNumFloat,
    p_msg_st_FieldNumText,
    p_msg_st_FieldValue,
    p_msg_st_Ready
  };

  // --------------------------------------------------------------------------
  // Methoden (Funktionen) für den Anwender
  // --------------------------------------------------------------------------
  //
  public:
    // Konstruktoren
    FollowMultDev();
    FollowMultDev(SocManNet * inNetHnd);
    FollowMultDev(SocManNet * inNetHnd, char * commObject);

    void init(SocManNet * inNetHnd, char * commObject);

    bool getIntStatus(ReceivedValueM * intVal);
    bool getFloatStatus(ReceivedValueM * floatVal);
    bool getTextStatus(ReceivedValueM * textVal);

    void getValue(IntegerValueMD * intVal);
    void getValue(FloatValueMD * floatVal);
    void getValue(TextValueMD * textVal);

  // --------------------------------------------------------------------------
  //  Oeffentliche Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
    unsigned int getStatistic(char * strPtr, int devIdx);

  // -------------------------------------------------------------------------
  // Globale Variablen f�r die Anwendung
  // -------------------------------------------------------------------------
  //
  public:
    static char * defaultObject;
    bool          enabled;
    int           recParseCounter;
    //char          testBuf1[256];
    //char          testBuf2[256];
    //char          testBuf3[256];

  // -------------------------------------------------------------------------
  // Inhalt des eingegangenen Telegramms
  // -------------------------------------------------------------------------
  //
  public:

    int         deviceKeyList[MAXNRSRC];
    int         pduCount[MAXNRSRC];
    int         applicationKey[MAXNRSRC];
    int         deviceKey[MAXNRSRC];
    int         deviceState[MAXNRSRC];
    char        deviceName[MAXNRSRC][DATA_OBJ_NAME_LEN];
    int         posX[MAXNRSRC];
    int         posY[MAXNRSRC];
    int         posZ[MAXNRSRC];
    int         baseState[MAXNRSRC];
    int         baseMode[MAXNRSRC];

    int         intCount;
    int         floatCount;
    int         textCount;

    int         intArray[MAXNRSRC][MAXNRINT];
    double      floatArray[MAXNRSRC][MAXNRFLOAT];
    char        textArray[MAXNRSRC][MAXNRTEXT][TEXTVAL_LEN_MAX];

    int         maxDeviceIdx;

  // -------------------------------------------------------------------------
  // Verarbeitung des eingegangenen Telegramms
  // -------------------------------------------------------------------------
  //
  unsigned int idxFieldPduCount;
  unsigned int idxFieldDeviceKey;
  unsigned int idxFieldDeviceState;
  unsigned int idxFieldDeviceName;
  unsigned int idxFieldDeviceTime;
  unsigned int idxFieldDevicePosX;
  unsigned int idxFieldDevicePosY;
  unsigned int idxFieldDevicePosZ;
  unsigned int idxFieldDeviceAppState;
  unsigned int idxFieldDeviceAppMode;
  unsigned int idxFieldIntCount;
  unsigned int idxFieldFloatCount;
  unsigned int idxFieldTextCount;
  unsigned int idxFieldValue;

  int           pduDataIdxField[pdiCount];
  bool          busyMsgAnalysis;

  // --------------------------------------------------------------------------
  //  globale Variablen fuer Debugzwecke
  // --------------------------------------------------------------------------
 public:
  bool debugOn;  // Verwaltung der Debug-Ausgabe

  // --------------------------------------------------------------------------
  // lokale Variablen fuer die Kommunikationsschnittstelle
  //---------------------------------------------------------------------------
  //
  private:
    SocManNet * netHnd;

  // --------------------------------------------------------------------------
  // Interne Variablen fuer Statistik
  // --------------------------------------------------------------------------
    unsigned int cntRecMsg;

  // --------------------------------------------------------------------------
  // Funktionen zur Verarbeitung des eingegangenen Telegramms
  // --------------------------------------------------------------------------
  //
  public:
    void evtRecMsg(char * msg, unsigned int msgLen);
    int parseMsg2(char * msg, unsigned int msgLen);
    int storeDataMsg2(char * msg, unsigned int msgLen);

  // --------------------------------------------------------------------------
  // lokale Funktionen fuer das Lesen von Werte und Status der Werte
  // --------------------------------------------------------------------------
  //
  private:
    void clrValStatus(int * status);
    void setMaskValStatus(int * status);
    void clrMaskValStatus(int * status);

  // --------------------------------------------------------------------------
  // lokale Funktionen fuer Debugzwecke
  // --------------------------------------------------------------------------
  //
  private:
    void writeDebug(char * str);
};

#endif // FollowMultiDev_h
