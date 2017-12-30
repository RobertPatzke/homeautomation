// Demo-Program for using library LoopCheck with Arduino
#include "Arduino.h"

#include "LoopCheck.h"

#define TestLed 13
// The built in LED of some Arduino boards (and similar development boards like
// ESP32 Dev) seem to be connected to a pin (most 13), which is also used for the
// serial transmit (TxD) or other purposes.
// The blink example of Arduino works, because they switch the pin and then
// they freeze the program bei function <delay(milliseconds)>.
// But if you leave loop() for Arduino internal functions, the pin is
// no more valid, it is used for TxD (or some other purpose).
// In other words, with some (many?) boards, the blink example of Arduino does 
// not work without the delay in loop-function.
// Because we now use a software-timer instead of a delay,
// we must use another pin to connect our own LED or look for a built-in LED
// which is not used outside loop-function.
// E.g. the blue LED of ESP32 DEVKIT V1 (doit) is connected at pin 2.

LoopCheck loopCheck;
// Creating an instance of class LoopCheck
// Some compilers take a huge basic load of program space, if you use the
// new operator. That is the reason, why we use a static instantiation.

//The setup function is called once at startup of the sketch
//
void setup()
{
  pinMode(TestLed, OUTPUT);
  // Setting a free pin for output (connect a LED via serial resistor)
}

// The loop function is called in an endless loop
//
void loop()
{
  loopCheck.begin();
  // This method has to be called, whenever entering the cyclic used function

  if(loopCheck.timerMilli(0, 500, 0))
  {
    // This is a timer of LoopCheck:
    // The first argument is an index (0 - NrOfTimerTasks) which is identifying
    // a timer. timerMicro and timerMilli use the same function in LooopCheck.cpp
    // and so You have only NrOfLoopTasks timers, either timerMilli or timerMicro.
    // The second argument is the cycle time (in milliseconds with timerMilli
    // and in microseconds with timerMicro.
    // The third argument is the number of cycles. The value 0 stands for an
    // endless running of the timer.

    if(loopCheck.toggle(0))
    {
      // This is a toggle of LoopCheck:
      // This function returns alternating true and false. The argument is an
      // index (0 - NrOfToggleTasks) to identify the toggle. There is no relation
      // between the indexes of timers and toggles.

      digitalWrite(TestLed, HIGH);
      // Switching on the LED at pin TestLed
    }
    else
    {
      digitalWrite(TestLed, LOW);
      // Switching off the LED
    }
  }

  loopCheck.end();
  // This method has to be called, whenever leaving the cyclic used function
}

// With using LoopCheck, the blinking of a LED does not freeze the loop function.
// Depending on the speed of your CPU, this programm takes mikroseconds or only
// fractions of microsecond in one loop and the LED is switched on and off every second.
// You can use up to NrOfTimerTask timers (defined in LoopCheck.h) inside loop
// without creating a big load for the CPU.
// The timers do not fire in the same cycle. If You use many timers, they do
// not meet in the same cycle of the loop. So it will not happen, that your
// application creates a long duration of loop if all timers meet the same
// ending time.
