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

// Die Definitionen werden hier in den meisten Fällen grundsetzlich ausgewertet,
// nicht über spezifische Werte.
// Die Abfrage geschieht also mit #ifdef
// Daraus folgt hier eine Liste der Möglichkeiten, die beliebig erweitert werden kann.
// Die Auswahl erfolgt schließlich über aus- bzw. einkommentieren.
//

// IDE, Editor, etc.
// ---------------------------------------------------------------------------
//
#define smnSloeber
//#define smnArduinoIDE
//#define smnPlatformIO

// Hardware, Boards, etc.
// ---------------------------------------------------------------------------
//

#define smnESP32_DEV
//#define smnNodeMCU10
//#define smnArduinoDUE
//#define smnArduinoShieldEth
//#define smnArduinoShieldWiFi

// Prozessoren, Microcontroller, etc.
// ---------------------------------------------------------------------------
//
#define smnESP32
//#define smnESP8266
//#define smnSAM3X

#endif  // _environment_h
