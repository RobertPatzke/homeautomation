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

#define NrOfLoopTasks       8
#define NrOfOnceTasks       8
#define NrOfToggleTasks     8
#define CalcAverageDepth    32

#ifndef _environment_h
  #include "environment.h"
#endif

#ifdef smnSloeber
  #include "Arduino.h"
  #define SYSMICSEC    micros()
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

  bool          periodAlarm;    // Aufrufdistanz > 1 Millisekunde
  unsigned int  alarmCount;     // Anzahl der Überschreitungen

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
  unsigned long backgroundMicros;       // Zeit, die außerhalb von loop()
                                        // verstrichen ist (in Mikrosekunden)
  unsigned long loopMicros;             // Zeit, die innerhalb von loop()
                                        // verstrichen ist (in Mikrosekunden)
  unsigned long loopStartMicros;        // Loop-Startzeit (us seit CPU-Start)
  unsigned long loopEndMicros;          // Loop-Endezeit (us seit CPU-Start)

  unsigned long backgroundMaxMicros;    // Maximale Zeit außerhalb loop()
  unsigned long backgroundMinMicros;    // Minimale Zeit außerhalb loop()
  unsigned long backgroundAvgMicros;    // Mittlere Zeit außerhal loop() {32}
  unsigned long backgroundSumMicros;    // Summe für Mittelwertberechnung

  unsigned long loopMaxMicros;          // Maximale Zeit innerhalb loop()
  unsigned long loopMinMicros;          // Minimale Zeit innerhalb loop()
  unsigned long loopAvgMicros;          // Mittlere Zeit innerhalb loop()
  unsigned long loopSumMicros;          // Summe für Mittelwertberechnung

  unsigned long loopCounter;            // Anzahl der loop()-Durchläufe

  int           calcAvgCounter;         // Zähler für die Mittelwertbildung
  bool          firstLoop;              // Spezielle Kennzeichnung erste loop()
  bool          taskHappened;           // Kennzeichnung: Es lief ein LoopTask

  TimerTask     timerTaskList[NrOfLoopTasks];    // Steuerung der zyklischen
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

  unsigned int  periodMicros;           // Zeit zwischen zwei loop-Aufrufen
  unsigned int  periodMinMicros;
  unsigned int  periodMaxMicros;
  bool          periodFailAlarm;        // periodMicros > Millisekunde
  unsigned int  periodFailCount;        // Anzahl der Überschreitungen

private:
  // -------------------------------------------------------------------------
  // Lokale Funktionen
  // -------------------------------------------------------------------------
  //
  void initTasks();
  void initStatistics();

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
  // Diese Funktion muss als Bedingung (if) aufgerufen werden, um den
  // nachfolgenden Block {} mit der Wiederholzeit <repeatTime> auszuführen
  // Für jede Taskschleife muss ein anderer Index <taskIdx> aus dem Bereich
  // 0 <= taskIdx < MaxNrOfLoopTasks angegeben werden.s
  // Mit <repetitions> wird angegeben, wie oft der Durchlauf überhaupt erfolgt.
  // Der Wert 0 gibt an, dass der Task für immer läuft

  bool timerMilli(int taskIdx, unsigned long repeatTime, unsigned int repetitions);

  bool once(int taskIdx, unsigned int nrOfLoops);
  // Diese Funktion liefert nur einmal den Wert <true>
  // nach Ablauf von nrOfLoops Aufrufen

  bool toggle(int taskIdx);
  // Diese Funktion liefert abwechselnd die Werte <true> oder <false>

  unsigned long timerCycle(int taskIdx);
  // Rückgabe des aktuellen Timerablaufes (startet ab 0).

  unsigned long operationTime(OpHourMeter *opHourMeter);
  // Die Zeit ab Start der CPU

  unsigned long getStatistics(LoopStatistics *statistics);
  // Statistik über Ablaufzeiten

  void resetStatistics();
  // Rücksetzen der Statistikdaten

  // -------------------------------------------------------------------------
  // Anwendervariablen
  // -------------------------------------------------------------------------
  //

};

//-----------------------------------------------------------------------------
#endif

