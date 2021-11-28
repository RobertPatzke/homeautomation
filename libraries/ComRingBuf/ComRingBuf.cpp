//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   ComRingBuf.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   21. November 2021
//
// Der Inhalt dieser Datei sind Festlegungen zur Gestaltung eines Ringpuffers.
//

#include "ComRingBuf.h"

  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------

  ComRingBuf::ComRingBuf()
  {
    rbReadIdx = 0;
    rbWriteIdx = 0;
    sbReadIdx = 0;
    sbWriteIdx = 0;
    newLineMode = NewLineModeNL;
  }

  void ComRingBuf::begin(IntrfSerial *ser)
  {
    serIf = ser;
  }

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void ComRingBuf::setNewLineMode(byte nlMode)
  {
    newLineMode = nlMode;
  }


  // --------------------------------------------------------------------------
  // Schnittstellen
  // --------------------------------------------------------------------------
  //
  // Byte aus dem Sendepuffer lesen
  //
  bool  ComRingBuf::getByteSnd(byte *dest)
  {
    if(sbReadIdx == sbWriteIdx) return(false);

    *dest = sndBuffer[sbReadIdx];
    sbReadIdx++;
    if(sbReadIdx >= sbSize)
      sbReadIdx = 0;
    return(true);
  }

  // Byte in den Empfangspuffer schreiben
  //
  void  ComRingBuf::putByteRec(byte b)
  {
    int space = rbReadIdx - rbWriteIdx - 1;
    if(space == 0) return;
  }


// ----------------------------------------------------------------------------
// Writing and reading data via circular buffer (default usage)
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// Lesen (Empfangsvorgänge)
// ----------------------------------------------------------------------------

void  ComRingBuf::setReadBuffer(int size, byte *bufPtr)
{
  recBuffer = bufPtr;
  rbSize = size;
  rbReadIdx = 0;
  rbWriteIdx = 0;
}

int   ComRingBuf::getChr()
{
  int retv;

  if(rbReadIdx == rbWriteIdx)
    return(EOF);

  retv = recBuffer[rbReadIdx];
  rbReadIdx++;
  if(rbReadIdx >= rbSize)
    rbReadIdx = 0;
  return(retv);
}

void  ComRingBuf::clrRecBuf()
{
  rbReadIdx  = 0;
  rbWriteIdx = 0;
}

int   ComRingBuf::getAll(byte *buffer)
{
  int   count, i;
  int   tmpInt;

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

int   ComRingBuf::getCount(int len, byte *buffer)
{
  int  count, i;
  int  tmpInt;

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

int   ComRingBuf::getCountStr(int len, char *buffer)
{
  int nrChar;

  nrChar = getCount(len, (uint8_t *) buffer);
  if(nrChar == EOF) return(EOF);

  buffer[nrChar] = 0;
  return(nrChar);
}

int   ComRingBuf::getLine(char *buffer)
{
  bool      eol;
  int       count, i;
  int       tmpInt;

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

int   ComRingBuf::getLineDec(int *intValue)
{
  bool      eol, inVal;
  int       count, i, j;
  int       tmpInt;
  char      c;
  char      buffer[32];

  if(rbReadIdx == rbWriteIdx)
    return(0);

  tmpInt = rbWriteIdx - rbReadIdx;
  if(tmpInt < 0)
    count = tmpInt + rbSize;
  else
    count = tmpInt;

  if(count > 30)
    count = 30;

  eol = false;

  j = 0;
  inVal = false;

  for(i = 0; i < count; i++)
  {
    c = recBuffer[rbReadIdx];
    if(!inVal)
    {
      if(c > '9')
      {
        rbReadIdx++;
        if(rbReadIdx >= rbSize)
          rbReadIdx = 0;
        continue;
      }
      inVal = true;
    }

    if(!eol)
    {
    if(c == '\r' || c == '\n')
      eol = true;
    }
    else
    {
      if(c != '\r' && c != '\n')
        break;
    }
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;
    buffer[j++] = c;
  }

  if(!eol) return(0);

  buffer[j] = 0;
  *intValue = atoi(buffer);
  return(i);
}

char ComRingBuf::getC()
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


int   ComRingBuf::waitLine(int waitLoop, char *buffer)
{
  char inChar;
  bool eol;

  if(loopCount == 0)
  {
    tmpIdx = 0;
  }

  tmpVal = inCount();
  if(tmpVal < 1)
  {
    loopCount++;
    if(loopCount < waitLoop)
      return(0);
    else
    {
      loopCount = 0;
      return(EOF);
    }
  }

  eol = false;

  for(int i = 0; i < tmpVal; i++)
  {
    inChar = getC();
    buffer[tmpIdx++] = inChar;
    if(inChar == '\r' || inChar == '\n')
    {
      eol = true;
      break;
    }
  }

  if(eol)
  {
    buffer[tmpIdx] = 0;
    loopCount = 0;
    return(tmpIdx);
  }

  return(0);
}

//int   ComRingBuf::waitLineDec(int waitLoop, int *intValue)
//{
//};

int   ComRingBuf::chkLine(char *rsp)
{
  int     i,chkVal;
  char    chkChar;

  chkVal = inCount();
  if(chkVal <= strlen(rsp))
    return(0);

  chkVal = 0;

  for(i = 0; i < strlen(rsp); i++)
  {
    chkChar = getC();
    if(rsp[i] != chkChar)
    {
      chkVal = -100000;
      break;
    }
    else
      chkVal++;
  }

  if(chkVal < 0) chkVal = EOF;

  while( (recBuffer[rbReadIdx] == '\r' || recBuffer[rbReadIdx] == '\n' )
         && (rbReadIdx != rbWriteIdx))
  {
    rbReadIdx++;
    if(rbReadIdx >= rbSize)
      rbReadIdx = 0;
  }

  return(chkVal);
}

int   ComRingBuf::chkBuf(char *rsp)
{
  int     i,chkVal;
  char    chkChar;

  chkVal = inCount();
  if(chkVal < strlen(rsp))
    return(0);

  chkVal = 0;

  for(i = 0; i < strlen(rsp); i++)
  {
    chkChar = getC();
    if(rsp[i] != chkChar)
      chkVal = -100;
    else
      chkVal++;
  }

  return(chkVal);
}

int   ComRingBuf::waitAll(int waitLoop, byte *buffer)
{
}

int   ComRingBuf::waitChkBuf(int waitLoop, char *rsp)
{
  int   i;
  int   chkVal;
  char  chkChar;

  if(loopCount == 0)
  {
    tmpVal = strlen(rsp);
  }

  chkVal = inCount();
  if(chkVal < tmpVal)
  {
    loopCount++;
    if(loopCount < waitLoop)
      return(0);
    else
    {
      loopCount = 0;
      return(-10);
    }
  }

  chkChar = getC();

  if(rsp[0] != chkChar)
    return(0);

  if(tmpVal == 1)
  {
     loopCount = 0;
     return(1);
  }

  chkVal = 1;

  for(i = 1; i < tmpVal; i++)
  {
    chkChar = getC();
    if(rsp[i] != chkChar)
      return(0);
    else
      chkVal++;
  }

  loopCount = 0;
  return(chkVal);
}

int   ComRingBuf::inCount(void)
{
  int count = rbWriteIdx - rbReadIdx;
  if(count < 0)
    count += rbSize;
  return(count);
}

int   ComRingBuf::getRestChar(byte tagChr, int len, byte *buffer)
{
  int       count, i, j;
  byte      inChr;
  int       tmpInt;
  bool      tagged;

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

int   ComRingBuf::getRestStr(char *tagStr, int len, byte *buffer)
{
  int       count, i, j, tmpIdx;
  byte      inChr;
  int       tmpInt;
  bool      tagged;
  int       tagLen;
  int       tagIdx;

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

int   ComRingBuf::reqChkLine(char *req, char *rsp)
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
        if(rsp[i] != chkChar)
          chkVal = -100;
        else
          chkVal++;
      }

      while(     (recBuffer[rbReadIdx] == '\r' || recBuffer[rbReadIdx] == '\n')
          && (rbReadIdx != rbWriteIdx) )
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
// Schreiben (Sendevorgänge)
// ----------------------------------------------------------------------------

void  ComRingBuf::setWriteBuffer(int size, byte *bufPtr)
{
  sndBuffer = bufPtr;
  sbSize = size;
  sbReadIdx = 0;
  sbWriteIdx = 0;
}

int   ComRingBuf::putChr(int chr)
{
  int       space;
  bool      txDone;

  if(sndBuffer == NULL) return(EOF);
  if(serIf == NULL) return(EOF);

  space = getSpace();

  if(space == 0)
  {
    // Wenn der Sendepuffer voll ist, dann kann der Entlader feststecken
    serIf->resuSend();
    return(EOF);
  }

  if(sbReadIdx == sbWriteIdx)
  {
    // Wenn der Sendepuffer leer ist, dann kann das Zeichen evt.
    // direkt gesendet werden.
    txDone = serIf->condSend(chr);
    if(txDone) return(chr);
  }

  putBufB(chr);
  return(chr);
}

int   ComRingBuf::putStr(char *msg)
{
  int sIdx = 0;

  if(sndBuffer == NULL) return(EOF);
  if(serIf == NULL) return(EOF);

  int space = getSpace();
  int len = strlen(msg);
  if(space < len)
  {
    serIf->resuSend();
    return(EOF);
  }

  if(sbReadIdx == sbWriteIdx)
  {
    // Wenn der Sendepuffer leer ist, dann kann das erste Zeichen evt.
    // direkt gesendet werden.
    if(serIf->condSend(msg[0]))
      sIdx = 1;
  }

  for (int i = sIdx; i < len; i++)
   {
     sndBuffer[sbWriteIdx] = msg[i];
     sbWriteIdx++;
     if(sbWriteIdx >= sbSize)
       sbWriteIdx = 0;
   }

  return(len);
}

int   ComRingBuf::putSeq(byte *msg, int n)
{
  int sIdx = 0;

  if(sndBuffer == NULL) return(EOF);
  if(serIf == NULL) return(EOF);

  int space = getSpace();

  if(space < n)
  {
    serIf->resuSend();
    return(EOF);
  }

  if(sbReadIdx == sbWriteIdx)
  {
    // Wenn der Sendepuffer leer ist, dann kann das erste Zeichen evt.
    // direkt gesendet werden.
    if(serIf->condSend(msg[0]))
      sIdx = 1;
  }

  for (int i = sIdx; i < n; i++)
   {
     sndBuffer[sbWriteIdx] = msg[i];
     sbWriteIdx++;
     if(sbWriteIdx >= sbSize)
       sbWriteIdx = 0;
   }

  return(n);

}

int   ComRingBuf::putNL()
{
  int retv = 0;

  if(newLineMode & NewLineModeCR)
  {
    putBufB('\r');
    retv++;
  }

  if(newLineMode & NewLineModeNL)
  {
    putBufB('\n');
    retv++;
  }

  return(retv);
}

int   ComRingBuf::putLine(char *msg)
{
  int retv, nl;

  retv = putStr(msg);
  if(retv < 0)
    return(retv);

  nl = putNL();
  return(retv);
}

//int   ComRingBuf::putLine(char *msg, char c)
//{
//}

//int   ComRingBuf::putLine(char *msg, int n)
//{
//}


  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //









