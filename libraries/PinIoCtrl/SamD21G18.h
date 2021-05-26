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
//-----------------------------------------------------------------------------
#endif //_SamD21G18_h
