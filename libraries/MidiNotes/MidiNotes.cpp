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
//

#include "MidiNotes.h"

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------
//

void MidiNotes::begin(int inBpm, NoteDiv inRes, int inMidiCycle, IntrfBuf *inCRB)
{
  dword       stdNoteMicroTick;

  crb = inCRB;                        // Zeiger auf Ringpuffer
  midiCycle = inMidiCycle;            // Zuklus der ZM in Mikrosekunden
  bpm = inBpm;                        // Beats Per Minute (Metronom)
  stdNoteMicroTick = 60000000 / bpm;  // Viertelnotenlänge in Mikrosekunden
  stdNoteCount = stdNoteMicroTick / inMidiCycle;  // "  in Zyklen der ZM
  stdNoteTick = 60000 / bpm;          // Viertelnotenlänge in Millisekunden
  minNoteTick = stdNoteTick / inRes;  // Zu erwartende kürzeste Note (ms)

  typeList[nti0].length   = 8 * stdNoteCount;
  setNoteType(nti0);

  typeList[nti1].length   = 4 * stdNoteCount;
  setNoteType(nti1);

  typeList[nti2p].length  = 3 * stdNoteCount;
  setNoteType(nti2p);

  typeList[nti2].length   = 2 * stdNoteCount;
  setNoteType(nti2);

  typeList[nti4p].length  = stdNoteCount + stdNoteCount / 2;
  setNoteType(nti4p);

  // Standard-Note = Viertelnote (
  typeList[nti4].length   = stdNoteCount;
  setNoteType(nti4);

  typeList[nti8].length   = stdNoteCount / 2;
  setNoteType(nti8);

  typeList[nti8p].length  = stdNoteCount / 2 + stdNoteCount / 4;
  setNoteType(nti8p);

  typeList[nti16].length  = stdNoteCount / 4;
  setNoteType(nti16);

  typeList[nti16p].length = stdNoteCount / 4 + stdNoteCount / 8;
  setNoteType(nti16p);

  typeList[nti32].length  = stdNoteCount / 8;
  setNoteType(nti32);

  typeList[nti32p].length = stdNoteCount / 8 + stdNoteCount / 16;
  setNoteType(nti32p);

  typeList[nti64].length  = stdNoteCount / 16;
  setNoteType(nti64);

  typeList[nti64p].length = stdNoteCount / 16 + stdNoteCount / 32;
  setNoteType(nti64p);

  opMode = momIdle;
  setChannel(1);
  stopRun = false;
  stoppedRun = false;
  next(smInit);
}


// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
void MidiNotes::setNoteType(NoteTypeIdx nt)
{
  NoteTypePtr typePtr;

  typePtr = &typeList[nt];
  typePtr->attack   = 0;
  typePtr->decay    = 0;
  typePtr->sustain  = typePtr->length;
  typePtr->release  = 0;
  typePtr->pause    = (typePtr->length * 20) / 100;

  typePtr->deltaAttack    = 0;
  typePtr->deltaDecay     = 0;
  typePtr->percentSustain = 70;
  typePtr->deltaRelease   = 0;
}


void MidiNotes::setNoteType(NoteTypeIdx nt, byte pAttL, byte pDecL, byte pSusL, byte pRelL,
                            byte pPauL, byte dAtt, byte dDec, byte pSusV, byte dRel)
{
  NoteTypePtr typePtr;

  typePtr = &typeList[nt];
  typePtr->attack   = (typePtr->length * pAttL) / 100;
  typePtr->decay    = (typePtr->length * pDecL) / 100;
  typePtr->sustain  = (typePtr->length * pSusL) / 100;
  typePtr->release  = (typePtr->length * pRelL) / 100;
  typePtr->pause    = (typePtr->length * pPauL) / 100;

  typePtr->deltaAttack    = dAtt;
  typePtr->deltaDecay     = dDec;
  typePtr->percentSustain = pSusV;
  typePtr->deltaRelease   = dRel;
}

int MidiNotes::addChordNote(NoteTypeIdx nti, byte val, byte vel)
{
  NotePtr notePtr;
  int     i;

  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
    {
      notePtr->mode     = NoteModeRun;
      notePtr->typeIdx  = nti;
      notePtr->value    = val;
      notePtr->veloc    = vel;
      break;
    }
  }
  return(i);
}

void MidiNotes::setChannel(int chnVal)
{
  if(chnVal < 1) chnVal = 1;
  if(chnVal > 16) chnVal = 16;
  chn = chnVal - 1;
}

// ----------------------------------------------------------------------------
// Betrieb
// ----------------------------------------------------------------------------
//
void MidiNotes::setOpMode(MidiOpMode mom)
{
  opMode = mom;
}

int MidiNotes::getOpMode()
{
  return(opMode);
}


void MidiNotes::setChordNote(int idx, NoteTypeIdx nti, int val, int vel)
{
  if(idx < 0) return;
  if(idx >= MaxNrNoteSim) return;

  if(nti >= 0 && nti < ntiNr)
    newNote[idx].typeIdx = nti;

  if(val >= 0 && val <= 127)
    newNote[idx].value = val;

  if(vel >= 0 && vel <= 127)
    newNote[idx].veloc = vel;

  newNote[idx].newVal = true;
}

void MidiNotes::setDeltaNote(int idx, byte val, byte vel)
{
  deltaNote[idx].oldValue   = deltaNote[idx].value;
  deltaNote[idx].oldVeloc   = deltaNote[idx].veloc;
  deltaNote[idx].value  = val;
  deltaNote[idx].veloc  = vel;
  deltaNote[idx].newVal = true;
  lastDeltaIdx = idx;
}


// ----------------------------------------------------------------------------
// Steuerung, Zustandsmaschine
// ----------------------------------------------------------------------------
//

void MidiNotes::stop()
{
  stopRun = true;
}

void MidiNotes::resume()
{
  stopRun = false;
  stoppedRun = false;
}

void MidiNotes::run()
{
  runCounter++;
  if(cycleCnt > 0) cycleCnt--;

  if(nextState != NULL)
    (this->*nextState)();
}

void MidiNotes::smInit()
{
  next(smIdle);
}

void MidiNotes::smIdle()
{
  switch(opMode)
  {
    case momIdle:
      break;

    case momSequence:
      next(smNoteOn);
      break;

    case momRunDelta:
      next(smWaitDeltaNote);
      break;
  }
}

// ============================================================================
// m o m S e q u e n c e
// ============================================================================
//
void MidiNotes::smNoteOn()
{
  int   i, j, tIdx;
  bool  doAttack;
  dword attack, sustain;

  if(stopRun || stoppedRun)         // Unterbrechen/Stoppen des Ablaufs
  {                                 // vor dem Einschalten einer Note
    stoppedRun = true;
    return;
  }

  if(crb == NULL)                   // Ohne Ausgabepuffer in Wartezustand
  {
    next(smIdle);
    return;
  }

  doAttack = false;                 // Voreinstellung kein Aufklingen

  // Auslesen der Noten aus dem Akkordspeicher
  //
  j = 0;
  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];

    if(i == 0)  // erste Note
    {
      if(notePtr->mode == NoteModeEmpty)  // Wenn die erste Note leer ist
      {                                   // dann in den Wartezustand
        next(smIdle);
        return;
      }
      noteSeq[j++] = 0x90 | chn;          // ansonsten startet die Notenfolge **
    }
    else        // weitere Noten
    {
      if(notePtr->mode == NoteModeEmpty)  // bei leerer Note Schleife beendet
        break;
    }

    // Die Noten im Akkordspeicher können durch aktuelle Noten
    // ersetzt werden.

    if(newNote[i].newVal) // wenn neue Note vorliegt
    {
      newNote[i].newVal = false;                  // neue Note quittieren
      notePtr->typeIdx = newNote[i].typeIdx;      // und Inhalte im
      notePtr->value = newNote[i].value;          // Akkordspeicher
      notePtr->veloc = newNote[i].veloc;          // überschreiben
    }

    noteSeq[j++] = notePtr->value;        // Notenwert in Sequenz eintragen   **

    // Daten für die Note aus der Typenliste holen
    //
    tIdx = notePtr->typeIdx;
    typePtr = &typeList[tIdx];
    notePtr->cntAttack  = typePtr->attack;    // Aufklingzeit in Zähler
    notePtr->cntDecay   = typePtr->decay;     // Abklingzeit in Zähler
    notePtr->cntSustain = typePtr->sustain;   // Klingzeit in Zähler
    notePtr->cntRelease = typePtr->release;   // Ausklingzeit in Zähler
    notePtr->cntPause   = typePtr->pause;     // Pausenzeit in Zähler

    if(notePtr->cntAttack != 0)   // Wenn ein Attack-Wert gegeben ist
    {
      doAttack = true;            // dann attack markieren
      attack =                    // und den Wert auf den erste Schritt setzen
          (typePtr->deltaAttack * notePtr->veloc) / 100;
      if(attack > 127) attack = 127;
      noteSeq[j++] = attack;              // Lautstärke in Sequenz eintragen  **
    }
    else  // ohne Attack-Wert geht es hier gleich in Sustain weiter
    {
      sustain = (typePtr->percentSustain * notePtr->veloc) / 100;
      if(sustain > 127) sustain = 127;
      noteSeq[j++] = sustain;             // Lautstärke in Sequenz eintragen  **
    }
  }

  crb->putSeq(noteSeq, j);                // Sequenz an Puffer übergeben   *****

  if(doAttack)
    next(smAttack);
  else
    next(smSustain);
}

void MidiNotes::smAttack()
{

}

void MidiNotes::smDecay()
{

}

// TODO
// Es können noch nicht Noten unterschiedlicher Länge in einem Akkord
// verarbeitet werden. Bei mehreren eingetragenen Noten würde die
// kürzeste Note den Ablauf bestimmen.

void MidiNotes::smSustain()
{
  int   i;
  bool  sustFin;

  sustFin = false;
  for(i = 0; i < MaxNrNoteSim; i++)
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
      break;

    if(notePtr->cntSustain > 0)     // Die Sustain-Zeit in diesem Zustand verweilen
      notePtr->cntSustain--;
    else
      sustFin = true;
  }

  if(sustFin)
    next(smNoteOff);
}

void MidiNotes::smRelease()
{

}

void MidiNotes::smNoteOff()
{
  int   i,j;

  j = 0;
  for(i = 0; i < MaxNrNoteSim; i++)   // Alle Noten im Akkord bearbeiten
  {
    notePtr = &chord[i];
    if(notePtr->mode == NoteModeEmpty)
      break;

    if(i == 0)
    {
      noteSeq[j++] = 0x80 | chn;      // Erste Note bestimmt den Befehl AUS    **
      absPause = notePtr->cntPause;
    }

    noteSeq[j++] = notePtr->value;    //Erste und weitere Noten liefern Liste  **
    noteSeq[j++] = 64;
  }

  crb->putSeq(noteSeq, j);            // Sequenz an Puffer übergeben   *****

  next(smPause);
}

void MidiNotes::smPause()
{
  if(absPause > 0)
  {
    absPause--;
    return;
  }

  if(opMode != momSequence)
    next(smIdle);
  else
    next(smNoteOn);
}

// ============================================================================
// m o m R u n D e l t a
// ============================================================================
//
void MidiNotes::smWaitDeltaNote()
{
  if(opMode != momRunDelta)
  {
    next(smReleaseOldNote);
    return;
  }

  if(!deltaNote[lastDeltaIdx].newVal) return;

  next(smReleaseOldNote);
}

void MidiNotes::smReleaseOldNote()
{
  int j = 0;

  if(deltaNote[lastDeltaIdx].oldValue != 0)
  {
    noteSeq[j++] = 0x80 | chn;
    noteSeq[j++] = deltaNote[lastDeltaIdx].oldValue;
    noteSeq[j++] = 64;
    crb->putSeq(noteSeq, j);
  }

  if(opMode != momRunDelta)
    next(smIdle);
  else
    next(smStartNewNote);
}

void MidiNotes::smStartNewNote()
{
  int j = 0;

  noteSeq[j++] = 0x90 | chn;
  noteSeq[j++] = deltaNote[lastDeltaIdx].value;
  noteSeq[j++] = deltaNote[lastDeltaIdx].veloc;
  crb->putSeq(noteSeq, j);

  deltaNote[lastDeltaIdx].newVal = false;
  next(smWaitDeltaNote);
}

// ----------------------------------------------------------------------------
// Debugging
// ----------------------------------------------------------------------------
//









