// ---------------------------------------------------------------------------
// File:        StateMachine.cpp
// Editors:     Robert Patzke,
// Start:       07. February 2018
// Last change: 07. February 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include "StateMachine.h"

// ---------------------------------------------------------------------------
// Constructors and initialisations
// ---------------------------------------------------------------------------
//

int  StateMachine_InstCounter;
// This should be a static variable inside the class, but the C/C++ compiler
// of ESP8266 (did not test any other) does not handle it correctly.

StateMachine::StateMachine(StatePtr firstState, StatePtr anyState, int cycle)
{
  nextState     = firstState;
  doAlways      = anyState;
  cycleTime     = cycle;
  frequency     = 1000 / cycle;
  delay         = 0;
  delaySet      = 0;
  repeatDelay   = false;

  StateMachine_InstCounter++;
  instNumber    = StateMachine_InstCounter;
}

// ---------------------------------------------------------------------------
// run      State enter function, has to be cyclic called
// ---------------------------------------------------------------------------
//
void StateMachine::run()
{
  runCounter++;

  if(timeOutCounter > 0)
    timeOutCounter--;

  if(timeMeasureOn)
    timeMeasureCounter++;

  if(doAlways != NULL)
    doAlways();

  if(delay > 0)
  {
    delay--;
    return;
  }

  if(repeatDelay)
    delay = delaySet;

  if(useProgList)
  {
    if(progIndex == progEndIdx)
    {
      if(loopProgList)
      {
        progIndex = 0;
        nextState = progList[progIndex];
        progIndex++;
      }
      else
      {
        useProgList = false;
        nextState = finProgState;
      }
    }
    else
    {
      nextState = progList[progIndex];
      progIndex++;
    }
  }

  if(nextState != NULL)
    nextState();
  else
    noStateCounter++;

}

// ---------------------------------------------------------------------------
// service functions/methods for manipulating the state machine
// ---------------------------------------------------------------------------
//

// checking a state for staying (not enter new state)
//
bool StateMachine::stayHere()
{
  if(repeatState > 0)
  {
    repeatState--;
    if(repeatState == 0)
    {
      repeatDelay = false;
    }
    return(true);
  }

  firstEnterToggle = true;
  return(false);
}

// setting delay before next state
//
void StateMachine::setDelay(int delayTime)
{
  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;
}

// setting internal speed of the state machine
// can only be slower than the calling frequency
//
void StateMachine::setSpeed(int freq)
{
  delaySet      = delay = frequency / freq;
  repeatDelay   = true;
}

// setting next state to be called by run
//
void StateMachine::enter(StatePtr next)
{
  if(stayHere()) return;

  pastState = nextState;
  nextState = next;
}

// setting next state with delay (before)
//
void StateMachine::enter(StatePtr next, int delayTime)
{
  if(stayHere()) return;

  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;

  pastState = nextState;
  nextState = next;
}

// setting next state with repetition
//
void StateMachine::enterRep(StatePtr next, int count)
{
  if(stayHere()) return;

  repeatState   = count;
  pastState     = nextState;
  nextState     = next;
}

// setting next state with repetition and delay
//
void StateMachine::enterRep(StatePtr next, int count, int delayTime)
{
  if(stayHere()) return;

  delaySet = delay = (delayTime * frequency) / 1000;
  repeatDelay = true;

  repeatState   = count;
  pastState     = nextState;
  nextState     = next;
}

// setting state and state after
//
void StateMachine::enterVia(StatePtr next, StatePtr after)
{
  if(stayHere()) return;

  pastState     = nextState;
  nextState     = next;
  futureState   = after;
}

// setting state and state after with delay
//
void StateMachine::enterVia(StatePtr next, StatePtr after, int delayTime)
{
  if(stayHere()) return;

  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;

  pastState     = nextState;
  nextState     = next;
  futureState   = after;
}

// setting state to state list (program)
//
void StateMachine::enterList(StatePtr fin)
{
  if(stayHere()) return;

  pastState     = nextState;
  useProgList   = true;
  progIndex     = 0;
  finProgState  = fin;
}

// setting state to state list (program)and delay
//
void StateMachine::enterList(StatePtr fin, int delayTime)
{
  if(stayHere()) return;

  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;

  pastState     = nextState;
  useProgList   = true;
  progIndex     = 0;
  finProgState  = fin;
}

// setting state to state list (program) at a position
//
void StateMachine::enterListAt(StatePtr fin, int index)
{
  if(stayHere()) return;

  pastState     = nextState;
  useProgList   = true;
  progIndex     = index;
  finProgState  = fin;
}

// setting state to state list (program)at a position and delay
//
void StateMachine::enterListAt(StatePtr fin, int index, int delayTime)
{
  if(stayHere()) return;

  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;

  pastState     = nextState;
  useProgList   = true;
  progIndex     = index;
  finProgState  = fin;
}

// detecting the first call of a state
//
bool StateMachine::firstEnter()
{
  if(!firstEnterToggle)
    return(false);
  firstEnterToggle = false;
  return(true);
}

// set the time-out value (milliseconds)
//
void StateMachine::setTimeOut(int toValue)
{
  timeOutCounter = (toValue * frequency) / 1000;
}

// check the time-out counter
//
bool StateMachine::timeOut()
{
  if(timeOutCounter > 0)
    return(false);
  return(true);
}

// start time measurement
//
void StateMachine::startTimeMeasure()
{
  timeMeasureCounter = 0;
  timeMeasureOn = true;
}

int StateMachine::getTimeMeasure(bool stop)
{
  if(stop)
    timeMeasureOn = false;
  return(cycleTime * timeMeasureCounter);
}

// ---------------------------------------------------------------------------
// Debug and statistic functions/methods
// ---------------------------------------------------------------------------
//
int StateMachine::getDelay()
{
  return(delay);
}
