//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   RowMath.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   21. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen zur Gestaltung eines Ringpuffers.
//

#ifndef RowMath_h
#define RowMath_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "arduinoDefs.h"

#define MaxNrOfFilter 4

typedef struct  _FilterResult
{
  short   avg;
  short   hp;
  short   hp1;
  int     intI;
  int     intII;
  int     sum;
} FilterResult, *FilterResultPtr;

// ----------------------------------------------------------------------------
//                            R o w M a t h
// ----------------------------------------------------------------------------
class RowMath
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //

  //
  //
  short     lastValue;        // zuletzt eingegebener wert
  short     oldValue;         // davor eingegebener Wert
  short     *calcBuffer;      // Buffer for meas values
  word      bufSize;          // Buffer size
  word      writeIdx;         // Write index

  word      calcIdx[MaxNrOfFilter]; // Berechnungsindex
  word      count[MaxNrOfFilter];   // Anzahl Werte Calc
  int       sum[MaxNrOfFilter];     // Messwertsumme
  int       intI[MaxNrOfFilter];    // Erstes Integral
  int       intII[MaxNrOfFilter];   // Zweites Integral
  short     hp[MaxNrOfFilter];      // Hochpass


  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Inline-Funktionen
  // --------------------------------------------------------------------------
  //


public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  // Zuweisen eines Speichers (*bufPtr) der Größe size für den Puffer
  //
  void  begin(int size, short *bufPtr);


  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setEdge(int idx, int count);

  // --------------------------------------------------------------------------
  // Schnittstellen
  // --------------------------------------------------------------------------
  //


  // --------------------------------------------------------------------------
  // Steuerung und Datenaustausch
  // --------------------------------------------------------------------------
  //
  void  enterValue(short val);
  void  filter(int idx, FilterResultPtr result);
  void  resetInts(int idx);

  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //

};


// ----------------------------------------------------------------------------
#endif // beacon_h
