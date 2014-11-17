package com.example.test;

import com.example.test.util.SystemUiHider;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * An example full-screen activity that shows and hides the system UI (i.e. status bar and
 * navigation/system bar) with user interaction.
 *
 * @see SystemUiHider
 */
public class FullscreenActivity extends Activity {
  private final AndroidV8 v8 = new AndroidV8(this);

  private static class AndroidV8 extends V8 {
      private final Activity context;
      private final Parser.ViewParser parser;
      
      AndroidV8(Activity context) {
        this.context = context;
        parser = new Parser.ViewParser(this);
      }
      
      @Override
      public void alert(String message) {
        new AlertDialog.Builder(context).setMessage(message).show();
      }

      @Override
      public Element getElementById(String id) {
        final View view = context.findViewById(id.hashCode());
        if (view == null) {
          return null;
        }
        return new Element() {
          @Override
          public void setAttribute(String name, String value) {
            parser.setProperty(view, name, value);
          }

          @Override
          public String getAttribute(String name) {
            return parser.getProperty(view, name);
          }
        };
      }
    }
  
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
       
        String html = "";
        html += "<?xml version='1.0' encoding='utf-8'?>";
        html += "<LinearLayout xmlns:android='http://schemas.android.com/apk/res/android'";
        html += "    android:layout_width='fill_parent' ";
        html += "    android:layout_height='fill_parent' ";
        html += "    android:background='#DC143C' ";
        html += "    android:orientation='vertical' >";
        html += "  <TextView android:id='@+id/heytext'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:text='Hello, I am a TextView 2' />";
        html += "  <TextView android:id='@+id/formattedtext'";
        html += "    android:layout_width='fill_parent'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:text='foo bar'/>";
        html += "  <EditText android:id='@+id/edittext'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:onKey='onKey'";
        html += "    android:text='I am a text editor!' />";
        html += "  <Button android:id='@+id/button2'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:onClick='javascript: function foo() { alert(\"hi dynamic\") };'";
        html += "    android:text='define foo!' />";
        html += "  <Button android:id='@+id/button3'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:onClick='javascript: foo();'";
        html += "    android:text='execute foo!' />";
        html += "  <Button android:id='@+id/button4'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:onClick='bar'";
        html += "    android:text='execute bar!' />";
        html += "  <Button android:id='@+id/button5'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:visibility='visible'";
        html += "    android:onClick='javascript: hide(\"@+id/button5\");'";
        html += "    android:text='hide me!' />";
        html += "  <CheckBox android:id='@+id/checkbox1'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:text='meat'";
        html += "    android:onClick='alert(\"clicked!\")'/>";        
        html += "  <RadioGroup";
        html += "    android:layout_width='fill_parent'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:orientation='vertical'>";
        html += "    <RadioButton android:id='@+id/radio1'";
        html += "      android:layout_width='wrap_content'";
        html += "      android:layout_height='wrap_content'";
        html += "      android:text='pirates'";
        html += "      />";
        html += "    <RadioButton android:id='@+id/radio2'";
        html += "      android:layout_width='wrap_content'";
        html += "      android:layout_height='wrap_content'";
        html += "      android:text='ninjas'";
        html += "      />";
        html += "  </RadioGroup>";
        html += "  <ToggleButton ";
        html += "    android:id='@+id/togglebutton'";
        html += "    android:layout_width='wrap_content'";
        html += "    android:layout_height='wrap_content'";
        html += "    android:textOn='Vibrate on'";
        html += "    android:textOff='Vibrate off'";
        html += "    />";
        html += "  <Script>";
        html += "    function foo() { return 'hi from foo!'; };";
        html += "    function bar() {";
        html += "      var a = document.getElementById('@+id/heytext');";
        html += "      alert(a.getAttribute('android:text'));";
        html += "      a.setAttribute('android:text', 'setting attributes!');";
        html += "    }";
        html += "    function hide(name) {";
        html += "      var a = document.getElementById(name);";
        html += "      a.setAttribute('android:visibility', 'invisible');";
        html += "    }";
        html += "    function onKey(keyCode) {";
        html += "      alert(keyCode);";
        html += "    }";
        html += "  </Script>";
        html += "  <Script>";
        html += "    alert(foo());";
        html += "  </Script>";
        html += "</LinearLayout>";
        
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setLayoutParams(new LayoutParams(
            LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));


        LinearLayout addressBar = new LinearLayout(this);
        addressBar.setOrientation(LinearLayout.HORIZONTAL);

        EditText editText = new EditText(this);
        LayoutParams lparams = new LayoutParams(
            LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        editText.setLayoutParams(lparams);
        editText.setTextAppearance(this, android.R.attr.textAppearanceLarge);
        editText.setText("http://sgo.to");
        addressBar.addView(editText);
        
        Button goButton = new Button(this);
        goButton.setLayoutParams(lparams);
        goButton.setText("go!");
        addressBar.addView(goButton);
        
        layout.addView(addressBar);
        
        View content = new Parser(this, v8).parse(html);        
        layout.addView(content);
        
        setContentView(layout);
        
        View bar = findViewById("@+id/button2".hashCode());
        View foo = findViewById("@+id/heytext".hashCode());
        View hello = findViewById("@+id/edittext".hashCode());
        View world = findViewById("@+id/formattedtext".hashCode());
    }
}
