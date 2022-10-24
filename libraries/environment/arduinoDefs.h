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
// Constant Values
// ---------------------------------------------------------------------------
//
#ifndef PI
#define PI          3.1415926535897932384626433832795
#endif
#ifndef HALF_PI
#define HALF_PI     1.5707963267948966192313216916398
#endif
#ifndef TWO_PI
#define TWO_PI      6.283185307179586476925286766559
#endif
#ifndef DEG_TO_RAG
#define DEG_TO_RAD  0.017453292519943295769236907684886
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG  57.295779513082320876798154814105
#endif
#ifndef EULER
#define EULER       2.718281828459045235360287471352
#endif

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
