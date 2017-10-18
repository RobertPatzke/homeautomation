//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Software Loop Checking and Timing
// Datei:   testLoopCheck.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (see Wikipedia: Creative Commons)
//
// This program was developed and tested with Visual Studio 2010
// Following configuration has to be done in C/C++ -> Preprocessor Definitions
//
// UseGithubPath
// Visual Studio searches for includes in the environment of the sources.
// With the switch UseGithubPath the #include directives in source files are
// relativ to the structure of the tree as it is used on Github repository.
// So it will work after simple copying the tree from Github.
//
// smnDEFBYBUILD
// With this switch the first part of file environment.h will be used.
// It is possible to use environment.h for many purposes. 
// Without defining smnDEFBYBUILD the file has to be edited to fit to your IDE
// and your microcontroller targets
//
// smnWIN32_VS
// This switch opens a list of definitions in environment.h controlling 
// conditional compilation in many source files

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#include "../LoopCheck.h"

LoopCheck loopCheck;

int main(int argc, char *argv[])
{
  time_t          timeSec;
  struct  tm      *timeStructPtr;
  struct  timeb   ftimeStruct;
  lcDateTime      dateTime;
  int             msecOldPC, msecOldLC, msecOldOP;
  int             secOldPC, secOldLC, secOldOP;
  LoopStatistics  statistic;

  // -------------------------------------------------------------------------
  // setup
  // -------------------------------------------------------------------------
  //
  printf("Testing simulation of Arduino with Windows\n");
  printf("Check loop behaviour with LoopCheck, demonstrate clocks\n");
  
  // Getting PC time with milliseconds resolution
  //
  ftime(&ftimeStruct);
  timeSec = ftimeStruct.time;
  timeStructPtr = gmtime(&timeSec);

  // Preparing some Variables to calculate and show time difference
  //
  msecOldPC = msecOldLC = dateTime.Millisecond = ftimeStruct.millitm;
  secOldPC = secOldLC = dateTime.Second       = timeStructPtr->tm_sec;

  // Setting the software clock of LoopCheck to PC time
  //
  dateTime.Minute       = timeStructPtr->tm_min;
  dateTime.Hour         = timeStructPtr->tm_hour;
  dateTime.Day          = timeStructPtr->tm_mday;
  dateTime.Month        = timeStructPtr->tm_mon + 1;
  dateTime.Year         = timeStructPtr->tm_year + 1900;

  loopCheck.setDateTime(dateTime);

  // -------------------------------------------------------------------------
  // loop
  // -------------------------------------------------------------------------
  //
  while(1)
  {
    loopCheck.begin();      // this function has to be called first in loop
    // -----------------------------------------------------------------------

    // Comparing the software clock with the PC clock and showing the difference
    //
    if(loopCheck.timerMilli(0,1000,0))
    {
      //
      // This gets true once every second (1000 milliseconds)
      // The error is that of the PC counter (clock or performance timer)

      // Get the current PC time with milliseconds
      //
      ftime(&ftimeStruct);
      timeSec = ftimeStruct.time;
      timeStructPtr = gmtime(&timeSec);

      // Get the current time of LoopCheck software clock
      //
      loopCheck.getDateTime(&dateTime);

      // Show the PC time and the LoopCheck time and their differences to the last measurement
      // a second back (do not mind the overflow error with the distance)
      //
      printf("PC Time = %02d:%02d:%02d,%03d Diff=%4d     LC Time = %02d:%02d:%02d,%03d Diff=%4d\n",
        timeStructPtr->tm_hour, timeStructPtr->tm_min, timeStructPtr->tm_sec, ftimeStruct.millitm,
        ftimeStruct.millitm - msecOldPC + 1000*(timeStructPtr->tm_sec - secOldPC),
        dateTime.Hour, dateTime.Minute, dateTime.Second, dateTime.Millisecond,
        dateTime.Millisecond - msecOldLC + 1000*(dateTime.Second - secOldLC));
      //
      // You will see, that the timer of LoopCheck is rather accurate. 
      // The variation of repetition time is less than 1% with some exceptions.
      // Also the LoopCheck software clock is nearly perfect, because the
      // underlaying counters are synchronised to the OS clock

      // preperation for the measurement of next cycle
      //
      msecOldPC = ftimeStruct.millitm;
      msecOldLC = dateTime.Millisecond;
      secOldPC = timeStructPtr->tm_sec;
      secOldLC = dateTime.Second;

      // to show the difference between microcontrollers and OS based computers
      // we will look into the statistics of LoopCheck
      //
      loopCheck.getStatistics(&statistic);

      // alarmCount is incremented, whenever the time between 2 loops exceeds PeriodMinTime
      // (adjust PeriodMinTime in LoopCheck.h for your target system)
      //
      printf("Loop violations: %4d ", statistic.alarmCount);
      //
      // we have also a classification of exceeding the millisecond of loop cycle time
      // counting the number of exceedings indexed by number of milliseconds.
      //
      for(int i = 0; i < LoopScreeningGrades - 1; i++)
        printf("  %2dms: %4d", i+1 , statistic.rtSreening[i]);
      printf("  >%2dms: %4d\r\n\n", LoopScreeningGrades - 1, statistic.rtSreening[LoopScreeningGrades - 1]);

    }

    // -----------------------------------------------------------------------
    loopCheck.end();        // this function has to be called last in the loop
  }
}
