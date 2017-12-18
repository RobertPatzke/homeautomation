// ---------------------------------------------------------------------------
// This module is for sending broadcast messages (Twitter) with a predefined
// telegram format as described at
// http://homeautomation.x-api.de/wikidha/index.php?title=Telegramm
// It will work only in connection with module SocManNet.java
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      15.12.2017
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//

package hsh.mplab.socmannet;

import android.annotation.SuppressLint;
import java.io.IOException;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import java.text.SimpleDateFormat;

import java.util.Date;
import java.util.Locale;

public class Twitter 
{
  // Datentypen
  //
  public enum Speed 
  {
    normal,
    high,
    low,
    nrOfSpeeds
  }
  
  // Globale Variablen für die Anwendung
  //
  public int        pduCounterLimit = 100;
  public int        errorCode;
  public String     errorMsg;
  public String     resultMsg;
  
  public boolean    enabled;
  public Speed      speed;
  
  public int        posX;
  public int        posY;
  public int        posZ;
  public int        baseState;
  public int        baseMode;
  
  public int[]      intValArray;  
  public float[]    floatValArray;
  public String[]   textValArray;
  
  public int        broadcastPort;
  public int        defaultPort = 4100;
  public String     defaultObject = "TestTwitter";
  public int        applicationKey = 0;
  public int        deviceKey = 1234;
  public int        deviceState = 33;
  public String     deviceName = "SP001";
  
  // Lokale Variablen für die interne Verarbeitung
  //
  InetAddress       broadcastAdr;
  DatagramSocket    localSocket;
  DatagramPacket    localPacket;
  String            objectName;
  String            pduHeader;
  String            localMAC;
  String            localIP;
  
  // -------------------------------------------------------------------------
  //region Konstruktoren und Initialisierungen
  // -------------------------------------------------------------------------
  
  public Twitter()
  {
    init(defaultPort, defaultObject, 4, 4, 4, Speed.normal);
  }

  public Twitter(String commObject)
  {
    init(defaultPort, commObject, 4, 4, 4, Speed.normal);
  }

  public Twitter(Speed inSpeed)
  {
    init(defaultPort, defaultObject, 4, 4, 4, inSpeed);
  }

  public Twitter(String commObject, Speed inSpeed)
  {
    init(defaultPort, commObject, 4, 4, 4, inSpeed);
  }

  public Twitter(int port, String commObject)
  {
    init(port, commObject, 4, 4, 4, Speed.normal);
  }

  public Twitter(int port, String commObject, Speed inSpeed)
  {
    init(port, commObject, 4, 4, 4, inSpeed);
  }

  public Twitter(int port, String commObject, int nrInt, int nrFloat, int nrText)
  {
    init(port, commObject, nrInt, nrFloat, nrText, Speed.normal);
  }
  
  public Twitter(String commObject, int nrInt, int nrFloat, int nrText)
  {
    init(defaultPort, commObject, nrInt, nrFloat, nrText, Speed.normal);
  }
  
  public Twitter(String commObject, int nrInt, int nrFloat, int nrText, Speed inSpeed)
  {
    init(defaultPort, commObject, nrInt, nrFloat, nrText, inSpeed);
  }
  
  void init(int port, String commObject, int nrInt, int nrFloat, int nrText, Speed inSpeed)
  {
    errorCode         = 0;
    broadcastPort     = port;
    objectName        = commObject;
    speed             = inSpeed;
    
    if(nrInt > 0)
      intValArray     = new int[nrInt];
    if(nrFloat > 0)
      floatValArray   = new float[nrFloat];
    if(nrText > 0)
      textValArray    = new String[nrText];
        
    SocManNet.init(false);
    errorCode     = SocManNet.errorCode;
    errorMsg      = SocManNet.errorMsg;
    resultMsg     = SocManNet.resultMsg;
    broadcastAdr  = SocManNet.broadcastAdr;
    localMAC      = SocManNet.localMAC;
    localIP       = SocManNet.localIP;
    
    if(errorCode != 0)
      return;
    
    pduHeader       = createMsgHeader();
    try
    {
      localSocket     = new DatagramSocket();
    }
    catch(Exception exc)
    {
      errorMsg = exc.getMessage();
      errorCode = 3;
    }
  }

  public void init(String comObject, int nrInt, int nrFloat, int nrText, Speed speed)
  {
    init(defaultPort, comObject, nrInt, nrFloat, nrText, speed);
  }
  // endregion

  // -------------------------------------------------------------------------
  // region Einfache Hilfsfunktionen
  // -------------------------------------------------------------------------
  
  String getByteString(byte[] byteArray)
  {
    String retStr = "";
    
    if(byteArray == null)
    {
      retStr = "00-00-00-00-00-00";
      return(retStr);
    }
    
    for(int i = 0; i < byteArray.length; i++)
    {
      retStr += String.format("%02X-", byteArray[i]);
    }
    
    return(retStr.substring(0, retStr.length()-1));
  }
  
  // endregion
  
  // -------------------------------------------------------------------------
  // region Anwenderfunktionen
  // -------------------------------------------------------------------------

  public enum RunStatus
  {
    Wait,
    Create,
    Send,
    Error,
    NrOfStates
  }
  
  public RunStatus runStatus = RunStatus.Wait;
  public RunStatus runError  = RunStatus.NrOfStates;

  String    runErrorMsg;
  int       delayCounter;
  int       speedCounter = 8;
  byte[]    sendBuffer;
  
  public void run(int secFactor, int delay)
  {
    int  fin;
    int  speedLimit;
    
    if(!enabled)
    {
      if(runStatus == RunStatus.Wait)
        return;
    }
    
    if(delay > 0)
    {
      delayCounter++;
      if(delayCounter < delay) return;
    }
    delayCounter = 0;
    
    switch(runStatus)
    {
      case Wait:
        speedCounter++;
        if(speed == Speed.high)
          speedLimit = secFactor/10;
        else if(speed == Speed.low)
          speedLimit = secFactor * 10;
        else
          speedLimit = secFactor;
        if(speedCounter >= speedLimit)
        {
          speedCounter  = 8;
          runStatus     = RunStatus.Create;
          errorCode     = 0;
        }
        break;
    
      case Create:
        fin = createPDU(secFactor);
        if(fin == 1)
        {
          sendBuffer = createPDUMsg.getBytes();
          sendBuffer[sendBuffer.length - 1] = 0;
          localPacket = 
              new DatagramPacket(sendBuffer,sendBuffer.length,broadcastAdr,broadcastPort);
          runStatus = RunStatus.Send;
        }
        break;
        
      case Send:
        try
        {
          localSocket.send(localPacket);
          runStatus = RunStatus.Wait;
        }
        catch(IOException exc)
        {
          runErrorMsg = exc.getMessage();
          errorMsg    = "run.Send Error: " + runErrorMsg;
          errorCode   = 1;
          runError    = runStatus;
          runStatus   = RunStatus.Error;
        }
        break;

      case Error:
        
        break;
        
      default:
        runStatus = RunStatus.Wait;
        break;
    }
  }
  
  public void run(int secFactor)
  {
    run(secFactor, 0);
  }
  
  
  public void setIntValue(int idx, int value)
  {
    if(idx < 0) return;
    if(intValArray == null) return;
    if(idx >= intValArray.length) return;
    intValArray[idx] = value;
  }
  
  public void setFloatValue(int idx, float value)
  {
    if(idx < 0) return;
    if(floatValArray == null) return;
    if(idx >= floatValArray.length) return;
    floatValArray[idx] = value;
  }
  
  public void setTextValue(int idx, String value)
  {
    if(idx < 0) return;
    if(textValArray == null) return;
    if(idx >= textValArray.length) return;
    textValArray[idx] = value;
  }
  
  //endregion

  // -------------------------------------------------------------------------
  //region Telegrammkonstruktion
  // -------------------------------------------------------------------------
  // Das Telegramm wird in einer Zustandsmaschine zusammengesetzt

  enum CrPDUStatus
  {
    Header,
    Time,
    Standard,
    ValueHeader,
    IntValues,
    FloatValues,
    TextValues,
    NrOfStates
  }
  
  CrPDUStatus crPDUStatus = CrPDUStatus.Header;
  
  String    createPDUMsg;
  boolean   testDebug01 = false;
  
  @SuppressLint("SimpleDateFormat")
  int createPDU(int frequency)
  {
    int  ready = 0;
    
    if(testDebug01)
    {
      createPDUMsg = "Einfach nur ein Telegramm";
      return(1);
    }
    
    switch(crPDUStatus)
    {
      case Header:
        createPDUMsg = pduHeader + createDeviceHeader();
        crPDUStatus = CrPDUStatus.Time;
        break;
      
      case Time:
        Date actTime = new Date();
        if(frequency > 0)
        {
          long actMillisec = actTime.getTime();
          actMillisec += 3 * 1000 / frequency;
          actTime = new Date(actMillisec);
        }
        createPDUMsg += pduFromTime(actTime);
        crPDUStatus = CrPDUStatus.Standard;
        break;
        
      case Standard:
        createPDUMsg += ";" + posX + ";" + posY + ";" + posZ + ";"
                            + baseState + ";" + baseMode; 
        crPDUStatus = CrPDUStatus.ValueHeader;
        break;
        
      case ValueHeader:
        int intLen, floatLen, textLen;
        
        if(intValArray == null)
          intLen = 0;
        else
          intLen = intValArray.length;
        
        if(floatValArray == null)
          floatLen = 0;
        else
          floatLen = floatValArray.length;
        
        if(textValArray == null)
          textLen = 0;
        else
          textLen = textValArray.length;
        
        createPDUMsg += ";" + intLen + ";" + floatLen + ";" + textLen;
        crPDUStatus = CrPDUStatus.IntValues;
        break;
        
      case IntValues:
        if(intValArray != null)
        {
          for(int i = 0; i < intValArray.length; i++)
            createPDUMsg += ";" + intValArray[i];
        }
        crPDUStatus = CrPDUStatus.FloatValues;
        break;
        
      case FloatValues:
        if(floatValArray != null)
        {
          for(int i = 0; i < floatValArray.length; i++)
            createPDUMsg += ";" + floatValArray[i];
        }
        crPDUStatus = CrPDUStatus.TextValues;
        break;
        
      case TextValues:
        if(textValArray != null)
        {
          for(int i = 0; i < textValArray.length; i++)
            createPDUMsg += ";" + textValArray[i];
        }
        createPDUMsg += "::0";  // Beim der Verarbeitung des Telegramms wird das letzte Zeichen
                                // mit #00 überschrieben, um einen C-String zu generieren.
                                // Hier wird '0' als entsprechender Platzhalter angehängt
        ready = 1;
        crPDUStatus = CrPDUStatus.Header;
        break;
        
      default:
        ready = 1;
        crPDUStatus = CrPDUStatus.Header;
        break;
    }
    
    return(ready);
  }
  
  
  String createMsgHeader()
  {
    String headerStr = "N10:" + localMAC + ":" + localIP + ":53:0:" + objectName + ":0:";
    return(headerStr);
  }
  
  int pduCounter = 0;
  
  String createDeviceHeader()
  {
    pduCounter++;
    if(pduCounter >= pduCounterLimit)
      pduCounter = 1;

    String headerStr = pduCounter + ";" + applicationKey + ";" + deviceKey + ";" + deviceState + ";" + deviceName + ";";
    return(headerStr);
  }
  
  //@SuppressWarnings("deprecation")
  String pduFromTime(Date inTime)
  {
    String    pdu;
    //int       zoneDiff;
    //int       zoneDiffHours;
    //int       zoneDiffMinutes;
    
    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.S", Locale.GERMANY);

    pdu = dateFormat.format(inTime);

    /*
    zoneDiff = inTime.getTimezoneOffset();
    
    if(zoneDiff < 0)
    {
      zoneDiffHours = -zoneDiff / 60;
      zoneDiffMinutes = -zoneDiff % 60;
    }
    else
    {
      zoneDiffHours = zoneDiff / 60;
      zoneDiffMinutes = zoneDiff % 60;
    }   
    
    if (zoneDiff >= 0)
    {
      pdu += String.format("+%02d:%02d", zoneDiffHours, zoneDiffMinutes);       
    }
    else 
    {
      pdu += String.format("-%02d:%02d", zoneDiffHours, zoneDiffMinutes);       
    }
    */
    
    return (pdu);
  }

  public String getPDU()
  {
    int fin;
    
    do
    {
      fin = createPDU(0);
    }
    while (fin == 0);
    
    return(createPDUMsg);
  }
  
  //endregion 
  
}
