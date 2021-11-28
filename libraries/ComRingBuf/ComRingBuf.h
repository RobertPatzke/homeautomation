//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   ComRingBuf.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   21. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen zur Gestaltung eines Ringpuffers.
//

#ifndef ComRingBuf_h
#define ComRingBuf_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "arduinoDefs.h"
#include "IntrfBuf.h"
#include "IntrfSerial.h"

#define NewLineModeCR     0x01
#define NewLineModeNL     0x02

// ----------------------------------------------------------------------------
//                            C o m R i n g B u f
// ----------------------------------------------------------------------------
class ComRingBuf : IntrfBuf
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  // Zugang zur Peripherie
  //
  IntrfSerial *serIf;

  // Lesen und Schreiben von Zeichen (Bytes)
  //
  byte      *ptrSend;         // Der (veraenderliche) Sendezeiger
  byte      *ptrRec;          // Der (veraenderliche) Empfangszeiger
  int       maxRec;           // Maximale Anzahl zu empfangender Bytes
  byte      endChrRec;        // Abschlusszeichen beim Empfang
  byte      condMaskCom;      // Bedingungen fuer den Datenaustausch
  byte      newLineMode;      // Art für eine neue Zeile (CR/LF)

  byte      *recBuffer;       // Receive ring buffer start address
  word      rbReadIdx;        // Read index
  word      rbWriteIdx;       // Write index
  word      rbSize;           // Buffer size

  byte      *sndBuffer;       // Transmit ring buffer start address
  word      sbReadIdx;        // Read index
  word      sbWriteIdx;       // Write index
  word      sbSize;           // Buffer size

  int       loopCount;        // For internal time out checking
  int       reqChkState;      // State of request/check procedure
  int       tmpVal;           // Variable for temporary data storage
  int       tmpIdx;           // Variable for temporary array index


  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  char  getC();
  int   putNL();

  // --------------------------------------------------------------------------
  // Inline-Funktionen
  // --------------------------------------------------------------------------
  //
  void putBufB(byte b)
  {
    sndBuffer[sbWriteIdx] = b;
    sbWriteIdx++;
    if(sbWriteIdx >= sbSize)
      sbWriteIdx = 0;
  }

  int getSpace()
  {
    int space = sbReadIdx - sbWriteIdx - 1;
    if(space < 0) space += sbSize;
    return(space);
  }


public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------
  ComRingBuf();

  void begin(IntrfSerial *ser);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setNewLineMode(byte nlMode);

  // --------------------------------------------------------------------------
  // Schnittstellen
  // --------------------------------------------------------------------------
  //
  bool  getByteSnd(byte *dest);
  void  putByteRec(byte b);       // Byte vom Empfang an Puffer geben


  // Zuweisen eines Speichers (*bufPtr) der Größe size für den Lesepuffer
  //
  void  setReadBuffer(int size, byte *bufPtr);


  // --------------------------------------------------------------------------
  // Steuerung
  // --------------------------------------------------------------------------
  //

  // ----------------------------------------------
  // Ein einzelnes Zeichen aus dem Ringpuffer lesen
  // ----------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst das älteste Zeichen aus dem Ringpuffer
  //
  int   getChr();

  // ----------------------------------------------
  // Löschen des Emmpfangspuffers
  // ----------------------------------------------
  //
  void  clrRecBuf();

  // --------------------------------------------------
  // Alle empfangenen Zeichen aus dem Ringpuffer lesen
  // --------------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst die Anzahl der empfangenen Zeichen
  //
  int   getAll(byte *buffer);

  // ----------------------------------------------------------
  // Begrenzte Anzahl empfangener Zeichen aus Ringpuffer lesen
  // ----------------------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst die Anzahl der ausgelesenen Zeichen
  //
  int   getCount(int count, byte *buffer);

  // ------------------------------------------------------------------------
  // Begrenzte Anzahl Zeichen als 0-terminierten String aus Ringpuffer lesen
  // ------------------------------------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst die Anzahl der ausgelesenen Zeichen
  //
  int   getCountStr(int count, char *buffer);

  // ---------------------------------------------------------
  // Die nächste Zeile (Zeichen bis CR und/oder LF) als String
  // ---------------------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst die Anzahl der ausgelesenen Zeichen
  //
  int   getLine(char *buffer);

  // -----------------------------------------------
  // Die nächste im Puffer enthaltene Zeile auslesen
  // und die darin enthaltene Dezimalzahl übergeben
  // -----------------------------------------------
  // Rückgabe EOF (-1), wenn kein Zeichen vorhanden
  // sonst die Anzahl der ausgelesenen Zeichen
  // oder 0, wenn keine Dezimalzahl enthalten war
  //
  int   getLineDec(int *intValue);

  // ---------------------------------------------------------
  // Warten auf Zeile (Zeichen bis CR und/oder LF) als String
  // ---------------------------------------------------------
  // Rückgabe EOF (-1), wenn Wartezyklen verstrichen
  // Rückgabe 0, solange kein Zeilenende gelesen
  // sonst die Anzahl der ausgelesenen Zeichen
  //
  int   waitLine(int waitLoop, char *buffer);

  //int   waitLineDec(int waitLoop, int *intValue);

  // ---------------------------------------------------------
  // Testen der Zeile im Puffer
  // ---------------------------------------------------------
  // Rückgabe 0, wenn Teststring (noch) nicht enthalten
  // sonst die Länge des Teststring
  // EOF, wenn Zeile im Puffer (gelöscht) nicht passte
  //
  int   chkLine(char *rsp);

  int   chkBuf(char *rsp);
  int   waitAll(int waitLoop, byte *buffer);
  int   waitChkBuf(int waitLoop, char *rsp);
  int   inCount(void);
  int   getRestChar(byte tagChr, int len, byte *buffer);
  int   getRestStr(char *tagStr, int len, byte *buffer);
  int   reqChkLine(char *req, char *rsp);

  void  setWriteBuffer(int size, byte *bufPtr);
  int   putChr(int chr);
  int   putStr(char *msg);
  int   putSeq(byte *msg, int n);
  int   putLine(char *msg);
  //int   putLine(char *msg, char c);
  //int   putLine(char *msg, int n);

  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //

};


// ----------------------------------------------------------------------------
#endif // beacon_h
