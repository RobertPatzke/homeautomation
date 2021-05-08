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

#include "arduinoDefs.h"
#include "bleSpec.h"

// Datenstruktur für das zu sendende Telegramm
//
typedef struct _bcPdu
{
  byte  head;       // Header = PDU-Typ und Adresskennung (S0 bei nRF52840)
  byte  len;        // Länge des Telegramms inkl. Adresse (LENGTH bei nRF52840)
  byte  adr0;       // niedrigstwertiges Adressbyte (S1 bei nRF52840)
  byte  adr1;       //
  byte  adr2;       //      Das ist die Geräteadresse, die hier wahlfrei ist
  byte  adr3;       //      Sie wird zur Identifikation des Gerätes verwendet
  byte  adr4;       //
  byte  adr5;       // höchstwertiges Addressbyte
  byte  data[31];   // Nutzdaten (maximale Anzahl nach BLE-Spez.)
} bcPdu, *bcPduPtr;

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
  bcPdu   pdu;

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  Beacon(mbcType type, measId id);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // Steuerung des Beacon
  // --------------------------------------------------------------------------
  //
  void start();

};


// ----------------------------------------------------------------------------
#endif // beacon_h
