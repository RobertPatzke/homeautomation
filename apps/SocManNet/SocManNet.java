package hsh.mplab.socmannet;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;

import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.List;

public class SocManNet
{
  public enum DeviceState
  {
    Init,
    Config,
    Run,
    Prog,
    Maintaining,
    NeedMaint,
    Defect
  }

  static final  public int  pduHdMac        = 1;
  static final  public int  pduHdIp         = 2;
  static final  public int  pduHdObject     = 5;
  static final  public int  pduHdCount      = 7;
  
  static final  public int  pduElAppKey     = 1;
  static final  public int  pduElDevKey     = 2;
  static final  public int  pduElDevState   = 3;
  static final  public int  pduElDevName    = 4;
  static final  public int  pduElTime       = 5;
  static final  public int  pduElPosX       = 6;
  static final  public int  pduElPosY       = 7;
  static final  public int  pduElPosZ       = 8;
  static final  public int  pduElState      = 9;
  static final  public int  pduElMode       = 10;
  
  static final  public int  pduElTwNrInt    = 11;
  static final  public int  pduElTwNrFloat  = 12;
  static final  public int  pduElTwNrText   = 13;
  
  static final  public int  pduElTwValue    = 14;
  
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
      // Auswerten der verfügbaren Internetadressen
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
      
      //hostName        = localHost.getHostName();
      
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
  // Anwenderfunktionen
  // -------------------------------------------------------------------------

  static public int getTinyDeviceId()
  {
    int val = macBytes[0] & 0xFF;
    return(val);
  }

  static public int getSmallDeviceId()
  {
    int high  = (macBytes[1] & 0xFF) << 8;
    int low   = macBytes[0] & 0xFF;
    return(high + low);
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

  static public class SmnClient
  {
    public enum ConnStatus
    {
      TestConnection,
      ComServerInit,
      ComServerReady,
      ComServerBusy,
      WaitBeforeNextTry,
      ThreadClosed
    }

    // -----------------------------------------------------------------------
    // Subclass WatchServer is the thread for controlling the connection
    // -----------------------------------------------------------------------

    WatchServer watchServer;

    private class WatchServer extends Thread
    {
      Socket      socket;
      InetAddress ipAdr;
      int         port;
      boolean     threadRuns;
      ConnStatus  connStatus;
      int         stopLatencyTime;        // Min time to stop thread
      int         waitAvailabilityTime;   // Wait time after no availability
      int         waitIoErrorTime;        // Wait time after IO error
      int         ioErrorCount;
      int         waitAvailCount;
      int         repCounter;
      boolean     textMode;
      boolean     writeError;
      boolean     writeCheckError;

      BufferedReader        inputText;
      BufferedInputStream   inputBin;
      PrintWriter           outputText;
      PrintStream           outputBin;

      char[]      outMessage, inMessage;
      byte[]      outData, inData;
      boolean     doSend;

      WatchServer(InetAddress ipAdr, int port)
      {
        this.ipAdr  = ipAdr;
        this.port   = port;
        connStatus  = ConnStatus.TestConnection;

        stopLatencyTime       = 20;
        waitAvailabilityTime  = 200;
        waitIoErrorTime       = 500;

        ioErrorCount    = 0;
        waitAvailCount  = 0;
        repCounter      = 3;

        textMode  = true;
      }

      public void run()
      {
        int   waitCounter = 10;
        int   nrRec = 0;

        try
        {
          // -----------------------------------------------------------------
          // State machine for handling connection to server
          // -----------------------------------------------------------------
          //
          while (threadRuns)
          {
            switch (connStatus)
            {
              // ---------------------------------------------------------------
              case TestConnection:              // Test server availability
              // ---------------------------------------------------------------
                try
                {
                  socket = new Socket(ipAdr, port);
                }
                catch (UnknownHostException exc)
                {
                  // Server is not available
                  //
                  socket = null;
                  connStatus = ConnStatus.WaitBeforeNextTry;
                  waitCounter = waitAvailabilityTime / stopLatencyTime;
                  waitAvailCount++;
                  break;
                }
                catch (IOException exc)
                {
                  // Something wrong, handling will follow if exception occurs
                  //
                  socket = null;
                  connStatus = ConnStatus.WaitBeforeNextTry;
                  waitCounter = waitIoErrorTime / stopLatencyTime;
                  ioErrorCount++;
                  break;
                }

                connStatus = ConnStatus.ComServerInit;
                break;

              // ---------------------------------------------------------------
              case WaitBeforeNextTry:
              // ---------------------------------------------------------------
                Thread.sleep(stopLatencyTime);
                waitCounter--;
                if (waitCounter <= 0)
                  connStatus = ConnStatus.TestConnection;
                break;

              // ---------------------------------------------------------------
              case ComServerInit:
              // ---------------------------------------------------------------
                if (textMode)
                {
                  inputText = new BufferedReader
                          (
                                  new InputStreamReader(socket.getInputStream())
                          );
                  outputText = new PrintWriter(socket.getOutputStream());
                } else
                {
                  inputBin = new BufferedInputStream(socket.getInputStream());
                  outputBin = new PrintStream(socket.getOutputStream());
                }
                connStatus = ConnStatus.ComServerReady;
                writeError = false;
                writeCheckError = false;
                doSend = false;
                break;


              // ---------------------------------------------------------------
              case ComServerReady:
              // ---------------------------------------------------------------
                if (doSend && !writeError)
                  {
                    connStatus = ConnStatus.ComServerBusy;
                    repCounter = 3;
                    break;
                  }

                try
                {
                  if (textMode)
                  {
                    if (inputText.ready())
                    {
                      nrRec = inputText.read(inMessage);
                    }
                    else
                    {
                      Thread.sleep(0);
                    }
                  }
                  else
                  {
                    nrRec = inputBin.available();
                    if (nrRec > 0)
                    {
                      nrRec = inputBin.read(inData);
                    }
                    else
                    {
                      Thread.sleep(0);
                    }
                  }
                }
                catch (IOException exc)
                {
                  // Do not know, what to do here till now
                  //
                  Thread.sleep(stopLatencyTime);
                  break;
                }
                break;

              // ---------------------------------------------------------------
              case ComServerBusy:
              // ---------------------------------------------------------------
                try
                {
                  if (textMode)
                  {
                    outputText.print(outMessage);
                    if (outputText.checkError())
                    {
                      repCounter--;
                      if (repCounter > 0)
                      {
                        Thread.sleep(stopLatencyTime);
                        break;
                      }
                      else
                      {
                        writeError = true;
                        writeCheckError = true;
                        connStatus = ConnStatus.ComServerReady;
                        break;
                      }
                    }
                  }
                  else
                  {
                    outputBin.write(outData);
                    if (outputBin.checkError())
                    {
                      repCounter--;
                      if (repCounter > 0)
                      {
                        Thread.sleep(stopLatencyTime);
                        break;
                      }
                      else
                      {
                        writeError = true;
                        writeCheckError = true;
                        connStatus = ConnStatus.ComServerReady;
                        break;
                      }
                    }
                  }
                }
                catch (IOException exc)
                {
                  // I hope, this exception is thrown, if the server is not
                  // available anymore. I will test ist later.
                  writeError = true;
                  connStatus = ConnStatus.ComServerReady;
                  break;
                }
                doSend = false;
                break;

              case ThreadClosed:
                // This status will never be reached, because ist is set with
                // returning from run(). It is implemented to avoid warnings.
                break;
            }
          } // end of while()
          connStatus = ConnStatus.ThreadClosed;
        }
        catch (InterruptedException exc)
        {
          // May be later we use the possibility to interrupt the thread directly
          //
          connStatus = ConnStatus.ThreadClosed;
          return;
        }
        catch (IOException exc)
        {
          // At the moment I do not know what to do if this happens
          // I will check the context if it happens to get more information
          //
          connStatus = ConnStatus.ThreadClosed;
          return;
        }
      }
    }

    // -----------------------------------------------------------------------
    // Constructors and initialisations
    // -----------------------------------------------------------------------
    //

    public SmnClient(InetAddress IpAddress, int port)
    {
      watchServer = new WatchServer(IpAddress, port);
      watchServer.threadRuns = true;
      watchServer.start();
    }

    // -----------------------------------------------------------------------
    // User functions (methods)
    // -----------------------------------------------------------------------
    //

    public void setWaitParameters
            (int waitAvail, int waitIoError, int stopLatency)
    {
      watchServer.stopLatencyTime       = stopLatency;
      watchServer.waitAvailabilityTime  = waitAvail;
      watchServer.waitIoErrorTime       = waitIoError;
    }

    public int write(String text)
    {
      if(watchServer.writeError)
        return(-2);

      if(watchServer.doSend)
        return(-1);

      watchServer.outMessage = text.toCharArray();
      watchServer.doSend = true;

      return(0);
    }

  } // end class smnClient

  static public SmnClient connectServer(InetAddress ipAdr, int port)
  {
    SmnClient client = new SmnClient(ipAdr, port);
    return client;
  }

} // end class SocManNet



