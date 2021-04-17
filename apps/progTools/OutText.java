// ---------------------------------------------------------------------------
// This module is for sending (log-) messages to a TextView from threads
// messages may be extended with date and are buffered for fast return
// ---------------------------------------------------------------------------
// Author:    Prof. Dr.-Ing. Robert Patzke (HS Hannover / MFP GmbH)
// Date:      17.04.2021
// Licence:   CC-BY-SA
// ---------------------------------------------------------------------------
// Editors:   (Please add name and date)
//
//

package hsh.mplab.progtools;

import android.app.Activity;
import android.widget.TextView;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import static android.os.SystemClock.elapsedRealtime;

// -------------------------------------------------------------------------
// Using graphical elements invoked by other threads (state machine)
// -------------------------------------------------------------------------
// Graphical elements may not be accessed in other threads than in main.
// Therefore it is necessary to provide a shell for other threads, which
// puts the request of threads onto the queue of the main thread.
//
public class OutText
{
  Activity  activity;
  TextView  textView;
  boolean   stamp;
  long      startRealTime;

  int       nrMsg;
  int       outIdx;

  class RunBuffer
  {
    int     mode;
    String  outMsg;
    boolean busy;

    RunBuffer()
    {
      busy = false;
    }

    void output(String msg, int md)
    {
      busy = true;
      outMsg  = msg;   // use global reference for the message
      mode    = md;

      activity.runOnUiThread
          (
              new Runnable()
              {
                @Override
                public void run()   // will be called by UI-Thread
                {
                  if(textView == null) return;

                  if(mode == 0)
                    textView.setText(outMsg);
                  else if(mode == 1)
                    textView.append(outMsg);

                  busy = false;
                }
              }
          );

    }

  }

  RunBuffer[]   runBuffers;

  // ---------------------------------------------------------------------------
  // Initialisation
  // ---------------------------------------------------------------------------

  public OutText(Activity parent, TextView tv, int inNr)
  {
    activity  = parent;
    textView  = tv;
    if(textView != null)
      textView.setText("");
    stamp = false;
    startRealTime = elapsedRealtime();

    nrMsg   = inNr;
    outIdx  = 0;

    runBuffers = new RunBuffer[inNr];

    for(int i = 0; i < nrMsg; i++)
      runBuffers[i] = new RunBuffer();
  }

  // ---------------------------------------------------------------------------
  // local methods/functions
  // ---------------------------------------------------------------------------

  private void output(String msg, int md, boolean tSt)
  {
    String outMsg;

    for(int i = 0; i < nrMsg; i++)
    {
      if(!runBuffers[i].busy)
      {
        if(tSt)
          outMsg = String.format("%1$,010d ",elapsedRealtime() - startRealTime) + msg;
        else
          outMsg = msg;
        runBuffers[i].output(outMsg,md);
        break;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // public methods/functions
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  public void setStamp(boolean stVal)
  {
    stamp = stVal;
  }

  // ---------------------------------------------------------------------------
  public void replace(String msg)
  {
    output(msg,0, stamp);
  }

  public void replace(String msg, boolean wT)
  {
    output(msg,0, wT);
  }

  // ---------------------------------------------------------------------------
  public void print(String msg)
  {
    output(msg, 1, stamp);
  }

  public void print(String msg, boolean wT)
  {
    output(msg, 1, wT);
  }

  // ---------------------------------------------------------------------------
  public void println(String msg)
  {
    output(msg + "\n", 1, stamp);
  }

  public void println(String msg, boolean wT)
  {
    output(msg + "\n", 1, false);
  }

  // ---------------------------------------------------------------------------
  public void newLine(int nr)
  {
    if(nr <= 1)
    {
      output("\n",1, false);
      return;
    }

    StringBuilder strB = new StringBuilder();

    for(int i = 0; i < nr; i++)
    {
      strB.append("\n");
    }

    output(strB.toString(),1, false);
  }

  // ---------------------------------------------------------------------------
  public void newLine()
  {
    output("\n",1, false);
  }

  // ---------------------------------------------------------------------------
  public void prtDateTime()
  {
    Date currentDateTime = Calendar.getInstance().getTime();
    DateFormat df = new SimpleDateFormat("EEEE, dd.MM.yyyy HH:mm:ss");
    output(df.format(currentDateTime),1, stamp);
  }

  public void prtlnDateTime()
  {
    Date currentDateTime = Calendar.getInstance().getTime();
    DateFormat df = new SimpleDateFormat("EEEE, dd.MM.yyyy HH:mm:ss");
    output(df.format(currentDateTime) + "\n",1, stamp);
  }


}
