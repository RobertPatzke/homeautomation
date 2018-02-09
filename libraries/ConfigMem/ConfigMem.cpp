//-----------------------------------------------------------------------------
// Thema:   Configuration Memory / Upload/Download and Management
// Datei:   ConfigMem.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "ConfigMem.h"


// ---------------------------------------------------------------------------
// construction and initilisation
// ---------------------------------------------------------------------------
//

ConfigMem::ConfigMem(){;}

ConfigMem::ConfigMem(int nrOfPages)
{
  begin(nrOfPages);
}

void ConfigMem::begin(int nrOfPages)
{
  int     memSize;

  memSize = nrOfPages * PageSizeNVR;
  if(memSize > SizeNVR)
    memSize = SizeNVR;
  EEPROM.begin(memSize);
}


// -------------------------------------------------------------------------
// user functions
// -------------------------------------------------------------------------
//

// -------------------------------------------------------------------------
// access EEPROM, basic configuration (page 0)
// -------------------------------------------------------------------------
//
bool ConfigMem::promHasData()
{
  byte promVal;

  promVal = EEPROM.read(0x90);
  if(promVal != 'V') return(false);
  promVal = EEPROM.read(0x9D);
  if(promVal != 'M') return(false);
  promVal = EEPROM.read(0x9E);
  if(promVal != 'F') return(false);
  promVal = EEPROM.read(0x9F);
  if(promVal != 'P') return(false);
  return(true);
}

void ConfigMem::promInit()
{
  byte promVal;

  for(int i = 0; i < 256; i++)
  {
    promVal = pgm_read_dword_near(confDeviceName + i);
    EEPROM.write(i, promVal);
  }
}


int ConfigMem::startServer()
{

}
