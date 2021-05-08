package hsh.mplab.progtools;

import android.app.Activity;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import hsh.mplab.monitorsmn.R;

/**
 * TODO: document your custom view class.
 */
public class LabSpinner extends FrameLayout
{
  boolean useExtraAdapter = true;

  public  int   infoStatIdxDefault;
  public  int   infoStatIdxOk;
  public  int   infoStatIdxTimeOut;
  public  int   infoStatIdxError;


  static List<LabSpinner>   labSpinnerList = null;
  static int    labSpinnerInstanceCount = 0;


  class DispValue
  {
    String  text;
    int     color;

    public DispValue(String inText, int inColor)
    {
      text = inText;
      color = inColor;
    }
  }

  class ExtraAdapter extends ArrayAdapter
  {
    private Context   context;
    List<DispValue>   list;
    List<String>      chkList;
    TextView          tvTemplate;

    public ExtraAdapter(Context inContext, int inRes, List<DispValue> inList)
    {
      super(inContext, inRes, inList);
      context = inContext;
      list    = inList;
    }

    @Override
    public View getView(int pos, View convView, ViewGroup parent)
    {
      View row = convView;
      if(row == null)
      {
        //LayoutInflater li = activity.getLayoutInflater();
        //row = li.inflate(R.layout.spinner_item, parent, false);
        row = new TextView(context);
      }

      DispValue item = list.get(pos);

      if(item != null)
      {
        TextView tv = (TextView) row;
        tv.setTextColor(item.color);
        tv.setBackgroundColor(spinBgColor);
        tv.setTextSize(spinTextSize);
        tv.setPadding((int) spinTextPadding, (int) spinTextPadding,
                      (int) spinTextPadding, (int) spinTextPadding);
        tv.setText(item.text);
      }
      return(row);
    }

    @Override
    public View getDropDownView(int pos, View convView, ViewGroup parent)
    {
      View row = convView;
      if(row == null)
      {
        //LayoutInflater li = activity.getLayoutInflater();
        //row = li.inflate(R.layout.spinner_item, parent, false);
        row = new TextView(context);
      }

      DispValue item = list.get(pos);

      if(item != null)
      {
        TextView tv = (TextView) row;
        tv.setTextColor(item.color);
        tv.setBackgroundColor(itemBgColor);
        tv.setTextSize(itemTextSize);
        tv.setPadding((int) itemTextPadding, (int) itemTextPadding,
                      (int) itemTextPadding, (int) itemTextPadding);
        tv.setText(item.text);
      }
      return(row);
    }

  }

  private Drawable mExampleDrawable;

  private float             dpRatio;

  private Activity          activity;
  private Spinner           spinner;
  private AdapterView.OnItemSelectedListener itemSelectedListener;
  private TextView          selectedView;
  private int               selectedPos;
  static  int               lastSelectedPos;
  private boolean           markSelection;
  static  boolean           markAnySelected;

  private ExtraAdapter      valArray;
  private ArrayAdapter      strArray;
  private TextView          label;
  private View              view;
  private List<DispValue>   valList = new ArrayList<DispValue>();
  private List<String>      strList = new ArrayList<String>();

  private int[]             statValueArray;
  private int[]             statColorArray;
  private String[]          statStringArray;
  private int               resId;

  private float             labSize;

  private float             spinTextSize;
  private float             spinTextPadding;
  private int               spinBgColor;

  private float             itemTextSize;
  private float             itemTextPadding;
  private int               itemBgColor;

  public LabSpinner(Context context)
  {
    super(context);
    init(null, 0);
  }

  public LabSpinner(Context context, AttributeSet attrs)
  {
    super(context, attrs);
    init(attrs, 0);

  }

  public LabSpinner(Context context, AttributeSet attrs, int defStyle)
  {
    super(context, attrs, defStyle);
    init(attrs, defStyle);
  }

  // ***********************************************************************
  private void init(AttributeSet attrs, int defStyle)
  {
    if(labSpinnerInstanceCount == 0)
      labSpinnerList = new ArrayList<LabSpinner>();
    labSpinnerInstanceCount++;

    activity = getActivity();

    // Load attributes
    final TypedArray a = getContext().obtainStyledAttributes(
        attrs, R.styleable.LabSpinner, defStyle, 0);

    dpRatio = getContext().getResources().getDisplayMetrics().density;

    if (a.hasValue(R.styleable.LabSpinner_exampleDrawable))
    {
      mExampleDrawable = a.getDrawable(
          R.styleable.LabSpinner_exampleDrawable);
      mExampleDrawable.setCallback(this);
    }

    resId = a.getResourceId(R.styleable.LabSpinner_LabSpinStatValues,0);
    statValueArray = a.getResources().getIntArray(resId);

    resId = a.getResourceId(R.styleable.LabSpinner_LabSpinStatColors,0);
    statColorArray = a.getResources().getIntArray(resId);

    resId = a.getResourceId(R.styleable.LabSpinner_LabSpinStatStrings,0);
    statStringArray = a.getResources().getStringArray(resId);

    infoStatIdxDefault = a.getInteger(R.styleable.LabSpinner_LabSpinIdxDefault, 0);
    infoStatIdxOk = a.getInteger(R.styleable.LabSpinner_LabSpinIdxOk, 1);
    infoStatIdxTimeOut = a.getInteger(R.styleable.LabSpinner_LabSpinIdxTimeOut, 2);
    infoStatIdxError = a.getInteger(R.styleable.LabSpinner_LabSpinIdxError, 3);

    spinBgColor = a.getInteger(R.styleable.LabSpinner_SpinBgColor, 0);
    spinTextSize = a.getDimension(R.styleable.LabSpinner_SpinTextSize, 15) / dpRatio;
    spinTextPadding = a.getDimension(R.styleable.LabSpinner_SpinTextPadding, 5) / dpRatio;

    itemBgColor = a.getInteger(R.styleable.LabSpinner_ItemBgColor, 0);
    itemTextSize = a.getDimension(R.styleable.LabSpinner_ItemTextSize, 15) / dpRatio;
    itemTextPadding = a.getDimension(R.styleable.LabSpinner_ItemTextPadding, 5) / dpRatio;

    view    = inflate(getContext(),R.layout.lab_spinner,this);

    // ----------------------------------------------------------------------
    label   = (TextView) view.findViewById(R.id.tvLabSpinner);
    // ----------------------------------------------------------------------
    String str = a.getString(R.styleable.LabSpinner_LabText);
    if(str == null) str = "#####";
    label.setText(str);

    int color = a.getColor(R.styleable.LabSpinner_LabColor, Color.BLACK);
    label.setTextColor(color);

    labSize = a.getDimension(R.styleable.LabSpinner_LabSize, 14);
    label.setTextSize(labSize/dpRatio);

    // -----------------------------------------------------------------------
    spinner = (Spinner) view.findViewById(R.id.spLabSpinner);
    // -----------------------------------------------------------------------
    //float dist = a.getDimension(R.styleable.LabSpinner_LabSpinPos, 0);
    //if(dist < 0.1) dist = 40;
    //MarginLayoutParams mlp = (MarginLayoutParams) spinner.getLayoutParams();
    //mlp.leftMargin = (int) (dist/dpRatio);
    //mlp.leftMargin = (int) dist; Exception
    //spinner.setLayoutParams(mlp);

    if(useExtraAdapter)
    {
      valList.add(new DispValue("Item001", Color.LTGRAY));
      valList.add(new DispValue("Item002", Color.LTGRAY));
      valList.add(new DispValue("Item003", Color.LTGRAY));

      valArray =
          new ExtraAdapter(getContext(), R.layout.spinner_item, valList);
      valArray.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
      spinner.setAdapter(valArray);
    }
    else
    {
      strList.add("Item001");
      strList.add("Item002");
      strList.add("Item003");

      strArray =
          new ArrayAdapter(getContext(), R.layout.spinner_item, strList);
      strArray.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
      spinner.setAdapter(strArray);
    }

    itemSelectedListener =
        new AdapterView.OnItemSelectedListener() {

          @Override
          public void onItemSelected(AdapterView<?> parent, View view, int pos, long id)
          {
            selectedPos = pos;
            lastSelectedPos = pos;
            selectedView = (TextView) view;
            markSelection = true;
            markAnySelected = true;
          }

          @Override
          public void onNothingSelected(AdapterView<?> parent)
          {

          }
        };


    spinner.setOnItemSelectedListener(itemSelectedListener);

    a.recycle();
  }
  // ***********************************************************************


  private Activity getActivity()
  {
    Context context = getContext();
    while (context instanceof ContextWrapper)
    {
      if (context instanceof Activity)
      {
        return (Activity)context;
      }
      context = ((ContextWrapper)context).getBaseContext();
    }
    return null;
  }

  private void notifyDataChange()
  {
    if(activity == null) return;

    activity.runOnUiThread
        (
            new Runnable()
            {
              @Override
              public void run()   // will be called by UI-Thread
              {
                if(useExtraAdapter)
                  valArray.notifyDataSetChanged();
                else
                  strArray.notifyDataSetChanged();
              }
            }
        );
  }

  int   posToSelect;

  private void setSelection(int pos)
  {
    if(activity == null) return;
    posToSelect = pos;

    activity.runOnUiThread
        (
            new Runnable()
            {
              @Override
              public void run()   // will be called by UI-Thread
              {
                spinner.setSelection(posToSelect);
              }
            }
        );
  }


  // ***********************************************************************
  public int clear()
  {
    int nr = valList.size();
    valList.clear();
    return(nr);
  }

  // ***********************************************************************
  public int add(String val, int inColor)
  {
    if(useExtraAdapter)
    {
      DispValue dv = new DispValue(val, inColor);
      valList.add(dv);
      return (valList.size());
    }
    else
    {
      strList.add(val);
      return (strList.size());
    }
  }

  // ***********************************************************************
  public String get(int idx)
  {
    if(useExtraAdapter)
    {
      if (idx >= valList.size())
        return (null);
      return (valList.get(idx).text);
    }
    else
    {
      if (idx >= strList.size())
        return (null);
      return (strList.get(idx));
    }
  }

  // ***********************************************************************
  public int addOrReplace(String val, int idx, int inStatIdx)
  {
    int vls;

    if(useExtraAdapter)
    {
      vls = valList.size();

      if (idx < vls)
      {
        DispValue dv = valList.get(idx);
        if (dv.text.equals(val) && dv.color == statColorArray[inStatIdx])
          return (vls);

        dv.text = val;
        dv.color = statColorArray[inStatIdx];
        valList.set(idx, dv);
        if (spinner.getSelectedItemPosition() == idx)
          notifyDataChange();
        return (vls);
      }

      if (idx == vls)
      {
        DispValue dv = new DispValue(val, statColorArray[inStatIdx]);
        valList.add(dv);
        return (vls + 1);
      }
    }
    else
    {
      vls = strList.size();

      if (idx < vls)
      {
        if(strList.get(idx).equals(val))
          return (vls);

        strList.set(idx, val);
        if (spinner.getSelectedItemPosition() == idx)
          notifyDataChange();
        return (vls);
      }

      if (idx == vls)
      {
        strList.add(val);
        return (vls + 1);
      }

    }
    return(-vls);
  }

  public int addOrReplace(String val, int idx)
  {
    return(addOrReplace(val, idx, infoStatIdxDefault));
  }

  public void addToList()
  {
    if(labSpinnerList.contains(this))
      return;
    labSpinnerList.add(this);
  }

  // ***********************************************************************
  //
  public void clearSelection()
  {
    markSelection = false;
  }

  public boolean newSelection()
  {
    if(!markSelection) return(false);

    markSelection = false;
    return(true);
  }

  public int getNewSelectedPos()
  {
    if(!markSelection) return(-1);
    markSelection = false;
    return(selectedPos);
  }

  static public void clearAnySelected()
  {
    markAnySelected = false;
  }

  static public boolean anySelected()
  {
    if(!markAnySelected) return(false);

    markAnySelected = false;
    return(true);
  }

  static public int getLastSelectedPos()
  {
    return(lastSelectedPos);
  }

  static public int getLastNewSelectedPos()
  {
    if(!markAnySelected) return(-1);
    markAnySelected = false;
    return(lastSelectedPos);
  }

  // ***********************************************************************
  //
  static public void setAllSelections(int pos)
  {
    LabSpinner labSpinner;

    for(int i = 0; i < labSpinnerList.size(); i++)
    {
      labSpinner = labSpinnerList.get(i);
      if(labSpinner.selectedPos != pos)
        labSpinner.setSelection(pos);
    }
  }

}
