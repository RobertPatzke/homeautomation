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

void ConfigMem::promClear()
{
  for(int i = 0; i < 256; i++)
    EEPROM.write(i, 0);
}

void ConfigMem::promInit()
{
  byte promVal = 0;

  for(int i = 0; i < 256; i++)
  {
    promVal = pgm_read_byte_near(confDeviceName + i);
    EEPROM.write(i, promVal);
  }
}

void  ConfigMem::getIpAddress(byte *bList)
{
  for(int i = 0; i < 4; i++)
    bList[i] = EEPROM.read(0x50 + i);
  bList[4] = EEPROM.read(0x5A);
}

void  ConfigMem::getMacAddress(byte *bList)
{
  for(int i = 0; i < 6; i++)
    bList[i] = EEPROM.read(0x54 + i);
}

void  ConfigMem::getPorts(byte *bList)
{
  for(int i = 0; i < 8; i++)
    bList[i] = EEPROM.read(0x60 + i);
}

void ConfigMem::getNetName(byte *bList)
{
  byte val;

  for(int i = 0; i < 16; i++)
  {
    val = EEPROM.read(0xA0 + i);
    if(val != ' ')
      bList[i] = val;
    else
    {
      bList[i] = 0;
      break;
    }
  }
}

void ConfigMem::getNetPass(byte *bList)
{
  byte val;

  for(int i = 0; i < 16; i++)
  {
    val = EEPROM.read(0xB0 + i);
    if(val != ' ')
      bList[i] = val;
    else
    {
      bList[i] = 0;
      break;
    }
  }
}


bool ConfigMem::getDhcp()
{
  byte val = EEPROM.read(0x5B);
  if(val == 0)
    return (false);
  else
    return(true);
}



int ConfigMem::startServer()
{

}
