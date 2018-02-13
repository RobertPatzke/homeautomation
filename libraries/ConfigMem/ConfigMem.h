//-----------------------------------------------------------------------------
// Thema:   Configuration Memory / Upload/Download and Management
// Datei:   ConfigMem.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "environment.h"

#ifdef smnSloeber
  #include "Arduino.h"
#endif

#ifndef _ConfigMem_h
#define _ConfigMem_h
//-----------------------------------------------------------------------------

#if defined(smnESP8266) || defined(smnESP32)
  #include  "EEPROM.h"
  #define   SizeNVR     4096
#endif

#ifdef smnSAM3X

 not implemented jet

#endif

#define PageSizeNVR     256

// ---------------------------------------------------------------------------
// Original configuration data (defined with the IDE)
// ---------------------------------------------------------------------------

// I defined the following literals in my properties/settings for the compiler
// with -DsmnSpecificConfig and -DConfFile=\"myConfFile\"
#ifdef smnSpecificConfig

#define CONFIG_PAGE0
#include"/media/win/work/Hochschule/Forschung/Heimautomatisierung/slws8266/checkSo4ch/Sonoff4chConf.h"
#else

 // Page 0
 // -------------------------------------------------------------------------
 // L0
 const PROGMEM byte confDeviceName[16] =
          {'M','y','D','e','v','i','c','e','N','a','m','e',' ',' ',' ',' '};
 // L1
 const PROGMEM byte confLocationName[16] =
          {'M','y','L','i','v','i','n','g','R','o','o','m',' ',' ',' ',' '};
 // L2
 const PROGMEM byte confLocationPosX[4] =                         {0,0,0,0};
 const PROGMEM byte confLocationPosY[4] =                         {0,0,0,0};
 const PROGMEM byte confLocationPosZ[4] =                         {0,0,0,0};
 const PROGMEM byte confAppCode[2]      =                             {0,0};
 const PROGMEM byte confAppKey[2]       =                             {0,0};

 // L3
 const PROGMEM byte confReserve1[16]    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

 // L4
 const PROGMEM byte confDeviceType[16] =
          {'S','t','d','D','e','v','i','c','e','T','y','p','e',' ',' ',' '};
 // L5
 const PROGMEM byte confIPAddress[4] =                     {192,168,90,221};
 const PROGMEM byte confMacAddress[6] =     {0x90,0xA2,0xDA,0x0F,0x1B,0x88};
 const PROGMEM byte confExtNet[6] =                   {1,1,' ',' ',' ',' '};
                                                // gateway,dhcp
 // L6
 const PROGMEM byte confPortBroadcast[2] =         {4100 >> 8,4100 & 0x0FF};
 const PROGMEM byte confPortCfgServer[2] =         {4001 >> 8,4001 & 0x0FF};
 const PROGMEM byte confPortPrgServer[2] =         {4010 >> 8,4010 & 0x0FF};
 const PROGMEM byte confPortExtServer[2] =         {4011 >> 8,4011 & 0x0FF};
 const PROGMEM byte confReserve2[8]     = {' ',' ',' ',' ',' ',' ',' ',' '};
 // L7
 const PROGMEM byte confAuthorPrim[16] =
          {'M','y','N','a','m','e',',','M','y','N','a','m','e',' ',' ',' '};
 // L8
 const PROGMEM byte confAuthorSec[16] =
          {'H','e','r','N','a','m','e',',','H','e','r','n','a','m','e',' '};
 // L9
 const PROGMEM byte confVersionDate[16] =
          {'V','2','0','1','8','0','2','0','9','1','7','2','3','M','F','P'};
 //         |      V left and MFP right are mandatory           |   |   |
 //                for checking programmed EEPROM with promData()
 // LA
 const PROGMEM byte confNetwork[16] =
          {'M','P','Z','-','L','a','b','o','r',' ',' ',' ',' ',' ',' ',' '};
 // LB
 const PROGMEM byte confPass[16] =
          {'M','P','Z','M','P','Z','M','P','Z',' ',' ',' ',' ',' ',' ',' '};
 // LC
 const PROGMEM byte confCompanyPrim[16] =
          {'H','S',' ','H','a','n','n','o','v','e','r',' ',' ',' ',' ',' '};
 // LD
 const PROGMEM byte confCompanySec[16] =
          {'S','t','u','d','e','n','t','-','A','t','-','H','o','m','e',' '};
 // LE
 const PROGMEM byte confDevTwitter[16] =
          {'D','H','A','_','S','w','i','t','c','h',' ',' ',' ',' ',' ',' '};
 // LF
 const PROGMEM byte confDevFollower[16] =
          {'D','H','A','_','P','e','r','s','o','n',' ',' ',' ',' ',' ',' '};



#endif



// ---------------------------------------------------------------------------
// class ConfigMem
// ---------------------------------------------------------------------------

class ConfigMem
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //


private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //


  // -------------------------------------------------------------------------
  // local functions
  // -------------------------------------------------------------------------
  //
   char hex(byte b);
   unsigned short getWordProm(int adr);
   unsigned long  getDwordProm(int adr);
   void copyProg(int adr, const byte *src, int cnt);

public:
  // -------------------------------------------------------------------------
  // construction and initilisation
  // -------------------------------------------------------------------------
  //
  ConfigMem(int nrOfPages);
  ConfigMem();

  void begin(int nrOfPages);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  bool  promHasData();              // Check if EEPROM has data
  void  promInit();                 // copy base data to page 0
  void  promClear();                // delete page 0
  void  getIpAddress(byte *bList);
  void  getMacAddress(byte *bList);
  void  getPorts(byte *bList);
  void  getNetName(byte *bList);
  void  getNetPass(byte *bList);
  bool  getDhcp();
  int   getPosX();
  int   getPosY();
  int   getPosZ();

  void  getDevTwitterName(byte *bList);
  int   getApplicationKey();

  int   startServer();


  // -------------------------------------------------------------------------
  // user variables
  // -------------------------------------------------------------------------
  //


};


//-----------------------------------------------------------------------------
#endif
