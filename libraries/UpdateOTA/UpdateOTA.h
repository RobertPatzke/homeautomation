
// ---------------------------------------------------------------------------
// File:        UpdateOTA.h
// Editors:     Robert Patzke,
// Start:       17. February 2018
// Last change: 18. February 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#ifndef _UpdateOTA_h
#define _UpdateOTA_h
// ---------------------------------------------------------------------------
//


//#define smnArduinoOTA
#define smnWebServerOTA

#ifdef smnArduinoOTA
  #include "ArduinoOTA.h"
#endif

#ifdef smnWebServerOTA
  #include "ESP8266WebServer.h"
  #include "ESP8266mDNS.h"
  #include "ESP8266HTTPUpdateServer.h"
#endif

// ---------------------------------------------------------------------------
// class UpdateOTA
// ---------------------------------------------------------------------------
//

class UpdateOTAclass
{
private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  bool  enabled;                 // Set true to enable handling

#ifdef smnWebServerOTA

  #ifdef smnESP8266
  ESP8266WebServer          updateServer;
  ESP8266HTTPUpdateServer   updater;
  #endif

#endif


public:
  // -------------------------------------------------------------------------
  // Konstruktor
  // -------------------------------------------------------------------------
  //
  UpdateOTAclass();

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void      run();               // has to be cyclic called
  int       begin(char *name);   // call for starting update

  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //
  int  updState;
  int  updError;
  int  updProgress;

};

extern UpdateOTAclass UpdateOTA;

// ---------------------------------------------------------------------------
#endif // _UpdateOTA_h
