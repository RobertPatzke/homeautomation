//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Midi.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   27. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen für steuerbare Midi-Controller.
//

#include "Midi.h"

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------
//

void Midi::begin(int inBpm, NoteDiv inRes, int inMidiCycle, ComRingBuf *inCRB)
{
  dword       stdNoteMicroTick;

  crb = inCRB;                        // Zeiger auf Ringpuffer
  midiCycle = inMidiCycle;            // Zuklus der ZM in Mikrosekunden
  bpm = inBpm;                        // Beats Per Minute (Metronom)
  stdNoteMicroTick = 60000000 / bpm;  // Viertelnotenlänge in Mikrosekunden
  stdNoteCount = stdNoteMicroTick / inMidiCycle;  // "  in Zyklen der ZM
  stdNoteTick = 60000 / bpm;          // Viertelnotenlänge in Millisekunden
  minNoteTick = stdNoteTick / inRes;  // Zu erwartende kürzeste Note (ms)

  typeList[nti0].length   = 10 * stdNoteCount;
  typeList[nti1].length   = 4 * stdNoteCount;
  typeList[nti2].length   = 2 * stdNoteCount;
  typeList[nti2p].length  = 3 * stdNoteCount;
  typeList[nti4].length   = stdNoteCount;
  typeList[nti4p].length  = stdNoteCount + stdNoteCount / 2;
  typeList[nti8].length   = stdNoteCount / 2;
  typeList[nti8p].length  = stdNoteCount / 2 + stdNoteCount / 4;
  typeList[nti16].length  = stdNoteCount / 4;
  typeList[nti16p].length = stdNoteCount / 4 + stdNoteCount / 8;
  typeList[nti32].length  = stdNoteCount / 8;
  typeList[nti32p].length = stdNoteCount / 8 + stdNoteCount / 16;
  typeList[nti64].length  = stdNoteCount / 16;
  typeList[nti64p].length = stdNoteCount / 16 + stdNoteCount / 32;

  opMode = momIdle;
  chn = 0;
  next(smInit);
}


// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
void Midi::setNoteType(NoteTypeIdx nt)
{
  NoteTypePtr typePtr;

  typePtr = &typeList[nt];
  typePtr->attack   = 0;
  typePtr->decay    = 0;
  typePtr->sustain  = typePtr->length;
  typePtr->release  = 0;

  typePtr->deltaAttack    = 0;
  typePtr->deltaDecay     = 0;
  typePtr->percentSustain = 70;
  typePtr->deltaRelease   = 0;
}


void Midi::setNoteType(NoteTypeIdx nt, dword att, dword dec, dword sus, dword rel,
                       byte dAtt, byte dDec, byte pSus, byte dRel)
{
  NoteTypePtr typePtr;

  typePtr = &typeList[nt];
  typePtr->attack   = att;
  typePtr->decay    = dec;
  typePtr->sustain  = sus;
  typePtr->release  = rel;

  typePtr->deltaAttack    = dAtt;
  typePtr->deltaDecay     = dDec;
  typePtr->percentSustain = pSus;
  typePtr->deltaRelease   = dRel;
}

int Midi::addChordNote(NoteTypeIdx nti, byte val, byte vel)
{
  NotePtr notePtr;
  int     i;

  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
    {
      notePtr->mode   = NoteModeRun;
      notePtr->nIdx   = nti;
      notePtr->value  = val;
      notePtr->veloc  = vel;
      break;
    }
  }
  return(i);
}


// ----------------------------------------------------------------------------
// Betrieb
// ----------------------------------------------------------------------------
//
void Midi::setOpMode(MidiOpMode mom)
{
  opMode = mom;
}


// ----------------------------------------------------------------------------
// Steuerung, Zustandsmaschine
// ----------------------------------------------------------------------------
//
void Midi::run()
{
  runCounter++;
  if(cycleCnt > 0) cycleCnt--;

  if(nextState != NULL)
    (this->*nextState)();
}

void Midi::smInit()
{
  next(smIdle);
}

void Midi::smIdle()
{
  switch(opMode)
  {
    case momIdle:
      break;

    case momSequence:
      next(smNoteOn);
      break;

    case momRunDelta:
      break;
  }
}

void Midi::smNoteOn()
{
  int   i, j, nIdx;
  bool  doAttack;
  dword attack, sustain;

  if(crb == NULL)
  {
    next(smIdle);
    return;
  }

  doAttack = false;
  j = 0;
  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];

    if(i == 0)
    {
      if(notePtr->mode == NoteModeEmpty)
      {
        next(smIdle);
        return;
      }
      noteSeq[j++] = 0x90 | chn;
    }
    else
    {
      if(notePtr->mode == NoteModeEmpty)
        break;
    }

    noteSeq[j++] = notePtr->value;

    nIdx = notePtr->nIdx;
    typePtr = &typeList[nIdx];
    notePtr->cntAttack  = typePtr->attack;
    notePtr->cntDecay   = typePtr->decay;
    notePtr->cntSustain = typePtr->sustain;
    notePtr->cntRelease = typePtr->release;

    if(notePtr->cntAttack != 0)
    {
      doAttack = true;
      attack = (typePtr->deltaAttack * notePtr->veloc) / 100;
      if(attack > 127) attack = 127;
      noteSeq[j++] = attack;
    }
    else
    {
      sustain = (typePtr->percentSustain * notePtr->veloc) / 100;
      if(sustain > 127) sustain = 127;
      noteSeq[j++] = sustain;
    }
  }

  crb->putSeq(noteSeq, j);

  if(doAttack)
    next(smAttack);
  else
    next(smSustain);
}

void Midi::smAttack()
{

}

void Midi::smDecay()
{

}

void Midi::smSustain()
{
  int   i;
  bool  sustFin;

  sustFin = false;
  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
      break;

    if(notePtr->cntSustain > 0)
      notePtr->cntSustain--;
    else
      sustFin = true;
  }

  if(sustFin)
    next(smNoteOff);
}

void Midi::smRelease()
{

}

void Midi::smNoteOff()
{
  int   i,j;

  j = 0;
  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
      break;

    if(i == 0)
    {
      noteSeq[j++] = 0x80 | chn;
    }

    noteSeq[j++] = notePtr->value;
    noteSeq[j++] = 0;
  }

  crb->putSeq(noteSeq, j);
  absPause = 500000 / midiCycle;

  next(smPause);
}

void Midi::smPause()
{
  if(absPause > 0)
  {
    absPause--;
    return;
  }

  next(smNoteOn);
}

// ----------------------------------------------------------------------------
// Debugging
// ----------------------------------------------------------------------------
//









