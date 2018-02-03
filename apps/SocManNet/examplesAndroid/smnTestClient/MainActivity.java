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
    //timerInit();
  }


  int     defaultPort   = 4001;
  String  ipAdrFileName = "IpAdr01.txt";

  // -------------------------------------------------------------------------
  // Handling IP address input:  EditText id = etIpAdr, Button id = butIpAdr
  //           and status view:  TextView id = tvStatus
  // -------------------------------------------------------------------------
  //

  TextView        tvStatus;
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
      butIpAdr.setTextColor(Color.GRAY);
    }
    else
    {
      tvStatus.setText(R.string.ipInitError);
    }
  }

  // -------------------------------------------------------------------------
  // Testing Client
  // -------------------------------------------------------------------------

  Button  butSmnClient;

  void initClient()
  {

  }

  public void butSmnClient_Click(View view)
  {
  }



}

