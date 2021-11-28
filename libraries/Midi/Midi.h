//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Midi.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   27. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen für steuerbare Midi-Controller
//

#ifndef Midi_h
#define Midi_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"
#include "ComRingBuf.h"

#define MaxNrNoteSim    4
#define MaxMidiSeq      (2 * MaxNrNoteSim + 1)

typedef enum  _NoteDiv
{
  nd4   = 1,
  nd8   = 2,
  nd16  = 4,
  nd32  = 8,
  nd64  = 16
} NoteDiv;

typedef enum  _MidiOpMode
{
  momIdle,
  momSequence,
  momRunDelta
} MidiOpMode;


// ----------------------------------------------------------------------------
//                            M i d i
// ----------------------------------------------------------------------------
//
class Midi
{
#define next(x) nextState = &Midi::x

public:
  // -------------------------------------------------------------------------
  // Öffentliche Datentypen
  // -------------------------------------------------------------------------
  //
  typedef enum _NoteTypeIdx
  {
    nti0  = 0,
    nti1,
    nti2,
    nti2p,
    nti4,
    nti4p,
    nti8,
    nti8p,
    nti16,
    nti16p,
    nti32,
    nti32p,
    nti64,
    nti64p,
    ntiNr
  } NoteTypeIdx;


private:
  // -------------------------------------------------------------------------
  // Private Datentypen
  // -------------------------------------------------------------------------
  //
  typedef void (Midi::*cbVector)(void);

  typedef struct _NoteType
  {
    dword   length;
    dword   attack;
    dword   decay;
    dword   sustain;
    dword   release;
    byte    deltaAttack;
    byte    deltaDecay;
    byte    percentSustain;
    byte    deltaRelease;
  } NoteType, *NoteTypePtr;

  typedef struct  _Note
  {
    byte      mode;
    byte      nIdx;
    byte      value;
    byte      veloc;
    dword     cntAttack;
    dword     cntDecay;
    dword     cntSustain;
    dword     cntRelease;
  } Note, *NotePtr;

#define NoteModeEmpty     0x00
#define NoteModeRun       0x01
#define NoteModeDoChange  0x02

  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  ComRingBuf  *crb;
  cbVector    nextState;

  MidiOpMode  opMode;

  dword     runCounter;
  dword     cycleCnt;

  dword     midiCycle;      // Zustandstakt in Mikrosekunden
  dword     minNoteTick;    // minimale Notendauer in Millisekunden
  dword     bpm;            // Beats per Minute (Metronom)
  dword     stdNoteTick;    // Dauer einer Viertelnote in Millisekunden
  dword     stdNoteCount;   // Viertelnote in Zyklen der Zustandsmaschine

  Note      chord[MaxNrNoteSim];  // Liste der simultanen Noten (Akkord)
  NoteType  typeList[ntiNr];      // Liste der Notentypen
  byte      chn;                  // Aktueller Kanal
  byte      noteSeq[MaxMidiSeq];  // Lokaler Telegrammaufbau

  NotePtr     notePtr;      // Temporäre Notendaten
  NoteTypePtr typePtr;      // Temporärer Notentyp

  dword     absPause;       // Pause für den zyklischen Ablauf


  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //

  // Zustandsmaschine
  // -----------------------------
  void smInit();
  void smIdle();

  void smNoteOn();
  void smAttack();
  void smDecay();
  void smSustain();
  void smRelease();
  void smNoteOff();
  void smPause();


  // --------------------------------------------------------------------------
  // Inline-Funktionen
  // --------------------------------------------------------------------------
  //

public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------
  void begin(int inBpm, NoteDiv inRes, int inMidiCycle, ComRingBuf *inCRB);


  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setNoteType(NoteTypeIdx nt);

  void setNoteType(NoteTypeIdx nt, dword att, dword dec, dword sus, dword rel,
                   byte dAtt, byte dDec, byte dSus, byte dRel);

  int addChordNote(NoteTypeIdx nti, byte val, byte vel);

  // --------------------------------------------------------------------------
  // Betrieb
  // --------------------------------------------------------------------------
  //
  void setOpMode(MidiOpMode mom);

  // --------------------------------------------------------------------------
  // Steuerung, Zustandsmaschine
  // --------------------------------------------------------------------------
  //
  void run();

  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //

};


// ----------------------------------------------------------------------------
#endif // Midi_h
