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

int ConfigMem::startServer()
{

}
