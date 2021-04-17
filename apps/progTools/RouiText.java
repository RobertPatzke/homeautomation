// ---------------------------------------------------------------------------
// This module is for sending simple messages to a TextView from threads
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

public class RouiText
{
  Activity  activity;
  TextView  textView;
  String    outMsg;

  // ---------------------------------------------------------------------------
  // Initialisation
  // ---------------------------------------------------------------------------

  public RouiText(Activity parent, TextView tv)
  {
    activity = parent;
    textView = tv;
  }

  // ---------------------------------------------------------------------------
  // Methods
  // ---------------------------------------------------------------------------

  public void print(String msg)
  {
    outMsg  = msg;   // use global reference for the message

    activity.runOnUiThread
        (
            new Runnable()
            {
              @Override
              public void run()   // will be called by UI-Thread
              {
                if(textView == null) return;
                textView.setText(outMsg);
              }
            }
        );
  }


}
