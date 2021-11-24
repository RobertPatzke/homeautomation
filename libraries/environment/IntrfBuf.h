//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   IntrfBuf.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   22. November 2021
//
// Eine Schnittstelle zu Puffern (speziell Ringpuffer für Kommunikation)
//

#ifndef IntrfBuf_h
#define IntrfBuf_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"


class IntrfBuf
{
public:
  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //


  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //
  virtual bool  getByteSnd(byte *dest);   // Byte aus Puffer zum Senden holen
                                          // Rückgabe FALSE: Puffer leer

};

// ----------------------------------------------------------------------------
#endif  // IntrfBuf_h
