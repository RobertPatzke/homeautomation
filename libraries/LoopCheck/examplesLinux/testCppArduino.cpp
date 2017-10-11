//============================================================================
// Name        : testCppArduino.cpp
// Author      : Robert Patzke
// Version     :
// Copyright   : (c) MFP 2017
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "LoopCheck.h"
// environment.h will be included in LoopCheck.h
// You have to #define smnSimLinux there for IDE
// and comment out other definitions

LoopCheck loopCheck;

int main()
{
  printf("Test Arduino Software Environment\n");
  printf("Endless loop, terminate with Ctrl-C\n");

	while(1)
	{
	  loopCheck.begin();

	  if(loopCheck.timerMilli(0, 1000, 0))
	  {
	    printf(".\n");
	  }

	  loopCheck.end();
	}

  return 0;
}
