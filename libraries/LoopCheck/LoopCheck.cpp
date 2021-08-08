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
    initClock();
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

    year    = 0;
    day     = 0;
    hour    = 0;
    min     = 0;
    sec     = 0;
    msec    = 0;

    measureRuntime = 0;

    calcAvgCounter = 0;
  }

  void LoopCheck::initTasks()
  {
    for (int i = 0; i < NrOfTimerTasks; i++)
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

  void LoopCheck::initClock()
  {
    strcpy(dateTimeStr,"2017-12-07T17:11:35.456+00:00");
    dtYear      = 2017;
    dtMonth     = 12;
    dtDay       = 7;
    dtHour      = 17;
    dtMin       = 11;
    dtSec       = 35;
    dtmSec      = 456;
  }

  // -------------------------------------------------------------------------
  // Anwenderfunktionen
  // -------------------------------------------------------------------------
  //

  void LoopCheck::begin()
  {
    unsigned int    cycleMillis;
    unsigned int    restMicros;
    unsigned int    tmpInt;
    unsigned int    tmpInt100, tmpInt10, tmpInt1;
    div_t           divResult;

    loopStartMicros = SYSMICSEC;
    clockCycleMicros = loopStartMicros - lastClockMicros + lastRestMicros;
    //
    // Zeit seit dem letzten Aufruf von begin()

  
    //
    if(firstLoop == true)
    {
      clockCycleMicros = 0;
      lastClockMicros = loopStartMicros;
    }

    // Aufteilen in Millisekunden und Mikrosekunden
    //
    divResult = DIV((int) clockCycleMicros, 1000);

    restMicros = divResult.rem;
    cycleMillis = divResult.quot;

    if(cycleMillis > 0)
    {
      lastRestMicros = restMicros;
      lastClockMicros = loopStartMicros;
      msec += cycleMillis;
      dtmSec += cycleMillis;

      // Betriebsstundenzähler
      //
      if(msec >= 1000)
      {
        msec -= 1000;
        sec++;
        measureRuntime++;

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

      // Software-Uhr
      //
      if(dtmSec >= 1000)
      {
        dtmSec -= 1000;

        dtSec++;
        dateTimeStr[20] = '0';
        dateTimeStr[21] = '0';
        dateTimeStr[22] = '0';

        if(dtSec == 60)
        {
          dtSec = 0;
          dtMin++;
          dateTimeStr[17] = '0';
          dateTimeStr[18] = '0';

          if(dtMin == 60)
          {
            dtMin = 0;
            dtHour++;
            dateTimeStr[14] = '0';
            dateTimeStr[15] = '0';

            if(dtHour == 24)
            {
              dtHour = 0;
              dtDay++;
              dateTimeStr[11] = '0';
              dateTimeStr[12] = '0';

              if  (
                       (dtDay == (febLen + 1) && dtMonth == 2)
                    || (dtDay == 31 && (dtMonth == 4 ||
                                        dtMonth == 6 ||
                                        dtMonth == 9 ||
                                        dtMonth == 11))
                    || dtDay == 32
                  )
              {
                dtDay = 1;
                dtMonth++;
                dateTimeStr[8] = '0';
                dateTimeStr[9] = '1';

                if(dtMonth == 13)
                {
                  dtMonth = 1;
                  dtYear++;
                  tmpInt = dtYear - 2000;
                  if((tmpInt % 4) == 0)
                    febLen = 29;
                  else
                    febLen = 28;
                  dateTimeStr[5] = '0';
                  dateTimeStr[6] = '1';

                  divResult   = DIV(tmpInt,100);
                  tmpInt100   = divResult.quot;
                  tmpInt      = divResult.rem;

                  divResult   = DIV(tmpInt,10);
                  tmpInt10    = divResult.quot;
                  tmpInt1     = divResult.rem;

                  dateTimeStr[1] = (char) (tmpInt100 | 0x30);
                  dateTimeStr[2] = (char) (tmpInt10  | 0x30);
                  dateTimeStr[3] = (char) (tmpInt1   | 0x30);
                }
                else
                {
                  divResult      = DIV(dtMonth,10);
                  dateTimeStr[5] = (char) (divResult.quot | 0x30);
                  dateTimeStr[6] = (char) (divResult.rem  | 0x30);
                }
              }
              else
              {
                divResult      = DIV(dtDay,10);
                dateTimeStr[8] = (char) (divResult.quot | 0x30);
                dateTimeStr[9] = (char) (divResult.rem  | 0x30);
              }
            }
            else
            {
              divResult       = DIV(dtHour,10);
              dateTimeStr[11] = (char) (divResult.quot | 0x30);
              dateTimeStr[12] = (char) (divResult.rem  | 0x30);
            }
          }
          else
          {
            divResult       = DIV(dtMin,10);
            dateTimeStr[14] = (char) (divResult.quot | 0x30);
            dateTimeStr[15] = (char) (divResult.rem  | 0x30);
          }
        }
        else
        {
          divResult     = DIV(dtSec,10);
          dateTimeStr[17] = (char) (divResult.quot | 0x30);
          dateTimeStr[18] = (char) (divResult.rem  | 0x30);
        }
      }
      else
      {
        divResult   = DIV(dtmSec,100);
        tmpInt100   = divResult.quot;
        tmpInt      = divResult.rem;

        divResult   = DIV(tmpInt,10);
        tmpInt10    = divResult.quot;
        tmpInt1     = divResult.rem;

        dateTimeStr[20] = (char) (tmpInt100 | 0x30);
        dateTimeStr[21] = (char) (tmpInt10  | 0x30);
        dateTimeStr[22] = (char) (tmpInt1   | 0x30);
      }

    }

    if(firstLoop == false)
    {
      backgroundMicros = loopStartMicros - loopEndMicros;
      if(backgroundMicros > backgroundMaxMicros)
        backgroundMaxMicros = backgroundMicros;
      if(backgroundMicros < backgroundMinMicros)
        backgroundMinMicros = backgroundMicros;
      periodMicros = loopStartMicros - lastStartMicros;
      if(periodMicros > periodMaxMicros)
        periodMaxMicros = periodMicros;
      periodSumMicros += periodMicros;
      if(periodMicros < periodMinMicros)
        periodMinMicros = periodMicros;
      if(periodMicros > PeriodMinTime)
      {
        periodFailAlarm = true;
        periodFailCount++;
      }

      divResult = DIV(periodMicros, 1000);
      if(divResult.quot > 0)
      {
        if(divResult.quot >= LoopScreeningGrades)
          ++loopScreening[LoopScreeningGrades - 1];
        else
          ++loopScreening[divResult.quot -1];
      }
    } // if()

    lastStartMicros = loopStartMicros;

  }


  unsigned int LoopCheck::done()
  {
    return(SYSMICSEC - loopStartMicros);
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
        periodAvgMicros = periodSumMicros / CalcAverageDepth;
        calcAvgCounter = 0;
        loopSumMicros = 0;
        backgroundSumMicros = 0;
        periodSumMicros = 0;
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
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions, unsigned long delay)
  {
    TimerTask     *ctrlPtr;
    unsigned long calcMics;

    // Test the limit of enabled timers
    //
    if(taskIdx < 0) return(false);
    if(taskIdx >= NrOfTimerTasks) return(false);

    // Get the reference to timer data for the selected timer
    //
    ctrlPtr = &timerTaskList[taskIdx];

    // If the timer task has finished, we are ready here
    //
    if(ctrlPtr->finished == true) return(false);

    // If it is the first run (initialisation 1)
    //
    if(ctrlPtr->firstRun == true)
    {
      ctrlPtr->firstRun     = false;
      ctrlPtr->repCounter   = repetitions;
      ctrlPtr->delayCounter = delay;
    }

    // If counting is not started yet (initialisation 2)
    //
    if(ctrlPtr->counterStarted == false)
    {
      ctrlPtr->startCount = loopStartMicros;
      ctrlPtr->counterStarted = true;
      return(false);
    }

    // If another count task has happened in this loop, we have to wait
    //
    if(taskHappened == true) return(false);

    // Calculate the number of microseconds since the start of the counter
    //
    calcMics = loopStartMicros - ctrlPtr->startCount;
    ctrlPtr->ticks = calcMics;

    // If there is a delay, wait the delay time
    //
    if(ctrlPtr->delayCounter > 0)
    {
      if(calcMics < ctrlPtr->delayCounter)
        return(false);
      else
      {
        ctrlPtr->delayCounter = 0;                // delay finished
        ctrlPtr->startCount = loopStartMicros;    // reset counter
      }
      return(false);
    }

    // There is no delay (or delay is finished)
    // If repeatTime is not passed, leave with FALSE
    //
    if(calcMics < repeatTime)
    {
      return(false);
    }

    // One counter period finished
    //
    taskHappened            = true;               // disable other timers in this loop
    ctrlPtr->counterStarted = false;              // prepare resetting the counter
    ctrlPtr->runCounter++;                        // count the timer events

    // If the number of periods is limited finish in time
    //
    if(ctrlPtr->repCounter > 0)
    {
      ctrlPtr->repCounter--;
      if(ctrlPtr->repCounter == 0)
        ctrlPtr->finished = true;
    }

    return(true);
  }

  bool LoopCheck::timerMicro
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions)
  {
    return(timerMicro(taskIdx, repeatTime, repetitions, 0));
  }


  bool LoopCheck::timerMilli
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions, unsigned long delay)
  {
    return(timerMicro(taskIdx, repeatTime * 1000, repetitions, delay * 1000));
  }

  bool LoopCheck::timerMilli
    (int taskIdx, unsigned long repeatTime, unsigned int repetitions)
  {
    return(timerMicro(taskIdx,repeatTime * 1000,repetitions,0));
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
    if(taskIdx >= NrOfTimerTasks) return(0);
    return(timerTaskList[taskIdx].runCounter);
  }

  bool LoopCheck::timerCycleMod(int taskIdx, int modulo)
  {
    div_t divResult;

    if(taskIdx < 0) return(0);
    if(taskIdx >= NrOfTimerTasks) return(0);
    divResult   = DIV(timerTaskList[taskIdx].runCounter,modulo);
    if(divResult.rem == 0)
      return(true);
    else
      return(false);
  }

  unsigned long LoopCheck::tick(int taskIdx)
  {
    if(taskIdx < 0) return(0);
    if(taskIdx >= NrOfTimerTasks) return(0);
    return(timerTaskList[taskIdx].ticks);
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
    statistics->avgPeriod   =   periodAvgMicros;

    for (int i = 0; i < LoopScreeningGrades; i++)
      statistics->rtScreening[i] = loopScreening[i];

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
    periodAvgMicros         = 0;
    periodMicros            = 0;
    periodFailAlarm         = false;

    for (int i = 0; i < LoopScreeningGrades; i++)
      loopScreening[i] = 0;

    calcAvgCounter          = 0;
  }

  bool LoopCheck::setDateTime(const char *dtStr)
  {
    int tmpInt;

     if(strlen(dtStr) < 23) return(false);
     strcpy(dateTimeStr,dtStr);
     dtYear   = (dateTimeStr[0]  & 0x0F) * 1000 +
                (dateTimeStr[1]  & 0x0F) * 100  +
                (dateTimeStr[2]  & 0x0F) * 10   +
                (dateTimeStr[3]  & 0x0F);

     tmpInt = dtYear - 2000;
     if((tmpInt % 4) == 0)
       febLen = 29;
     else
       febLen = 28;


     dtMonth  = (dateTimeStr[5]  & 0x0F) * 10   +
                (dateTimeStr[6]  & 0x0F);

     dtDay    = (dateTimeStr[8]  & 0x0F) * 10   +
                (dateTimeStr[9]  & 0x0F);

     dtHour   = (dateTimeStr[11] & 0x0F) * 10   +
                (dateTimeStr[12] & 0x0F);

     dtMin    = (dateTimeStr[14] & 0x0F) * 10   +
                (dateTimeStr[15] & 0x0F);

     dtSec    = (dateTimeStr[17] & 0x0F) * 10   +
                (dateTimeStr[18] & 0x0F);

     dtmSec   = (dateTimeStr[20] & 0x0F) * 10   +
                (dateTimeStr[21] & 0x0F) * 10   +
                (dateTimeStr[22] & 0x0F);

     return(true);
  }

  bool LoopCheck::setDateTime(lcDateTime dt)
  {
    div_t   divResult;
    int     tmpInt;

    dtYear    = dt.Year;

    tmpInt = dtYear - 2000;
    if((tmpInt % 4) == 0)
      febLen = 29;
    else
      febLen = 28;


    divResult = DIV(dtYear,1000);
    dateTimeStr[0] = (char) (0x30 + divResult.quot);

    divResult = DIV(divResult.rem,100);
    dateTimeStr[1] = (char) (0x30 + divResult.quot);

    divResult = DIV(divResult.rem,10);
    dateTimeStr[2] = (char) (0x30 + divResult.quot);
    dateTimeStr[3] = (char) (0x30 + divResult.rem);

    dtMonth   = dt.Month;
    divResult = DIV(dtMonth,10);
    dateTimeStr[5] = (char) (0x30 + divResult.quot);
    dateTimeStr[6] = (char) (0x30 + divResult.rem);

    dtDay     = dt.Day;
    divResult = DIV(dtDay,10);
    dateTimeStr[8] = (char) (0x30 + divResult.quot);
    dateTimeStr[9] = (char) (0x30 + divResult.rem);

    dtHour    = dt.Hour;
    divResult = DIV(dtHour,10);
    dateTimeStr[11] = (char) (0x30 + divResult.quot);
    dateTimeStr[12] = (char) (0x30 + divResult.rem);

    dtMin     = dt.Minute;
    divResult = DIV(dtMin,10);
    dateTimeStr[14] = (char) (0x30 + divResult.quot);
    dateTimeStr[15] = (char) (0x30 + divResult.rem);

    dtSec     = dt.Second;
    divResult = DIV(dtSec,10);
    dateTimeStr[17] = (char) (0x30 + divResult.quot);
    dateTimeStr[18] = (char) (0x30 + divResult.rem);

    dtmSec    = dt.Millisecond;
    divResult = DIV(dtmSec, 100);
    dateTimeStr[20] = (char) (0x30 + divResult.quot);
    divResult = DIV(divResult.rem, 10);
    dateTimeStr[21] = (char) (0x30 + divResult.quot);
    dateTimeStr[22] = (char) (0x30 + divResult.rem);

    return(true);
  }

  bool LoopCheck::getDateTime(lcDateTime *dt)
  {
    dt->Year        = dtYear;
    dt->Month       = dtMonth;
    dt->Day         = dtDay;
    dt->Hour        = dtHour;
    dt->Minute      = dtMin;
    dt->Second      = dtSec;
    dt->Millisecond = dtmSec;
    return(true);
  }

  const char * LoopCheck::refDateTime()
  {
    return(dateTimeStr);
  }

  unsigned long LoopCheck::locMicros()
  {
#ifdef smnSimLinux
    struct timespec clockTime;
    unsigned long retv;

    clock_gettime(CLOCK_MONOTONIC, &clockTime);
    retv = clockTime.tv_nsec / 1000;
    return(retv);
#endif

#ifdef smnSimWindows
    LARGE_INTEGER countValue, frequency, result;

    QueryPerformanceCounter(&countValue);
    QueryPerformanceFrequency(&frequency);

    result.QuadPart = (countValue.QuadPart * 1000000) / frequency.QuadPart;
    return((unsigned long) result.QuadPart);
#endif

#ifdef smnSloeber
    return(micros());
#endif
  }

#ifdef smnESP8266
  div_t LoopCheck::locDiv(int numer, int denom)
  {
    div_t retv;

    retv.quot = numer / denom;
    retv.rem  = numer % denom;

    return(retv);
  }
#endif

  void LoopCheck::startTimeMeasure()
  {
    measureTimeSet = SYSMICSEC;
  }

  unsigned long LoopCheck::getTimeMeasure()
  {
    return(SYSMICSEC - measureTimeSet);
  }

  unsigned long LoopCheck::getRuntime()
  {
    return(measureRuntime);
  }

  void LoopCheck::hexAsc(char * dest, byte val)
  {
    char cv;

    cv = val >> 4;
    if(cv < 10)
      cv += 0x30;
    else
      cv += 0x37;
    dest[0] = cv;

    cv = val & 0x0F;
    if(cv < 10)
      cv += 0x30;
    else
      cv += 0x37;
    dest[1] = cv;

    dest[2] = '\0';
  }


  // -------------------------------------------------------------------------
  // Debug-Funktionen
  // -------------------------------------------------------------------------
  //
#ifdef smnLoopCheckDebug

  void LoopCheck::dbgGetStatistics(char *buffer, int idxItem)
  {
    switch(idxItem)
    {
      case 0:
        sprintf(buffer,"lT=%d, lMaxT=%d, lMinT=%d, lAvgT=%d",
            loopMicros,loopMaxMicros,loopMinMicros,loopAvgMicros);
        break;

      case 1:
        sprintf(buffer,"bT=%d, bMaxT=%d, bMinT=%d, bAvgT=%d",
            backgroundMicros,backgroundMaxMicros,backgroundMinMicros,backgroundAvgMicros);
        break;

      case 2:
        sprintf(buffer,"rtAlCnt=%d, lCnt=%d, Scr=%d,%d,%d,%d,%d,%d",
            periodFailCount, loopCounter, loopScreening[0],loopScreening[1],loopScreening[2],loopScreening[3],loopScreening[4],loopScreening[5]);
        break;
    }
  }

#endif
