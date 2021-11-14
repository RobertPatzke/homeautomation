//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   Monitor.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   15. Mai 2021
//
// Der Monitor dient zum direkten Zugriff auf die Ressourcen eines
// Mikrocontrollers über die serielle Schnittstelle.
// ACHTUNG!
// Er ist nicht für die Anwendung des "Serial Monitor" aus der Arduino-IDE
// bzw. aus Eclipse/Sloeber gedacht, sondern für ein typisches "Terminal".
// Verwendet wurde bei der Entwicklung unter Linux das GtkTerm.
//

#include "Monitor.h"

//-----------------------------------------------------------------------------
// Initialisierungen
//-----------------------------------------------------------------------------

void Monitor::init(int inMode, int inCpu, LoopCheck *inLcPtr, IntrfTw *inTwPtr)
{
  mode          = inMode;
  cpu           = inCpu;
  wrIdx         = 0;
  rdIdx         = 0;
  blkOut        = false;
  blkIn         = false;
  inIdx         = 0;
  info          = NULL;
  readOffsAddr  = 0;
  doReadReg     = false;
  extraIn       = false;
  lcPtr         = inLcPtr;
  twiPtr        = inTwPtr;
  nrOfChnChar   = '@';

  nextState =
      &Monitor::waitEnter;
}


Monitor::Monitor(int inMode, int inCpu)
{
  init(inMode, inCpu, NULL, NULL);
}

Monitor::Monitor(int inMode, int inCpu, LoopCheck *inLcPtr)
{
  init(inMode, inCpu, inLcPtr, NULL);
}

Monitor::Monitor(int inMode, int inCpu, LoopCheck *inLcPtr, IntrfTw *inTwiPtr)
{
  init(inMode, inCpu, inLcPtr, inTwiPtr);
}

//-----------------------------------------------------------------------------
// Konfiguration, Hilfsfunktionen
//-----------------------------------------------------------------------------
//
int Monitor::putBuf(char c)
{
  if(blkIn) return(-1);

  int free = rdIdx - wrIdx - 1;
  if(free < 0) free += BufSize;
  if(free < 1) return(0);
  buffer[wrIdx++] = c;
  if(wrIdx == BufSize)
    wrIdx = 0;
  return(1);
}

int Monitor::putBuf(char *txt)
{
  if(blkIn) return(-1);

  int free = rdIdx - wrIdx - 1;
  int size = strlen(txt);
  if(free < 0) free += BufSize;
  if(free < size) return(0);
  for(int i = 0; i < size; i++)
  {
  buffer[wrIdx++] = txt[i];
  if(wrIdx == BufSize)
    wrIdx = 0;
  }
  return(size);
}

char Monitor::getBuf()
{
  int num = wrIdx - rdIdx;
  if(num == 0) return('\0');
  char c = buffer[rdIdx++];
  if(rdIdx == BufSize)
    rdIdx = 0;
  return(c);
}

void Monitor::clrBuf()
{
  wrIdx = 0;
  rdIdx = 0;
}

void Monitor::sendConfig()
{
  int           nrChn   = nrOfChnChar & 0x3F;
  int           bufIdx  = 0;
  CfgMeasChnPtr cfgPtr;

  // Visualisierungskanäle (Anzahl) anfordern
  outChar[bufIdx++] = '&';
  outChar[bufIdx++] = '@';    // Kanalnummer
  outChar[bufIdx++] = '@';    // Typ
  outChar[bufIdx++] = nrOfChnChar;
  outChar[bufIdx++] = '$';

  for(int i = 0; i < nrChn; i++)
  {
    cfgPtr = &cfgChnArr[i];
    outChar[bufIdx++] = '&';
    outChar[bufIdx++] = (i+1) | 0x40;
    outChar[bufIdx++] = cfgPtr->type;
    hexWord(&outChar[bufIdx], cfgPtr->maxVal);
    bufIdx += 4;
    hexWord(&outChar[bufIdx], cfgPtr->minVal);
    bufIdx += 4;
    if(cfgPtr->name != NULL)
    {
      bufIdx += cpyStr(&outChar[bufIdx], cfgPtr->name);
    }
    outChar[bufIdx++] = '$';
  }
  outChar[bufIdx] = '\0';
  out(outChar);
}

//-----------------------------------------------------------------------------
// Lokale Abläufe
//-----------------------------------------------------------------------------
//

void Monitor::waitEnter()
{
  char  c;

  busy = false;

  if(!keyHit()) return;
  c = keyIn();
  if(c != '\r' && c != '\n')
  {
    lastKeyIn = c;
    return;
  }

  busy = true;

  blkIn   = true;
  blkOut  = true;
  GoPrm
}

void Monitor::getKey()
{
  char  cin,c1,c0;

  if(!keyHit()) return;
  cin = keyIn();
  if(mode & modeEcho)
    out(cin);

  c0 = c1 = '\0';

  if(inIdx == 0)
    c0 = cin;
  else if(inIdx == 1)
  {
    c0 = inChar[0];
    c1 = cin;
  }
  else if(inIdx == 2)
  {
    c0 = inChar[0];
    c1 = inChar[1];
  }

  switch(c0)
  {
    case '\r':
      out("\r\n");
      blkIn   = false;
      blkOut  = false;
      GoWt
      break;

    case 'c':
    case 'C':
      if(inIdx == 0)
      {
        inChar[inIdx] = cin;
        inIdx++;
      }
      else if(inIdx == 1)
      {

        if(cin >= '0' && cin <= '9')
        {
          inIdx = 0;
          out('=');
          int cIdx = cin - 0x30;
          if(cFlag[cIdx])
          {
            cFlag[cIdx] = false;
            out('0');
          }
          else
          {
            cFlag[cIdx] = true;
            out('1');
          }
          GoPrm
        }

        else if(cin == 'f' || cin == 'F')
        {
          inChar[inIdx] = cin;
          inIdx++;
        }
      }

      else if(inIdx == 2)
      {
        inIdx = 0;
        if(cin == 'g' || cin == 'G')
        {
          sendConfig();
          GoPrm
        }
      }
      else
        GoPrm
      break;

    case 'i':
    case 'I':
      if(inIdx == 0)
      {
        inChar[inIdx] = cin;
        inIdx++;
      }
      else if(inIdx == 1)
      {
        inIdx = 0;
        out(' ');
        if(cin == 'a' || cin == 'A')
          nextState = &Monitor::getTwiAdr;
        else if(cin == 'l' || cin == 'L')
          nextState = &Monitor::readTwiList;
        else if(cin == 'r' || cin == 'R')
          nextState = &Monitor::readTwiByte;
        else if(cin == 'w' || cin == 'W')
          nextState = &Monitor::writeTwiByte;
      }
      break;

    case 'r':
    case 'R':
      if(inIdx == 0)
      {
        inChar[inIdx] = cin;
        inIdx++;
      }
      else if(inIdx == 1)
      {
        inIdx = 0;
        out(' ');
        if(cin == 'o' || cin == 'O')
          nextState = &Monitor::getRdOffsAdr;
        else if(cin == 'r' || cin == 'R')
          nextState = &Monitor::readRegVal;
      }
      break;

    case 't':
    case 'T':
      if(inIdx == 0)
      {
        inChar[inIdx] = cin;
        inIdx++;
      }
      else if(inIdx == 1)
      {
        inIdx = 0;
        nextState = &Monitor::getTiming;
        if(cin == 'l' || cin == 'L')
        {
          cmdMode1 = 'L';
        }
        else if(cin == 'b' || cin == 'B')
        {
          cmdMode1 = 'B';
        }
        else if(cin == 'c' || cin == 'C')
        {
          cmdMode1 = 'C';
        }
        else if(cin == 'r' || cin == 'R')
        {
          if(lcPtr != NULL)
          {
            lcPtr->resetStatistics();
            out(' ');
          }
          GoPrm
        }
        else
        {
          GoPrm
        }
      }
      break;

    case 'V':
    case 'v':
      out(' ');
      nextState = &Monitor::version;
      break;

  }
}

void Monitor::prompt()
{
  if(mode & modeNl)
    out('\n');

  out("\rM>");
  GoInp
}

void Monitor::version()
{
  out("Monitor: Version 0.1, May 16, 2021");
  GoPrm
}

void Monitor::getRdOffsAdr()
{
  char  cin;
  dword val;
  int   valIdx;

  if(!keyHit()) return;
  cin = keyIn();
  readOffsAddr = 0;

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx < 16)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    valIdx = 0;
    inIdx--;
    while(inIdx >= 0)
    {
      val = inChar[inIdx];
      readOffsAddr |= val << valIdx * 4;
      inIdx--;
      valIdx++;
    }

    // TEST
    //out(" = ");
    //out(readOffsAddr);
    inIdx = 0;
    GoPrm
  }
}

void Monitor::readRegVal()
{
  char  cin;
  dword val,adr;
  int   valIdx;

  dword *regPtr;

  if(!keyHit()) return;
  cin = keyIn();
  adr = 0;

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx < 16)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    valIdx = 0;
    inIdx--;
    while(inIdx >= 0)
    {
      val = inChar[inIdx];
      adr |= val << valIdx * 4;
      inIdx--;
      valIdx++;
    }

    regPtr = (dword *) (readOffsAddr + adr);
    val = *regPtr;

    out(": ");
    hexDword(outChar, val);
    out(outChar);
    inIdx = 0;
    GoPrm
  }
}

void Monitor::getTiming()
{
  LoopStatistics  lStat;
  unsigned int    maxVal;
  unsigned int    minVal;
  unsigned int    avgVal;
  char  cin;

  if(lcPtr == NULL)
  {
    outl("Kein LoopCheck.");
    GoPrm
    return;
  }

  if(!keyHit()) return;
  cin = keyIn();
  if(mode & modeEcho)
    out(cin);

  if(cin == 'r' || cin == 'R')
  {
    out(' ');
    lcPtr->getStatistics(&lStat);
    if(cmdMode1 == 'L')
    {
      maxVal = lStat.loopMaxTime;
      minVal = lStat.loopMinTime;
      avgVal = lStat.loopAvgTime;
    }
    else if(cmdMode1 == 'B')
    {
      maxVal = lStat.bgMaxTime;
      minVal = lStat.bgMinTime;
      avgVal = lStat.bgAvgTime;
    }
    else if(cmdMode1 == 'C')
    {
      maxVal = lStat.maxPeriod;
      minVal = lStat.minPeriod;
      avgVal = lStat.avgPeriod;
    }
    else
    {
      maxVal = 0;
      minVal = 0;
      avgVal = 0;
    }

    out(maxVal); out("/"); out(minVal); out("/"); out(avgVal); out("\r");
    GoPrm
  }
  else if(cin == 'm' || cin == 'M')
  {
    lcPtr->resetStatistics();
    lcPtr->startTimeMeasure();
    nextState = &Monitor::getLoopMeasure;
  }

}

void Monitor::getLoopMeasure()
{
  LoopStatistics  lStat;
  unsigned int    maxVal;
  unsigned int    minVal;
  unsigned int    avgVal;

  if(lcPtr->getTimeMeasure() < 1000000)
    return;

  out(' ');
  lcPtr->getStatistics(&lStat);
  if(cmdMode1 == 'L')
  {
    maxVal = lStat.loopMaxTime;
    minVal = lStat.loopMinTime;
    avgVal = lStat.loopAvgTime;
  }
  else if(cmdMode1 == 'B')
  {
    maxVal = lStat.bgMaxTime;
    minVal = lStat.bgMinTime;
    avgVal = lStat.bgAvgTime;
  }
  else if(cmdMode1 == 'C')
  {
    maxVal = lStat.maxPeriod;
    minVal = lStat.minPeriod;
    avgVal = lStat.avgPeriod;
  }
  else
  {
    maxVal = 0;
    minVal = 0;
    avgVal = 0;
  }

  out(maxVal); out("/"); out(minVal); out("/"); out(avgVal); out("\r");
  GoPrm
}


void Monitor::getTwiAdr()
{
  char  cin;

  if(!keyHit()) return;
  cin = keyIn();
  twiAdr = 0;

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx < 2)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    twiAdr = (inChar[0] << 4) | inChar[1];

    out(" = ");
    out(twiAdr);

    inIdx = 0;
    GoPrm
  }
}

void Monitor::readTwiList()
{
  char  cin;
  int   reg;
  int   anz;

  char  tmpOut[3];

  TwiStatus twiStatus;

  if(twiPtr == NULL)
  {
    out("no Twi");
    inIdx = 0;
    GoPrm
  }

  if(!keyHit()) return;
  cin = keyIn();

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx == 1)
      out(" ");

    if(inIdx < 4)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    reg = (inChar[0] << 4) | inChar[1];
    anz = (inChar[2] << 4) | inChar[3];

    twiByteSeq.len = anz;
    twiByteSeq.valueRef = byteArray;

    twiStatus = twiPtr->readByteRegSeq(twiAdr, reg, &twiByteSeq);

    out(" [");
    out((int) twiStatus);
    out("] ");

    if((int) twiStatus == 128)
    {
      for(int i = 0; i < anz; i++)
      {
        hexByte(tmpOut, byteArray[i]);
        out(tmpOut);
        if(i != (anz-1)) out(':');
      }
    }

    inIdx = 0;
    GoPrm
  }
}

void Monitor::readTwiByte()
{
  char  cin;
  int   reg;
  byte  val;

  if(twiPtr == NULL)
  {
    out("no Twi");
    inIdx = 0;
    GoPrm
  }

  if(!keyHit()) return;
  cin = keyIn();

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx < 2)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    reg = (inChar[0] << 4) | inChar[1];

    val = twiPtr->readByteReg(twiAdr, reg);

    out(" = ");
    binByte(outChar, val);
    out(outChar);

    inIdx = 0;
    GoPrm
  }
}

void Monitor::writeTwiByte()
{
  char  cin;
  int   reg;
  int   val;

  TwiStatus twiStatus;

  if(twiPtr == NULL)
  {
    out("no Twi");
    inIdx = 0;
    GoPrm
  }

  if(!keyHit()) return;
  cin = keyIn();

  if(cin != '\r')
  {
    if(cin >= '0' && cin <= '9')
      inChar[inIdx] = cin - 0x30;
    else if(cin >= 'A' && cin <= 'F')
      inChar[inIdx] = cin - 0x37;
    else if(cin >= 'a' && cin <= 'f')
      inChar[inIdx] = cin - 0x57;
    else
      return;

    out(cin);
    if(inIdx == 1)
      out(" ");

    if(inIdx < 4)
      inIdx++;
    else
      out((char) 0x08);
  }
  else
  {
    reg = (inChar[0] << 4) | inChar[1];
    val = (inChar[2] << 4) | inChar[3];

    twiStatus = twiPtr->writeByteReg(twiAdr, reg, val);

    out(" : ");
    out((int) twiStatus);

    inIdx = 0;
    GoPrm
  }
}

//-----------------------------------------------------------------------------
// Lokale Schnittstelle
//-----------------------------------------------------------------------------
//
void Monitor::print(char c, int eol)
{
  putBuf(c);
  if(eol & eolCR)
    putBuf('\r');
  if(eol & eolLF)
    putBuf('\n');
}

void Monitor::print(char *txt, int eol)
{
  if(txt != NULL)
    putBuf(txt);
  if(eol & eolCR)
    putBuf('\r');
  if(eol & eolLF)
    putBuf('\n');
}

void Monitor::print(byte *hex, int nr, char fill, int eol)
{
  if(hex == NULL) return;

  for(int i = 0; i < nr; i++)
  {
    hexByte(tmpChar,hex[i]);
    tmpChar[2] = fill;
    tmpChar[3] = '\0';
    putBuf(tmpChar);
  }

  if(eol & eolCR)
    putBuf('\r');
  if(eol & eolLF)
    putBuf('\n');
}

void Monitor::print(unsigned int iVal, int eol)
{
  char  iBuf[16];

  itoa(iVal, iBuf, 10);
  putBuf(iBuf);
  if(eol & eolCR)
    putBuf('\r');
  if(eol & eolLF)
    putBuf('\n');
}

void Monitor::prints(int iVal, int eol)
{
  char  iBuf[16];

  itoa(iVal, iBuf, 10);
  putBuf(iBuf);
  if(eol & eolCR)
    putBuf('\r');
  if(eol & eolLF)
    putBuf('\n');
}

//-----------------------------------------------------------------------------
// Datenaufbereitung
//-----------------------------------------------------------------------------
//
void Monitor::hexByte(char * dest, byte val)
{
  char cv;

  cv = val >> 4;
  if(cv < 10)
    cv += 0x30;
  else
    cv += 0x37;
  dest[0] = cv;

  cv = val & 0x0F;
  if(cv < 10)
    cv += 0x30;
  else
    cv += 0x37;
  dest[1] = cv;

  dest[2] = '\0';
}

void Monitor::binByte(char * dest, byte val)
{
  byte mask;

  mask = 0x80;

  for(int i = 0; i < 8; i++)
  {
    if((val & mask) != 0)
      dest[i] = '1';
    else
      dest[i] = '0';
    mask >>= 1;
  }

  dest[8] = '\0';
}

int   Monitor::cpyStr(char *dest, char *src)
{
  int   i = 0;

  while((dest[i] = src[i]) != '\0') i++;
  return(i);
}

void Monitor::binDword(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  binByte(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 16;
  binByte(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 8;
  binByte(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal;
  binByte(&dest[idx], bVal);
  idx += 8;

  dest[idx] = '\0';
}

void Monitor::hexDword(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  hexByte(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 16;
  hexByte(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 8;
  hexByte(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal;
  hexByte(&dest[idx], bVal);
  idx += 2;

  dest[idx] = '\0';
}

void Monitor::binWord(char *dest, word wVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = wVal >> 8;
  binByte(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = wVal;
  binByte(&dest[idx], bVal);
  idx += 8;

  dest[idx] = '\0';
}

void Monitor::hexWord(char *dest, word wVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = wVal >> 8;
  hexByte(&dest[idx], bVal);
  idx += 2;

  bVal = wVal;
  hexByte(&dest[idx], bVal);
  idx += 2;

  dest[idx] = '\0';
}



//-----------------------------------------------------------------------------
// Anwenderschnittstelle
//-----------------------------------------------------------------------------
//

void Monitor::run()
{
  char c;

  if(!blkOut)
  {
    c = getBuf();
    if(c != '\0')
      smnSerial.print(c);
  }
  (this->*nextState)();
}

void Monitor::cprint(char c)
{
  print(c, 0);
}

void Monitor::cprintln(char c)
{
  print(c, eolCR | eolLF);
}

void Monitor::cprintcr(char c)
{
  print(c, eolCR);
}

void Monitor::print(char *txt)
{
  print(txt, 0);
}

void Monitor::println(char *txt)
{
  print(txt, eolCR | eolLF);
}

void Monitor::println()
{
  print((char *) NULL, eolCR | eolLF);
}

void Monitor::printcr(char *txt)
{
  print(txt, eolCR);
}

void Monitor::printcr()
{
  print((char *) NULL, eolCR);
}

void Monitor::print(unsigned int iVal)
{
  print(iVal, 0);
}

void Monitor::prints(int iVal)
{
  prints(iVal, 0);
}

void Monitor::println(unsigned int iVal)
{
  print(iVal, eolCR | eolLF);
}

void Monitor::printcr(unsigned int iVal)
{
  print(iVal, eolCR);
}

void Monitor::print(byte *iVal, int nr, char fill)
{
  print(iVal, nr, fill, 0);
}

void Monitor::printcr(byte *iVal, int nr, char fill)
{
  print(iVal, nr, fill, eolCR);
}

void Monitor::println(byte *iVal, int nr, char fill)
{
  print(iVal, nr, fill, eolCR | eolLF);
}

void Monitor::setInfo(char *txt)
{
  info = txt;
}

void Monitor::config(int inNrOfChn)
{
  if(inNrOfChn < 0) return;

  if(inNrOfChn > MaxChn)
    inNrOfChn = MaxChn;

  nrOfChnChar = inNrOfChn | 0x40;
}

void Monitor::config(int inChn, char inType, word inMax, word inMin, char *inName)
{
  if(inChn < 1) return;

  if(inChn > MaxChn)
    inChn = MaxChn;

  CfgMeasChnPtr chnPtr = &cfgChnArr[inChn-1];
  chnPtr->maxVal = inMax;
  chnPtr->minVal = inMin;
  chnPtr->name = inName;
  chnPtr->type = inType;
}


