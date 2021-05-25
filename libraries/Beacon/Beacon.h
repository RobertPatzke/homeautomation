//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Beacon.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   8. Mai 2021
//
// Der Inhalt dieser Datei sind Festlegungen zur Gestaltung eines sog. Beacon.
// Das ist ein relativ kleines Datenpaket (ca. 30 Bytes), das zyklisch per
// Rundruf versendet wird (Leuchtfeuer).
// Die Dateninhalte sind variabel, also es können damit genauso definierte
// Daten (iBeacon, Eddystone, etc.) wie auch Messwerte versendet werden.
//

#ifndef Beacon_h
#define Beacon_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "arduinoDefs.h"
#include "bleSpec.h"
#include "IntrfRadio.h"

// Datenstrukturen für die Nutzdaten
// Sie können über eine cast-Anweisung in der Telegrammstruktur
// an der Stelle .data bedient werden
//
typedef struct _measBase
{
  byte  counter;    // zyklischer Telegrammmzähler
  byte  type;       // Kennzeichnung der Datenstruktur
  byte  id;         // Kennzeichnung der Inhalte (Verteilung)
  dword time;       // Zeit der Messwertermittlung
  word  meas[12];   // Liste/Array der Messwerte
} measBase, *measBasePtr;

// Datentypen (type in measBase) bzw. Beacontypen
// Bei Beacons von anderen Autoren (z.B. iBeacon oder Eddystone) ist die
// Datenstruktur spezifisch und der Typ des Beacon kann beim Empfang nicht
// über das Element measBase.type ermittelt werden.
//
typedef enum _mbcType
{
  iBeacon,            // von Apple definiert, Datenstruktur im Internet
  eddy,               // von Google definiert, Datenstruktur im Internet
  mbcBasic,           // zyklische Messwertübertragung, no scan, no connection
  mbcPlus             // zyklische Messw.. mit Nachfrage (scan)
} mbcType, *mbcTypePtr;

// Identifikator für die Art der Daten
//
typedef enum _measId
{
  app                 // Gestaltung/Bedeutung der Daten aus Anwendung
} measId, *measIdPtr;



// ----------------------------------------------------------------------------
//                            B e a c o n
// ----------------------------------------------------------------------------
class Beacon
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  IntrfRadio  *radio;
  bcPdu       pdu;

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void  init01(mbcType type, measId id);

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Beacon(IntrfRadio *inRadio, mbcType type, measId id);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setDevAddress(BD_ADR bdAdr);
  void setDevAddress(word head, dword body);
  void setRadioInterface(IntrfRadio * inRadio);

  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void  start();
  int   send();
  int   send(TxStatePtr refState, int chnr);

};


// ----------------------------------------------------------------------------
#endif // beacon_h
