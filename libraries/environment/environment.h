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
//
  #ifdef smnUBUNTU_ECLIPSE
  // Meine Entwicklungs- und Testumgebung auf dem Notebook unter UBUNTU

    #define smnSimLinux
    #define smnNotebook
    #define smnLINUX
    #define smnDebugLinuxConsole

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
#define smnESP32_DEV
//#define smnNodeMCU10
//#define smnArduinoDUE
//#define smnArduinoShieldEth
//#define smnArduinoShieldWiFi

// Prozessoren, Microcontroller, Betriebssysteme, etc.
// ---------------------------------------------------------------------------
//
//#define smnLINUX
#define smnESP32
//#define smnESP8266
//#define smnSAM3X

#endif  // smnDEFBYBUILD

#endif  // _environment_h
