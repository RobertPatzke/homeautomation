//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    SamD21G18.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _SamD21G18_h
#define _SamD21G18_h
//-----------------------------------------------------------------------------

typedef struct 
{
  volatile  dword   DIR;         // Data Direction
  volatile  dword   DIRCLR;      // Data Direction Clear
  volatile  dword   DIRSET;      // Data Direction Set
  volatile  dword   DIRTGL;      // Data Direction Toggle
  volatile  dword   OUT;         // Data Output Value
  volatile  dword   OUTCLR;      // Data Output Value Clear
  volatile  dword   OUTSET;      // Data Output Value Set
  volatile  dword   OUTTGL;      // Data Output Value Toggle
  volatile  dword   IN;          // Data Input Value
  volatile  dword   CTRL;        // Control
  volatile  dword   WRCONFIG;    // Write Configuration
  volatile  dword   Reserved;
  volatile  byte    PMUX[16];    // Peripheral Multiplexing
  volatile  byte    PINCFG[32];  // Pin Configuration
} PortReg, *PortRegPtr;

#define PortA   0x41004400
#define PortB   0x41004480

#define PaPtr   ((PortRegPtr) PortA)
#define PbPtr   ((PortRegPtr) PortB)

#define Dio00   { PaPtr, MSK(11) }
#define Dio01   { PaPtr, MSK(10) }
#define Dio02   { PaPtr, MSK(15) }
#define Dio03   { PaPtr, MSK(09) }
#define Dio04   { PaPtr, MSK(08) }
#define Dio05   { PaPtr, MSK(13) }
#define Dio06   { PaPtr, MSK(20) }
#define Dio07   { PaPtr, MSK(21) }
#define Dio08   { PaPtr, MSK(06) }
#define Dio09   { PaPtr, MSK(07) }
#define Dio10   { PaPtr, MSK(18) }
#define Dio11   { PaPtr, MSK(16) }
#define Dio12   { PaPtr, MSK(11) }
#define Dio13   { PaPtr, MSK(17) }
#define Dio20   { PaPtr, MSK(22) }
#define Dio21   { PaPtr, MSK(23) }

//-----------------------------------------------------------------------------
#endif //_SamD21G18_h
