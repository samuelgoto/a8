package com.example.test;

import android.content.Context;
import android.content.res.Resources;
import android.test.ActivityInstrumentationTestCase2;
import android.test.mock.MockContext;
import android.view.View;

import com.example.test.Element;

import junit.framework.TestCase;


/**
 */
public class V8Test extends TestCase {
  private static class TestV8 extends V8 {
    Optional<String> message = Optional.absent();

    @Override
    public void alert(String message) {
      this.message = Optional.of(message);
    }

    @Override
    public Element getElementById(String id) {
      if (!"a".equals(id)) {
        return  null;
      }
      return new Element() {
        private String foo;

        @Override
        public void setAttribute(String name, String value) {
          foo = value;
        }

        @Override
        public String getAttribute(String name) {
          return foo;
        }
      };
    }
    
    public void assertAlert(String message) {
      assertTrue(this.message.isPresent());
      assertEquals(message, this.message.get());
    }
  }
  
  public void testBasic() {
    TestV8 v8 = new TestV8();
    assertEquals("4", v8.execute("2+2"));
  }

  public void testFunction() {
    V8 v8 = new TestV8();
    assertEquals("undefined", v8.execute("function foo() { return 2; }"));
    assertEquals("2", v8.execute("foo();"));
  }

  public void testAlert() {
    TestV8 v8 = new TestV8();
    assertEquals("undefined", v8.execute("alert('hi');"));
    v8.assertAlert("hi");
  }

  public void testTest() {
    TestV8 v8 = new TestV8();
    assertEquals("hi from Test", v8.execute("test();"));
  }

  public void testGetElementById() {
    TestV8 v8 = new TestV8();
    assertEquals("[object Object]", v8.execute("document"));
    assertEquals("[object Object]", v8.execute("document.getElementById('a')"));
    assertEquals("null", v8.execute("document.getElementById('b')"));
  }

  public void testElement() {
    TestV8 v8 = new TestV8();
    assertEquals("undefined", v8.execute("var a = document.getElementById('a')"));
    assertEquals("undefined", v8.execute("a.setAttribute('foo', 'bar');"));
    assertEquals("bar", v8.execute("a.getAttribute('foo');"));
  }

  public void testCall() {
    TestV8 v8 = new TestV8();
    assertEquals("undefined", v8.execute("function foo(a, b) { return a + b;}"));
    assertEquals("4", v8.call("foo", 1, 3));
    assertEquals("6", v8.call("foo", 2, 4));
  }
  
  public void testRegex() {
    assertTrue("foo".trim().matches("[a-zA-Z]+$"));
    assertTrue(" bar ".trim().matches("[a-zA-Z]+$"));
    assertFalse("foo()".trim().matches("[a-zA-Z]+$"));
  }
}

