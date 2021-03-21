//-----------------------------------------------------------------------------
// Thema:       Configuration Memory
// Datei:       ConfigMem.cpp
// Editors:     Robert Patzke,
// Start:       09. Februar 2018
// Last change: 10. Februar 2021
// URI/URL:     www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "ConfigMem.h"

//#define ConfigMemDebug

#ifdef ConfigMemDebug

#define dp(x)   Serial.print(x)
#define dpl(x)  Serial.println(x)
char cmds[128];
byte cmds_byte[65];
char cmds_char[33];

#endif


// ---------------------------------------------------------------------------
// construction and initilisation
// ---------------------------------------------------------------------------
//

ConfigMem::ConfigMem(){;} // @suppress("Class members should be properly initialized")

void ConfigMem::begin(int nrOfPages)
{
  int           memSize;
  unsigned long startMics;

  startMics = micros();

  memSize = nrOfPages * PageSizeNVR;
  if(memSize > SizeNVR)
    memSize = SizeNVR;

#if defined(smnESP32) || defined(smnESP8266)
  EEPROM.begin(memSize);
#endif

  durBegin = micros() - startMics;
}

// -------------------------------------------------------------------------
// auxiliary functions
// -------------------------------------------------------------------------
//
char ConfigMem::hex(byte b)
{
  char retv;

  retv = (b & 0x0F) + 0x30;
  if(retv > 0x39) retv += 7;
  return(retv);
}

unsigned short  ConfigMem::getWordProm(int adr)
{
  unsigned short retv;
  retv = ((unsigned short)EEPROM.read(adr) << 8) + EEPROM.read(adr + 1);
  return(retv);
}

void  ConfigMem::setWordProm(int adr, unsigned short value, bool save)
{
  byte  val;

  val = (byte) (value >> 8);
  EEPROM.write(adr, val);
  val = (byte) (value & 0x0FF);
  EEPROM.write(adr+1, val);

  if(save)
    commitIncCounter();
}

unsigned long ConfigMem::getDwordProm(int adr)
{
  unsigned long retv;
  retv =  ((unsigned long)EEPROM.read(adr) << 24) +
          ((unsigned long)EEPROM.read(adr+1) << 16) +
          ((unsigned long)EEPROM.read(adr+2) << 8) + EEPROM.read(adr+3);
  return(retv);
}

void  ConfigMem::setDwordProm(int adr, unsigned long value, bool save)
{
  byte  val;

  val = (byte) (value >> 24);
  EEPROM.write(adr, val);
  val = (byte) (value >> 16);
  EEPROM.write(adr+1, val);
  val = (byte) (value >> 8);
  EEPROM.write(adr+2, val);
  val = (byte) (value & 0x0FF);
  EEPROM.write(adr+3, val);

  if(save)
    commitIncCounter();
}


void ConfigMem::copyProg(int adr, const byte *src, int cnt)
{
  int   i;
#ifdef ConfigMemDebug
  int   j = 0;
#endif

  for(i = 0; i < cnt; i++)
  {
    byte promVal;

    promVal = pgm_read_byte_near(src + i);
    EEPROM.write(adr + i, promVal);

#ifdef ConfigMemDebug
    cmds_byte[j++] = ' ';
    cmds_byte[j++] = hex(promVal >> 4);
    cmds_byte[j++] = hex(promVal);
    if((promVal < 0x20) || (promVal > 0x7F)) cmds_char[i] = '.';
    else cmds_char[i] = (char) promVal;
#endif
  }
#ifdef ConfigMemDebug
  cmds_byte[j] = 0;
  cmds_char[i] = '\0';
#endif
}

#ifdef ConfigMemDebug

char hexDeb(byte b)
{
  char retv;

  retv = (b & 0x0F) + 0x30;
  if(retv > 0x39) retv += 7;
  return(retv);
}


void cmdPrintData(int promAdr,char *lab)
{
  sprintf(cmds,"%s [%02X]   %s    %s",lab,promAdr,cmds_byte,cmds_char);
  dpl(cmds);
  delay(0);
}

void cmdGetPrintData(int promAdr,char *lab,int cnt)
{
  byte  promVal;
  int   i;
  int   j = 0;

  for(i = 0; i < cnt; i++)
  {
    promVal = EEPROM.read(promAdr + i);
    cmds_byte[j++] = ' ';
    cmds_byte[j++] = hexDeb(promVal >> 4);
    cmds_byte[j++] = hexDeb(promVal);
    if((promVal < 0x20) || (promVal > 0x7F)) cmds_char[i] = '.';
    else cmds_char[i] = (char) promVal;
  }
  cmds_byte[j] = 0;
  cmds_char[i] = 0;
  cmdPrintData(promAdr, lab);
}
#endif

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

#ifdef ConfigMemDebug
  dpl("Debug ConfigMem.promHasData");
#endif

  promVal = EEPROM.read(cdaCoVeDa);
#ifdef ConfigMemDebug
  dp(promVal); dp(" ");
  dpl((char) promVal);
#endif
  if(promVal != 'V') return(false);

  promVal = EEPROM.read(cdaCoVeDa + 0x0D);
#ifdef ConfigMemDebug
  dp(promVal); dp(" ");
  dpl((char) promVal);
#endif
  if(promVal != 'M') return(false);

  promVal = EEPROM.read(cdaCoVeDa + 0x0E);
#ifdef ConfigMemDebug
  dp(promVal); dp(" ");
  dpl((char) promVal);
#endif
  if(promVal != 'F') return(false);

  promVal = EEPROM.read(cdaCoVeDa + 0x0F);
#ifdef ConfigMemDebug
  dp(promVal); dp(" ");
  dpl((char) promVal);
#endif
  if(promVal != 'P') return(false);
  return(true);
}

void ConfigMem::promClear()
{
  unsigned long startMics;

  startMics = micros();

  for(int i = 0; i < 256; i++)
    EEPROM.write(i, 0);

  durClear = micros() - startMics;
}

void ConfigMem::promInit()
{
  unsigned long startMics;
  int           promAdr = 0;

  startMics = micros();

#ifdef ConfigMemDebug
  dpl(" Debug ConfigMem.promInit()");
#endif

  copyProg(promAdr,confDeviceName, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confDeviceName   ");
#endif
  promAdr += 16;

  copyProg(promAdr,confLocationName, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confLocationName ");
#endif
  promAdr += 16;

  copyProg(promAdr,confLocationPosX, 4);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confLocationPosX ");
#endif
  promAdr += 4;

  copyProg(promAdr,confLocationPosY, 4);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confLocationPosY ");
#endif
  promAdr += 4;

  copyProg(promAdr,confLocationPosZ, 4);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confLocationPosZ ");
#endif
  promAdr += 4;

  copyProg(promAdr,confAppCode, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confAppCode      ");
#endif
  promAdr += 2;

  copyProg(promAdr,confAppKey, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confAppKey       ");
#endif
  promAdr += 2;

  copyProg(promAdr,confReserve1, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confReserve1     ");
#endif
  promAdr += 16;

  copyProg(promAdr,confDeviceType, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confDeviceType   ");
#endif
  promAdr += 16;

  copyProg(promAdr,confIPAddress, 4);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confIPAddress    ");
#endif
  promAdr += 4;

  copyProg(promAdr,confMacAddress, 6);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confMacAddress   ");
#endif
  promAdr += 6;

  copyProg(promAdr,confExtNet, 6);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confExtNet       ");
#endif
  promAdr += 6;

  copyProg(promAdr,confPortBroadcast, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confPortBroadcast");
#endif
  promAdr += 2;

  copyProg(promAdr,confPortCfgServer, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confPortCfgServer");
#endif
  promAdr += 2;

  copyProg(promAdr,confPortPrgServer, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confPortPrgServer");
#endif
  promAdr += 2;

  copyProg(promAdr,confPortExtServer, 2);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confPortExtServer");
#endif
  promAdr += 2;

  copyProg(promAdr,confPerPins, 8);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confReserve2     ");
#endif
  promAdr += 8;

  copyProg(promAdr,confAuthorPrim, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confAuthorPrim   ");
#endif
  promAdr += 16;

  copyProg(promAdr,confAuthorSec, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confAuthorSec    ");
#endif
  promAdr += 16;

  copyProg(promAdr,confVersionDate, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confVersionDate  ");
#endif
  promAdr += 16;

  copyProg(promAdr,confNetwork, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confNetwork      ");
#endif
  promAdr += 16;

  copyProg(promAdr,confPass, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confPass         ");
#endif
  promAdr += 16;

  copyProg(promAdr,confCompanyPrim, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confCompanyPrim  ");
#endif
  promAdr += 16;

  copyProg(promAdr,confCompanySec, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confCompanySec   ");
#endif
  promAdr += 16;

  copyProg(promAdr,confDevTwitter, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confDevTwitter   ");
#endif
  promAdr += 16;

  copyProg(promAdr,confDevFollower, 16);
#ifdef ConfigMemDebug
  cmdPrintData(promAdr, (char *) "confDevFollower  ");
#endif

#if defined(smnESP32) || defined(smnESP8266)
  commitIncCounter();
#endif

  durInit = micros() - startMics;
}


void  ConfigMem::getIpAddress(byte *bList)
{
  for(int i = 0; i < 4; i++)
    bList[i] = EEPROM.read(cdaCoIPA + i);   // IP-Address
  bList[4] = EEPROM.read(cdaCoIPA + 0x0A);  // Gateway-Address (Device)

#ifdef ConfigMemDebug
  sprintf(cmds,"ConfigMem.getIpAddress: %d.%d.%d.%d  %d",bList[0],bList[1],bList[2],bList[3],bList[4]);
  dpl(cmds);
#endif
}

void  ConfigMem::getMacAddress(byte *bList)
{
  for(int i = 0; i < 6; i++)
    bList[i] = EEPROM.read(cdaCoIPA + 0x04 + i);
}


bool ConfigMem::getDhcp()
{
  byte val = EEPROM.read(cdaCoIPA + 0x0B);
  if(val == 0)
    return (false);
  else
    return(true);
}


void  ConfigMem::getPorts(byte *bList)
{
  for(int i = 0; i < 8; i++)
    bList[i] = EEPROM.read(cdaCoPoLi + i);
}

void ConfigMem::getNetName(byte *bList)
{
  byte val;

  for(int i = 0; i < 16; i++)
  {
    val = EEPROM.read(cdaCoNet + i);
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
    val = EEPROM.read(cdaCoPass + i);
    if(val != ' ')
      bList[i] = val;
    else
    {
      bList[i] = 0;
      break;
    }
  }
}


void ConfigMem::getDevTwitterName(byte *bList)
{
  byte val;

  for(int i = 0; i < 16; i++)
  {
    val = EEPROM.read(cdaCoDevTw + i);
    if(val != ' ')
      bList[i] = val;
    else
    {
      bList[i] = 0;
      break;
    }
  }
}


void ConfigMem::getDeviceName(byte *bList)
{
  byte val;

  for(int i = 0; i < 16; i++)
  {
    val = EEPROM.read(cdaCoDeNa + i);
    if(val != ' ')
      bList[i] = val;
    else
    {
      bList[i] = 0;
      break;
    }
  }
}

long   ConfigMem::getPos(int xyz)
{
  long retv;

  int adr = cdaCoLoPo + (xyz << 2);
  retv = getDwordProm(adr);
  return(retv);
}

int   ConfigMem::getConfPin()
{
  byte  val;

  val = EEPROM.read(cdaCoPoLi + 0x0A);
  if(val == 0x0FF)
    return(-1);
  else
    return(val);
}

bool ConfigMem::getAppKey(int * appKey)
{
  byte  val;
  val = EEPROM.read(cdaCoIPA + 0x5C);
  if(val == 0)
    return(false);
  *appKey = getWordProm(cdaCoLoPo + 0x0E);
  return(true);
}


void  ConfigMem::incOperCounter(bool save)
{
  unsigned long val;

  val = getDwordProm(cdaCoStats);
  val++;
  setDwordProm(cdaCoStats, val, save);
}

void  ConfigMem::setOperCounter(unsigned long value, bool save)
{
  setDwordProm(cdaCoStats, value, save);
}

unsigned long ConfigMem::getOperCounter()
{
  return(getDwordProm(cdaCoStats));
}


void  ConfigMem::incOnOffCounter(bool save)
{
  unsigned short val;

  val = getWordProm(cdaCoStats+4);
  val++;
  setWordProm(cdaCoStats+4, val, save);
}

void  ConfigMem::setOnOffCounter(unsigned short value, bool save)
{
  setWordProm(cdaCoStats+4, value, save);
}

unsigned short  ConfigMem::getOnOffCounter()
{
  return(getWordProm(cdaCoStats+4));
}

void  ConfigMem::incOnOffValue(bool save)
{
  unsigned short val;

  val = getWordProm(cdaCoStats+6);
  val++;
  setWordProm(cdaCoStats+6, val, save);
}

void  ConfigMem::setOnOffValue(unsigned short value, bool save)
{
  setDwordProm(cdaCoStats+6, value, save);
}

int  ConfigMem::getOnOffValue()
{
  return(getWordProm(cdaCoStats+6));
}


void  ConfigMem::commitIncCounter()
{
  unsigned long val;

  val = getDwordProm(cdaCoStats+8);
  val++;
  setDwordProm(cdaCoStats+8, val, false);
  EEPROM.commit();
}



int ConfigMem::startServer()
{
  return(0);
}
