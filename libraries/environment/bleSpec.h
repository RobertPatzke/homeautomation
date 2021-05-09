//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   bleSpec.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   8. Mai 2021
//
// Der Inhalt dieser Datei ist der Blootooth Spezifikation 5.2 entnommen
// bzw. daran angelehnt.
// Insbesondere dem Teil 6, Low Energy Controller
//

#ifndef bleSpec_h
#define bleSpec_h
// ----------------------------------------------------------------------------

#ifndef octet
  typedef unsigned char octet;
#endif

#define VersionBTS  52
#define TypeBTS     BLE

// ----------------------------------------------------------------------------
// Festlegungen für die Bewerbungskanäle (Advertizing Physical Channels)
// ----------------------------------------------------------------------------

// Basisfrequenz in MHz
#define BaseFrequency   2400

// Kanalfrequenzen (Offsets zur Basisfrequenz)
#define FrqOffsAdvChn1  2
#define FrqOffsADvChn2  26
#define FrqOffsAdvChn3  80

// Zugriffsadresse (Access Address)
#define AdvAccAddr  0x8E89BED6

// Geräteadresse
typedef octet  BD_ADR[6];

// Telegrammtypen (PDU Types)
#define ADV_IND           0x0
#define ADV_DIRECT_IND    0x1
#define ADV_NONCONN_IND   0x2
#define SCAN_REQ          0x3
#define SCAN_RSP          0x4
#define CONNECT_IND       0x5
#define ADV_SCAN_IND      0x6

// Kennzeichnung Art der Geräteadresse (Device Address Mark, TxAdd = 1, random)
#define DevAdrType  0x2

// Telegrammkopf ohne Längenbyte
#define HeadS0B     ((ADV_NONCONN_IND << 4) | DevAdrType)
#define HeadS0BS    ((ADV_SCAN_IND << 4) | DevAdrType)

// Datenstruktur für das zu sendende Telegramm
// bei der Bewerbung (Advertising)
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

// Telegrammtypen
//
typedef enum  _blePduType
{
  bptAdv,           // Standard-Bewerbungstelegramm
  bptAux
} blePduType;

// ----------------------------------------------------------------------------
#endif  // bleSpec_h
