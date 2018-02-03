package hsh.mplab.smntestclient;

import android.app.Activity;
import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
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
  String  ipAdrPortStr;
  int     port;
  boolean isIpAdr;
  boolean portGiven;
  boolean hideKeyboard;
  Context context;

  public KeyHandler(Context userContext, int setMode, boolean doneHide)
  {
    modus           = setMode;
    hideKeyboard    = doneHide;
    context         = userContext;
    clear();
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

        if(!locEnterDown) break;

        EditText edt = (EditText) view;
        String input = edt.getText().toString();
        String[] ipPort  = input.split(":");
        if(ipPort.length == 2)
        {
          try
          {
            port = Integer.parseInt(ipPort[1]);
            portGiven = true;
          }
          catch (NumberFormatException exc)
          {
            portGiven = false;
          }
        }
        String[] element = ipPort[0].split("\\.");
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
          {
            ipAdrStr = ipPort[0];
            if(portGiven)
              ipAdrPortStr = input;
            if(hideKeyboard)
            {
              InputMethodManager imm = (InputMethodManager)
                      context.getSystemService(context.INPUT_METHOD_SERVICE);
              imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
            }
          }
          else
            edt.setText("");
        }
        retv = true;

        break;

      default:
        break;
    }

    return(retv);
  }

  // -------------------------------------------------------------------------
  // User functions (methods) for accessing results
  // -------------------------------------------------------------------------

  public void clear()
  {
    locEnterDown    = false;
    enterDownCount  = 0;
    ipAdrByte       = new byte[4];
    isIpAdr         = false;
    portGiven       = false;
  }

  public boolean enterDown(boolean reset)
  {
    if(locEnterDown)
    {
      if(reset)
        locEnterDown = false;
      return(true);
    }
    return(false);
  }

  public boolean ipEntered(boolean reset)
  {
    if(isIpAdr)
    {
      if(reset)
       isIpAdr = false;
      return(true);
    }
    return(false);
  }

  public boolean portEntered(boolean reset)
  {
    if(portGiven)
    {
      if(reset)
        portGiven = false;
      return(true);
    }
    return(false);
  }

  public String getIpStr()
  {
    if(!isIpAdr)
      return(null);

    return(ipAdrStr);
  }

  public int getPort()
  {
    if(!portGiven)
      return(-1);

    return(port);
  }

  public String getIpPortStr()
  {
    if(portGiven && isIpAdr)
      return(ipAdrPortStr);
    return(null);
  }

}

