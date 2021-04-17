// ---------------------------------------------------------------------------
// This module provides tools for state machines
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      17.04.2021
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//
// The enumeration SmState is application specific and that is not a good
// environment for a module, which should be used in many applications
// So this module has to be copied and modified for different applications
// Originally it was a pert of class StateMachineHelper,
// now it is used in that class
//

package hsh.mplab.progtools;

public enum SmState   // Definition of possible internal states
{
  MinVal,             // First Entry
  Prepare,            // Preparation of the state machine
  InitTwitter,        // Our internal process is initialising
  ConfigTwitter,      // Configuration of Twitter will be done here
  InitFollower,       // next initialisation
  Error,              // Error state if initialisation fails
  Wait,               // Some Waiting time as part of the process
  Evaluate,           // Do something with other Twitters values
  ConfigPos,          // Configuration of local position
  MaxVal              // Last Entry
}

