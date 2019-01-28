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

#include "Arduino.h"

//
// Flash-Speicher
//
prog_uchar scCopyRightMsg[] PROGMEM = {"MFP GmbH, 2013"};
prog_uchar scTopicMsg[]     PROGMEM = {"USART auf Interrupt"};
prog_uchar scAuthorMsg[]    PROGMEM = {"Dr.-Ing. Robert Patzke"};
prog_uchar scVersionMsg[]   PROGMEM = {"Version 19.01.28.0"};

// #define IntTxdTest
#include  "SerialCom.h"

// ----------------------------------------------------------------------------
// Konstruktoren, Initialisierungen und Verwaltungen
// ----------------------------------------------------------------------------
//

SerialCom::SerialCom(int chn)
{
  sndBuffer   = NULL;
  recBuffer   = NULL;
  condMaskCom = BM_SND_RINGBUF;
  reqChkState = 0;
  init(chn);
}


SerialCom *scPtr = NULL;

void UART_Handler()
{
  if(scPtr == NULL) return;
  scPtr->IrqHandler();
}

SerialCom *sc0Ptr = NULL;

void USART0_Handler()
{
  if(sc0Ptr == NULL) return;
  sc0Ptr->IrqHandler();
}

SerialCom *sc1Ptr = NULL;

void USART1_Handler()
{
  if(sc1Ptr == NULL) return;
  sc1Ptr->IrqHandler();
}

SerialCom *sc2Ptr = NULL;

void USART2_Handler()
{
  if(sc2Ptr == NULL) return;
  sc2Ptr->IrqHandler();
}

SerialCom *sc3Ptr = NULL;

void USART3_Handler()
{
  if(sc3Ptr == NULL) return;
  sc3Ptr->IrqHandler();
}

void SerialCom::init(int chn)
{
  #ifdef IntTxdTest
  loopCounter = 0;
  loopDelay   = 0;
  toggle10    = 0;
  toggle100   = 0;
  toggle1000  = 0;
  #endif

  // Fuer chn werden die Werte 0-3 erwartet
  // diese werden auf die PIDs 17 bis 20 abgebildet.
  // Damit werden die spezifischen Einstellungen fuer
  // USART0 bis USART3 vorgenommen.

  // Vorbereiten der Werte zum Einschalten des Taktgenerators
  // -----------------------------------------------------------
  if(chn < 0)
    pidUsart = 8;
  else
    pidUsart  = 17 + (chn & 0x03);

  pmcPcerVal    = 1 << pidUsart;
  // -----------------------------------------------------------
  // Die tatsaechliche Ausfuehrung erfolgt in der start-Funktion

  // PIO-Multiplexing, muss fuer die einzelnen Faelle getrennt
  // bearbeitet werden.
  //
  switch(pidUsart)
  {
    // ---------------------------------------------------------
    // chn = 3
    // Arduino Due: Pin 14 / TX3 / D.04 und Pin 15 / RX3 / D.03
    // ---------------------------------------------------------
    case 20:    // USART3
      // Normale PIO-Funktion im Disable-Register  abschalten
      //
      REG_PIOD_PDR = PIO_PD5B_RXD3 | PIO_PD4B_TXD3;

      // Peripheriefunktion B auswählen
      //
      REG_PIOD_ABSR = PIO_PD5B_RXD3 | PIO_PD4B_TXD3;

      // Zeiger setzen für die Manipulation des USART
      //
      usartPtr = USART3;
      sc3Ptr = this;

      break;

    // -----------------------------------------------------------
    // chn = 2
    // Arduino Due: Pin A11 / (TX) / B.20 und Pin 52 / (RX) / B.21
    // -----------------------------------------------------------
    case 19:    // USART2
      // Normale PIO-Funktion im Disable-Register abschalten
      //
      REG_PIOB_PDR = PIO_PB21A_RXD2 | PIO_PB20A_TXD2;

      // Peripheriefunktion A ist per Default ausgew�hlt
      //

      // Zeiger setzen f�r die Manipulation des USART
      //
      usartPtr = USART2;
      sc2Ptr = this;
      break;

    // ---------------------------------------------------------
    // chn = 1
    // Arduino Due: Pin 16 / TX2 / A.13 und Pin 17 / RX2 / A.12
    // ---------------------------------------------------------
    case 18:    // USART1
      // Normale PIO-Funktion im Disable-Register  abschalten
      //
      REG_PIOA_PDR = PIO_PA12A_RXD1 | PIO_PA13A_TXD1;

      // Peripheriefunktion A ist per Default ausgew�hlt
      //

      // Zeiger setzen für die Manipulation des USART
      //
      usartPtr = USART1;
      sc1Ptr = this;
      break;

    // ---------------------------------------------------------
    // chn = 0
    // Arduino Due: Pin 18 / TX1 / A.11 und Pin 19 / RX1 / A.10
    // ---------------------------------------------------------
    case 17:    // USART0
      // Normale PIO-Funktion im Disable-Register  abschalten
      //
      REG_PIOA_PDR = PIO_PA10A_RXD0 | PIO_PA11A_TXD0;

      // Peripheriefunktion A ist per Default ausgew�hlt
      //

      // Zeiger setzen für die Manipulation des USART
      //
      usartPtr = USART0;
      sc0Ptr = this;
      break;

      // ---------------------------------------------------------
      // chn < 0
      // Arduino Due: Pin 1 / TX0 / A.09 und Pin 0 / RX0 / A.08
      // ---------------------------------------------------------
      case 8:    // UART
        // Normale PIO-Funktion im Disable-Register  abschalten
        //
        REG_PIOA_PDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

        // Peripheriefunktion A ist per Default ausgew�hlt
        //

        // Zeiger setzen für die Manipulation des USART
        //
        usartPtr = (Usart *) UART;
        scPtr = this;
        break;
  }

  //condMaskCom = BM_REC_NOT_COND;
}

void SerialCom::start(int baud)
{
  Uart *uartPtr;

  // Einschalten des Taktgenerators für den USART
  //
  REG_PMC_PCER0 = pmcPcerVal;

  if(pidUsart == 8)
  {
    uartPtr = (Uart *) usartPtr;

    // Abschalten des DMA-Betriebs
    //
    uartPtr->UART_PTCR =
        UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

    // Initialisierung des UART
    //
    uartPtr->UART_CR =
        UART_CR_RSTRX | UART_CR_RSTTX | // Reset Receiver und Transmitter
        UART_CR_RXDIS | UART_CR_TXDIS;  // und Stilllegen bis fertig hier

    // Erst mal alle Interrupts sperren
    //
    uartPtr->UART_IDR = 0xFFFFFFFF;

  if(customCom)uartPtr->UART_MR =   // Alles Standard bis auf
    uartDataBits  |       // DatenBits
    uartStopBits  |       // StopBits
    uartParitaet  |       // Paritaet
    uartmsbf;           // MSBFirst

    uartPtr->UART_MR =    // Alles Standard bis auf
        UART_MR_PAR_NO;   // keine Paritaet


    // Baudrate einstellen
    //
    uartPtr->UART_BRGR = (SystemCoreClock / baud) / 16 ;

    // Interrupts freischalten
    //
    NVIC_EnableIRQ((IRQn_Type)pidUsart);

    uartPtr->UART_IER =
        UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME;
    // | UART_IER_TXRDY;
    // ACHTUNG, System arbeitet nicht mehr,
    // sobald TX-Interrupt freigegeben wird
    // Endlos-Schleife ueber Interrupt/IRQ-Handler

    // UART freigeben fuer Senden und Empfangen
    //
    uartPtr->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
  }
  else
  {
  // Abschalten des DMA-Betriebs
  //
  usartPtr->US_PTCR =
    US_PTCR_RXTDIS | US_PTCR_TXTDIS ;

  // Initialisierung des USART
  //
  usartPtr->US_CR =
    US_CR_RSTRX | US_CR_RSTTX |     // Reset Receiver und Transmitter
    US_CR_RXDIS | US_CR_TXDIS;      // und Stilllegen bis fertig hier

  // Erst mal alle Interrupts sperren
  //
  usartPtr->US_IDR = 0xFFFFFFFF;

  if(customCom)usartPtr->US_MR =                    // Alles Standard bis auf
      uartDataBits  |       // DatenBits
      uartStopBits  |       // StopBits
      uartParitaet  |       // Paritaet
      uartmsbf;           // MSBFirst

  usartPtr->US_MR =                 // Alles Standard bis auf
    US_MR_CHRL_8_BIT |              // 8-Bit
    US_MR_PAR_NO;                   // keine Paritaet

  // Baudrate einstellen
  //
  usartPtr->US_BRGR = (SystemCoreClock / baud) / 16 ;

  // Interrupts freischalten
  //
  NVIC_EnableIRQ((IRQn_Type)pidUsart);
  usartPtr->US_IER =
  US_IER_RXRDY | US_IER_OVRE | US_IER_FRAME;
  // | US_IER_TXRDY;
  // ACHTUNG, System arbeitet nicht mehr,
  // sobald TX-Interrupt freigegeben wird
  // Endlos-Schleife ueber Interrupt/IRQ-Handler

  // USART freigeben fuer Senden und Empfangen
  //
  usartPtr->US_CR = US_CR_RXEN | US_CR_TXEN;
  }
}

void SerialCom::start(int baud, int userDataBits, int stopBits, int paritaet, int msbf)
{
  switch(userDataBits)
  {
  //Setzen der DatenBits [CHRL] im US_MR_
      case 5:
        uartDataBits = US_MR_CHRL_5_BIT;
        break;
      case 6:
        uartDataBits = US_MR_CHRL_6_BIT;
        break;
      case 7:
        uartDataBits = US_MR_CHRL_7_BIT;
        break;
      case 8:
        uartDataBits = US_MR_CHRL_8_BIT;
        break;
  }


  switch(stopBits)
  {
  //Setzen der Stopbits [NBSTOP] im US_MR_
      case 1:
        uartStopBits = US_MR_NBSTOP_1_BIT;
        break;
      case 2:
        uartStopBits = US_MR_NBSTOP_1_5_BIT;
        break;
      case 3:
        uartStopBits = US_MR_NBSTOP_2_BIT;
        break;
  }

  switch(paritaet)
  {
  //Setzen der paritaet [PAR] im US_MR_
      case 1:
        uartParitaet = US_MR_PAR_EVEN;
        break;
      case 2:
        uartParitaet = US_MR_PAR_ODD;
        break;
      case 3:
        uartParitaet = US_MR_PAR_SPACE;
        break;
      case 4:
        uartParitaet = US_MR_PAR_MARK;
        break;
      case 5:
        uartParitaet = US_MR_PAR_NO;
        break;
      case 6:
        uartParitaet = US_MR_PAR_MULTIDROP;
        break;
  }

  switch(msbf)
  {
  //Setze MSB/LSB [MSBF] im US_MR_
      case 1:
        uartmsbf = US_MR_MSBF;
        //MSBFirst
        break;
      case 2:
        uartmsbf = (0x0u << 16);
        //LSBFirst
        break;
  }
  customCom =true;
  start(baud);

}

void SerialCom::stop()
{
  // Interrupts sperren
  //
  NVIC_DisableIRQ((IRQn_Type)pidUsart);

  // Ausschalten des Taktgenerators f�r den USART
  //
  REG_PMC_PCDR0 = pmcPcerVal;
}

// ----------------------------------------------------------------------------
// Interrupt-Handler
// ----------------------------------------------------------------------------
//

void SerialCom::IrqHandler()
{
  uint8_t inByte;
  Uart    *uartPtr;


#ifdef IntTxdTest
  showCounting(1000);
#endif

  if(pidUsart == 8)
  // ----------------------------------------------------------------------------
  // UART used for Tx0 and Rx0 at DUE (USB program interface)
  // ----------------------------------------------------------------------------
  {
    uartPtr = (Uart *) usartPtr;

    status = uartPtr->UART_SR;

    // --------------------------------------------------------------------------
    // Transmit-Interrupt (Sendepuffer ist frei)
    // --------------------------------------------------------------------------
    //
    if(status & UART_SR_TXRDY)
    {
      txInt = true;

      // Der Sendepuffer ist frei
      //
      if(condMaskCom & BM_SND_RINGBUF)
      {
        restSend = sbWriteIdx - sbReadIdx;

        if(restSend > 0)
        {
          // es sind noch Zeichen zum Senden da
          //
          uartPtr->UART_THR = sndBuffer[sbReadIdx];
          sbReadIdx++;
          if(sbReadIdx >= sbSize)
            sbReadIdx = 0;
          restSend--;
        }
      }
      else
      {
        if(restSend > 0)
        {
          // es sind noch Zeichen zum Senden da
          //
          uartPtr->UART_THR = *ptrSend;
          ptrSend++;
          restSend--;
        }
      }

      // Der Transmit-Interrupt wird gesperrt,
      // wenn das letzte Zeichen ausgegeben wurde.
      if(restSend < 1)
      {
        uartPtr->UART_IDR = UART_IDR_TXRDY;
      }
    }

    // --------------------------------------------------------------------------
    // Receive-Interrupt (Zeichen wurde empfangen)
    // --------------------------------------------------------------------------
    //

    if(status & UART_SR_RXRDY)
    {
      // -----------------------------------------
      inByte = uartPtr->UART_RHR;

      if(condMaskCom & BM_REC_RINGBUF)
      {
        recBuffer[rbWriteIdx] = inByte;
        rbWriteIdx++;
        if(rbWriteIdx >= rbSize)
          rbWriteIdx = 0;
        if(rbWriteIdx == rbReadIdx)
        {
          rbReadIdx = rbWriteIdx + 1;
          if(rbReadIdx >= rbSize)
            rbReadIdx = 0;
        }
      }
      else
      {
        if(restRec > 0)
        {
          // Ein Byte wurde empfangen und
          // es sind noch Speicherplaetze frei
          // Byte speichern
          //
          *ptrRec = inByte;
          nrRec++;
          restRec--;
          ptrRec++;

          // Pruefen, ob Weiterempfangen noetig/moeglich ist
          //
          if((condMaskCom & BM_REC_END_CHR) && (inByte == endChrRec))
          {
            restRec = 0;
          }
        }
      }
      // -----------------------------------------
    }

  }
  // --------------------------------------------------------------------------
  else
  // ----------------------------------------------------------------------------
  // USART used for Tx1/4 and Rx1/4 at DUE (Serial interfaces 1 to 3)
  // ----------------------------------------------------------------------------
  {
    status = usartPtr->US_CSR;

    // --------------------------------------------------------------------------
    // Transmit-Interrupt (Sendepuffer ist frei)
    // --------------------------------------------------------------------------
    //
    if(status & US_CSR_TXRDY)
    {
      // Der Sendepuffer ist frei
      //
      txInt = true;

      // Der Sendepuffer ist frei
      //
      if(condMaskCom & BM_SND_RINGBUF)
      {
        restSend = sbWriteIdx - sbReadIdx;

        if(restSend > 0)
        {
          // es sind noch Zeichen zum Senden da
          //
          usartPtr->US_THR = sndBuffer[sbReadIdx];
          sbReadIdx++;
          if(sbReadIdx >= sbSize)
            sbReadIdx = 0;
          restSend--;
        }
      }
      else
      {
        if(restSend > 0)
        {
          // es sind noch Zeichen zum Senden da
          //
          usartPtr->US_THR = *ptrSend;
          ptrSend++;
          restSend--;
        }
      }

    // Der Transmit-Interrupt wird gesperrt,
    // wenn das letzte Zeichen ausgegeben wurde.
    if(restSend < 1)
      usartPtr->US_IDR = US_IDR_TXRDY;
    }

    // --------------------------------------------------------------------------
    // Receive-Interrupt (Zeichen wurde empfangen)
    // --------------------------------------------------------------------------
    //
    if(status & US_CSR_RXRDY)
    {
      // -----------------------------------------
      inByte = usartPtr->US_RHR;

      if(condMaskCom & BM_REC_RINGBUF)
      {
        recBuffer[rbWriteIdx] = inByte;
        rbWriteIdx++;
        if(rbWriteIdx >= rbSize)
          rbWriteIdx = 0;
        if(rbWriteIdx == rbReadIdx)
        {
          rbReadIdx = rbWriteIdx + 1;
          if(rbReadIdx >= rbSize)
            rbReadIdx = 0;
        }
      }
      else
      {
        if(restRec > 0)
        {
          // Ein Byte wurde empfangen und
          // es sind noch Speicherplaetze frei
          // Byte speichern
          //
          *ptrRec = inByte;
          nrRec++;
          restRec--;
          ptrRec++;

          // Pruefen, ob Weiterempfangen noetig/moeglich ist
          //
          if((condMaskCom & BM_REC_END_CHR) && (inByte == endChrRec))
          {
            restRec = 0;
          }
        }
      }
      // -----------------------------------------
    }

  }
}

// ----------------------------------------------------------------------------
// Direct writing and reading data
// ----------------------------------------------------------------------------
//

void SerialCom::write(uint8_t *wrPtr, int nrOfBytes)
{
  Uart *uartPtr;

  if(pidUsart == 8)
  {
    uartPtr = (Uart *) usartPtr;

    restSend  = nrOfBytes - 1;
    uartPtr->UART_THR = *wrPtr;

    if(nrOfBytes > 1)
    {
      ptrSend   = wrPtr + 1;
      uartPtr->UART_IER = UART_IER_TXRDY;
    }
  }
  else
  {
    restSend  = nrOfBytes - 1;
    usartPtr->US_THR = *wrPtr;

    if(nrOfBytes > 1)
    {
      ptrSend   = wrPtr + 1;
      usartPtr->US_IER = US_IER_TXRDY;
    }
  }
}

void SerialCom::write(uint8_t sngByte)
{
  Uart *uartPtr;

  restSend = 0;

  if(pidUsart == 8)
  {
    uartPtr = (Uart *) usartPtr;
    uartPtr->UART_THR = sngByte;
  }
  else
  {
    usartPtr->US_THR = sngByte;
  }
}

void SerialCom::read(uint8_t *rdPtr, int nrOfBytes)
{
  ptrRec = rdPtr;
  nrRec =  0;
  restRec = nrOfBytes;
  condMaskCom = BM_REC_NOT_COND;
}

void SerialCom::read(uint8_t *rdPtr, int maxNrOfBytes, uint8_t endChr)
{
  ptrRec = rdPtr;
  nrRec =  0;
  restRec = maxNrOfBytes;
  endChrRec = endChr;
  condMaskCom = condMaskCom | BM_REC_END_CHR;
}

// ----------------------------------------------------------------------------
// Writing and reading data via circular buffer (default usage)
// ----------------------------------------------------------------------------
//

void SerialCom::setReadBuffer(uint8_t *bufPtr, int size)
{
  recBuffer = bufPtr;
  rbSize = size;
  rbReadIdx = 0;
  rbWriteIdx = 0;
  condMaskCom |= BM_REC_RINGBUF;
}

int SerialCom::getChr()
{
  int retv;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(EOF);

  retv = recBuffer[rbReadIdx];
  rbReadIdx++;
  if(rbReadIdx >= rbSize)
    rbReadIdx = 0;
  return(retv);
}

void SerialCom::clrRecBuf()
{
  rbReadIdx  = 0;
  rbWriteIdx = 0;
}

int SerialCom::getAll(uint8_t *buffer)
{
  uint16_t  count, i;
  int       tmpInt;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(EOF);

  tmpInt = rbWriteIdx - rbReadIdx;
  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  for(i = 0; i < count; i++)
  {
    buffer[i] = recBuffer[rbReadIdx];
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;
  }

  return(count);
}

int SerialCom::inCount()
{
  int count = rbWriteIdx - rbReadIdx;
  if(count < 0)
    count += rbSize;
  return(count);
}

int SerialCom::getCount(uint8_t *buffer, int len)
{
  uint16_t  count, i;
  int       tmpInt;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(0);

  tmpInt = rbWriteIdx - rbReadIdx;
  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  if(len > count)
    len = count;

  for(i = 0; i < len; i++)
  {
    buffer[i] = recBuffer[rbReadIdx];
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;
  }

  return(len);
}

char SerialCom::getC()
{
  char retC;

  if(rbReadIdx == rbWriteIdx)
    return(0);

  retC = recBuffer[rbReadIdx];
  rbReadIdx++;
  if(rbReadIdx >= rbSize)
    rbReadIdx = 0;

  return(retC);
}

int SerialCom::getCount(char *buffer, int len)
{
  int nrChar;

  nrChar = getCount((uint8_t *) buffer, len);
  buffer[nrChar] = 0;
  return(nrChar);
}

int SerialCom::getLine(char *buffer)
{
  bool      eol;
  uint16_t  count, i;
  int       tmpInt;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(0);

  tmpInt = rbWriteIdx - rbReadIdx;
  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  eol = false;

  for(i = 0; i < count; i++)
  {
    buffer[i] = recBuffer[rbReadIdx];
    if(!eol)
    {
    if(buffer[i] == '\r' || buffer[i] == '\n')
      eol = true;
    }
    else
    {
      if(buffer[i] != '\r' && buffer[i] != '\n')
        break;
    }
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;
  }

  if(!eol) return(0);

  buffer[i] = 0;
  return(i);
}

int SerialCom::getRestChar(uint8_t tagChr, uint8_t *buffer, int len)
{
  uint16_t  count, i, j;
  uint8_t   inChr;
  int       tmpInt;
  bool      tagged;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(0);

  tmpInt = rbWriteIdx - rbReadIdx;
  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  if(len > count)
    len = count;

  tagged = false;
  j = 0;

  for(i = 0; i < len; i++)
  {
    inChr = recBuffer[rbReadIdx];
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;

    if(!tagged)
    {
      if(inChr != tagChr)
        continue;

      tagged = true;
      continue;
    }

    buffer[j++] = inChr;
  }

  if(!tagged) j = -1;

  return(j);
}

int SerialCom::getRestStr(char *tagStr, uint8_t *buffer, int len)
{
  uint16_t  count, i, j, tmpIdx;
  uint8_t   inChr;
  int       tmpInt;
  bool      tagged;
  int       tagLen;
  int       tagIdx;

  if(!(condMaskCom & BM_REC_RINGBUF))
    return(EOF);
  if(rbReadIdx == rbWriteIdx)
    return(0);

  tmpIdx = rbReadIdx;
  tmpInt = rbWriteIdx - tmpIdx;

  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  if(len > count)
    len = count;

  tagged = false;
  j = 0;

  tagLen = (int) strlen(tagStr);
  tagIdx = 0;

  if(len < tagLen)
    return(0);

  for(i = 0; i < len; i++)
  {
    inChr = recBuffer[tmpIdx];
    tmpIdx++;
    if(tmpIdx >= rbSize)
      tmpIdx = 0;

    if(!tagged)
    {
      if(inChr != tagStr[tagIdx])
      {
        tagIdx = 0;
        continue;
      }

      tagIdx++;

      if(tagIdx == tagLen)
        tagged = true;
      continue;
    }

    buffer[j++] = inChr;
  }

  if(!tagged) return(EOF);
  else
  {
    rbReadIdx = tmpIdx;
    return(j);
  }
}



void SerialCom::setWriteBuffer(uint8_t *bufPtr, int size)
{
  sndBuffer = bufPtr;
  sbSize = size;
  sbReadIdx = 0;
  sbWriteIdx = 0;
  condMaskCom |= BM_SND_RINGBUF;
}

void SerialCom::putNL()
{
  sndBuffer[sbWriteIdx] = '\r';
  sbWriteIdx++;
  if(sbWriteIdx >= sbSize)
    sbWriteIdx = 0;

  sndBuffer[sbWriteIdx] = '\n';
  sbWriteIdx++;
  if(sbWriteIdx >= sbSize)
    sbWriteIdx = 0;
}

int SerialCom::putChr(int chr)
{
  int16_t   space;
  bool      txiOn;

  if(sndBuffer == NULL)
    return(EOF);

  space = sbReadIdx - sbWriteIdx - 1;
  if(space < 0)
    space += sbSize;
  if(space == 0)
    return(EOF);

  if(pidUsart == 8)
    txiOn = (((Uart *) usartPtr)->UART_IMR & UART_IMR_TXRDY) != 0;
  else
    txiOn = (usartPtr->US_IMR & US_IMR_TXRDY) != 0;

  if(!txiOn)
  {
    if(pidUsart == 8)
    {
      ((Uart *) usartPtr)->UART_THR = chr;
      ((Uart *) usartPtr)->UART_IER = UART_IER_TXRDY;
    }
    else
    {
      usartPtr->US_THR = chr;
      usartPtr->US_IER = US_IER_TXRDY;
    }
  }
  else
  {
  sndBuffer[sbWriteIdx] = chr;
  sbWriteIdx++;
  if(sbWriteIdx >= sbSize)
    sbWriteIdx = 0;
  }

  return(chr);
}

int SerialCom::putStr(char *msg, bool eol)
{
  int16_t   space;
  int16_t   len;
  int       sIdx;
  bool      txiOn;

  if(sndBuffer == NULL)
    return(EOF);

  space = sbReadIdx - sbWriteIdx - 1;
  if(space < 0)
    space += sbSize;

  len = strlen((const char *)msg);

  if(space < len)
    return(EOF);

  if(pidUsart == 8)
    txiOn = (((Uart *) usartPtr)->UART_IMR & UART_IMR_TXRDY) != 0;
  else
    txiOn = (usartPtr->US_IMR & US_IMR_TXRDY) != 0;

  if(txiOn)
    sIdx = 0;
  else
    sIdx = 1;

  for (int i = sIdx; i < len; i++)
  {
    sndBuffer[sbWriteIdx] = msg[i];
    sbWriteIdx++;
    if(sbWriteIdx >= sbSize)
      sbWriteIdx = 0;
  }

  if(eol)
  {
    putNL();
  }

  if(!txiOn)
  {
    if(pidUsart == 8)
    {
      ((Uart *) usartPtr)->UART_THR = msg[0];
      ((Uart *) usartPtr)->UART_IER = UART_IER_TXRDY;
    }
    else
    {
      usartPtr->US_THR = msg[0];
      usartPtr->US_IER = US_IER_TXRDY;
    }
  }

  return(len);
}

int SerialCom::putStr(char *msg)
{
  return putStr(msg, false);
}

int SerialCom::putLine(char *msg)
{
  return putStr(msg, true);
}

int SerialCom::putStr(char *msg, int n, bool eol)
{
  int16_t   space;
  int16_t   len;
  int       sIdx;
  bool      txiOn;

  if(sndBuffer == NULL)
    return(EOF);

  space = sbReadIdx - sbWriteIdx - 1;
  if(space < 0)
    space += sbSize;

  len = strlen((const char *)msg);
  if(n < len) len = n;

  if(space < len)
    return(EOF);

  if(pidUsart == 8)
    txiOn = (((Uart *) usartPtr)->UART_IMR & UART_IMR_TXRDY) != 0;
  else
    txiOn = (usartPtr->US_IMR & US_IMR_TXRDY) != 0;

  if(txiOn)
    sIdx = 0;
  else
    sIdx = 1;

  for (int i = sIdx; i < len; i++)
  {
    sndBuffer[sbWriteIdx] = msg[i];
    sbWriteIdx++;
    if(sbWriteIdx >= sbSize)
      sbWriteIdx = 0;
  }

  if(eol)
    putNL();

  if(!txiOn)
  {
    if(pidUsart == 8)
    {
      ((Uart *) usartPtr)->UART_THR = msg[0];
      ((Uart *) usartPtr)->UART_IER = UART_IER_TXRDY;
    }
    else
    {
      usartPtr->US_THR = msg[0];
      usartPtr->US_IER = US_IER_TXRDY;
    }
  }

  return(len);
}

int SerialCom::putStr(char *msg, int n)
{
  return putStr(msg, n, false);
}

int SerialCom::putLine(char *msg, int n)
{
  return putStr(msg, n, true);
}


int   SerialCom::reqChkLine(char *req, char *rsp)
{
  int   i;
  int   chkVal;
  char  chkChar;

  switch(reqChkState)
  {
    case 0:
      rbReadIdx  = 0;
      rbWriteIdx = 0;
      chkVal = putLine(req);
      if(chkVal <= 0)
        return(EOF);
      tmpVal = strlen(rsp);
      reqChkState = 1;
      return(0);

    case 1:
      chkVal = inCount();
      if(chkVal <= tmpVal)
        return(0);
      chkVal = 0;

      for(i = 0; i < tmpVal; i++)
      {
        chkChar = getC();
        if(req[i] != chkChar)
          chkVal = -100;
        else
          chkVal++;
      }

      while(recBuffer[rbReadIdx] == '\r' || recBuffer[rbReadIdx] == '\n' )
      {
        rbReadIdx++;
        if(rbReadIdx >= rbSize)
          rbReadIdx = 0;
      }

      reqChkState = 0;
      return(chkVal);
  }

  return(-1000);    // internal error with <reqChkState>
}



// ----------------------------------------------------------------------------
// Service function for non interrupt operation
// ----------------------------------------------------------------------------
// This function is not used, it is an option, but the class has to be
// extended to use it
//
void  SerialCom::run(void)
{
  status = usartPtr->US_CSR;

  // --------------------------------------------------------------------------
  // Abfrage, ob Sendepuffer frei und Zeichen zu senden sind
  // --------------------------------------------------------------------------
  //
  if((status & US_CSR_TXRDY) && (restSend > 0))
  {
    // Der Sendepuffer ist frei und
    // es sind noch Zeichen zum Senden da
    //
    usartPtr->US_THR = *ptrSend;
    ptrSend++;
    restSend--;
  }

}

// ----------------------------------------------------------------------------
// Test und Debug
// ----------------------------------------------------------------------------
//

#ifdef IntTxdTest

void  SerialCom::showCounting(int delayCount)
{
  if(loopDelay > 0)
  {
    loopDelay--;
    return;
  }

  loopDelay = delayCount;
  loopCounter++;

  if(loopCounter % 10) return;
  // Alle 10 Millisekunden hier
  //
  if(toggle10 == 0)
  {
    Pin25On;
    toggle10 = 1;
  }
  else
  {
    Pin25Off;
    toggle10 = 0;
  }

  if(loopCounter % 100) return;
  // Alle 100 Millisekunden
  //
  if(toggle100 == 0)
  {
    Pin27On;
    toggle100 = 1;
  }
  else
  {
    Pin27Off;
    toggle100 = 0;
  }

  if(loopCounter % 1000) return;
  // Jede Sekunde
  //
  if(toggle1000 == 0)
  {
    Pin29On;
    toggle1000 = 1;
  }
  else
  {
    Pin29Off;
    toggle1000 = 0;
  }
}

#endif
