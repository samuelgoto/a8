#include <jni.h>
#include <string.h>
#include <v8.h>
#include <iostream>
#include <stdlib.h>

namespace v8 {

class Document {
 public:
  Document(JNIEnv* jvm, jobject javaThis) : jvm_(jvm), javaThis_(javaThis) {
  }
  ~Document() {
    std::cout << "deleted2" << std::endl;
    // jvm_->DeleteGlobalRef(javaThis_);
  }
  JNIEnv* jvm_;
  jobject javaThis_;
  const char* getElementById(std::string id);
  void alert(std::string id);
};

class Engine {
 public:
  Engine(Isolate* isolate, Document doc) :
      isolate_(isolate),
      scope_(isolate),
      context_(Context::New(isolate)),
      persistent_context_(isolate, context_),
      doc_(doc) {
  }

  ~Engine() {
    std::cout << "deleted" << std::endl;
    persistent_context_.Dispose(); 
  }

public:
  void init();
  std::string execute(const char* code);

private:

  Isolate* isolate_;
  HandleScope scope_;
  Handle<Context> context_;
  Persistent<Context> persistent_context_;
  v8::Document doc_;
};

void Document::alert(std::string message) {
  jclass clazz = jvm_->FindClass("com/example/test/V8");

  jstring argument = jvm_->NewStringUTF(message.c_str());

  jmethodID callback = jvm_->GetMethodID(clazz,
      "alert", "(Ljava/lang/String;)V");

  jvm_->CallVoidMethod(javaThis_, callback, argument);
}

const char* Document::getElementById(std::string id) {
  jclass clazz = jvm_->FindClass("com/example/test/V8");

  jstring argument = jvm_->NewStringUTF(id.c_str());

  jmethodID callback = jvm_->GetMethodID(clazz,
      "getElementById", "(Ljava/lang/String;)Ljava/lang/String;");

  jobject result = jvm_->CallObjectMethod(javaThis_, callback, argument);
  const char* str = jvm_->GetStringUTFChars((jstring) result, NULL);

  return str;
}

void GetElementById(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() != 1) {
    args.GetReturnValue().Set(ThrowException(Exception::RangeError(
        String::New("Argument required"))));
  }

  // get the param
  String::Utf8Value id(args[0]->ToString());

  Local<Object> self = args.Holder();
  Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
  void* ptr = wrap->Value();
  const char* value = static_cast<Document*>(ptr)->getElementById(
      std::string(*id));
  args.GetReturnValue().Set(String::New(value));
}

void Alert(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() != 1) {
    args.GetReturnValue().Set(ThrowException(Exception::RangeError(
        String::New("Argument required"))));
  }

  // get the param
  String::Utf8Value id(args[0]->ToString());

  Local<Value> data = args.Data();
  Local<External> wrap = Local<External>::Cast(data);
  void* ptr = wrap->Value();

  static_cast<Document*>(ptr)->alert(std::string(*id));
}

void Test(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(String::New("hi from Test"));
}

void Engine::init() {
  Context::Scope context_scope(context_);

  Local<FunctionTemplate> alert = FunctionTemplate::New(Alert, External::New(&doc_));

  alert->GetFunction();

  context_->Global()->Set(String::New("alert"),
			 alert->GetFunction());

  Local<FunctionTemplate> test = FunctionTemplate::New(Test);
  context_->Global()->Set(String::New("test"),
			 test->GetFunction());

  Local<ObjectTemplate> document = ObjectTemplate::New();
  document->SetInternalFieldCount(1);
  document->Set(String::New("getElementById"),
             FunctionTemplate::New(GetElementById)->GetFunction());
  Local<Object> instance = document->NewInstance();
  instance->SetInternalField(0, External::New(&doc_));
  context_->Global()->Set(String::New("document"), instance);
}

std::string Engine::execute(const char* code) {
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

extern "C" {

Engine *engine;

void Init(Document p) {
  engine = new v8::Engine(v8::Isolate::GetCurrent(), p);
  engine->init();
}

std::string Execute(std::string code) {
  return engine->execute(code.c_str());
}

void Destroy() {
  delete engine;
}

jstring Java_com_example_test_V8_execute(JNIEnv* env, jobject javaThis, jstring javaCode) {

  const char *code = env->GetStringUTFChars(javaCode, 0);

  std::string result = Execute(code);
  return env->NewStringUTF(result.c_str());
}

void Java_com_example_test_V8_init(JNIEnv* env, jobject javaThis) {

  jobject object = env->NewGlobalRef(javaThis);

  Document doc(env, object);
  Init(doc);
}


}
}


int main(int argc, char* argv[]) {

  v8::Document p(NULL, NULL);
  v8::Init(p);

  printf("0: %s\n", v8::Execute("2+2").c_str());
  printf("1: %s\n", v8::Execute("function foo() { return 2+2; };").c_str());
  printf("2: %s\n", v8::Execute("foo();").c_str());
  printf("3: %s\n", v8::Execute("test();").c_str());

  v8::Destroy();

  return 0;
}
