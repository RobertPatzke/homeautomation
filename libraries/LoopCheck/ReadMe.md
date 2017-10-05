# Tools for cyclic called procedures
Arduino Sketches are build on two basic functions. 
*void setup()* is called once when the CPU is reset and programmers place their initialisation code here.
*void loop()* is called in an endless loop, i.e. a cyclic entered function. 
But the cycle time is not determined, it depends on the speed of the CPU and the used resources.
Many examples for Arduino use the function *delay(milliseconds)* to organise a kind of timing. 
But this function is really freezing your program for the given number of milliseconds.
Using a real timer is a good solution, but some CPUs have only less timers 
and they sometimes are already used for some libraries.

The tools presented with LoopCheck-library give You the features of (many) timers inside *loop()* 
based on the Arduino-function *micros()* which is called with the macro SYSMICSEC, 
defined in *LoopCheck.h*.

You will find, that there is another file included: *environment.h*, which you can find here:
https://github.com/RobertPatzke/homeautomation/blob/developer/libraries/environment/environment.h
*environment.h* defines the IDE you are using, the CPU and specific development boards. 
Code is in several parts conditional, depending on the definitions you make in *environment.h*.
You will see, that this library is not fixed to Arduino, it may be used for any environment
where cyclic called functions happen.
