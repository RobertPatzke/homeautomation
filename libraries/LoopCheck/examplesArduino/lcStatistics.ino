// Using LoopCheck-Library for information about the loop
//

#include "Arduino.h"
#include "LoopCheck.h"

LoopCheck loopCheck;
// Creating an instance of class LoopCheck
// Some compilers take a huge basic load of program space, if you use the
// new operator. That is the reason, why we use a static instantiation.


//The setup function is called once at startup of the sketch
//
void setup()
{
  Serial.begin(115200);
  // Prepare serial interface to print information

  Serial.println("Example for using loop statistics");
}

OpHourMeter     cpuRunTime;
// This structure will hold the runtime of the CPU since reset
// organised in years,days,hours,minutes,seconds and milliseconds
// after calling <operationTime>

LoopStatistics  statistics;
// This structure will hold statistical information about the loop
// after calling <getStatistics>

bool    useDelay;
// A marker to show the influence of delay function on LoopCheck operation time

// The loop function is called in an endless loop
//
void loop()
{
  loopCheck.begin();
  // This method has to be called, whenever entering the cyclic used function

  if(loopCheck.once(0,1))
  {
    // 1. arg is task index (0-7), 2. arg is loop number
    // I.e. : do only once with the first loop
    useDelay = true;
    // Of course this could be better done in the setup-function (above),
    // but it is a chance to show the once-function of LoopCheck.
    // Mind the 2. argument, you can do setups subsequently because
    // once-function is skipping the entered number of loops
  }

  if(loopCheck.timerMilli(0, 1000, 25))
  {
    // The timer-function returns true whenever 1000 milliseconds passed
    // but only 25 times
    // The example <lcBlink.ino> for more details

    loopCheck.operationTime(&cpuRunTime);
    Serial.printf("%02dH,%02dm,%02ds\r\n",
                  cpuRunTime.Hours,cpuRunTime.Minutes,cpuRunTime.Seconds);
  }

  if(useDelay == true) delay(33);
  // To have something to measure for the statistical information
  // But see the influence on the operation time at the end of the code

  if(loopCheck.timerMilli(1, 2000, 10))
  {
    // Do the following every 2 seconds but only 10 times

    loopCheck.getStatistics(&statistics);
    Serial.printf("AvgInLoopTime: %05d[us], AvgOutLoopTime: %05d[us]\r\n",
                  statistics.loopAvgTime,statistics.bgAvgTime);
  }

  if(loopCheck.timerMilli(2, 10000, 1))
  {
    // switch off the delay after 10 seconds
    useDelay = false;
  }

  loopCheck.end();
  // This method has to be called, whenever leaving the cyclic used function
}

// This example looks rather complex for simply showing the elapsed operation time.
// But it had to be shown, that the operation time counter only works
// if the loop cycle time (time between two following loops) is less than
// 1 millisecond.
// This is not a mistake. LoopCheck was built to watch and control the behaviour
// of the Arduino loop and it shall be handled as a mistake, if you write a
// program that holds the CPU such a long time.
// Check the statistics to observe also the influence of resources running in background.


