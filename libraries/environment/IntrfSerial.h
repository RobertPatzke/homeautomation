//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfSerial.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   17. November 2021
//
// Eine Schnittstelle zu den seriellen Schnittstellen in Mikrocontrollern
// oder Boards mit Mikrocontrollern und seriellen schnittstellen
//

#ifndef IntrfSerial_h
#define IntrfSerial_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"
#include "IntrfBuf.h"

typedef enum _SerSpeed
{
  Baud1200,
  Baud2400,
  Baud4800,
  Baud9600,
  Baud14400,
  Baud19200,
  Baud28800,
  Baud31250,
  Baud38400,
  Baud56000,
  Baud57600,
  Baud76800,
  Baud115200,
  Baud230400,
  Baud250000,
  Baud460800,
  Baud921600,
  Baud1Meg
} SerSpeed;

typedef enum _SerType
{
  stStd,      // Typischer Ausgang ca. 2 mA
  stPow,      // Starker Ausgang ca. 10 mA
  stCur       // Open Collector/Drain ca. 10 mA Stromschleife
} SerType;

typedef struct _SerParams
{
  int         inst;           // Nummer (Index) der Ser-Instanz
  int         txdPort;        // Nummer (Index) des Port fuer TX
  int         txdPin;         // Nummer (Index) des Pin fuer TX
  int         rxdPort;        // Nummer (Index) des Port fuer RX
  int         rxdPin;         // Nummer (Index) des Pin fuer RX
  SerSpeed    speed;          // Bitrate (Baud)
  SerType     type;           // Ausgang
} SerParams, *SerParamsPtr;

typedef enum _SerError
{
  SEnoError
} SerError;


class IntrfSerial
{
public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  virtual void  begin(SerParamsPtr serParPtr, IntrfBuf * bufferIf);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  virtual void resuSend();    // Fortsetzen des Interrupt-Sendebetriebs
  virtual void startSend();   // Starten des Sendebetriebs
  virtual void stopSend();    // Anhalten des Sendebetriebs

  virtual void startRec();    // Starten des Empfangsbetriebs
  virtual void stopRec();     // Anhalten des Empfangsbetriebs


  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //
  virtual bool condSend(byte c);  // Bedingtes senden eines Zeichens


};

// ----------------------------------------------------------------------------
#endif  // IntrfRadio_h
