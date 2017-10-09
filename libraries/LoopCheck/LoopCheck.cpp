//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Software Loop Checking and Timing
// Datei:   LoopCheck.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "LoopCheck.h"

  // -------------------------------------------------------------------------
  // Konstruktoren und Initialisierungen
  // -------------------------------------------------------------------------
  //
  LoopCheck::LoopCheck()
  {
    firstLoop       = true;
    taskHappened    = false;
    toggleMilli     = true;

    initStatistics();
    initTasks();
  }

  void LoopCheck::initStatistics()
  {
    backgroundMicros        = 0;
    loopMicros              = 0;
    loopStartMicros         = 0;
    loopEndMicros           = 0;
    backgroundMaxMicros     = 0;
    backgroundMinMicros     = (unsigned long) -1;
    backgroundAvgMicros     = 0;
    loopMaxMicros           = 0;
    loopMinMicros           = (unsigned long) -1;
    loopAvgMicros           = 0;
    loopCounter             = 0;
    periodFailCount         = 0;
    periodMaxMicros         = 0;
    periodMinMicros         = (unsigned int) -1;
    periodMicros            = 0;
    periodFailAlarm         = false;

    calcAvgCounter          = 0;
  }

  void LoopCheck::initTasks()
  {
    for (int i = 0; i < NrOfLoopTasks; i++)
    {
      timerTaskList[i].counterStarted    = false;
      timerTaskList[i].finished          = false;
      timerTaskList[i].firstRun          = true;
      timerTaskList[i].runCounter        = 0;
    }

    for( int i = 0; i < NrOfOnceTasks; i++)
    {
      onceTaskList[i].finished      = false;
      onceTaskList[i].firstRun      = true;
      onceTaskList[i].waitCounter   = 0;
    }
  }

  // -------------------------------------------------------------------------
  // Anwenderfunktionen
  // -------------------------------------------------------------------------
  //
  void LoopCheck::begin()
  {
    unsigned int    restMicros;
    unsigned long   lastMicros;

    lastMicros = loopStartMicros;
    loopStartMicros = SYSMICSEC;
    restMicros = (int) (loopStartMicros % 1000);

    if((restMicros > 500) && (toggleMilli == true))
    {
      msec++;
      if(msec == 1000)
      {
        msec = 0;
        sec++;
        if(sec == 60)
        {
          sec = 0;
          min++;
          if(min == 60)
          {
            min = 0;
            hour++;
            if(hour == 24)
            {
              hour = 0;
              day++;
              if(day == 365)
              {
                day = 0;
                year++;
              }
            }
          }
        }
      }
      toggleMilli = false;
    }

    if((restMicros < 500) && (toggleMilli == false))
    {
      toggleMilli = true;
    }


    if(firstLoop == false)
    {
      backgroundMicros = loopStartMicros - loopEndMicros;
      if(backgroundMicros > backgroundMaxMicros)
        backgroundMaxMicros = backgroundMicros;
      if(backgroundMicros < backgroundMinMicros)
        backgroundMinMicros = backgroundMicros;
      periodMicros = loopStartMicros - lastMicros;
      if(periodMicros > periodMaxMicros)
        periodMaxMicros = periodMicros;
      if(periodMicros < periodMinMicros)
        periodMinMicros = periodMicros;
      if(periodMicros > 1000)
      {
        periodFailAlarm = true;
        periodFailCount++;
      }
    }
  }

  void LoopCheck::end()
  {
    loopEndMicros = SYSMICSEC;
    loopMicros = loopEndMicros - loopStartMicros;
    if(loopMicros > loopMaxMicros)
      loopMaxMicros = loopMicros;
    if(loopMicros < loopMinMicros)
      loopMinMicros = loopMicros;

    if(firstLoop == false)
    {
      loopSumMicros += loopMicros;
      backgroundSumMicros += backgroundMicros;
      calcAvgCounter++;
      if(calcAvgCounter == CalcAverageDepth)
      {
        loopAvgMicros = loopSumMicros / CalcAverageDepth;
        backgroundAvgMicros = backgroundSumMicros / CalcAverageDepth;
        calcAvgCounter = 0;
        loopSumMicros = 0;
        backgroundSumMicros = 0;
      }
    }
    else
    {
      loopAvgMicros = loopMicros;
      backgroundAvgMicros = backgroundMicros;
    }

    loopCounter++;
    firstLoop = false;
    taskHappened = false;
  }

  bool LoopCheck::timerMicro
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions)
  {
    TimerTask *ctrlPtr;

    if(taskIdx < 0) return(false);
    if(taskIdx >= NrOfLoopTasks) return(false);

    ctrlPtr = &timerTaskList[taskIdx];

    if(ctrlPtr->finished == true) return(false);

    if(ctrlPtr->firstRun == true)
    {
      ctrlPtr->firstRun     = false;
      ctrlPtr->repCounter   = repetitions;
    }

    if(ctrlPtr->counterStarted == false)
    {
      ctrlPtr->startCount = loopStartMicros;
      ctrlPtr->counterStarted = true;
      return(false);
    }

    if(taskHappened == true) return(false);

    if((loopStartMicros - ctrlPtr->startCount) < repeatTime)
      return(false);

    taskHappened            = true;
    ctrlPtr->counterStarted = false;
    ctrlPtr->runCounter++;

    if(ctrlPtr->repCounter > 0)
    {
      ctrlPtr->repCounter--;
      if(ctrlPtr->repCounter == 0)
        ctrlPtr->finished = true;
    }

    return(true);
  }

  bool LoopCheck::timerMilli
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions)
  {
    return(timerMicro(taskIdx,repeatTime * 1000,repetitions));
  }

  bool LoopCheck::once(int taskIdx, unsigned int nrOfLoops)
  {
    if(taskIdx < 0) return(false);
    if(taskIdx >= NrOfOnceTasks) return(false);

    if(onceTaskList[taskIdx].finished == true) return(false);
    if(nrOfLoops <= 1)
    {
      onceTaskList[taskIdx].finished = true;
      return(true);
    }

    if(onceTaskList[taskIdx].firstRun == true)
    {
      onceTaskList[taskIdx].firstRun = false;
      onceTaskList[taskIdx].waitCounter = nrOfLoops;
    }

    onceTaskList[taskIdx].waitCounter--;
    if(onceTaskList[taskIdx].waitCounter > 0)
      return(false);

    onceTaskList[taskIdx].finished = true;
    return(true);
  }

  bool LoopCheck::toggle(int taskIdx)
  {
    bool    toggleBit;

    if(taskIdx < 0) return(false);
    if(taskIdx >= NrOfToggleTasks) return(false);
    toggleBit = toggleTaskList[taskIdx];
    toggleTaskList[taskIdx] = !toggleBit;
    return(toggleBit);
  }

  unsigned long LoopCheck::timerCycle(int taskIdx)
  {
    if(taskIdx < 0) return(0);
    if(taskIdx >= NrOfLoopTasks) return(0);
    return(timerTaskList[taskIdx].runCounter);
  }

  unsigned long LoopCheck::operationTime(OpHourMeter *opHourMeter)
  {
    opHourMeter->Milliseconds    = msec;
    opHourMeter->Seconds         = sec;
    opHourMeter->Minutes         = min;
    opHourMeter->Hours           = hour;
    opHourMeter->Days            = day;
    opHourMeter->Years           = year;
    return(loopStartMicros);
  }

  unsigned long LoopCheck::getStatistics(LoopStatistics *statistics)
  {
    statistics->loopTime    =   (unsigned int) loopMicros;
    statistics->loopMaxTime =   (unsigned int) loopMaxMicros;
    statistics->loopMinTime =   (unsigned int) loopMinMicros;
    statistics->loopAvgTime =   (unsigned int) loopAvgMicros;

    statistics->bgTime      =   (unsigned int) backgroundMicros;
    statistics->bgMaxTime   =   (unsigned int) backgroundMaxMicros;
    statistics->bgMinTime   =   (unsigned int) backgroundMinMicros;
    statistics->bgAvgTime   =   (unsigned int) backgroundAvgMicros;

    statistics->alarmCount  =   periodFailCount;
    statistics->periodAlarm =   periodFailAlarm;
    periodFailAlarm = false;

    statistics->loopPeriod  =   periodMicros;
    statistics->maxPeriod   =   periodMaxMicros;
    statistics->minPeriod   =   periodMinMicros;

    return(loopCounter);
  }

  void LoopCheck::resetStatistics()
  {
    backgroundMicros        = 0;
    loopMicros              = 0;

    backgroundMaxMicros     = 0;
    backgroundMinMicros     = (unsigned long) -1;
    backgroundAvgMicros     = 0;

    loopMaxMicros           = 0;
    loopMinMicros           = (unsigned long) -1;
    loopAvgMicros           = 0;
    loopCounter             = 0;

    periodFailCount         = 0;
    periodMaxMicros         = 0;
    periodMinMicros         = (unsigned int) -1;
    periodMicros            = 0;
    periodFailAlarm         = false;

    calcAvgCounter          = 0;
  }

  // -------------------------------------------------------------------------
  // Debug-Funktionen
  // -------------------------------------------------------------------------
  //
