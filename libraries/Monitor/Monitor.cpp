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
  mode        = inMode;
  cpu         = inCpu;
  wrIdx       = 0;
  rdIdx       = 0;
  blkOut      = false;
  blkIn       = false;
  inIdx       = 0;
  info        = NULL;

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
          cFlag[cIdx] = true;
        }
        blkIn   = false;
        blkOut  = false;
        GoWt
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



//-----------------------------------------------------------------------------
// Lokale Schnittstelle
//-----------------------------------------------------------------------------
//
void Monitor::print(char *txt, bool nl)
{
  if(txt != NULL)
    putBuf(txt);
  if(nl)
  {
    putBuf('\r');
    putBuf('\n');
  }
}

void Monitor::print(unsigned int iVal, bool nl)
{
  char  iBuf[16];

  itoa(iVal, iBuf, 10);
  putBuf(iBuf);
  if(nl)
  {
    putBuf('\r');
    putBuf('\n');
  }
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
  print(txt, false);
}

void Monitor::println(char *txt)
{
  print(txt, true);
}

void Monitor::println()
{
  print((char *) NULL, true);
}

void Monitor::print(unsigned int iVal)
{
  print(iVal, false);
}

void Monitor::println(unsigned int iVal)
{
  print(iVal, true);
}

void Monitor::setInfo(char *txt)
{
  info = txt;
}


