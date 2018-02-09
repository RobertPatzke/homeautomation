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
  #include ConfFile
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
 const PROGMEM byte confLocationPosX[8] = {'0',' ',' ',' ',' ',' ',' ',' '};
 const PROGMEM byte confLocationPosY[8] = {'0',' ',' ',' ',' ',' ',' ',' '};
 // L3
 const PROGMEM byte confLocationPosZ[8] = {'0',' ',' ',' ',' ',' ',' ',' '};
 const PROGMEM byte confAppCode[8]      = {'0',' ',' ',' ',' ',' ',' ',' '};
 // L4
 const PROGMEM byte confDeviceType[16] =
          {'S','t','d','D','e','v','i','c','e','T','y','p','e',' ',' ',' '};
 // L5
 const PROGMEM byte confIPAddress[16] =
          {'1','9','2','.','1','6','8','.','9','9','.','2','0','1',' ',' '};
 // L6
 const PROGMEM byte confPortBroadcast[4] =                {'4','1','0','0'};
 const PROGMEM byte confPortCfgServer[4] =                {'4','0','0','1'};
 const PROGMEM byte confPortPrgServer[4] =                {'4','0','1','0'};
 const PROGMEM byte confPortExtServer[4] =                {'4','0','1','1'};
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
  int   startServer();


  // -------------------------------------------------------------------------
  // user variables
  // -------------------------------------------------------------------------
  //


};


//-----------------------------------------------------------------------------
#endif
