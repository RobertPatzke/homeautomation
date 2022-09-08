//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   MidiNotes.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   27. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen für steuerbare Midi-Controller
//

#ifndef MidiNotes_h
#define MidiNotes_h
// ----------------------------------------------------------------------------

#include "arduinoDefs.h"
#include "ComRingBuf.h"

#define MaxNrNoteSim    4
#define MaxMidiSeq      (2 * MaxNrNoteSim + 1)

// Definierte Noten
//
#define SchlossC    60
#define Kammerton   69

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
//                            M i d i N o t e s
// ----------------------------------------------------------------------------
//
class MidiNotes
{
#define next(x) nextState = &MidiNotes::x

public:
  // -------------------------------------------------------------------------
  // Öffentliche Datentypen
  // -------------------------------------------------------------------------
  //
  typedef enum _NoteTypeIdx
  {
    nti0  = 0,
    nti1,
    nti2p,
    nti2,
    nti4p,
    nti4,
    nti8p,
    nti8,
    nti16p,
    nti16,
    nti32p,
    nti32,
    nti64p,
    nti64,
    ntiNr
  } NoteTypeIdx;


private:
  // -------------------------------------------------------------------------
  // Private Datentypen
  // -------------------------------------------------------------------------
  //
  typedef void (MidiNotes::*cbVector)(void);

  typedef struct _NoteType
  {
    dword   length;
    dword   attack;
    dword   decay;
    dword   sustain;
    dword   release;
    dword   pause;
    byte    deltaAttack;
    byte    deltaDecay;
    byte    percentSustain;
    byte    deltaRelease;
  } NoteType, *NoteTypePtr;

  typedef struct  _Note
  {
    byte      mode;
    byte      typeIdx;
    byte      value;
    byte      veloc;
    int       state;
    dword     cntAttack;
    dword     cntDecay;
    dword     cntSustain;
    dword     cntRelease;
    dword     cntPause;
  } Note, *NotePtr;

  typedef struct _NewNote
  {
    bool      newVal;
    byte      typeIdx;
    byte      value;
    byte      veloc;
  }NewNote;

#define NoteModeEmpty     0x00
#define NoteModeRun       0x01
#define NoteModeDoChange  0x02

  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  IntrfBuf    *crb;
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
  NewNote   newNote[MaxNrNoteSim];  // Übergabe neuer Noten

  bool      stopRun;           // Anhalten der Midi-Schleife
  bool      stoppedRun;        // Midi-Schleife angehalten


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
  void begin(int inBpm, NoteDiv inRes, int inMidiCycle, IntrfBuf *inCRB);


  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void  setNoteType(NoteTypeIdx nt);

  void  setNoteType(NoteTypeIdx nt, byte pAttL, byte pDecL, byte pSusL, byte pRelL,
                    byte dAtt, byte dDec, byte pSusV, byte dRel, byte pPauL);

  int   addChordNote(NoteTypeIdx nti, byte val, byte vel);

  void  setChannel(int chnVal);

  // --------------------------------------------------------------------------
  // Betrieb
  // --------------------------------------------------------------------------
  //
  void setOpMode(MidiOpMode mom);
  void setChordNote(int idx, NoteTypeIdx nti, int val, int vel);

  // --------------------------------------------------------------------------
  // Steuerung, Zustandsmaschine
  // --------------------------------------------------------------------------
  //
  void run();
  void stop();
  void resume();

  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //

};


// ----------------------------------------------------------------------------
#endif // MidiNotes_h
