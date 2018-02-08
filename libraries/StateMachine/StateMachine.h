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

  bool      useProgList;            // control progList usage
  bool      loopProgList;           // looping proglist
  StatePtr  progList[NrOfSteps];    // dynamically created state run list
  int       progIndex;              // Program counter (index to next state)
  int       progEndIdx;             // Index to next empty progList entry
  StatePtr  finProgState;           // State after using state list

  bool      firstEnterToggle;       // Is set to true when state changes
  int       timeOutCounter;         // automatic decremented counter
  int       timeMeasureCounter;     // triggered automatic incremented counter
  bool      timeMeasureOn;          // control of the counter

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
  int       cycleTime;          // Cycle time in milliseconds
  int       frequency;          // Frequency in Hertz (1/s)
  int       userStatus;         // A number presenting the visible state

  // statistic information
  //
  int   noStateCounter;
  int   instNumber;
  int   runCounter;

  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  StateMachine(StatePtr firstState, int cycle);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void      run();                                  // has to be cyclic called
  void      setDelay(int delayTime);                // delay before next state
  void      setSpeed(int freq);                     // internal run frequency

  void      enter(StatePtr state);                  // set next state to run
  void      enter(StatePtr state, int delayTime);   // ... delayed

  void      enterRep(StatePtr state, int count);    // repeat next state
  void      enterRep(StatePtr state, int count, int delayTime);

  void      enterVia(StatePtr next, StatePtr future);       // next and after
  void      enterVia(StatePtr next, StatePtr future, int delayTime);

  void      enterList(StatePtr fin);
  void      enterList(StatePtr fin, int delayTime);

  void      enterListAt(StatePtr fin, int index);
  void      enterListAt(StatePtr fin, int index, int delayTime);

  bool      firstEnter();       // true only, if the state is first entered

  void      setTimeOut(int toValue);        // Set time-out counter
  bool      timeOut();                      // Check time-out counter
  void      startTimeMeasure();             // Start time measurement
  int       getTimeMeasure(bool stop);      // Time in milliseconds

  // -------------------------------------------------------------------------
  // debug functions
  // -------------------------------------------------------------------------
  //
  int       getDelay();

};




// ---------------------------------------------------------------------------
#endif
