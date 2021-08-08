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
// Simple type definitions and settings
// ---------------------------------------------------------------------------
//
#undef  byte
#undef  word
#undef  dword

#define byte unsigned char
#define word unsigned short
#define dword unsigned long

#ifdef smnLinGcc64

#undef  byte
#undef  word
#undef  dword

#define byte unsigned char
#define word unsigned short
#define dword unsigned int

#endif

// ---------------------------------------------------------------------------
// Complex type definitions
// ---------------------------------------------------------------------------
// The following types are more complex definitions with Arduino.
// But here simple types are used if possible, because we only care for the
// environment that is needed by Social Manufacturing Network
//

#ifndef IPAddress
  #define IPAddress   byte *
#endif


#endif  // _arduinoDefs_h
