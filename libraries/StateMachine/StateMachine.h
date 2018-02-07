// ---------------------------------------------------------------------------
// File:        StateMachine.cpp
// Editors:     Robert Patzke,
// Start:       07. February 2018
// Last change: 07. February 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#ifndef _StateMachine_h
#define _StateMachine_h
// ---------------------------------------------------------------------------

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

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //


public:
  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //
  int   cycleTime;                      // Cycle time in milliseconds
  int   frequency;                      // Frequency in Hertz (1/s)

  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  StateMachine(StatePtr firstState, int cycle);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void      run();          // has to be cyclic called
  StatePtr  nextState;

};




// ---------------------------------------------------------------------------
#endif
