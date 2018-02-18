
// ---------------------------------------------------------------------------
// File:        UpdateOTA.cpp
// Editors:     Robert Patzke,
// Start:       17. February 2018
// Last change: 18. February 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//
#include "environment.h"
#include "UpdateOTA.h"

// ---------------------------------------------------------------------------
// Konstruktor
// ---------------------------------------------------------------------------
//
UpdateOTAclass::UpdateOTAclass() : updateServer(80)
{
  enabled = false;
}

// ---------------------------------------------------------------------------
// run      Update handle function, has to be cyclic called
// ---------------------------------------------------------------------------
//
void UpdateOTAclass::run()
{
  if(!enabled) return;

#ifdef smnWebServerOTA
  updateServer.handleClient();
#else
  #ifdef smnArduinoOTA
    ArduinoOTA.handle();
  #endif
#endif
}

// ---------------------------------------------------------------------------
// begin        Preparing device for update
// ---------------------------------------------------------------------------
//
int UpdateOTAclass::begin(char *name)
{
#ifdef smnWebServerOTA

  updState = 0;
  updError = 0;

  if(!MDNS.begin(name))
  {
    updError = 1;
    return(-1);
  }

  updater.setup(&updateServer);
  updateServer.begin();

  MDNS.addService("http", "tcp", 80);

  updState = 1;

#else
  #ifdef smnArduinoOTA

  updState = 1;
  updError = 0;

  ArduinoOTA.setHostname(name);

  ArduinoOTA.onStart([]()
    {
      updState = 3;
    });

  ArduinoOTA.onEnd([]()
    {
      updState = 4;
    });

  ArduinoOTA.onError([](ota_error_t error)
    {
      updError = error + 1;
    });

  ArduinoOTA.begin();
  updState = 2;

  #endif
#endif

  enabled = true;
  return(0);
}

UpdateOTAclass UpdateOTA;
