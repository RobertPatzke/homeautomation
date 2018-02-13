package hsh.mplab.socmannet;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Created by robertadmin on 04.02.18.
 */

public class SmnClient
{
  public enum ConnStatus
  {
    Undefined,
    TestConnection,
    ComServerInit,
    ComServerReady,
    ComServerBusy,
    WaitBeforeNextTry,
    UnexpectedError,
    ThreadClosed
  }

  public interface Event
  {
    void putStatus(ConnStatus status);
    void putErrorMsg(String errMsg);
    void putInfoMsg(String infoMsg);
  }

  Event event;

  // -----------------------------------------------------------------------
  // Subclass WatchServer is the thread for controlling the connection
  // -----------------------------------------------------------------------

  WatchServer watchServer;

  private class WatchServer extends Thread
  {
    Socket socket;
    InetAddress ipAdr;
    int         port;
    boolean     threadRuns;
    ConnStatus  connStatus;
    int         stopLatencyTime;        // Min time to stop thread
    int         waitAvailabilityTime;   // Wait time after no availability
    int         waitIoErrorTime;        // Wait time after IO error
    int         ioErrorCount;
    int         unexpectedErrCnt;
    int         waitAvailCount;
    int         repCounter;
    boolean     textMode;
    boolean     writeError;
    boolean     writeCheckError;

    BufferedReader inputText;
    BufferedInputStream inputBin;
    PrintWriter outputText;
    PrintStream outputBin;

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

      ioErrorCount      = 0;
      unexpectedErrCnt  = 0;
      waitAvailCount    = 0;
      repCounter        = 3;

      textMode  = true;
    }

    public void run()
    {
      int         waitCounter = 10;
      int         nrRec       = 0;
      ConnStatus  oldStatus   = ConnStatus.Undefined;

      try
      {
        // -----------------------------------------------------------------
        // State machine for handling connection to server
        // -----------------------------------------------------------------
        //
        while (threadRuns)
        {
          if(connStatus != oldStatus)
          {
            oldStatus = connStatus;
            if(event != null)
              event.putStatus(connStatus);
          }

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
                if(event != null)
                  event.putInfoMsg("Server not available " + waitAvailCount);
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
                if(event != null)
                  event.putErrorMsg(exc.getMessage() + " " + ioErrorCount);
                break;
              }
              catch (Exception exc)
              {
                if(event != null)
                  event.putErrorMsg(exc.getMessage());
                socket = null;
                connStatus = ConnStatus.UnexpectedError;
                unexpectedErrCnt++;
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

            case UnexpectedError:
              // This state has to be analyzed via debugger
              Thread.sleep(1000);
              break;

            case ThreadClosed:
              // This status will never be reached, because ist is set with
              // returning from run(). It is implemented to avoid warnings.
              break;
          }
        } // end of while()
        connStatus = ConnStatus.ThreadClosed;
        if(event != null)
          event.putStatus(connStatus);
      }
      catch (Exception exc)
      {
        connStatus = ConnStatus.ThreadClosed;
        if(event != null)
          event.putErrorMsg(exc.getMessage());
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

  public void setEvent(Event inEvent)
  {
    event = inEvent;
  }

  public void start()
  {
    if(watchServer != null)
      watchServer.start();
  }

  public void close()
  {
    watchServer.threadRuns = false;
  }

  static public String connectErrorMsg;

  static public SmnClient connectServer(String ipAdrStr, int port, Event inEvent)
  {
    InetAddress ipAdr;

    try
    {
      ipAdr = InetAddress.getByName(ipAdrStr);
    }
    catch (Exception exc)
    {
      connectErrorMsg = exc.getMessage();
      return (null);
    }
    SmnClient client = new SmnClient(ipAdr, port);
    client.setEvent(inEvent);
    client.start();
    return client;
  }



} // end class smnClient

