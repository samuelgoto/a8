#include <assert.h>
#include <v8.h>

namespace v8 {

  class ObjectWrap {
 public:
  ObjectWrap() {
    refs_ = 0;
  }


  virtual ~ObjectWrap() {
    if (persistent().IsEmpty()) return;
    assert(persistent().IsNearDeath());
    persistent().ClearWeak();
    persistent().Dispose();
  }


  template <class T>
  static inline T* Unwrap(v8::Handle<v8::Object> handle) {
    assert(!handle.IsEmpty());
    assert(handle->InternalFieldCount() > 0);
    // Cast to ObjectWrap before casting to T.  A direct cast from void
    // to T won't work right when T has more than one base class.
    // void* ptr = handle->GetAlignedPointerFromInternalField(0);
    Local<External> w = Local<External>::Cast(handle->GetInternalField(0));
    void* ptr = w->Value();
    ObjectWrap* wrap = static_cast<ObjectWrap*>(ptr);
    return static_cast<T*>(wrap);
  }

  inline v8::Local<v8::Object> handle() {
    return handle(v8::Isolate::GetCurrent());
  }

  inline v8::Local<v8::Object> handle(v8::Isolate* isolate) {
    return v8::Local<v8::Object>::New(isolate, persistent());
  }


  inline v8::Persistent<v8::Object>& persistent() {
    return handle_;
  }


 protected:
  inline void Wrap(v8::Handle<v8::Object> handle) {
    assert(persistent().IsEmpty());
    assert(handle->InternalFieldCount() > 0);
    handle->SetInternalField(0, External::New(this));
    // handle->SetAlignedPointerInInternalField(0, External::New(this));
    persistent().Reset(v8::Isolate::GetCurrent(), handle);
    MakeWeak();
  }


  inline void MakeWeak(void) {
    persistent().MakeWeak(this, WeakCallback);
    persistent().MarkIndependent();
  }

  /* Ref() marks the object as being attached to an event loop.
   * Refed objects will not be garbage collected, even if
   * all references are lost.
   */
  virtual void Ref() {
    assert(!persistent().IsEmpty());
    persistent().ClearWeak();
    refs_++;
  }

  /* Unref() marks an object as detached from the event loop.  This is its
   * default state.  When an object with a "weak" reference changes from
   * attached to detached state it will be freed. Be careful not to access
   * the object after making this call as it might be gone!
   * (A "weak reference" means an object that only has a
   * persistant handle.)
   *
   * DO NOT CALL THIS FROM DESTRUCTOR
   */
  virtual void Unref() {
    assert(!persistent().IsEmpty());
    assert(!persistent().IsWeak());
    assert(refs_ > 0);
    if (--refs_ == 0) MakeWeak();
  }

  int refs_;  // ro

 private:
  static void WeakCallback(v8::Isolate* isolate,
                           v8::Persistent<v8::Object>* pobj,
                           ObjectWrap* wrap) {
    v8::HandleScope scope(isolate);
    assert(wrap->refs_ == 0);
    assert(*pobj == wrap->persistent());
    assert((*pobj).IsNearDeath());
    delete wrap;
  }

  v8::Persistent<v8::Object> handle_;
};

}
