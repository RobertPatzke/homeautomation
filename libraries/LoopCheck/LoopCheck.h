//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   LoopCheck.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//
#ifndef _LoopCheck_h
#define _LoopCheck_h
//-----------------------------------------------------------------------------

#define PeriodMinTime       5000
// Wenn der Aufrufzyklus der Loop diese Zeit (in Mikrosekunden) überschreitet,
// dann wird ein Alarmbit gesetzt und ein Alarmzähler inkrementiert

#ifndef LoopScreeningGrades
  #define LoopScreeningGrades 6
#endif

#define NrOfTimerTasks      10

#define lcTimer0  0
#define lcTimer1  1
#define lcTimer2  2
#define lcTimer3  3
#define lcTimer4  4
#define lcTimer5  5
#define lcTimer6  6
#define lcTimer7  7
#define lcTimer8  8
#define lcTimer9  9


#define NrOfOnceTasks       4
#define NrOfToggleTasks     4
#define CalcAverageDepth    32

#ifdef UseGithubPath
  #include "../environment/environment.h"
#else
  #include "environment.h"
#endif

#if defined(smnSimLinux) || defined(smnSimWindows)
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #define SYSMICSEC locMicros()
#endif

#ifdef smnSimWindows
#include <Windows.h>
#endif

#ifdef smnSloeber
  #include "Arduino.h"
  #define SYSMICSEC    micros()
#endif

#ifdef smnESP8266
  #define DIV(x,y)    locDiv(x,y)
#else
  #define DIV(x,y)    div(x,y)
#endif

typedef struct _OpHourMeter
{
  int   Years;
  int   Days;
  int   Hours;
  int   Minutes;
  int   Seconds;
  int   Milliseconds;
} OpHourMeter;

typedef struct _lcDateTime
{
  int   Year;
  int   Month;
  int   Day;
  int   Hour;
  int   Minute;
  int   Second;
  int   Millisecond;
} lcDateTime;

typedef struct _LoopStatistics
{
  unsigned int  loopTime;       // Schleifenzeit in Mikrosekunden
  unsigned int  loopMaxTime;    // Maximale Schleifenzeit
  unsigned int  loopMinTime;    // Minimale Schleifenzeit
  unsigned int  loopAvgTime;    // Mittlere Schleifenzeit

  unsigned int  bgTime;         // Zeit außerhalb der Schleife
  unsigned int  bgMaxTime;      // Maximale Außenzeit
  unsigned int  bgMinTime;      // Minimale Außenzeit
  unsigned int  bgAvgTime;      // Mittlere Außenzeit

  unsigned int  loopPeriod;     // Zeit zwischen loop-Aufrufen
  unsigned int  maxPeriod;      // Maximale Aufrufdistanz
  unsigned int  minPeriod;      // Minimale Aufrufdistanz

  bool          periodAlarm;    // Aufrufdistanz > PeriodMinTime
  unsigned int  alarmCount;     // Anzahl der Überschreitungen

  unsigned int  rtScreening[LoopScreeningGrades];
  // Echtzeitüberwachung (Klassierung der ms Überschreitungen)
} LoopStatistics;


// ---------------------------------------------------------------------------
// class LoopCheck
// ---------------------------------------------------------------------------
//
class LoopCheck
{
  // -------------------------------------------------------------------------
  // Klassenspezifische Datentypen
  // -------------------------------------------------------------------------
  //
  typedef struct _TimerTask
  {
    bool            counterStarted;
    bool            finished;
    bool            firstRun;
    unsigned long   startCount;
    unsigned long   runCounter;
    unsigned long   delayCounter;
    unsigned long   ticks;
    unsigned int    repCounter;
  } TimerTask;

  typedef struct _OnceTask
  {
    bool            finished;
    bool            firstRun;
    unsigned int    waitCounter;
  } OnceTask;

private:
  // -------------------------------------------------------------------------
  // Lokale Variablen
  // -------------------------------------------------------------------------
  //
  unsigned long checkStartMicros;       // Zeit des ersten Aufrufs von begin()

  unsigned long backgroundMicros;       // Zeit, die außerhalb von loop()
                                        // verstrichen ist (in Mikrosekunden)
  unsigned long loopMicros;             // Zeit, die innerhalb von loop()
                                        // verstrichen ist (in Mikrosekunden)
  unsigned long loopStartMicros;        // Loop-Startzeit (us seit CPU-Start)
  unsigned long lastClockMicros;
  unsigned long lastStartMicros;
  unsigned long lastRestMicros;

  unsigned long loopEndMicros;          // Loop-Endezeit (us seit CPU-Start)
  unsigned long clockCycleMicros;       // Abstand zwischen zwei clock ticks
  unsigned long mainStartMicros;        // Zählerstand bei Programmstart

  unsigned long backgroundMaxMicros;    // Maximale Zeit außerhalb loop()
  unsigned long backgroundMinMicros;    // Minimale Zeit außerhalb loop()
  unsigned long backgroundAvgMicros;    // Mittlere Zeit außerhal loop() {32}
  unsigned long backgroundSumMicros;    // Summe für Mittelwertberechnung

  unsigned long loopMaxMicros;          // Maximale Zeit innerhalb loop()
  unsigned long loopMinMicros;          // Minimale Zeit innerhalb loop()
  unsigned long loopAvgMicros;          // Mittlere Zeit innerhalb loop()
  unsigned long loopSumMicros;          // Summe für Mittelwertberechnung

  unsigned long loopCounter;            // Anzahl der loop()-Durchläufe

  unsigned int  loopScreening[LoopScreeningGrades];

  int           calcAvgCounter;         // Zähler für die Mittelwertbildung
  bool          firstLoop;              // Spezielle Kennzeichnung erste loop()
  bool          taskHappened;           // Kennzeichnung: Es lief ein LoopTask

  TimerTask     timerTaskList[NrOfTimerTasks];  // Steuerung der zyklischen
                                                // Tasks (Timer-Ersatz in loop())
  OnceTask      onceTaskList[NrOfOnceTasks];
  bool          toggleTaskList[NrOfToggleTasks];

  int           year;               // Betriebsstundenzähler gesamt
  int           day;
  int           hour;
  int           min;
  int           sec;
  int           msec;
  bool          toggleMilli;

  int           dtYear;             // Zeit / Uhr
  int           dtMonth;
  int           dtDay;
  int           dtHour;
  int           dtMin;
  int           dtSec;
  int           dtmSec;
  int           febLen;
  char          dateTimeStr[30];

  unsigned int  periodMicros;           // Zeit zwischen zwei loop-Aufrufen
  unsigned int  periodMinMicros;
  unsigned int  periodMaxMicros;
  bool          periodFailAlarm;        // periodMicros > Millisekunde
  unsigned int  periodFailCount;        // Anzahl der Überschreitungen

  unsigned long measureTimeSet;         // Mikrosekunden-Offset Zeitmessung

  unsigned long measureRuntime;         // Laufzeit seit Start in Sekunden

private:
  // -------------------------------------------------------------------------
  // Lokale Funktionen
  // -------------------------------------------------------------------------
  //
  void initTasks();
  void initStatistics();
  void initClock();
  unsigned long locMicros();
#ifdef smnESP8266
  div_t locDiv(int numer, int denom);
#endif

public:
  // -------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // -------------------------------------------------------------------------
  //
  LoopCheck();

  // -------------------------------------------------------------------------
  // Anwenderfunktionen
  // -------------------------------------------------------------------------
  //
  void begin();     // Diese Funktion muss am Anfang der Schleife aufgerufen
                    // werden.

  void end();       // Diese Funktion muss am Ende der Schleife aufgerufen
                    // werden.

  bool timerMicro(int taskIdx, unsigned long repeatTime, unsigned int repetitions);
  bool timerMicro(int taskIdx, unsigned long repeatTime, unsigned int repetitions, unsigned long delay);
  // Diese Funktion muss als Bedingung (if) aufgerufen werden, um den
  // nachfolgenden Block {} mit der Wiederholzeit <repeatTime> auszuführen
  // Für jede Taskschleife muss ein anderer Index <taskIdx> aus dem Bereich
  // 0 <= taskIdx < MaxNrOfLoopTasks angegeben werden.
  // Mit <repetitions> wird angegeben, wie oft der Durchlauf überhaupt erfolgt.
  // Der Wert 0 gibt an, dass der Task für immer läuft

  bool timerMilli(int taskIdx, unsigned long repeatTime, unsigned int repetitions);
  bool timerMilli(int taskIdx, unsigned long repeatTime, unsigned int repetitions, unsigned long delay);

  bool once(int taskIdx, unsigned int nrOfLoops);
  // Diese Funktion liefert nur einmal den Wert <true>
  // nach Ablauf von nrOfLoops Aufrufen

  bool toggle(int taskIdx);
  // Diese Funktion liefert abwechselnd die Werte <true> oder <false>

  unsigned long timerCycle(int taskIdx);
  // Rückgabe des aktuellen Timerablaufes (startet ab 0).

  bool timerCycleMod(int taskIdx, int modulo);
  // Liefert alle <modulo> Timerabläufe den Wert <true>

  unsigned long tick(int taskIdx);
  // Rückgabe des aktuellen Zählwertes in Mikrosekunden

  unsigned long operationTime(OpHourMeter *opHourMeter);
  // Die Zeit ab Start der CPU

  unsigned long getStatistics(LoopStatistics *statistics);
  // Statistik über Ablaufzeiten

  void resetStatistics();
  // Rücksetzen der Statistikdaten

  bool setDateTime(const char *dtStr);
  // Setzen der Uhr über standardisierten String

  bool setDateTime(lcDateTime dt);
  // Setzen der Uhr über lokal definierte Struktur

  bool getDateTime(lcDateTime *dt);
  // Abfragen der Uhr über lokal definierte Struktur

  const char * refDateTime();
  // Zeiger auf Datum/Uhrzeit holen

  void startTimeMeasure();
  // Zeitmessung starten

  unsigned long getTimeMeasure();
  // Zeitmesswert holen

  unsigned long getRuntime();
  // Laufzeit in Sekunden


  // -------------------------------------------------------------------------
  // Debug-Funtionen
  // -------------------------------------------------------------------------
  //

#ifdef smnLoopCheckDebug
  void dbgGetStatistics(char *buffer, int idxItem);
#endif

};

//-----------------------------------------------------------------------------
#endif

