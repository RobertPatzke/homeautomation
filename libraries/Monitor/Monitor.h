//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Monitor.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   15. Mai 2021
//
// Der Monitor dient zum direkten Zugriff auf die Ressourcen eines
// Mikrocontrollers über die serielle Schnittstelle.
//

#include  "Arduino.h"
#include  "environment.h"
#include  "arduinoDefs.h"
#include  "LoopCheck.h"
#include  "IntrfTw.h"

#ifndef Monitor_h
#define Monitor_h
// ----------------------------------------------------------------------------

#define keyHit()  smnSerial.available()
#define keyIn()   smnSerial.read()
#define out(x)    smnSerial.print(x)
#define outl(x)   smnSerial.println(x)
#define GoInp     nextState = &Monitor::getKey;
#define GoPrm     nextState = &Monitor::prompt;
#define GoWt      nextState = &Monitor::waitEnter;

#define modeEcho  0x01
#define modeNl    0x02

#define eolCR     0x01
#define eolLF     0x02
#define eolNL     0x03

#define BufSize   512
#define MaxChn    32

class Monitor
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //
  typedef void (Monitor::*StatePtr)(void);
  typedef struct _ConfMeasChannel
  {
    word    maxVal;
    word    minVal;
    char    *name;
    char    type;
  } CfgMeasChn, *CfgMeasChnPtr;

private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  int       cpu;
  int       mode;

#ifdef smnNANOBLE33
  dword     *microTicValPtr;
  dword     *microTicCapPtr;
#endif

  char      buffer[BufSize];
  int       wrIdx;
  int       rdIdx;
  bool      blkOut;
  bool      blkIn;

  char      inChar[16];
  char      outChar[128];
  char      tmpChar[8];
  int       inIdx;
  bool      extraIn;

  char      cmdMode1;
  char      cmdMode2;

  char      *info;

  StatePtr  nextState;
  LoopCheck *lcPtr;

  IntrfTw     *twiPtr;
  int         twiAdr;
  TwiByteSeq  twiByteSeq;
  byte        byteArray[32];

  dword       readOffsAddr;
  bool        doReadReg;

  CfgMeasChn  cfgChnArr[MaxChn];
  char        nrOfChnChar;

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void  init(int mode, int cpu);
  void  init(int mode, int cpu, LoopCheck *inLcPtr);
  void  init(int mode, int cpu, LoopCheck *inLcPtr, IntrfTw *inTwPtr);

  void  waitEnter();
  void  prompt();
  void  getKey();
  void  version();
  void  getRdOffsAdr();
  void  readRegVal();
  void  getTiming();
  void  getLoopMeasure();

  void  getTwiAdr();
  void  readTwiList();
  void  readTwiByte();
  void  writeTwiByte();

  void  print(char c, int eol);
  void  print(char *txt, int eol);
  void  print(byte *hex, int nr, char fill, int eol);
  void  print(unsigned int iVal, int eol);
  void  prints(int iVal, int eol);

  dword micsecs()
  {
    *microTicCapPtr = 1;
    return(*microTicValPtr);
  }

  // --------------------------------------------------------------------------
  // Datenaufbereitung
  // --------------------------------------------------------------------------
  //
  void hexByte(char *dest, byte val);
  void binByte(char *dest, byte val);
  void hexWord(char *dest, word val);
  void binWord(char *dest, word val);
  void hexDword(char *dest, dword val);
  void binDword(char *dest, dword val);
  int  cpyStr(char *dest, char *src);

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Monitor(int mode, int cpu);
  Monitor(int mode, int cpu, LoopCheck *inLcPtr);
  Monitor(int mode, int cpu, LoopCheck *inLcPtr, IntrfTw *inTwiPtr);

  // --------------------------------------------------------------------------
  // Konfiguration und Hilfsfunktionen
  // --------------------------------------------------------------------------
  //
  void  setInfo(char *txt);
  int   putBuf(char c);
  int   putBuf(char *txt);
  char  getBuf();
  void  clrBuf();
  void  sendConfig();


  // --------------------------------------------------------------------------
  // Anwenderschnittstelle
  // --------------------------------------------------------------------------
  //

  // Funktionen
  //
  void run();
  void cprint(char c);
  void print(char *txt);
  void print(unsigned int iVal);
  void prints(int iVal);
  void print(byte *iVal, int nr, char fill);
  void printcr();
  void cprintcr(char c);
  void printcr(char *txt);
  void printcr(unsigned int iVal);
  void printcr(byte *iVal, int nr, char fill);
  void println();
  void cprintln(char c);
  void println(char *txt);
  void println(unsigned int iVal);
  void println(byte *iVal, int nr, char fill);

  void config(int inNrOfChn);
  void config(int inChn, char inType, word inMax, word inMin, char *inName);


  // Zustände (Variablen)
  //
  bool  busy;
  char  lastKeyIn;

  // Steuerbits (Kommandobits)
  //
  bool  cFlag[10];
  };

// ----------------------------------------------------------------------------
#endif // Monitor_h
