// ---------------------------------------------------------------------------
// File:        StateMachine.cpp
// Editors:     Robert Patzke,
// Start:       07. February 2018
// Last change: 22. February 2021
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include "StateMachine.h"

// ---------------------------------------------------------------------------
// Constructors and initialisations
// ---------------------------------------------------------------------------
//

int  StateMachine::StateMachine_InstCounter;

StateMachine::StateMachine(){;} // @suppress("Class members should be properly initialized")

StateMachine::StateMachine(StatePtr firstState, StatePtr anyState, int cycle)
{
  begin(firstState, anyState, cycle);
}

StateMachine::StateMachine(StatePtr firstState, StatePtr anyState, int cycle, MicsecFuPtr micsecFu)
{
  begin(firstState, anyState, cycle, micsecFu);
}

void StateMachine::begin(StatePtr firstState, StatePtr anyState, int cycle)
{
  begin(firstState, anyState, cycle, NULL);
}

void StateMachine::begin(StatePtr firstState, StatePtr anyState, int cycle, MicsecFuPtr micsecFu)
{
  nextState     = firstState;
  doAlways      = anyState;
  cycleTime     = cycle;
  frequency     = 1000 / cycle;
  delay         = 0;
  delaySet      = 0;
  repeatDelay   = false;
  userStatus    = 0;

  StateMachine_InstCounter++;
  instNumber    = StateMachine_InstCounter;
  micsFuPtr     = micsecFu;
}

// ---------------------------------------------------------------------------
// run      State enter function, has to be cyclic called
// ---------------------------------------------------------------------------
//
void StateMachine::run()
{
  unsigned long startMics = 0, diffMics;

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
  {
    if(micsFuPtr != NULL)
      startMics = micsFuPtr();

    nextState();

    if(micsFuPtr != NULL)
    {
      diffMics = micsFuPtr() - startMics;
      curStateRuntime = diffMics;

      if(diffMics > maxStateRuntime[0])
      {
        maxStateRuntime[0]  = diffMics;
        maxRuntimeNumber[0] = curStateNumber;
      }
      else if(diffMics > maxStateRuntime[1])
      {
        if(curStateNumber != maxRuntimeNumber[0])
        {
          maxStateRuntime[1]  = diffMics;
          maxRuntimeNumber[1] = curStateNumber;
        }
      }
      else if(diffMics > maxStateRuntime[2])
      {
        if(curStateNumber != maxRuntimeNumber[1])
        {
          maxStateRuntime[2]  = diffMics;
          maxRuntimeNumber[2] = curStateNumber;
        }
      }
      else if(diffMics > maxStateRuntime[3])
      {
        if(curStateNumber != maxRuntimeNumber[2])
        {
          maxStateRuntime[3]  = diffMics;
          maxRuntimeNumber[3] = curStateNumber;
        }
      }
    }
  }
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

// setting future state to be called by run
//
void StateMachine::enter()
{
  if(stayHere()) return;

  pastState = nextState;
  nextState = futureState;
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
void StateMachine::enterVia(StatePtr next, StatePtr then)
{
  if(stayHere()) return;

  pastState     = nextState;
  nextState     = next;
  futureState   = then;
}

// calling state
//
void StateMachine::call(StatePtr next)
{
  if(stayHere()) return;

  pastState     = nextState;
  futureState   = nextState;
  nextState     = next;
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

// calling state
//
void StateMachine::call(StatePtr next, int delayTime)
{
  if(stayHere()) return;

  delay = (delayTime * frequency) / 1000;
  repeatDelay = false;

  pastState     = nextState;
  futureState   = nextState;
  nextState     = next;
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

// counting local cycles
//
bool StateMachine::cycle(int cnt)
{
  if(callCycleCnt <= 0)
  {
    callCycleCnt = cnt;
    return(true);
  }
  callCycleCnt--;
  return(false);
}
//
bool StateMachine::cycleSec()
{
  if(callCycleCnt <= 0)
  {
    callCycleCnt = frequency;
    return(true);
  }
  callCycleCnt--;
  return(false);
}

// Alternativly returning true and false
//
bool StateMachine::toggle()
{
  markToggle = !markToggle;
  return(markToggle);
}

// Doing only once
//
bool StateMachine::oneShot()
{
  if(!markOneShot) return(false);

  markOneShot = false;
  return (true);
}

void StateMachine::setOneShot()
{
  markOneShot = true;
}


// Setable number of returning TRUE
//
void  StateMachine::setCondCounter(unsigned int condVal)
{
  condCounter = condVal;
}

bool  StateMachine::condOpen()
{
  if(condCounter == 0)
    return(false);
  condCounter--;
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

unsigned long StateMachine::getExtTimeMeasure(bool stop)
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
