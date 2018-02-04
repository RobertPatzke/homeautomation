package hsh.mplab.smntestclient;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

import hsh.mplab.socmannet.SmnClient;
import hsh.mplab.socmannet.SocManNet;

/**
 * Testing the Client of SocManNet
 * Created on February 2nd 2018 by Robert Patzke
 */


public class MainActivity extends AppCompatActivity
{

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    initIpAdr();
    initClient();
    //timerInit();
  }


  int     defaultPort   = 4001;
  String  ipAdrFileName = "IpAdr01.txt";

  int     colButActive;
  int     colButClosed;

  // -------------------------------------------------------------------------
  // Handling IP address input:  EditText id = etIpAdr, Button id = butIpAdr
  //           and status view:  TextView id = tvStatus
  // -------------------------------------------------------------------------
  //

  TextView        tvStatus;
  TextView        tvError;
  TextView        tvInfo;
  Button          butIpAdr;
  EditText        etIpAdr;
  String          etIpInitText;
  String          etIpInitStr;
  KeyHandler      etIpKeyHandler;

  String          serverIpAdrStr;
  int             serverPort;

  void initIpAdr()
  {
    String fileDirectory = getExternalFilesDir(null).toString();
    LineFile.setFilesDir(fileDirectory);

    etIpAdr = findViewById(R.id.etIpAdr);
    etIpInitStr = getResources().getString(R.string.ipInitText);
    etIpInitText = LineFile.getString(ipAdrFileName,etIpInitStr);
    etIpAdr.setText(etIpInitText);
    etIpKeyHandler = new KeyHandler(this,1,true);
    etIpAdr.setOnKeyListener(etIpKeyHandler);

    butIpAdr = findViewById(R.id.butIpAdr);

    tvStatus = findViewById(R.id.tvStatus);
    tvStatus.setText(fileDirectory);

    tvError = findViewById(R.id.tvError);
    tvInfo = findViewById(R.id.tvInfo);

    colButActive = getResources().getColor(R.color.black);
    colButClosed = getResources().getColor(R.color.gray);
  }

  public void etIpAdr_Click(View view)
  {
    if(etIpInitText.equals(etIpInitStr))
      etIpAdr.setText("");
    butIpAdr.setTextColor(Color.BLACK);
  }

  public void butIpAdr_Click(View view)
  {
    String ippStr;

    if(!etIpKeyHandler.ipEntered(false))
      etIpKeyHandler.setUpIpVars(etIpAdr);

    if(etIpKeyHandler.ipEntered(false))
    {
      if(etIpKeyHandler.portEntered(false))
        ippStr = etIpKeyHandler.getIpPortStr();
      else
        ippStr = etIpKeyHandler.getIpStr() + ":" + defaultPort;

      String result = LineFile.putString(ipAdrFileName,ippStr);
      if(result != null)
        tvStatus.setText(result);

      serverIpAdrStr = etIpKeyHandler.getIpStr();
      serverPort = etIpKeyHandler.getPort();

      etIpKeyHandler.clear();
      butIpAdr.setTextColor(colButClosed);
      butSmnClientLeft.setTextColor(colButActive);
    }
    else
    {
      tvError.setText(R.string.ipInitError);
    }
  }

  // -------------------------------------------------------------------------
  // Testing Client
  // -------------------------------------------------------------------------
  //
  SmnClient smnClient01;

  enum TestClientStatus
  {
    unborn,
    initialising,
    waitForServer
  }

  TestClientStatus   testClientStatus = TestClientStatus.unborn;

  Button  butSmnClientLeft;
  Button  butSmnClientRight;

  void initClient()
  {
    butSmnClientLeft = findViewById(R.id.butSmnClientLeft);
    butSmnClientRight = findViewById(R.id.butSmnClientRight);
  }

  public void butSmnClientLeft_Click(View view)
  {
    switch (testClientStatus)
    {
      case unborn:
        tvStatus.setText(R.string.tcsInitialising);
        smnClient01 = SmnClient.connectServer
          (serverIpAdrStr, serverPort, new HandleClientEvents());
        if(smnClient01 != null)
          testClientStatus = TestClientStatus.initialising;
        else
          tvError.setText("ERROR: " + SmnClient.connectErrorMsg);
        break;

      case waitForServer:
        butSmnClientLeft.setText(R.string.smnConnect);
        testClientStatus = TestClientStatus.unborn;
        smnClient01.close();
        break;
    }
  }

  void setButSmnClientLeft(SmnClient.ConnStatus connStat)
  {
    if(testClientStatus == TestClientStatus.initialising)
    {
      if(   connStat == SmnClient.ConnStatus.TestConnection
         || connStat == SmnClient.ConnStatus.WaitBeforeNextTry )
      {
        testClientStatus = TestClientStatus.waitForServer;
        butSmnClientLeft.setText(R.string.smnClose);
      }
    }
  }

  public void butSmnClientRight_Click(View view)
  {
  }

  // -------------------------------------------------------------------------
  // Support for accessing UIThread
  // -------------------------------------------------------------------------
  //
  class HandleClientEvents implements SmnClient.Event
  {
    public void putStatus(final SmnClient.ConnStatus connStat)
    {
      if(tvStatus == null) return;
      final String msg = "ClientStatus = " + connStat;

      Runnable displayStatus = new Runnable()
      {
        @Override
        public void run()
        {
          tvStatus.setText(msg);
          setButSmnClientLeft(connStat);
        }
      };

      runOnUiThread(displayStatus);
    }

    public void putErrorMsg(String errMsg)
    {
      if(tvError == null) return;
      final String msg = "ERROR: " + errMsg;

      Runnable displayError = new Runnable()
      {
        @Override
        public void run()
        {
          tvError.setText(msg);
        }
      };

      runOnUiThread(displayError);
    }

    public void putInfoMsg(String infMsg)
    {
      if(tvInfo == null) return;
      final String msg = infMsg;

      Runnable displayInfo = new Runnable()
      {
        @Override
        public void run()
        {
          tvInfo.setText(msg);
        }
      };

      runOnUiThread(displayInfo);
    }

  }

}

