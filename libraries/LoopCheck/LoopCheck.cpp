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

  void LoopCheck::initClock()
  {
    strcpy(dateTimeStr,"2017-10-09T17:11:35.456+00:00");
    dtYear      = 2017;
    dtMonth     = 10;
    dtDay       = 9;
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
#if defined(smnSimWindows) || defined(smnSimLinux)
    begin(true);
#else
    begin(false);
#endif
  }

  void LoopCheck::begin(bool resetTime)
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
    divResult = div((int) clockCycleMicros, 1000);

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
                       (dtDay == febLen && dtMonth == 2)
                    || (dtDay == 30 && (dtMonth == 4 ||
                                        dtMonth == 6 ||
                                        dtMonth == 9 ||
                                        dtMonth == 11))
                    || dtDay == 31
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

                  divResult   = div(tmpInt,100);
                  tmpInt100   = divResult.quot;
                  tmpInt      = divResult.rem;

                  divResult   = div(tmpInt,10);
                  tmpInt10    = divResult.quot;
                  tmpInt1     = divResult.rem;

                  dateTimeStr[1] = (char) (tmpInt100 | 0x30);
                  dateTimeStr[2] = (char) (tmpInt10  | 0x30);
                  dateTimeStr[3] = (char) (tmpInt1   | 0x30);
                }
                else
                {
                  divResult      = div(dtMonth,10);
                  dateTimeStr[5] = (char) (divResult.quot | 0x30);
                  dateTimeStr[6] = (char) (divResult.rem  | 0x30);
                }
              }
              else
              {
                divResult      = div(dtDay,10);
                dateTimeStr[8] = (char) (divResult.quot | 0x30);
                dateTimeStr[9] = (char) (divResult.rem  | 0x30);
              }
            }
            else
            {
              divResult       = div(dtHour,10);
              dateTimeStr[11] = (char) (divResult.quot | 0x30);
              dateTimeStr[12] = (char) (divResult.rem  | 0x30);
            }
          }
          else
          {
            divResult       = div(dtMin,10);
            dateTimeStr[14] = (char) (divResult.quot | 0x30);
            dateTimeStr[15] = (char) (divResult.rem  | 0x30);
          }
        }
        else
        {
          divResult     = div(dtSec,10);
          dateTimeStr[17] = (char) (divResult.quot | 0x30);
          dateTimeStr[18] = (char) (divResult.rem  | 0x30);
        }
      }
      else
      {
        divResult   = div(dtmSec,100);
        tmpInt100   = divResult.quot;
        tmpInt      = divResult.rem;

        divResult   = div(tmpInt,10);
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
      if(periodMicros < periodMinMicros)
        periodMinMicros = periodMicros;
      if(periodMicros > PeriodMinTime)
      {
        periodFailAlarm = true;
        periodFailCount++;
      }

      divResult = div(periodMicros, 1000);
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

    for (int i = 0; i < LoopScreeningGrades; i++)
      statistics->rtSreening[i] = loopScreening[i];

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

  bool LoopCheck::setDateTime(const char *dtStr)
  {
     if(strlen(dtStr) < 23) return(false);
     strcpy(dateTimeStr,dtStr);
     dtYear   = (dateTimeStr[0]  & 0x0F) * 1000 +
                (dateTimeStr[1]  & 0x0F) * 100  +
                (dateTimeStr[2]  & 0x0F) * 10   +
                (dateTimeStr[3]  & 0x0F);

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
    dtYear    = dt.Year;
    dtMonth   = dt.Month;
    dtDay     = dt.Day;
    dtHour    = dt.Hour;
    dtMin     = dt.Minute;
    dtSec     = dt.Second;
    dtmSec    = dt.Millisecond;
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
    int error;
    unsigned long retv;

    error = clock_gettime(CLOCK_MONOTONIC, &clockTime);
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
  }

  // -------------------------------------------------------------------------
  // Debug-Funktionen
  // -------------------------------------------------------------------------
  //
