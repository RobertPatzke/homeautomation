package hsh.mplab.smntestclient;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

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
    timerInit();
  }


  // -------------------------------------------------------------------------
  // EditText id = etIpAdr
  // -------------------------------------------------------------------------
  //

  EditText        etIpAdr;
  String          etIpInitText;
  String          etIpInitStr;
  KeyHandler      etIpKeyHandler;

  void initIpAdr()
  {
    etIpAdr = findViewById(R.id.etIpAdr);
    etIpInitStr = getResources().getString(R.string.ipInitText);
    etIpInitText = LineFile.getString("IpAdr01.txt",etIpInitStr);
    etIpAdr.setText(etIpInitText);
    etIpKeyHandler = new KeyHandler(1);
    etIpAdr.setOnKeyListener(etIpKeyHandler);
  }

  public void etIpAdr_Click(View view)
  {
    if(etIpInitText.equals(etIpInitStr))
      etIpAdr.setText("");
  }


  // -------------------------------------------------------------------------
  // State Machine
  // -------------------------------------------------------------------------
  // Ok, it is not necessary to have a state machine with any simple program,
  // but I got so much familiar with this kind of programming that I feel
  // somehow helpless with other concepts.
  // (I have to create a snippet for it, because on x times I just copy the
  // code from other projects, here from devices/android/baseDevice)
  //

  // -------------------------------------------------------------------------
  // Initialisation of a timer for running a state machine (automat)
  // -------------------------------------------------------------------------
  //
  Timer     smnTimer;
  TimerTask smnTimerTask;
  int       frequency;

  private void timerInit()
  {
    long smnTimerPeriod = 50;       // repetition time in milliseconds
    long smnTimerStartDelay = 500;  // start delay in milliseconds
                                    // Expecting some time extra for creating
                                    // all graphics we simply wait

    frequency = (int) (1000 / smnTimerPeriod);  // timer frequency is put to a
                                                // global variable for using it
                                                // in any environment

    smnTimerTask = new TimerTask()
    {
      @Override                       // creating a timer task and overriding
      public void run()               // its run method with ower own run method
      {
        stateMachine(frequency);      // cyclic calling our state machine
      }
    };

    smnTimer = new Timer();
    smnTimer.scheduleAtFixedRate(smnTimerTask, smnTimerStartDelay, smnTimerPeriod);
  }

  enum SmState          // Definition of possible internal states
  {
    Init,
    WaitAdress,
    Idle
  }

  // Variables used for the state machine
  //
  SmState   smState = SmState.Init;       // Initialising state machine
  int       globSeqCounter = 0;           // Counter for second interval
  int       waitCounter = 0;              // Counter for delays
  String    infoMsg;                      // Holding Messages
  boolean   oneShot;                      // Control single actions in loops
  int       stateLoopCounter = 0;         // Counting the ticks in state

  boolean   ipAddressValid = false;


  void stateMachine(int inFreq)
  {
    // -----------------------------------------------------------------------
    // state independent activities
    // -----------------------------------------------------------------------
    //

    stateLoopCounter++;     // simply counting the enters

    // do this independent from state every half second
    //
    globSeqCounter++;
    if (globSeqCounter >= inFreq / 2)
    {
      globSeqCounter = 0;

    }

    // -----------------------------------------------------------------------
    // state dependent activities
    // -----------------------------------------------------------------------
    //

    switch (smState)
    {
      // ---------------------------------------------------------------------
      case Init:                            // Initialisation
        // ---------------------------------------------------------------------
        // Initialising our application variables
        smState = SmState.WaitAdress;
        break;


      // ---------------------------------------------------------------------
      case WaitAdress:                      // Waiting for a valid Ip-Address
        // ---------------------------------------------------------------------
        //
        if (!ipAddressValid)
          break;
        smState = SmState.Idle;
        break;

      // ---------------------------------------------------------------------
      case Idle:
        // ---------------------------------------------------------------------
        break;

    }

  }

}

