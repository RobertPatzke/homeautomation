//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Definitions instead Arduino
// Datei:   arduinoDefs.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//
// These definitions shall encapsulate the Arduino environment
// The file should be included instead of Arduino.h for arbitrary environments
//

#ifndef _arduinoDefs_h
#define _arduinoDefs_h

// ---------------------------------------------------------------------------
// Simple type definitions
// ---------------------------------------------------------------------------
//
#ifndef byte
  #define byte      unsigned char
#endif

// ---------------------------------------------------------------------------
// Complex type definitions
// ---------------------------------------------------------------------------
// The following types are more complex definitions with Arduino.
// But here simple types are used if possible, because we only care for the
// environment that is needed by Social Manufacturing Network
//

#define IPAddress   byte *



#endif  // _arduinoDefs_h
