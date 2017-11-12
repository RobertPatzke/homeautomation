package hsh.mplab.socmannet;

import hsh.mplab.socmannet.Follower.PortFollow.FollowElement;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;

import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;


public class Follower
{
  static public PortFollowList portFollowList;
  
  // Globale Variablen für die Anwendung
  //
  public int        defaultPort = 4100;
  public String     defaultObject = "TestTwitter";
  public String     errorMsg;
  public String     resultMsg;
  public int        errorCode;
  public boolean    enabled = false;

  // Lokale Variablen 
  //
  InetAddress       broadcastAdr;
  int               broadcastPort;
  byte[]            localBuffer;
  DatagramChannel   localChannel;
  DatagramSocket    localSocket;
  SocketAddress     localAdr;
  LocalReceiver     recBroadcast;
  
  //--------------------------------------------------------------------------
  //region Konstruktoren und Initialisierungen
  //--------------------------------------------------------------------------
  //
  public Follower()
  {
    init(defaultPort, defaultObject);
  }

  public Follower(int port)
  {
    init(port, defaultObject);
  }

  public Follower(String commObject)
  {
    init(defaultPort, commObject);
  }

  private void init(int port, String commObject)
  {
    SocManNet.init(false);
    errorMsg  = SocManNet.errorMsg;
    errorCode = SocManNet.errorCode;
    resultMsg = SocManNet.resultMsg;
    if(errorCode != 0) return;
    
    broadcastAdr  = SocManNet.broadcastAdr;
    broadcastPort = port;
    localBuffer   = new byte[1024];
        
    if(portFollowList != null)
    {
      PortFollow portFollow = portFollowList.getPortFollow(port);
      if(portFollow != null)
      {
        portFollow.add(commObject, this);
        return;
      }
    }
    else
      portFollowList = new PortFollowList();
    
    try
    {
      localChannel  = DatagramChannel.open();
      localSocket   = localChannel.socket();
      localAdr      = new InetSocketAddress(broadcastAdr, broadcastPort);
      localSocket.bind(localAdr);
      localChannel.configureBlocking(false);
      
    }
    catch(Exception exc)
    {
      errorMsg = exc.getMessage();
      errorCode = 3;
      return;
    }

    PortFollow portFollow = new PortFollow(port);
    portFollow.add(commObject, this);
    portFollowList.add(portFollow);
    recBroadcast = new LocalReceiver(broadcastAdr.getAddress(),localChannel);
    recBroadcast.start();
  }

  // -------------------------------------------------------------------------
  // Verarbeitung des eingegangenen Telegramms
  // -------------------------------------------------------------------------
  //
  public int  pduCount;
  public int  intCount;
  public int  floatCount;
  public int  textCount;

  public int        applicationKey;
  public int        deviceKey;
  public int        deviceState;
  public String     deviceName;

  public int        posX;
  public int        posY;
  public int        posZ;
  public int        baseState;
  public int        baseMode;

  int[]     intArray;
  float[]   floatArray;
  String[]  stringArray;
  
  public void handleInput(String[] header, String[] elements)
  {
    int   intIdx, floatIdx, textIdx, elementIdx;
    
    pduCount    = Integer.parseInt(header[SocManNet.pduHdCount]);
    intCount    = Integer.parseInt(elements[SocManNet.pduElTwNrInt]);
    floatCount  = Integer.parseInt(elements[SocManNet.pduElTwNrFloat]);
    textCount   = Integer.parseInt(elements[SocManNet.pduElTwNrText]);

    applicationKey  = Integer.parseInt(elements[SocManNet.pduElAppKey]);
    deviceKey       = Integer.parseInt(elements[SocManNet.pduElDevKey]);
    deviceState     = Integer.parseInt(elements[SocManNet.pduElDevState]);
    deviceName      = elements[SocManNet.pduElDevName];

    posX        = Integer.parseInt(elements[SocManNet.pduElPosX]);
    posY        = Integer.parseInt(elements[SocManNet.pduElPosY]);
    posZ        = Integer.parseInt(elements[SocManNet.pduElPosZ]);

    baseState   = Integer.parseInt(elements[SocManNet.pduElState]);
    baseMode    = Integer.parseInt(elements[SocManNet.pduElMode]);

    elementIdx = SocManNet.pduElTwValue;
    
    if((elements.length - elementIdx) < (intCount + floatCount + textCount))
      return;
    
    if(intCount > 0)
    {
      if(intArray == null || intArray.length < intCount)
        intArray = new int[intCount]; 

      for(intIdx = 0; intIdx < intCount; intIdx++)
      {
        intArray[intIdx] = Integer.parseInt(elements[elementIdx]);
        elementIdx++;
      }
    }
    
    
    if(floatCount > 0)
    {
      if(floatArray == null || floatArray.length < floatCount)
        floatArray = new float[floatCount];
    
      for(floatIdx = 0; floatIdx < floatCount; floatIdx++)
      {
        floatArray[floatIdx] = Float.parseFloat(elements[elementIdx]);
        elementIdx++;
      }
    }
    
    if(textCount > 0)
    {
      if(stringArray == null || stringArray.length < textCount)
        stringArray = new String[textCount];
    
      for(textIdx = 0; textIdx < textCount; textIdx++)
      {
        stringArray[textIdx] = elements[elementIdx];
        elementIdx++;
      }
    }
    
  }

  // -------------------------------------------------------------------------
  // Auslesen der Werte durch den Anwender
  // -------------------------------------------------------------------------
  //
  
  public static class ValueStatus
  {
    public final static int    newVal   = 0;     // Es liegt ein neuer Wert vor
    public final static int    empty    = 1;     // Noch kein Telegramm oder neuer Speicher
    public final static int    none     = 2;     // Sender liefert keinen entsprechenden Wert
    public final static int    newPdu   = 3;     // Ein neues Telegramm eingetroffen
    public final static int    idx      = 4;     // F�r den Index liegt kein Wert vor
    public final static int    lostPdu  = 5;     // Mehrere Telegramme nach letztem Zugriff

    public final static int    length   = 8;     // Die L�nge des Bitfeldes
  }
  
  public class ReceivedValue
  {
    public int          idx;
    public BitSet       status;
    public int          pduCount;
    public boolean      newValue;
    public boolean      newPdu;
  }

  // -------------------------------------------------------------------------
  // Integer
  // -------------------------------------------------------------------------
  //
  public class IntegerValue extends ReceivedValue
  {
    public int          value;
    public int          oldValue = Integer.MAX_VALUE;
    
    public IntegerValue(int inIdx)
    {
      idx = inIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  int intValIdxCnt = 0;

  public IntegerValue getIntegerValueInst()
  {
    IntegerValue retValue = new IntegerValue(intValIdxCnt);
    intValIdxCnt++;
    return (retValue);
  }

  public IntegerValue getIntegerValueInst(int idx)
  {
    IntegerValue integerValue = new IntegerValue(idx);
    return(integerValue);
  }

  public boolean getIntStatus(ReceivedValue intVal)
  {
    if(intArray == null || intArray.length < intCount)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      intVal.status.set(ValueStatus.empty);
      return true;
    }

    if(intCount < 1)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      intVal.status.set(ValueStatus.none);
      return true;
    }

    if(intVal.pduCount == pduCount)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      return true;
    }

    if(intVal.idx >= intArray.length)
    {
      intVal.status.clear();
      intVal.newValue = false;
      intVal.status.set(ValueStatus.newPdu);
      intVal.status.set(ValueStatus.idx);
      intVal.newPdu = true;
      return true;
    }

    intVal.status.clear();
    intVal.newValue = false;
    intVal.status.set(ValueStatus.newPdu);
    intVal.newPdu = true;
    if((pduCount - intVal.pduCount) > 1)
      intVal.status.set(ValueStatus.lostPdu);
    return false;
  }

  public void getValue(IntegerValue intVal)
  {
    boolean fin;

    fin = getIntStatus(intVal);
    if(fin) return;

    int value = intArray[intVal.idx];
    if(value != intVal.value)
    {
      intVal.value = value;
      intVal.status.set(ValueStatus.newVal);
      intVal.newValue = true;
    }
  }

  // -------------------------------------------------------------------------
  // Float
  // -------------------------------------------------------------------------
  //
  public class FloatValue extends ReceivedValue
  {
    public float        value;
    public float        oldValue = Float.MAX_VALUE;
    
    public FloatValue(int inIdx)
    {
      idx = inIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  int floatValIdxCnt = 0;

  public FloatValue getFloatValueInst()
  {
    FloatValue retVal = new FloatValue(floatValIdxCnt);
    floatValIdxCnt++;
    return (retVal);
  }

  public FloatValue getFloatValueInst(int idx)
  {
    FloatValue floatValue = new FloatValue(idx);
    return(floatValue);
  }

  public boolean getFloatStatus(ReceivedValue floatVal)
  {
    if(floatArray == null || floatArray.length < floatCount)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      floatVal.status.set(ValueStatus.empty);
      return true;
    }

    if(floatCount < 1)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      floatVal.status.set(ValueStatus.none);
      return true;
    }

    if(floatVal.pduCount == pduCount)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      return true;
    }

    if(floatVal.idx >= floatArray.length)
    {
      floatVal.status.clear();
      floatVal.newValue = false;
      floatVal.status.set(ValueStatus.newPdu);
      floatVal.newPdu = true;
      floatVal.status.set(ValueStatus.idx);
      return true;
    }

    floatVal.status.clear();
    floatVal.newValue = false;
    floatVal.status.set(ValueStatus.newPdu);
    floatVal.newPdu = true;
    if((pduCount - floatVal.pduCount) > 1)
      floatVal.status.set(ValueStatus.lostPdu);
    return false;
  }

  public void getValue(FloatValue floatVal)
  {
    boolean fin;

    fin = getFloatStatus(floatVal);
    if(fin) return;

    float value = floatArray[floatVal.idx];
    if(value != floatVal.value)
    {
      floatVal.value = value;
      floatVal.status.set(ValueStatus.newVal);
      floatVal.newValue = true;
    }
  }

  // -------------------------------------------------------------------------
  // Text
  // -------------------------------------------------------------------------
  //
  public class TextValue extends ReceivedValue
  {
    public String       value;
    
    public TextValue(int inIdx)
    {
      idx = inIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  int textValIdxCnt = 0;

  public TextValue getTextValueInst()
  {
    TextValue retVal = new TextValue(textValIdxCnt);
    textValIdxCnt++;
    return (retVal);
  }

  public boolean getTextStatus(ReceivedValue textVal)
  {
    if(stringArray == null || stringArray.length < textCount)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      textVal.status.set(ValueStatus.empty);
      return true;
    }
    
    if(textCount < 1)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      textVal.status.set(ValueStatus.none);
      return true;
    }
    
    if(textVal.pduCount == pduCount)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      return true;
    }

    if(textVal.idx >= stringArray.length)
    {
      textVal.status.clear();
      textVal.newValue = false;
      textVal.status.set(ValueStatus.newPdu);
      textVal.newPdu = true;
      textVal.status.set(ValueStatus.idx);
      return true;
    }
    
    textVal.status.clear();
    textVal.status.set(ValueStatus.newPdu);
    textVal.newPdu = true;
    textVal.newValue = false;
    if((pduCount - textVal.pduCount) > 1)
      textVal.status.set(ValueStatus.lostPdu);
    return false;    
  }
  
  public void getValue(TextValue textVal)
  {
    boolean fin;
    
    fin = getTextStatus(textVal);
    if(fin) return;
    
    String value = stringArray[textVal.idx];
    if(!value.equals(textVal.value))
    {
      textVal.value = value;
      textVal.status.set(ValueStatus.newVal);
      textVal.newValue = true;
    }
  }

  // -------------------------------------------------------------------------
  // Empfänger für die Rundruftelegramme
  // -------------------------------------------------------------------------
  //
  public class LocalReceiver extends Thread
  {
    byte[]          ipMask;
    DatagramChannel channel;
    byte[]          receivedData;
    ByteBuffer      receiveBuffer;
    String[]        pduHeader;
    String[]        pduElements;
    
    public LocalReceiver(byte[] ipBytes, DatagramChannel inChannel)
    {
      channel       = inChannel;
      receivedData  = new byte[1500];
      receiveBuffer = ByteBuffer.wrap(receivedData);
      ipMask        = ipBytes;
    }
    
    public boolean doRun = true;
    
    public void run()
    {
      InetSocketAddress recAdr;
      int               waitMillisec;
      
      while(doRun)
      {
        receiveBuffer.clear();
        try
        {
          recAdr = (InetSocketAddress) channel.receive(receiveBuffer);
          waitMillisec = 1;
        }
        catch (Exception exc)
        {
          recAdr = null;
          waitMillisec = 10;
        }
        
        if(recAdr == null)
        {
          try
          {
          Thread.sleep(waitMillisec);
          continue;
          }
          catch (Exception exc)
          {
            break;
          }
        }
        
        // Prüfen der Adresse
        //
        InetAddress inetAdr = recAdr.getAddress();
        if(!inetAdr.equals(SocManNet.localHost))
          parsePdu(receiveBuffer);
      }
    }

    private void parsePdu(ByteBuffer recBuf)
    {
      if(portFollowList == null) return;
      
      int nrIn = recBuf.position();
      String pduStr = new String(receivedData, 0, nrIn-3);
      pduElements = pduStr.split(";");
      pduHeader = pduElements[0].split(":");
      portFollowList.enterPDU(pduHeader[SocManNet.pduHdObject], pduHeader, pduElements);
    }
    
  }

  // -------------------------------------------------------------------------
  // Verwaltung der verschiedenen Follower-Instanzen
  // -------------------------------------------------------------------------
  
  // -------------------------------------------------------
  // Port und die Liste angeschlossener Follower
  // -------------------------------------------------------
  //
  public class PortFollow
  {
    // -----------------------------------------------------
    // commObject (Name) und zugeh�rige Follower-Instanz
    // -----------------------------------------------------
    //
    public class FollowElement
    {
      String    commObject;
      Follower  follower;
    }
    
    public int port;
    public List<FollowElement> commObjectList;
    
    public PortFollow(int inPort)
    {
      port = inPort;
      commObjectList = new ArrayList<FollowElement>();
    }
    
    public void add(String commObject, Follower follower)
    {
      FollowElement fe = new FollowElement();
      fe.commObject = commObject;
      fe.follower = follower;
      commObjectList.add(fe);
    }
    
    public FollowElement getElement(String commObject)
    {
      FollowElement followElement;
      int idxEnd = commObjectList.size();
      
      for(int idx = 0; idx < idxEnd; idx++)
      {
        followElement = commObjectList.get(idx);
        if(followElement.commObject.equals(commObject))
          return(followElement);
      }
      return(null);
    }
  }
  
  // -------------------------------------------------------
  // Liste der Ports mit ihren Followern
  // -------------------------------------------------------
  //
  public class PortFollowList
  {
    List<PortFollow> itemList;
    
    public PortFollowList()
    {
      itemList = new ArrayList<PortFollow>();
    }
    
    public int size()
    {
      return(itemList.size());
    }
    
    public void add(PortFollow portFollow)
    {
      itemList.add(portFollow);
    }
    
    public PortFollow getPortFollow(int port)
    {
      PortFollow portFollow;
      
      int idxEnd = itemList.size();
      
      for(int idx = 0; idx < idxEnd; idx++)
      {
        portFollow = itemList.get(idx);
        if(portFollow.port == port)
          return(portFollow);
      }
      return(null);
    }
    
    public void enterPDU(String commObject, String[] header, String[] elements)
    {
      PortFollow    portFollow;
      FollowElement followElement;
      
      int idxEnd = itemList.size();
      
      for(int idx = 0; idx < idxEnd; idx++)
      {
        portFollow = itemList.get(idx);
        followElement = portFollow.getElement(commObject);
        if(followElement != null)
        {
          if(followElement.follower.enabled)
            followElement.follower.handleInput(header, elements);
        }
      }
    }
  }
  
}

