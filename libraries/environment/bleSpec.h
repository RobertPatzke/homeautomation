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

// ----------------------------------------------------------------------------
#endif  // bleSpec_h
