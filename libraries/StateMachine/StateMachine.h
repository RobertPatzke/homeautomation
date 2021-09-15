// ---------------------------------------------------------------------------
// File:        StateMachine.cpp
// Editors:     Robert Patzke,
// Start:       07. February 2018
// Last change: 07. February 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include    "stdlib.h"


#ifndef _StateMachine_h
#define _StateMachine_h
// ---------------------------------------------------------------------------

#define NrOfSteps       32

#define AUTBREAK(x,y)   { x.enter(y); return; }

// ---------------------------------------------------------------------------
// class StateMachine
// ---------------------------------------------------------------------------
//

class StateMachine
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //
  typedef void (*StatePtr)(void);
  typedef unsigned long (*MicsecFuPtr)(void);

private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  int       delay;              // Setting of delay for next state
  int       delaySet;           // Setup value for repeated delays
  bool      repeatDelay;        // If true, every state is delayed (speed set)
  int       repeatState;        // Controlled repeating a state
  bool      useVarState;        // breaking the fixed sequence of states by
                                // using a variable state (in futureState)

  bool        useProgList;            // control progList usage
  bool        loopProgList;           // looping proglist
  StatePtr    progList[NrOfSteps];    // dynamically created state run list
  int         progIndex;              // Program counter (index to next state)
  int         progEndIdx;             // Index to next empty progList entry
  StatePtr    finProgState;           // State after using state list
  MicsecFuPtr micsFuPtr;              // Pointer to micsec function

  bool      firstEnterToggle;       // Is set to true when state changes
  int       timeOutCounter;         // automatic decremented counter

  bool            timeMeasureOn;          // control of the measurement counter
  unsigned long   timeMeasureCounter;     // triggered automatic incremented counter

  int       callCycleCnt;           // For cycles inside a state
  bool      markToggle;             // For bit complements
  bool      markOneShot;            // for doing only one time

  unsigned int  condCounter;        // Counter for questionable conditions

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //
  bool stayHere();

public:
  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //
  StatePtr  nextState;          // Pointer for indirect calling next state
  StatePtr  pastState;          // Pointer of the past state
  StatePtr  futureState;        // Pointer to a future state (see useVarState)
  StatePtr  doAlways;           // Pointer to a always called state
  int       cycleTime;          // Cycle time in milliseconds
  int       frequency;          // Frequency in Hertz (1/s)
  int       userStatus;         // A number presenting the visible state

  // statistic information
  //
  static int   StateMachine_InstCounter;

  int   noStateCounter;         // Counter for empty running of state machine
  int   instNumber;             // Number of this instance
  int   runCounter;             // Counter for running states
  int   curStateNumber;         // Number of current state

  unsigned long   curStateRuntime;      // run time of latest state
  unsigned long   maxStateRuntime[4];   // Maximum run time of a state
  unsigned long   maxRuntimeNumber[4];  // Number of the state of maximum runtime

  // error and debug support
  //
  int   userError;

  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  StateMachine();
  StateMachine(StatePtr firstState, StatePtr anyState, int cycle);
  void begin(StatePtr firstState, StatePtr anyState, int cycle);
  StateMachine(StatePtr firstState, StatePtr anyState, int cycle, MicsecFuPtr micsecFu);
  void begin(StatePtr firstState, StatePtr anyState, int cycle, MicsecFuPtr micsecFu);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void      run();                                  // has to be cyclic called
  void      setDelay(int delayTime);                // delay before next state
  void      setSpeed(int freq);                     // internal run frequency

  void      enter();                                // set next to future state
  void      enter(StatePtr state);                  // set next state to run
  void      enter(StatePtr state, int delayTime);   // ... delayed

  void      enterRep(StatePtr state, int count);    // repeat next state
  void      enterRep(StatePtr state, int count, int delayTime);

  void      call(StatePtr state);                   // set next state and return
  void      call(StatePtr state, int delayTime);    // ... delayed

  void      enterVia(StatePtr next, StatePtr future);       // next and then
  void      enterVia(StatePtr next, StatePtr future, int delayTime);

  void      enterList(StatePtr fin);
  void      enterList(StatePtr fin, int delayTime);

  void      enterListAt(StatePtr fin, int index);
  void      enterListAt(StatePtr fin, int index, int delayTime);

  bool      firstEnter();       // true only, if the state is first entered
  void      resetEnter();       // reset first enter mark
  bool      cycle(int cnt);     // true only, if called <cnt> times
  bool      cycleSec();         // true only, if a second passed
  bool      toggle();           // alternating return true and false
  bool      oneShot();          // only one time true for a call
  void      setOneShot();       // preparing oneShot to be true

  void      setTimeOut(int toValue);        // Set time-out counter
  bool      timeOut();                      // Check time-out counter
  void      startTimeMeasure();             // Start time measurement
  int       getTimeMeasure(bool stop);      // Time in milliseconds

  unsigned long getExtTimeMeasure(bool stop);     // Time in milliseconds

  void      setCondCounter(unsigned int cntVal);  // Set condition counter
  bool      condOpen();                           // Ask for open conditions

  // -------------------------------------------------------------------------
  // debug functions
  // -------------------------------------------------------------------------
  //
  int       getDelay();

};




// ---------------------------------------------------------------------------
#endif
