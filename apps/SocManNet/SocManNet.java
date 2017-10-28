package hsh.mplab.libnet;

import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;

import java.util.Enumeration;
import java.util.List;

public class SocManNet
{
  static final  public int  pduHdMac        = 1;
  static final  public int  pduHdIp         = 2;
  static final  public int  pduHdObject     = 5;
  static final  public int  pduHdCount      = 7;
  
  static final  public int  pduElDevKey     = 1;
  static final  public int  pduElDevState   = 2;
  static final  public int  pduElDevName    = 3;
  static final  public int  pduElTime       = 4;
  static final  public int  pduElPosX       = 5;
  static final  public int  pduElPosY       = 6;
  static final  public int  pduElPosZ       = 7;
  static final  public int  pduElState      = 8;
  static final  public int  pduElMode       = 9;
  
  static final  public int  pduElTwNrInt    = 10;
  static final  public int  pduElTwNrFloat  = 11;
  static final  public int  pduElTwNrText   = 12;
  
  static final  public int  pduElTwValue    = 13;
  
  static public int        errorCode;
  static public String     errorMsg;
  static public String     resultMsg;
  
  static NetworkInterface  checkedIfc;
  static InetAddress       checkedAdr;
  
  static Enumeration<NetworkInterface> enumInetIfc;
  static Enumeration<InetAddress>      enumInetAdr;
  static List<InterfaceAddress>        listIfcAdr;

  static InetAddress       localHost;
  static InetAddress[]     inetAdrList;
  static InetAddress       broadcastAdr;

  static NetworkInterface  localInterface;
  static String            ifcName;
  static byte[]            macBytes;
  static byte[]            ipBytes;
  static byte[]            bcBytes;
  static String            localIP;
  static String            localMAC;
  static String            hostName;

  static boolean           initDone = false;
  
  static void init(boolean forceCheck)
  {
    if(!forceCheck)
    {
      if(initDone)
        return;
    }
    
    try
    {
      // ---------------------------------------------------------------------
      // Auswerten der verfügbaren Schnittstellen
      // ---------------------------------------------------------------------
      
      enumInetIfc     = NetworkInterface.getNetworkInterfaces();
      localInterface  = null;
      
      while(enumInetIfc.hasMoreElements())
      {
        checkedIfc = enumInetIfc.nextElement();
        if(checkedIfc.isLoopback()) continue;
        if(!checkedIfc.isUp()) continue;
        if(checkedIfc.getName().startsWith("p2p")) continue;
        localInterface = checkedIfc;
        break;
      }
      
      if(localInterface == null)
      {
        errorCode = 1;
        errorMsg = "Kein Netzwerk bereit";
        return;
      }
      
      ifcName         = localInterface.getName();
      macBytes        = localInterface.getHardwareAddress();
      localMAC        = getByteString(macBytes);

      // ---------------------------------------------------------------------
      // Auswerten der verf�gbaren Internetadressen
      // ---------------------------------------------------------------------
           
      enumInetAdr     = localInterface.getInetAddresses();
      localHost       = null;
      
      while(enumInetAdr.hasMoreElements())
      {
        checkedAdr = enumInetAdr.nextElement();
        if(checkedAdr.isAnyLocalAddress()) continue;
        if(checkedAdr.isLinkLocalAddress()) continue;
        if(checkedAdr.isLoopbackAddress()) continue;
        localHost = checkedAdr;
      }
      
      if(localHost == null)
      {
        errorCode = 2;
        errorMsg = "Keine Internetadresse";
        return;
      }
      
      hostName        = localHost.getHostName();
      
      ipBytes         = localHost.getAddress();
      localIP         = localHost.getHostAddress();
      bcBytes         = localHost.getAddress();
      bcBytes[3]      = (byte) 255;
      broadcastAdr    = InetAddress.getByAddress(bcBytes);
      resultMsg       = ifcName + " / " + localIP + " / " + localMAC;
      initDone        = true;
    }
    catch(Exception exc)
    {
      errorMsg = exc.getMessage();
      if(errorMsg == null)
      {
        errorMsg = exc.getCause().toString();
      }
      errorCode = 3;
    }

  }
  
  // -------------------------------------------------------------------------
  // region Einfache Hilfsfunktionen
  // -------------------------------------------------------------------------
  
  static public String getByteString(byte[] byteArray)
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
  

}
