//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   environment.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef _environment_h
#define _environment_h

#ifdef  smnDEFBYBUILD

// Alternative Festlegungen durch Settings in der Build-Umgebung der IDE
// Das erspart die Verwaltung mehrerer/unterschiedlicher environment.h Dateien
// in den Projekten
//
  #ifdef smnUBUNTU_ECLIPSE
  // Meine Entwicklungs- und Testumgebung auf dem Notebook unter UBUNTU
  // für Sketche, die auf dem PC laufen (u.a. zum Debuggen)

    #define smnSimLinux
    #define smnNotebook
    #define smnLINUX
    #define smnDebugLinuxConsole

  #endif

  #ifdef smnWIN32_VS
  // Meine Entwicklungs- und Testumgebung auf dem Notebook unter Windows
  // für Sketche, die auf dem PC laufen (u.a. zum Debuggen)

    #define smnSimWindows
    #define smnNotebook
    #define smnWIN32
    #define smnDebugWindowsConsole

  #endif

  #ifdef smnSLOELIN
  // Mit Sloeber auf Ubuntu/Linux für Entwicklungen zum ESP32
  //

    #define smnDebugArduino
    #define smnSloeber
    #define smnESP32_DEV
    #define smnESP32
    #define smnSerial Serial
  #endif

  #ifdef smnSLOEDUE
  // Mit Sloeber auf Ubuntu/Linux für Entwicklungen zum Arduino Due
  //

    #define smnDebugArduino
    #define smnSloeber
    #define smnArduinoDUE
    #define smnArduinoShieldEth
    #define smnSAM3X
    #define smnSerial Serial
  #endif

  #ifdef smnNANOBLE33
    #define smnDebugArduino
    #define smnSloeber
    #define smnArduinoNanoBLE33
    #define smnNRF52840
    #define smnSerial Serial
  #endif

  #ifdef smnSLOESAMD21
    #define smnDebugArduino
    #define smnSloeber
    #define smnArduinoZeroSamD21
    #define smnSAMD21G18
    #define smnSD21MINI
    #define smnSerial SerialUSB
  #endif

#else

// Die Definitionen werden hier in den meisten Fällen grundsetzlich ausgewertet,
// nicht über spezifische Werte.
// Die Abfrage geschieht also mit #ifdef
// Daraus folgt hier eine Liste der Möglichkeiten, die beliebig erweitert werden kann.
// Die Auswahl erfolgt schließlich über aus- bzw. einkommentieren.
//

// Übergeordnete Festlegungen
// ---------------------------------------------------------------------------
//
#define smnDebugArduino
//#define smnDebugLinuxConsole
//

// IDE, Editor, etc.
// ---------------------------------------------------------------------------
//
//#define smnSimLinux
#define smnSloeber
//#define smnArduinoIDE
//#define smnPlatformIO

// Hardware, Boards, etc.
// ---------------------------------------------------------------------------
//
//#define smnNotebook
//#define smnESP32_DEV
#define smnNodeMCU10
//#define smnArduinoDUE
//#define smnArduinoShieldEth
//#define smnArduinoShieldWiFi
#define smnSerial Serial

// Prozessoren, Microcontroller, Betriebssysteme, etc.
// ---------------------------------------------------------------------------
//
//#define smnLINUX
//#define smnESP32
#define smnESP8266
//#define smnSAM3X

#endif  // smnDEFBYBUILD

#endif  // _environment_h
