// ---------------------------------------------------------------------------
// This module provides tools for state machines
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      17.04.2021
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//
// The original idea was to create a class StateMachine, like that in C++
// used for the microcontroller software of DHA.
// But the author was not able to develop such a structure in JAVA,
// based on overwritten callback-functions.
// Therefore this helper-class was created to have similar features like in
// StateMachine-class but based on a switch-case grounded state machine

package hsh.mplab.progtools;

public class StateMachineHelper
{
  public  SmState nextState;

  private SmState pastState;
  private SmState futureState;

  private int     frequency;
  private int     globSeqCounter;
  private long    stateLoopCounter;
  private int     delay;
  private long    timeOutCounter;

  private int     repeatState;
  private boolean repeatDelay;
  private boolean firstEnterToggle;
  private boolean markToggle;
  private boolean markOneShot;

  // -------------------------------------------------------------------------
  // I n i t i a l i s a t i o n
  // -------------------------------------------------------------------------
  //
  public StateMachineHelper(SmState initState, int inFreq)
  {
    nextState         = initState;
    frequency         = inFreq;
    stateLoopCounter  = 0;
    globSeqCounter    = 0;
    delay             = 0;
    firstEnterToggle  = true;
  }

  // -------------------------------------------------------------------------
  // L o c a l   M e t h o d s
  // -------------------------------------------------------------------------
  //

  // checking a state for staying (not enter new state)
  //
  boolean stayHere()
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

  // -------------------------------------------------------------------------
  // U s e r   M e t h o d s
  // -------------------------------------------------------------------------
  //
  // -------------------------------------------------------------------------
  public boolean begin()
  {
    stateLoopCounter++;         // simply counting the enters

    if(timeOutCounter > 0) timeOutCounter--;

    // do this independent from state every half second
    //
    globSeqCounter++;
    if(globSeqCounter >= frequency/2)
    {
      globSeqCounter = 0;

    }

    if(delay > 0)
    {
      delay--;
      return(true);
    }

    return(false);
  }

  // -------------------------------------------------------------------------
  public void end()
  {

  }

  // -------------------------------------------------------------------------
  // detecting the first call of a state
  //
  public boolean firstEnter()
  {
    if(!firstEnterToggle)
      return(false);
    firstEnterToggle = false;
    return(true);
  }


  // -------------------------------------------------------------------------
  // setting future state to be called by run
  //
  public void enter()
  {
    if(stayHere()) return;

    pastState = nextState;
    nextState = futureState;
  }


  // -------------------------------------------------------------------------
  // setting next state to be called by run
  //
  public void enter(SmState next)
  {
    if(stayHere()) return;

    pastState   = nextState;
    nextState   = next;            // set the new state
  }

  // -------------------------------------------------------------------------
  // set the time-out value (milliseconds)
  //
  public void setTimeOut(int toValue)
  {
    timeOutCounter = (toValue * frequency) / 1000;
  }

  // -------------------------------------------------------------------------
  // check the time-out counter
  //
  public boolean timeOut()
  {
    if (timeOutCounter > 0)
      return (false);
    return (true);
  }

  // -------------------------------------------------------------------------
  // Alternativly returning true and false
  //
  public boolean toggle()
  {
    markToggle = !markToggle;
    return (markToggle);
  }

  // -------------------------------------------------------------------------
  // Doing only once
  //
  public boolean oneShot()
  {
    if(!markOneShot) return(false);

    markOneShot = false;
    return (true);
  }

  public void setOneShot()
  {
    markOneShot = true;
  }


}
