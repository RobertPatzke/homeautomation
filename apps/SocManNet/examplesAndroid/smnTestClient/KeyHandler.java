package hsh.mplab.smntestclient;

import android.view.KeyEvent;
import android.view.View;
import android.widget.EditText;

/**
 * Created by robertadmin on 02.02.18.
 */

public class KeyHandler implements View.OnKeyListener
{
  int     modus;
  boolean locEnterDown;
  int     enterDownCount;
  byte[]  ipAdrByte;
  String  ipAdrStr;
  boolean isIpAdr;

  public KeyHandler(int setMode)
  {
    modus           = setMode;
    locEnterDown    = false;
    enterDownCount  = 0;
    ipAdrByte       = new byte[4];
    isIpAdr         = false;
  }

  @Override
  public boolean onKey(View view, int keyCode, KeyEvent keyEvent)
  {
    boolean retv = false;

    int keyAction = keyEvent.getAction();

    if(keyAction == KeyEvent.ACTION_DOWN)
    {
      if(keyCode == KeyEvent.KEYCODE_ENTER)
      {
        locEnterDown = true;
        enterDownCount++;
      }
    }

    switch (modus)
    {
      case 1:
        EditText edt = (EditText) view;

        if(locEnterDown)
        {
          String input = edt.getText().toString();
          String[] element = input.split("\\.");
          if(element.length != 4)
          {
            isIpAdr = false;
            edt.setText("");
          }
          else
          {
            isIpAdr = true;

            for(int i = 0; i < 4; i++)
            {
              try
              {
                int ival = Integer.parseInt(element[i]);
                if(ival < 0 || ival > 255)
                {
                  isIpAdr = false;
                  break;
                }
                ipAdrByte[i] = (byte) ival;
              }
              catch (NumberFormatException exc)
              {
                isIpAdr = false;
                break;
              }
            }

            if(isIpAdr)
              ipAdrStr = input;
            else
              edt.setText("");
          }
          retv = true;
        }
        break;

      default:
        break;
    }

    return(retv);
  }

  public boolean enterDown()
  {
    if(locEnterDown)
    {
      locEnterDown = false;
      return(true);
    }
    return(false);
  }

  public boolean ipEntered()
  {
    if(isIpAdr)
    {
      isIpAdr = false;
      return(true);
    }
    return(false);
  }


}

