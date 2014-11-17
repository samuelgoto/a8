#include <jni.h>
#include <string.h>
#include <v8.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <vector>
#include <helloworld.h>

using namespace std;

namespace v8 {

  class Element : public ObjectWrap {
  public:

    static Handle<Value> New(Element* el){
      HandleScope handle_scope;
      Handle<ObjectTemplate> templ = ObjectTemplate::New();
      templ->SetInternalFieldCount(1);

      templ->Set(String::New("getAttribute"),
		 FunctionTemplate::New(GetAttribute)->GetFunction());

      templ->Set(String::New("setAttribute"),
		 FunctionTemplate::New(SetAttribute)->GetFunction());

      Local<Object> result = templ->NewInstance();
      el->Wrap(result);
      return handle_scope.Close(result);
    } 

    static void GetAttribute(const FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 1) {
	args.GetReturnValue()
	  .Set(ThrowException(Exception::RangeError(String::New("Argument required"))));
      }

      // get the param
      String::Utf8Value id(args[0]->ToString());

      Element *e = ObjectWrap::Unwrap<Element>(args.Holder());
      args.GetReturnValue().Set(String::New(e->getAttribute(*id).c_str()));
    }

    static void SetAttribute(const FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 2) {
	args.GetReturnValue()
	  .Set(ThrowException(Exception::RangeError(String::New("2 Arguments required"))));
      }

      // get the param
      String::Utf8Value name(args[0]->ToString());
      String::Utf8Value value(args[1]->ToString());

      Element *e = ObjectWrap::Unwrap<Element>(args.Holder());
      e->setAttribute(string(*name), string(*value));
    }

    virtual string getAttribute(string name) = 0;
    virtual void setAttribute(string name, string value) = 0;
  };

  class Document : public ObjectWrap {
  public:
    virtual Element* getElementById(string id) = 0;
    virtual void alert(string id) = 0;

    static Handle<Value> New(Document* doc){
      HandleScope handle_scope;

      Handle<ObjectTemplate> document = ObjectTemplate::New();
      document->SetInternalFieldCount(1);
      document->Set(String::New("getElementById"),
		    FunctionTemplate::New(GetElementById)->GetFunction());
      Local<Object> result = document->NewInstance();
      doc->Wrap(result);
      
      return handle_scope.Close(result);
    }

    static void GetElementById(const FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 1) {
	args.GetReturnValue()
	  .Set(ThrowException(Exception::RangeError(String::New("Argument required"))));
      }

      // get the param
      String::Utf8Value id(args[0]->ToString());

      Local<Object> self = args.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();

      string name(*id);
      Element* el = static_cast<Document*>(ptr)->getElementById(name);
    
      if (el == NULL) {
	args.GetReturnValue().Set(Null());
	return;
      }

      Handle<Value> f = Element::New(el);
      args.GetReturnValue().Set(f);
    }

    static void Alert(const FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 1) {
	args.GetReturnValue()
	  .Set(ThrowException(Exception::RangeError(String::New("Argument required"))));
      }

      // get the param
      String::Utf8Value id(args[0]->ToString());

      Local<Value> data = args.Data();
      Local<External> wrap = Local<External>::Cast(data);
      void* ptr = wrap->Value();

      static_cast<Document*>(ptr)->alert(string(*id));
    }
  
    static void Test(const FunctionCallbackInfo<Value>& args) {
      args.GetReturnValue().Set(String::New("hi from Test"));
    }
  };

  class AndroidObject {
  public:
    AndroidObject(JNIEnv* jvm, jobject javaThis) :
      jvm_(jvm), javaThis_(jvm->NewGlobalRef(javaThis)) {
    }
    ~AndroidObject() {
      jvm_->DeleteGlobalRef(javaThis_);
    }

  protected:
    JNIEnv* jvm_;
    jobject javaThis_;

    jmethodID method(string name, string method, string signature) {
      jclass clazz = jvm_->FindClass(name.c_str());
      jmethodID callback =
	jvm_->GetMethodID(clazz, method.c_str(), signature.c_str());
      return callback;
    }
  };

  class AndroidElement : public Element, public AndroidObject {
  public:
    AndroidElement(JNIEnv* jvm, jobject javaThis) :
      AndroidObject(jvm, javaThis) {
    }
    virtual string getAttribute(string name) {
      jstring argument = jvm_->NewStringUTF(name.c_str());

      jmethodID m = method("com/example/test/Element", "getAttribute", "(Ljava/lang/String;)Ljava/lang/String;");
      jobject result = jvm_->CallObjectMethod(javaThis_, m, argument);

      if (result == NULL) {
	// TODO(goto): I should probably return undefined here.
	return "";
      }

      const char* str = jvm_->GetStringUTFChars((jstring) result, NULL);

      return str;
    }
    virtual void setAttribute(string name, string value) {
      jstring javaName = jvm_->NewStringUTF(name.c_str());
      jstring javaValue = jvm_->NewStringUTF(value.c_str());

      jmethodID m = method("com/example/test/Element", "setAttribute", "(Ljava/lang/String;Ljava/lang/String;)V");
      jvm_->CallVoidMethod(javaThis_, m, javaName, javaValue);
    }
  };

  class AndroidDocument : public Document, public AndroidObject {
  public:
    AndroidDocument(JNIEnv* jvm, jobject javaThis) :
      AndroidObject(jvm, javaThis) {
    }

    virtual Element* getElementById(string id) {

      jstring argument = jvm_->NewStringUTF(id.c_str());

      jmethodID m = method("com/example/test/V8", "getElementById", "(Ljava/lang/String;)Lcom/example/test/Element;");
      jobject result = jvm_->CallObjectMethod(javaThis_, m, argument);

      return result != NULL ? new AndroidElement(jvm_, result) : NULL;
    }

    virtual void alert(string message) {
      jclass clazz = jvm_->FindClass("com/example/test/V8");

      jstring argument = jvm_->NewStringUTF(message.c_str());

      jmethodID callback =
	jvm_->GetMethodID(clazz, "alert", "(Ljava/lang/String;)V");

      jvm_->CallVoidMethod(javaThis_, callback, argument);
    }
  };

  class Engine {
  public:
    Engine(Isolate* isolate, Document* doc) :
      isolate_(isolate),
      scope_(isolate),
      context_(Context::New(isolate)),
      persistent_context_(isolate, context_),
      doc_(doc) {
    }

    ~Engine() {
      // cout << "deleting engine" << endl;
      persistent_context_.Dispose();
    }

  private:
    Isolate* isolate_;
    HandleScope scope_;
    Handle<Context> context_;
    Persistent<Context> persistent_context_;
    v8::Document* doc_;

  public:
    void init() {
      Context::Scope context_scope(context_);

      Local<FunctionTemplate> alert =
	FunctionTemplate::New(Document::Alert, External::New(doc_));

      context_->Global()->Set(String::New("alert"),
			      alert->GetFunction());

      Local<FunctionTemplate> test = FunctionTemplate::New(Document::Test);
      context_->Global()->Set(String::New("test"),
			      test->GetFunction());

      Handle<Value> instance = Document::New(doc_);
      context_->Global()->Set(String::New("document"), instance);
    }

    string execute(const char* code) {
      Context::Scope context_scope(context_);

      Handle<String> source = String::New(code);

      // Compile the source code.
      Handle<Script> script = Script::Compile(source);
  
      // Run the script to get the result.
      TryCatch trycatch;
      Handle<Value> result = script->Run();

      if (result.IsEmpty()) {  
	Handle<Value> exception = trycatch.Exception();
	String::AsciiValue message(exception);
	return *message;
      } else {
	String::AsciiValue ascii(result);
	return *ascii;
      }
    }

    string call(const char* code, vector<Handle<Value> > args) {
      Context::Scope context_scope(context_);

      // vector<Handle<Value> > args;
      // for(unsigned int i = 0; i != cargs.size(); i++) {
	/* std::cout << someVector[i]; ... */
	// Object arg = cargs[i];
      // args.push_back(*static_cast<Handle<Value>*>(cargs[i]));
      //}

      // args.push_back(Number::New(a));
      // args.push_back(Number::New(b));

      Local<Function> fun =
	Local<Function>::Cast(context_->Global()->Get(String::New(code)));

      // Run the script to get the result.
      TryCatch trycatch;
      Handle<Value> result = fun->Call(fun, args.size(), &args[0]);

      if (result.IsEmpty()) {  
	Handle<Value> exception = trycatch.Exception();
	String::AsciiValue message(exception);
	return *message;
      } else {
	String::AsciiValue ascii(result);
	return *ascii;
      }
    }
  };

  Engine *engine;

  void Init(Document *p) {
    engine = new v8::Engine(v8::Isolate::GetCurrent(), p);
    engine->init();
  }

  string Execute(string code) {
    return engine->execute(code.c_str());
  }

  void Destroy() {
    delete engine;
  }

  string Call(string function, vector<Handle<Value> > args) {
    return engine->call(function.c_str(), args);
  }

  extern "C" {
    jstring Java_com_example_test_V8_execute(JNIEnv* env, jobject javaThis, jstring javaCode) {
      const char *code = env->GetStringUTFChars(javaCode, 0);
      string result = Execute(code);
      return env->NewStringUTF(result.c_str());
    }

    jstring Java_com_example_test_V8_call(JNIEnv* env, jobject javaThis, jstring javaCode, jobjectArray array) {
      const char *code = env->GetStringUTFChars(javaCode, 0);
      vector<Handle<Value> > args;
      
      jclass integer = env->FindClass("java/lang/Integer");
      for (int i = 0; i < env->GetArrayLength(array); i++) {
	jobject arg = env->GetObjectArrayElement(array, i);

	// TODO(goto): add support for calling this interface with
	// more data types.
	if (env->IsInstanceOf(arg, integer)) {
	  // TODO(goto): find out if I can avoid this extra call here
	  // to pass integers from java to c++.
	  jmethodID intValue =
	    env->GetMethodID(integer, "intValue", "()I");
	  jint result = env->CallIntMethod(arg, intValue);
	  args.push_back(Number::New((int) result));
	}
      }

      string result = Call(code, args);
      return env->NewStringUTF(result.c_str());
    }

    void Java_com_example_test_V8_init(JNIEnv* env, jobject javaThis) {
      jobject object = env->NewGlobalRef(javaThis);
      AndroidDocument *doc = new AndroidDocument(env, object);
      Init(doc);
    }
  }
}

void assert_equals(string a, string b) {
  if (a == b) {
    return;
  }

  cout << "Assertion " << a.c_str() << " == " << b.c_str() << " failed." << endl;
}

string a;

class TestElement : public v8::Element {
private:
public:

  ~TestElement() {
    cout << "destructor!" << endl;
  }

  virtual string getAttribute(string name) {
    return a;
  }
  virtual void setAttribute(string name, string value) {
    a = value;
  }
};

class TestDocument : public v8::Document {
public:
  virtual v8::Element* getElementById(string id) {
    if (id != "a") {
      return NULL;
    }
    return new TestElement();
  }
  virtual void alert(string message) {
    cout << message.c_str() << endl;
  }
};

int main(int argc, char* argv[]) {
  v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

  TestDocument *p = new TestDocument();

  v8::Init(p);

  // Basic checks.
  assert_equals(v8::Execute("2+2"), "4");
  assert_equals(v8::Execute("test();"), "hi from Test");
  assert_equals(v8::Execute("document"), "[object Object]");
  assert_equals(v8::Execute("1 == 1"), "true");

  // Defines foo.
  assert_equals(v8::Execute("function foo() { return 2+2; };"), "undefined");
  // Uses foo.
  assert_equals(v8::Execute("foo();"), "4");

  assert_equals(v8::Execute("document.getElementById('a')"), "[object Object]");

  // Asserts that getElementById returns null for non existing element.
  assert_equals(v8::Execute("document.getElementById('b')"), "null");

  // Asserts that a.b is unset.
  assert_equals(v8::Execute("document.getElementById('a').getAttribute('b')"),
		"");
  // Sets a.b = 1
  assert_equals(v8::Execute("document.getElementById('a').setAttribute('b', 1)"),
  		"undefined");
  // Asserts that a.b is now 1
  assert_equals(v8::Execute("document.getElementById('a').getAttribute('b')"),
  		"1");

  // Assert that calling functions works
  assert_equals(v8::Execute("function add(a, b) {return a+b;};"), "undefined");
  // vector<std::Object> args;

  vector<v8::Handle<v8::Value> > args;
  args.push_back(v8::Number::New(2));
  args.push_back(v8::Number::New(2));
  assert_equals(v8::Call("add", args), "4");

  v8::Destroy();

  return 0;
}
