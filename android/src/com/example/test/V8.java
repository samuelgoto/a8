package com.example.test;

import android.app.AlertDialog;
import android.content.Context;
import android.view.View;

public abstract class V8 {
  V8() {
    this.init();
  }
  
  static {
    System.loadLibrary("helloworld");
  }

  public native String execute(String code);
  public native String call(String code, Object... args);
  private native void init(); 

  public abstract void alert(String message);
  public abstract Element getElementById(String id);
}
