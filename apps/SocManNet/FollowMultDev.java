package hsh.mplab.socmannet;

import hsh.mplab.socmannet.FollowMultDev.PortFollowMult.FollowElement;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;

import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;

import static android.os.SystemClock.elapsedRealtime;

public class FollowMultDev
{
  static public PortFollowList    portFollowList;
  static public boolean           monitorMode;

  DeviceList        deviceList;
  MonTwitterList    monTwitterList;

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
  public FollowMultDev()
  {
    init(defaultPort, defaultObject);
  }

  public FollowMultDev(int port)
  {
    init(port, defaultObject);
  }

  public FollowMultDev(String commObject)
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
    deviceList    = new DeviceList();

    if(portFollowList != null)
    {
      PortFollowMult portFollow = portFollowList.getPortFollow(port);
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

    PortFollowMult portFollow = new PortFollowMult(port);
    portFollow.add(commObject, this);
    portFollowList.add(portFollow);
    recBroadcast = new LocalReceiver(broadcastAdr.getAddress(),localChannel);
    recBroadcast.start();
  }

  public class MonTwitter
  {
    public  String    name;
    List<DeviceDHA>   devList;

    public MonTwitter()
    {
      devList = new ArrayList<>();
    }

    public void add(DeviceDHA dev)
    {
      int listCnt = devList.size();

      if(listCnt > 0)
      {
        for (int i = 0; i < listCnt; i++)
        {
          if (devList.get(i) == dev) return;
        }
      }
      devList.add(dev);
    }
  }

  public class MonTwitterList
  {
    public  int       idx;
    MonTwitter        monTwitter;
    List<MonTwitter>  itemList;

    public MonTwitterList()
    {
      itemList = new ArrayList<>();
    }

    public void add(DeviceDHA devDHA)
    {
      int listCnt = itemList.size();

      if(listCnt > 0)
      {
        for (int i = 0; i < listCnt; i++)
        {
          monTwitter = itemList.get(i);
          if(monTwitter.name.equals(devDHA.twitterName))
          {
            monTwitter.add(devDHA);
            return;
          }
        }
      }

      monTwitter = new MonTwitter();
      monTwitter.name = devDHA.twitterName;
      itemList.add(monTwitter);
    }

    public int size()
    {
      return(itemList.size());
    }
  }

  // -------------------------------------------------------------------------
  // Nur Monitor-Mode: Zugriff auf die Twitter-Liste
  // -------------------------------------------------------------------------
  //

  public int twitterCount()
  {
    if(!monitorMode) return(0);
    return(monTwitterList.size());
  }

  // -------------------------------------------------------------------------
  // Einzelbehandlung verschiedener Geräte mit demselben Twitter
  // -------------------------------------------------------------------------
  //

  public class DeviceDHA
  {
    int idx;

    long        recTime;
    long        lastRecTime;
    int         devLagCount;

    public int      userInt;
    public long     userLong;

    public String   macAdrStr;
    public String   ipAdrStr;

    public  int     lastPduCount;
    public  int     oldPduCount;
    public  int     pduCount;
    public  int     lostPduCount;
    public  int     burstPduCount;

    public  int     intCount;
    public  int     floatCount;
    public  int     textCount;

    public int      applicationKey;
    public int      deviceKey;
    public int      deviceState;
    public String   deviceName;
    public String   twitterName;    // only relevant in monitor mode

    public int      posX;
    public int      posY;
    public int      posZ;
    public int      baseState;
    public int      baseMode;

    int[]       intArray;
    float[]     floatArray;
    String[]    stringArray;
  }

  public class DeviceList
  {
    List<DeviceDHA>  itemList;

    public DeviceList()
    {
      itemList = new ArrayList<>();
    }

    public DeviceDHA getDevice(int devKey)
    {
      DeviceDHA dev;

      if(itemList.isEmpty())
      {
        dev = new DeviceDHA();
        dev.deviceKey = devKey;
        itemList.add(dev);
        return (dev);
      }

      int nrOfItems = itemList.size();

      for (int i = 0; i < nrOfItems; i++)
      {
        dev = itemList.get(i);
        if(dev.deviceKey == devKey)
          return(dev);
      }

      dev = new DeviceDHA();
      dev.deviceKey = devKey;
      itemList.add(dev);
      return(dev);
    }

    public DeviceDHA item(int idx)
    {
      if(idx < 0 || idx >= itemList.size())
        return(null);
      return(itemList.get(idx));
    }

    public int count()
    {
      return itemList.size();
    }
  }

  // -------------------------------------------------------------------------
  // Zugriffe auf die Geräteliste
  // -------------------------------------------------------------------------

  public int deviceCount()
  {
    return deviceList.count();
  }

  public long  deviceInputLag(int devIdx)
  {
    return (elapsedRealtime() - deviceList.item(devIdx).recTime);
  }

  public int devInputLagCount(int devIdx, long toVal)
  {
    int devLagVal;
    devLagVal = deviceList.item(devIdx).devLagCount;
    long recVal = deviceList.item(devIdx).recTime;
    long distVal = elapsedRealtime() - recVal;
    if(distVal < toVal) return(devLagVal);
    if(deviceList.item(devIdx).lastRecTime == recVal) return(devLagVal);
    deviceList.item(devIdx).lastRecTime = recVal;
    devLagVal++;
    deviceList.item(devIdx).devLagCount = devLagVal;
    return(devLagVal);
  }

  public DeviceDHA  getDevice(int devIdx)
  {
    return(deviceList.item(devIdx));
  }

  public void clearErrors()
  {
    for(int i = 0; i < deviceList.count(); i++)
    {
      DeviceDHA dev = deviceList.item(i);
      dev.lostPduCount = 0;
    }
  }

  // -------------------------------------------------------------------------
  // Verarbeitung des eingegangenen Telegramms
  // -------------------------------------------------------------------------
  //

  public void handleInput(String[] header, String[] elements)
  {
    int   intIdx, floatIdx, textIdx, elementIdx, deviceKey;

    deviceKey       = Integer.parseInt(elements[SocManNet.pduElDevKey]);
    DeviceDHA dev   = deviceList.getDevice(deviceKey);

    dev.recTime     = elapsedRealtime();
    dev.macAdrStr   = new String(header[SocManNet.pduHdMac]);
    dev.ipAdrStr    = new String(header[SocManNet.pduHdIp]);
    if(monitorMode)
    {
      dev.twitterName = new String(header[SocManNet.pduHdObject]);
      if(monTwitterList == null)
        monTwitterList = new MonTwitterList();
      monTwitterList.add(dev);
    }

    dev.lastPduCount  = dev.pduCount;
    dev.pduCount      = Integer.parseInt(header[SocManNet.pduHdCount]);

    if(dev.oldPduCount == dev.pduCount)
    {
      dev.burstPduCount++;
      return;
    }

    dev.oldPduCount = dev.pduCount;

    if(dev.lastPduCount != 0)
    {
      if (dev.pduCount < dev.lastPduCount)
      {
        dev.lastPduCount = 0;
      }
      dev.lostPduCount += dev.pduCount - dev.lastPduCount - 1;
    }

    dev.intCount    = Integer.parseInt(elements[SocManNet.pduElTwNrInt]);
    dev.floatCount  = Integer.parseInt(elements[SocManNet.pduElTwNrFloat]);
    dev.textCount   = Integer.parseInt(elements[SocManNet.pduElTwNrText]);

    dev.applicationKey  = Integer.parseInt(elements[SocManNet.pduElAppKey]);
    dev.deviceState     = Integer.parseInt(elements[SocManNet.pduElDevState]);
    dev.deviceName      = elements[SocManNet.pduElDevName];

    dev.posX        = Integer.parseInt(elements[SocManNet.pduElPosX]);
    dev.posY        = Integer.parseInt(elements[SocManNet.pduElPosY]);
    dev.posZ        = Integer.parseInt(elements[SocManNet.pduElPosZ]);

    dev.baseState   = Integer.parseInt(elements[SocManNet.pduElState]);
    dev.baseMode    = Integer.parseInt(elements[SocManNet.pduElMode]);

    elementIdx = SocManNet.pduElTwValue;

    if((elements.length - elementIdx) < (dev.intCount + dev.floatCount + dev.textCount))
      return;

    if(dev.intCount > 0)
    {
      if(dev.intArray == null || dev.intArray.length < dev.intCount)
        dev.intArray = new int[dev.intCount];

      for(intIdx = 0; intIdx < dev.intCount; intIdx++)
      {
        dev.intArray[intIdx] = Integer.parseInt(elements[elementIdx]);
        elementIdx++;
      }
    }


    if(dev.floatCount > 0)
    {
      if(dev.floatArray == null || dev.floatArray.length < dev.floatCount)
        dev.floatArray = new float[dev.floatCount];

      for(floatIdx = 0; floatIdx < dev.floatCount; floatIdx++)
      {
        dev.floatArray[floatIdx] = Float.parseFloat(elements[elementIdx]);
        elementIdx++;
      }
    }

    if(dev.textCount > 0)
    {
      if(dev.stringArray == null || dev.stringArray.length < dev.textCount)
        dev.stringArray = new String[dev.textCount];

      for(textIdx = 0; textIdx < dev.textCount; textIdx++)
      {
        dev.stringArray[textIdx] = elements[elementIdx];
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
    public int          valIdx;
    public int          devIdx;
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
    public int                value;
    public int                oldValue = Integer.MAX_VALUE;
    public DeviceDHA  device;

    public IntegerValue(int inValIdx, int inDevIdx)
    {
      valIdx = inValIdx;
      devIdx = inDevIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  public IntegerValue getIntegerValueInst(int valIdx, int devIdx)
  {
    IntegerValue integerValue = new IntegerValue(valIdx, devIdx);
    return(integerValue);
  }

  public boolean getIntStatus(ReceivedValue intVal, DeviceDHA dev)
  {
    if(dev == null) return true;

    if(dev.intArray == null || dev.intArray.length < dev.intCount)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      intVal.status.set(ValueStatus.empty);
      return true;
    }

    if(dev.intCount < 1)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      intVal.status.set(ValueStatus.none);
      return true;
    }

    if(intVal.pduCount == dev.pduCount)
    {
      intVal.status.clear();
      intVal.newPdu = false;
      intVal.newValue = false;
      return true;
    }

    if(intVal.valIdx >= dev.intArray.length)
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
    if((dev.pduCount - intVal.pduCount) > 1)
      intVal.status.set(ValueStatus.lostPdu);
    return false;
  }

  public boolean getIntStatus(ReceivedValue intVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);

    return getIntStatus(intVal, dev);
  }


  public void getValue(IntegerValue intVal, DeviceDHA dev)
  {
    boolean fin;

    fin = getIntStatus(intVal, dev);
    if(fin) return;

    int value = dev.intArray[intVal.valIdx];
    if(value != intVal.value)
    {
      intVal.value = value;
      intVal.status.set(ValueStatus.newVal);
      intVal.newValue = true;
    }
  }

  public void getValue(IntegerValue intVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);
    getValue(intVal, dev);
  }

  public class IntegerValueList
  {
    public boolean      anyNewVal;
    public boolean      anyNewPdu;
    int                 intIdx;
    List<IntegerValue>  itemList;

    public IntegerValueList(int inIdx)
    {
      itemList = new ArrayList<IntegerValue>();
      intIdx = inIdx;
    }

    public IntegerValue item(int devIdx)
    {
      if(devIdx < itemList.size())
        return itemList.get(devIdx);

      if(devIdx > itemList.size())
        return(null);

      IntegerValue iVal = new IntegerValue(intIdx, devIdx);
      iVal.device = deviceList.item(devIdx);
      itemList.add(iVal);
      return iVal;
    }
  }

  public IntegerValueList getIntegerValueList(int idx)
  {
    IntegerValueList intValueList = new IntegerValueList(idx);
    return(intValueList);
  }

  public void getValue(IntegerValueList intValList)
  {
    intValList.anyNewPdu = false;
    intValList.anyNewVal = false;

    int nrOfDev = deviceList.count();

    for(int i = 0; i < nrOfDev; i++)
    {
      IntegerValue iVal = intValList.item(i);
      if(iVal == null) break;

      DeviceDHA dev = iVal.device;
      getValue(iVal, dev);
      if(iVal.newPdu) intValList.anyNewPdu = true;
      if(iVal.newValue) intValList.anyNewVal = true;
    }
  }

  // -------------------------------------------------------------------------
  // Float
  // -------------------------------------------------------------------------
  //
  public class FloatValue extends ReceivedValue
  {
    public float      value;
    public float      oldValue = Float.MAX_VALUE;
    public DeviceDHA  device;

    public FloatValue(int inValIdx, int inDevIdx)
    {
      valIdx = inValIdx;
      devIdx = inDevIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  public FloatValue getFloatValueInst(int valIdx, int devIdx)
  {
    FloatValue floatValue = new FloatValue(valIdx, devIdx);
    return(floatValue);
  }

  public boolean getFloatStatus(ReceivedValue floatVal, DeviceDHA dev)
  {
    if(dev.floatArray == null || dev.floatArray.length < dev.floatCount)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      floatVal.status.set(ValueStatus.empty);
      return true;
    }

    if(dev.floatCount < 1)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      floatVal.status.set(ValueStatus.none);
      return true;
    }

    if(floatVal.pduCount == dev.pduCount)
    {
      floatVal.status.clear();
      floatVal.newPdu = false;
      floatVal.newValue = false;
      return true;
    }

    if(floatVal.valIdx >= dev.floatArray.length)
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
    if((dev.pduCount - floatVal.pduCount) > 1)
      floatVal.status.set(ValueStatus.lostPdu);
    return false;
  }

  public boolean getFloatStatus(ReceivedValue floatVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);
    return getFloatStatus(floatVal, dev);
  }

  public void getValue(FloatValue floatVal, DeviceDHA dev)
  {
    boolean fin;

    fin = getFloatStatus(floatVal, dev);
    if(fin) return;

    float value = dev.floatArray[floatVal.valIdx];
    if(value != floatVal.value)
    {
      floatVal.value = value;
      floatVal.status.set(ValueStatus.newVal);
      floatVal.newValue = true;
    }
  }

  public void getValue(FloatValue floatVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);
    getValue(floatVal, dev);
  }

  public class FloatValueList
  {
    public boolean    anyNewVal;
    public boolean    anyNewPdu;
    int               intIdx;
    List<FloatValue>  itemList;

    public FloatValueList(int inIdx)
    {
      itemList = new ArrayList<>();
      intIdx = inIdx;
    }

    public FloatValue item(int devIdx)
    {
      if(devIdx < itemList.size())
        return itemList.get(devIdx);

      if(devIdx > itemList.size())
        return(null);

      FloatValue fVal = new FloatValue(intIdx, devIdx);
      fVal.device = deviceList.item(devIdx);
      itemList.add(fVal);
      return fVal;
    }

  }

  public FloatValueList getFloatValueList(int idx)
  {
    FloatValueList floatValueList = new FloatValueList(idx);
    return(floatValueList);
  }

  public void getValue(FloatValueList fltValList)
  {
    fltValList.anyNewPdu = false;
    fltValList.anyNewVal = false;

    int nrOfDev = deviceList.count();

    for(int i = 0; i < nrOfDev; i++)
    {
      FloatValue fVal = fltValList.item(i);
      if(fVal == null) break;

      DeviceDHA dev = fVal.device;
      getValue(fVal, dev);
      if(fVal.newPdu) fltValList.anyNewPdu = true;
      if(fVal.newValue) fltValList.anyNewVal = true;
    }
  }


  // -------------------------------------------------------------------------
  // Text
  // -------------------------------------------------------------------------
  //
  public class TextValue extends ReceivedValue
  {
    public String             value;
    public DeviceDHA  device;

    public TextValue(int inValIdx, int inDevIdx)
    {
      valIdx = inValIdx;
      devIdx = inDevIdx;
      status = new BitSet(ValueStatus.length);
    }
  }

  public TextValue getTextValueInst(int valIdx, int devIdx)
  {
    TextValue retVal = new TextValue(valIdx, devIdx);
    return (retVal);
  }

  public boolean getTextStatus(ReceivedValue textVal, DeviceDHA dev)
  {
    if(dev.stringArray == null || dev.stringArray.length < dev.textCount)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      textVal.status.set(ValueStatus.empty);
      return true;
    }

    if(dev.textCount < 1)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      textVal.status.set(ValueStatus.none);
      return true;
    }

    if(textVal.pduCount == dev.pduCount)
    {
      textVal.status.clear();
      textVal.newPdu = false;
      textVal.newValue = false;
      return true;
    }

    if(textVal.valIdx >= dev.stringArray.length)
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
    if((dev.pduCount - textVal.pduCount) > 1)
      textVal.status.set(ValueStatus.lostPdu);
    return false;
  }

  public boolean getTextStatus(ReceivedValue textVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);
    return getTextStatus(textVal, dev);
  }

  public void getValue(TextValue textVal, DeviceDHA dev)
  {
    boolean fin;

    fin = getTextStatus(textVal, dev);
    if(fin) return;

    String value = dev.stringArray[textVal.valIdx];
    if(!value.equals(textVal.value))
    {
      textVal.value = value;
      textVal.status.set(ValueStatus.newVal);
      textVal.newValue = true;
    }
  }

  public void getValue(TextValue textVal, int devIdx)
  {
    DeviceDHA dev = deviceList.item(devIdx);
    getValue(textVal, dev);
  }

  public class TextValueList
  {
    public boolean    anyNewVal;
    public boolean    anyNewPdu;
    int               intIdx;
    List<TextValue>   itemList;

    public TextValueList(int inIdx)
    {
      itemList = new ArrayList<>();
      intIdx = inIdx;
    }

    public TextValue item(int devIdx)
    {
      if(devIdx < itemList.size())
        return itemList.get(devIdx);

      if(devIdx > itemList.size())
        return(null);

      TextValue tVal = new TextValue(intIdx, devIdx);
      tVal.device = deviceList.item(devIdx);
      itemList.add(tVal);
      return tVal;
    }

  }

  public TextValueList getTextValueList(int idx)
  {
    TextValueList txtValueList = new TextValueList(idx);
    return(txtValueList);
  }

  public void getValue(TextValueList txtValList)
  {
    txtValList.anyNewPdu = false;
    txtValList.anyNewVal = false;

    int nrOfDev = deviceList.count();

    for(int i = 0; i < nrOfDev; i++)
    {
      TextValue tVal = txtValList.item(i);
      if(tVal == null) break;

      DeviceDHA dev = tVal.device;
      getValue(tVal, dev);
      if(tVal.newPdu) txtValList.anyNewPdu = true;
      if(tVal.newValue) txtValList.anyNewVal = true;
    }
  }


  // -------------------------------------------------------------------------
  // Empfänger für die Rundruftelegramme
  // -------------------------------------------------------------------------
  //
  public static int allReceiveCounter = 0;
  public int  receiveCounter = 0;

  public static int allParseCounter = 0;
  public int  pduParseCounter = 0;


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

        receiveCounter++;
        allReceiveCounter++;

        // Prüfen der Adresse
        //
        InetAddress inetAdr = recAdr.getAddress();
        if(!inetAdr.equals(SocManNet.localHost))
          parsePdu(receiveBuffer);
      }
    }

    private void parsePdu(ByteBuffer recBuf)
    {
      pduParseCounter++;
      allParseCounter++;

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
  public class PortFollowMult
  {
    // -----------------------------------------------------
    // commObject (Name) und zugehörige Follower-Instanz
    // -----------------------------------------------------
    //
    public class FollowElement
    {
      String          commObject;
      FollowMultDev   follower;
    }

    public int port;
    public List<FollowElement> commObjectList;

    public PortFollowMult(int inPort)
    {
      port = inPort;
      commObjectList = new ArrayList<>();
    }

    public void add(String commObject, FollowMultDev follower)
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
        if(monitorMode)
        {
          if(followElement != null)
            return(followElement);
        }
        else
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
    List<PortFollowMult> itemList;

    public PortFollowList()
    {
      itemList = new ArrayList<>();
    }

    public int size()
    {
      return(itemList.size());
    }

    public void add(PortFollowMult portFollow)
    {
      itemList.add(portFollow);
    }

    public PortFollowMult getPortFollow(int port)
    {
      PortFollowMult portFollow;

      int idxEnd = itemList.size();

      for(int idx = 0; idx < idxEnd; idx++)
      {
        portFollow = itemList.get(idx);
        if(portFollow.port == port)
          return(portFollow);
      }
      return(null);
    }

    public PortFollowMult getPortFollow()
    {
      PortFollowMult portFollow;

      int idxEnd = itemList.size();

      for(int idx = 0; idx < idxEnd; idx++)
      {
        portFollow = itemList.get(idx);
        if(portFollow.port == broadcastPort)
          return(portFollow);
      }
      return(null);
    }


    public void enterPDU(String commObject, String[] header, String[] elements)
    {
      PortFollowMult portFollow;
      FollowElement  followElement;

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
        /*
        else if(monitorMode)
        {
          FollowMultDev tmpFollower = new FollowMultDev(commObject);
          tmpFollower.enabled = true;

          followElement = portFollow.getElement(commObject);
          if(followElement != null)
          {
            if(followElement.follower.enabled)
              followElement.follower.handleInput(header, elements);
          }
        }
        */
      }
    }
  }

}
