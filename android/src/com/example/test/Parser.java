package com.example.test;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.ToggleButton;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

/**
 * TODO: Insert description here. (generated by goto)
 */
class Parser {
  private final V8 v8;
  private final Context context;
  
  Parser(Context context, V8 v8) {
    this.v8 = v8;
    this.context = context;
  }
  
  private Document create(String html) throws ParserConfigurationException, SAXException, IOException {
    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    dbf.setNamespaceAware(false);
    dbf.setValidating(false);
    DocumentBuilder db = dbf.newDocumentBuilder();
    return db.parse(new InputSource(new ByteArrayInputStream(html.getBytes("utf-8"))));
  }
  
  static class ViewParser {
    private final V8 v8;
    
    ViewParser(V8 v8) {
      this.v8 = v8;
    }
    
    private interface Accessor {
      String get();
      void set(String value);
    }
    
    public String getProperty(View view, String name) {
      return accessor(view, name).get();
    }

    public void setProperty(View view, String name, String value) {
      accessor(view, name).set(value);
    }
    
    @TargetApi(Build.VERSION_CODES.HONEYCOMB)
    private Accessor accessor(final View view, String name) {
      if ("android:id".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            // TODO(goto): figure out what to do here.
            return null;
          }

          @Override
          public void set(String value) {
            view.setId(value.hashCode());
          }
        };
      } else if ("android:visibility".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            switch (view.getVisibility()) {
              case View.VISIBLE: return "visible";
              case View.INVISIBLE: return "invisible";
              case View.GONE: return "gone";
            }
            return "unknown";
          }

          @Override
          public void set(String value) {
            if ("visible".equals(value)) {
              view.setVisibility(View.VISIBLE);
            } else if ("invisible".equals(value)) {
              view.setVisibility(View.INVISIBLE);
            } else {
              view.setVisibility(View.GONE);                
            }
          }
        };
     }  else if ("android:background".equals(name)) {
       return new Accessor() {
         @Override
         public String get() {
           int color = Color.TRANSPARENT;
           Drawable background = view.getBackground();
           if (background instanceof ColorDrawable)
               color = ((ColorDrawable) background).getColor();
           return "";
         }

         @Override
         public void set(String value) {
           view.setBackgroundColor(Color.parseColor(value));
         }
       };
    } else if ("android:layout_width".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            return null;
          }

          @Override
          public void set(String value) {
            int width = 0;
            if ("wrap_content".equals(value)) {
              width = LayoutParams.WRAP_CONTENT;
            } else if ("fill_parent".equals(value)) {
              width = LayoutParams.MATCH_PARENT;
            }
            
            LayoutParams params = view.getLayoutParams();
            if (params == null) {
              params = new LayoutParams(
                  LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
            }
            LayoutParams result = new LayoutParams(width, params.height);
            view.setLayoutParams(result);
          }
        };
      } else if ("android:layout_height".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            return null;
          }

          @Override
          public void set(String value) {
            int height = 0;
            if ("wrap_content".equals(value)) {
              height = LayoutParams.WRAP_CONTENT;
            } else if ("fill_parent".equals(value)) {
              height = LayoutParams.MATCH_PARENT;
            }
            
            LayoutParams params = view.getLayoutParams();
            if (params == null) {
              params = new LayoutParams(
                  LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
            }
            view.setLayoutParams(new LayoutParams(params.width, height));
          }
        };
      } else if ("android:orientation".equals(name) && view instanceof LinearLayout) {
        return new Accessor() {
          @Override
          public String get() {
            return null;
          }

          @Override
          public void set(String value) {
            if ("horizontal".equals(value)) {
              ((LinearLayout) view).setOrientation(0);
            } else if ("vertical".equals(value)) {
              ((LinearLayout) view).setOrientation(1);
            } else {
              throw new UnsupportedOperationException("unknown orientation value");
            }
          }
        };
      } else if ("android:text".equals(name) && view instanceof TextView) {
        return new Accessor() {
          @Override
          public String get() {
            return ((TextView) view).getText().toString();
          }

          @Override
          public void set(String value) {
            ((TextView) view).setText(value);
          }
        };
      } else if ("android:textOn".equals(name) && view instanceof ToggleButton) {
        return new Accessor() {
          @Override
          public String get() {
            return ((ToggleButton) view).getTextOn().toString();
          }

          @Override
          public void set(String value) {
            ((ToggleButton) view).setTextOn(value);
          }
        };
      } else if ("android:textOff".equals(name) && view instanceof ToggleButton) {
        return new Accessor() {
          @Override
          public String get() {
            return ((ToggleButton) view).getTextOff().toString();
          }

          @Override
          public void set(String value) {
            ((ToggleButton) view).setTextOff(value);
          }
        };
      } else if ("android:onClick".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            return null;
          }

          @Override
          public void set(final String value) {           
            view.setOnClickListener(new OnClickListener() {
              @Override
              public void onClick(View view) {
                String code = value;
                if (value.startsWith("javascript:")) {
                  code = value.substring("javascript:".length());
                }
                
                String result = isCall(code) ? v8.call(code) : v8.execute(value);
              }
            });
          }
        };
      } else if ("android:onKey".equals(name)) {
        return new Accessor() {
          @Override
          public String get() {
            return null;
          }

          @Override
          public void set(final String value) {           
            view.setOnKeyListener(new OnKeyListener() {
              @Override
              public boolean onKey(View view, int keyCode, KeyEvent event) {
                String code = value;
                if (value.startsWith("javascript:")) {
                  code = value.substring("javascript:".length());
                }
                
                String result = isCall(code) ? v8.call(code, keyCode) : v8.execute(value);
                return "true".equals(result);
              }
            });
          }
        };
      }
      return null;
    }
    
    private boolean isCall(String code) {
      return code.trim().matches("[a-zA-Z]+$");
    }
  }
  

  private View create(Node node) {
    String name = node.getNodeName();
    
    if ("LinearLayout".equals(name)) {
      return new LinearLayout(context);
    } else if ("TextView".equals(name)) {
      return new TextView(context);
    } else if ("EditText".equals(name)) {
      return new EditText(context);
    } else if ("CheckBox".equals(name)) {
      return new CheckBox(context);
    } else if ("RadioGroup".equals(name)) {
      return new RadioGroup(context);
    } else if ("RadioButton".equals(name)) {
      return new RadioButton(context);      
    } else if ("ToggleButton".equals(name)) {
      return new ToggleButton(context);
    } else if ("Button".equals(name)) {
      return new Button(context);
    }
    
    throw new RuntimeException("Unknown node type");
  }
  
  private Optional<String> property(Node node, String name) {
    Node text = node.getAttributes().getNamedItem(name);
    if (text != null) {
      return Optional.of(text.getNodeValue());
    } else {
      return Optional.absent();
    }
  }

  private void maybeSetProperty(View view, String name, Node node) {
    Optional<String> value = property(node, name);
    if (value.isPresent()) {
      new ViewParser(v8).setProperty(view, name, value.get());
    }   
  }
  
  private void setProperties(View result, Node node) {
    maybeSetProperty(result, "android:layout_width", node);
    maybeSetProperty(result, "android:layout_height", node);
    maybeSetProperty(result, "android:id", node);
    maybeSetProperty(result, "android:onClick", node);
    maybeSetProperty(result, "android:onKey", node);
    maybeSetProperty(result, "android:orientation", node);
    maybeSetProperty(result, "android:text", node);
    maybeSetProperty(result, "android:visibility", node);
    maybeSetProperty(result, "android:textOn", node);
    maybeSetProperty(result, "android:textOff", node);
  }
  
  private View visit(Node node) {
    View result = create(node);
    
    setProperties(result, node);
    
    if (result instanceof ViewGroup) {
      ViewGroup group = (ViewGroup) result;
      
      // Only ViewGroups may have children.
      NodeList list = node.getChildNodes();
      for (int i = 0; i < list.getLength(); i++) {
        Node child = list.item(i);

        if ("#text".equals(child.getNodeName())) {
          // Ignores plain text between nodes.
          continue;
        } else if ("Script".equals(child.getNodeName())) {
          String code = child.getTextContent();
          v8.execute(code);
          continue;
        }
        
        // depth first
        View view = visit(child);
        group.addView(view);
      }
    }
    
    return result;
  }
  
  View parse(String html) {
    try {
      Document doc = create(html);
      return visit(doc.getFirstChild());
    } catch (ParserConfigurationException e) {
      throw new RuntimeException(e);
    } catch (SAXException e) {
      throw new RuntimeException(e);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}