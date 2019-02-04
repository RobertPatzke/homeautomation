//-----------------------------------------------------------------------------
// Thema:       Social Manufacturing Network / Development Environment
// Datei:       SerialCom.h
// Editor:      Robert Patzke
// Erstellt:    29.08.2013
// Bearbeitet:  28.01.2019
// URI/URL:     www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:      CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#ifndef SerialCom_h
#define SerialCom_h

#include "Arduino.h"

// Zuweisungen der Kanalnummern zu den Seriellen Schnittstellen
// auf dem Arduino-Due bzw. SAM3
//
#define SamCom0     -1
#define SamCom1     0
#define SamCom2     1
#define SamCom3     2
#define SamCom4     3

#define DueCom0     -1
#define DueCom1     0
#define DueCom2     1
#define DueCom3     3
#define DueComX     2

#define ModPE       0
#define ModPO       (0x1u << 9)
#define ModPN       (0x4u << 9)

#define ModDB5      0
#define ModDB6      (0x1u << 6)
#define ModDB7      (0x2u << 6)
#define ModDB8      (0x3u << 6)

#define ModSB1      0
#define ModSB15     (0x1u << 12)
#define ModSB2      (0x2u << 12)


#ifdef IntTxdTest
// -----------------------------------------
#define PinD25    0x00000001
#define Pin25On   REG_PIOD_ODSR &= ~PinD25
#define Pin25Off  REG_PIOD_ODSR |= PinD25

#define PinD27    0x00000004
#define Pin27On   REG_PIOD_ODSR &= ~PinD27
#define Pin27Off  REG_PIOD_ODSR |= PinD27

#define PinD29    0x00000040
#define Pin29On   REG_PIOD_ODSR &= ~PinD29
#define Pin29Off  REG_PIOD_ODSR |= PinD29
// -----------------------------------------
#endif

// Bit-Masken fuer Kommunikationsbedingungen
//
#define BM_REC_NOT_COND 0x00
// Keine Bedingungen beim Empfang
#define BM_REC_END_CHR  0x01
// Empfang Stoppen beim Eintreffen des vorgegebenen Zeichens
#define BM_REC_RINGBUF  0x02
// Receive characters in ring buffer
#define BM_SND_RINGBUF  0x04
// Transmit characters via ring buffer

typedef void (*scCallback)();

class SerialCom
{
  // --------------------------------------------------------------------------
  // Private Methoden fuer lokale Anwendungen
  // --------------------------------------------------------------------------
  //
  private:
#ifdef IntTxdTest
    void  showCounting(int delayCount);
#endif
    bool  startTransmit(void);
    void  putNL();
    int   putStrB(int newLine, char *msg);
    int   putStrB(int newLine, int nrChr, char *msg);
    char  getC();

  // --------------------------------------------------------------------------
  // Methoden (Funktionen) für den Anwender
  // --------------------------------------------------------------------------
  //
  public:                   // Konstruktoren
    SerialCom(int chn);
    void  run(void);
    void  IrqHandler(void);

    // Starten der Schnittstelle mit Uebrtragungsparametern
    //
    void  start(int baud);
    void  start(int baud, uint32_t mode);
    void  start(int baud, int userDataBits, int stopBits, int paritaet, int msbf);
    void  stop();

    // Lesen und Schreiben von Zeichen
    //
    void  write(uint8_t *wrPtr, int nrOfBytes);
    void  write(uint8_t snglByte);
    void  read(uint8_t *rdPtr, int nrOfBytes);
    void  read(uint8_t *rdPtr, int maxNrOfBytes, uint8_t endChr);

    void  setReadBuffer(int size, uint8_t *bufPtr);
    int   getChr(void);
    void  clrRecBuf();
    int   getAll(uint8_t *buffer);
    int   getCount(int count, uint8_t *buffer);
    int   getCount(int count, char *buffer);
    int   getLine(char *buffer);
    int   getLine(int *intValue);
    int   inCount(void);
    int   getRestChar(uint8_t tagChr, int len, uint8_t *buffer);
    int   getRestStr(char *tagStr, int len, uint8_t *buffer);
    int   reqChkLine(char *req, char *rsp);

    void  setWriteBuffer(int size, uint8_t *bufPtr);
    int   putChr(int chr);
    int   putStr(char *msg);
    int   putStr(char *msg, int n);
    int   putLine(char *msg);
    int   putLine(char *msg, int n);

  // --------------------------------------------------------------------------
  // interne Methoden (Funktionen)
  // --------------------------------------------------------------------------
  //
  private:
    void  init(int chn);

  // --------------------------------------------------------------------------
  // Variablen zur Steuerung/Anwendung
  // --------------------------------------------------------------------------
  //
  public:
    uint32_t  status;         // Status des letzten Interrupt
    uint16_t  restSend;       // Anzahl der zu sendenden Bytes
    uint16_t  nrRec;          // Anzahl der empfangenen Bytes
    uint16_t  restRec;        // Anzahl der ungelesenen Bytes

  // --------------------------------------------------------------------------
  // lokale Variablen
  // --------------------------------------------------------------------------
  //
  private:
    uint32_t  pmcPcerVal;       // Zum Steuern des Taktes
    Usart     *usartPtr;        // Zum Ansprechen des USART
    int       pidUsart;         // Berechnung des Identifier
    bool      txInt;            // Set true by IRQ-handler
    bool      customCom;        // Set true by start for custom Com settings
    int       uartDataBits;     // Variablen zum Start von
    int       uartStopBits;     // Sercom
    int       uartParitaet;     //
    int       uartmsbf;         //

    // Lesen und Schreiben von Zeichen (Bytes)
    //
    uint8_t   *ptrSend;         // Der (veraenderliche) Sendezeiger
    uint8_t   *ptrRec;          // Der (veraenderliche) Empfangszeiger
    int       maxRec;           // Maximale Anzahl zu empfangender Bytes
    uint8_t   endChrRec;        // Abschlusszeichen beim Empfang
    uint8_t   condMaskCom;      // Bedingungen fuer den Datenaustausch

    uint8_t   *recBuffer;       // Receive ring buffer start address
    uint16_t  rbReadIdx;        // Read index
    uint16_t  rbWriteIdx;       // Write index
    uint16_t  rbSize;           // Buffer size

    uint8_t   *sndBuffer;       // Transmit ring buffer start address
    uint16_t  sbReadIdx;        // Read index
    uint16_t  sbWriteIdx;       // Write index
    uint16_t  sbSize;           // Buffer size

    int       reqChkState;      // State of request/check procedure
    int       tmpVal;           // Variable for temporary data storage

    //---------------------------------------------------------------------------//
    //Attribute
    //---------------------------------------------------------------------------//
    //
  public:
            typedef enum
            { //Anzahl Datenbits
              CHRL_5  = 5,
              CHRL_6  = 6,
              CHRL_7  = 7,
              CHRL_8  = 8
            } mp_UART_MR_CHRL;

            typedef enum
            { //Anzahl Stopbits
              NBSTP_1   = 1,
              NBSTP_1_5   = 2,
              NBSTP_2   = 3
            } mp_UART_MR_NBSTP;

            typedef enum
            { //Paritaet
              PAR_EVEN  = 1,
              PAR_ODD   = 2,
              PAR_SPACE = 3,
              PAR_MARK  = 4,
              PAR_NO    = 5
            } mp_UART_MR_PAR;

            typedef enum
            { //MSB / LSB first
              MSBF  = 1,
              LSBF  = 2,
            } mp_UART_MR_MSBF;

    //---------------------------------------------------------------------------//


#ifdef IntTxdTest
    int       loopCounter;
    int       loopDelay;
    int       toggle10;
    int       toggle100;
    int       toggle1000;
#endif
};

#endif
