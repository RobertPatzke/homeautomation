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

Monitor::Monitor(int inMode, int inCpu)
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

  nextState =
      &Monitor::waitEnter;
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


//-----------------------------------------------------------------------------
// Lokale Abläufe
//-----------------------------------------------------------------------------
//

void Monitor::waitEnter()
{
  char  c;

  if(!keyHit()) return;
  c = keyIn();
  if(c != '\r' && c != '\n') return;

  blkIn   = true;
  blkOut  = true;
  GoPrm
}

void Monitor::getKey()
{
  char  cin,cc;

  if(!keyHit()) return;
  cin = keyIn();
  if(mode & modeEcho)
    out(cin);

  cc = '\0';

  if(inIdx == 0)
    cc = cin;
  else if(inIdx == 1)
    cc = inChar[0];
  else if(inIdx == 2)
    cc = inChar[1];

  switch(cc)
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
        inIdx = 0;
        if(cin >= '0' && cin <= '9')
        {
          int cIdx = cin - 0x30;
          if(cFlag[cIdx])
            cFlag[cIdx] = false;
          else
            cFlag[cIdx] = true;
        }
        out(' ');
        blkIn   = false;
        blkOut  = false;
        GoWt
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
    hexSeq(outChar, val);
    out(outChar);
    inIdx = 0;
    GoPrm
  }
}

//-----------------------------------------------------------------------------
// Lokale Schnittstelle
//-----------------------------------------------------------------------------
//
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
    hexAsc(tmpChar,hex[i]);
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

//-----------------------------------------------------------------------------
// Datenaufbereitung
//-----------------------------------------------------------------------------
//
void Monitor::hexAsc(char * dest, byte val)
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

void Monitor::binAsc(char * dest, byte val)
{
  byte mask;

  mask = 0x01;

  for(int i = 0; i < 8; i++)
  {
    if((val & mask) != 0)
      dest[i] = '1';
    else
      dest[i] = '0';
    mask <<= 1;
  }

  dest[8] = '\0';
}

int   Monitor::cpyStr(char *dest, char *src)
{
  int   i = 0;

  while((dest[i] = src[i]) != '\0') i++;
  return(i);
}

int Monitor::binSeq(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 16;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 8;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal;
  binAsc(&dest[idx], bVal);
  idx += 8;

  dest[idx] = '\0';
  return(idx);
}

int Monitor::hexSeq(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 16;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 8;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  dest[idx] = '\0';
  return(idx);
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


