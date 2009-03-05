#ifndef V8_CPPWRAPPER_H_
#define V8_CPPWRAPPER_H_

#include "v8.h"

namespace v8 {

/**
 * \class CppWrapper
 * \author Patrick Gansterer <paroga@paroga.com>
 *
 * A helperclass to easy use a C++ classe in JavaScript.
 *
 * The following example shows how to use a CppWrapper.
 *
 * We will use the C++ class Point for this example:
 *
 * \code
 * struct Point
 * {
 *    Point() : x_(0), y_(0) {}
 *    Point(int x, int y) : x_(x), y_(y) {}
 * 
 *    int getX() const { return x_; }
 *    int getY() const { return y_; }
 *    void setX(int value) { x_ = value; }
 *    void setY(int value) { y_ = value; }
 *    bool isNull() const { return x_ == 0 && y_ == 0; }
 *    void multiply(int value) { x_ *= value; y_ *= value; }
 * 
 *    int x_, y_;
 * };
 * \endcode
 *
 * First the Javascript functions must be mapped to the C++ functions:
 *
 * \code
 * v8::CppWrapper<Point> point_wrapper;
 * point_wrapper.SetAccessor<int>("x", &Point::getX, &Point::setX);
 * point_wrapper.SetAccessor<int>("y", &Point::getY, &Point::setY);
 * point_wrapper.SetFunction<bool>("isNull", &Point::isNull);
 * point_wrapper.SetFunction<void, int>("multiply", &Point::multiply);
 * \endcode
 *
 * To create new instaces with JavaScript, the global ObjectTemplate
 * has to map it to the wrapper. It is also possible to add instaces to global.
 *
 * \code
 * global->Set(String::New("Point"), point_wrapper.NewFunctionTemplate());
 *
 * Point *p = new Point(10, 20);
 * context->Global()->Set(String::New("p"), point_wrapper.NewInstance(p));
 * \endcode
 */
 
namespace ConvertFunctions
{
 template <class S>
 static Handle<Value> ToV8Value(S value) { }

 template <class S>
 static S FromV8Value(Local<Value> value) { }


 template <>
 Handle<Value> ToV8Value<bool>(bool value)
 {
  return Boolean::New(value);
 }

 template <>
 Handle<Value> ToV8Value<double>(double value)
 {
  return Number::New(value);
 }

 template <>
 Handle<Value> ToV8Value<int32_t>(int32_t value)
 {
  return Integer::New(value);
 }

 template <>
 Handle<Value> ToV8Value<const char*>(const char *value)
 {
  return String::New(value);
 }

 template <>
 Handle<Value> ToV8Value<const uint16_t*>(const uint16_t *value)
 {
  return String::New(value);
 }


 template <>
 bool FromV8Value<bool>(Local<Value> value)
 {
  return value->BooleanValue();
 }

 template <>
 double FromV8Value<double>(Local<Value> value)
 {
  return value->NumberValue();
 }

 template <>
 int32_t FromV8Value<int32_t>(Local<Value> value)
 {
  return value->Int32Value();
 }

 template <>
 uint32_t FromV8Value<uint32_t>(Local<Value> value)
 {
  return value->Uint32Value();
 }

 template <>
 int64_t FromV8Value<int64_t>(Local<Value> value)
 {
  return value->IntegerValue();
 }

}  // namespace v8::ConvertFunctions
 
template <class R> class CppWrapperInternal
{
public:
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14, class S15, class S16>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14, S15 a15, S16 a16)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14, a15, a16));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14, class S15>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14, S15 a15)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14, a15));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5, a6));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4, a5));
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3, a4));
 }
 
 template <class T, class S, class S1, class S2, class S3>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2, a3));
 }
 
 template <class T, class S, class S1, class S2>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1, a2));
 }
 
 template <class T, class S, class S1>
 static Handle<Value> CallCallback1(T* cpp_object, S callback, S1 a1)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)(a1));
 }
 
 template <class T, class S>
 static Handle<Value> CallCallback(T* cpp_object, S callback)
 {
  return ConvertFunctions::ToV8Value<R>((cpp_object->*callback)());
 }
};

template <> class CppWrapperInternal<void>
{
public:
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14, class S15, class S16>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14, S15 a15, S16 a16)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14, a15, a16);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14, class S15>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14, S15 a15)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14, a15);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13, class S14>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13, S14 a14)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13, a14);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12, class S13>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12, S13 a13)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12, a13);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11, class S12>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11, S12 a12)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11, a12);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10, class S11>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10, S11 a11)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10, a11);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9,
  class S10>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9,
  S10 a10)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9,
   a10);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8, S9 a9)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7, S8 a8)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7, a8);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6, class S7>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6, S7 a7)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6, a7);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5, class S6>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5, S6 a6)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5, a6);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4, class S5>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4, S5 a5)
 {
  (cpp_object->*callback)(a1, a2, a3, a4, a5);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3, class S4>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3, S4 a4)
 {
  (cpp_object->*callback)(a1, a2, a3, a4);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2, class S3>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2, S3 a3)
 {
  (cpp_object->*callback)(a1, a2, a3);
  return Undefined();
 }
 
 template <class T, class S, class S1, class S2>
 static Handle<Value> CallCallback(T *cpp_object, S callback, S1 a1, S2 a2)
 {
  (cpp_object->*callback)(a1, a2);
  return Undefined();
 }
 
 template <class T, class S, class S1>
 static Handle<Value> CallCallback1(T* cpp_object, S callback, S1 a1)
 {
  (cpp_object->*callback)(a1);
  return Undefined();
 }
 
 template <class T, class S>
 static Handle<Value> CallCallback(T* cpp_object, S callback)
 {
  (cpp_object->*callback)();
  return Undefined();
 }
};

template <class T> class CppWrapper
{
 class InvokeableBase
 {
 public:
  virtual Handle<Value> Call(T *cpp_object, const Arguments &args) = 0;

 protected:
  template <class S>
  static inline S GetArgument(int index, const Arguments &args)
  {
   return ConvertFunctions::FromV8Value<S>(args[index]);
  }
 };

 class GetAccessorBase
 {
 public:
  virtual Handle<Value> Get(T *cpp_object) const = 0;
 };

 class SetAccessorBase
 {
 public:
  virtual void Set(T *cpp_object, Local<Value> value) const = 0;
 };


 class GetSetAccessors
 {
 public:
  GetSetAccessors(GetAccessorBase *getter, SetAccessorBase *setter)
   : get_accessor(getter), set_accessor(setter) {}

  inline Handle<Value> Get(T *cpp_object) const
  {
   return get_accessor->Get(cpp_object);
  }

  inline void Set(T *cpp_object, Local<Value> value) const
  {
   return set_accessor->Set(cpp_object, value);
  }

 private:
  GetAccessorBase *get_accessor;
  SetAccessorBase *set_accessor;
 };


 template <class S>
 class Getter1 : public GetAccessorBase
 {
 public:
  typedef S (T::*Signature)(void);

  Getter1(Signature getter) : getter_(getter) {}

  virtual Handle<Value> Get(T *cpp_object) const
  {
   return ConvertFunctions::ToV8Value<S>((cpp_object->*getter_)());
  }

 private:
  Signature getter_;
 };

 template <class S>
 class Getter2 : public GetAccessorBase
 {
 public:
  typedef S (T::*Signature)(void) const;

  Getter2(Signature getter) : getter_(getter) {}

  virtual Handle<Value> Get(T *cpp_object) const
  {
   return ConvertFunctions::ToV8Value<S>((cpp_object->*getter_)());
  }

 private:
  Signature getter_;
 };


 template <class S>
 class Setter1 : public SetAccessorBase
 {
 public:
  typedef void (T::*Signature)(S);

  Setter1(Signature setter) : setter_(setter) {}

  virtual void Set(T *cpp_object, Local<Value> value) const
  {
   (cpp_object->*setter_)(ConvertFunctions::FromV8Value<S>(value));
  }

 private:
  Signature setter_;
 };

 template <class S>
 class Setter2 : public SetAccessorBase
 {
 public:
  typedef void (T::*Signature)(const S&);

  Setter2(Signature setter) : setter_(setter) {}

  virtual void Set(T *cpp_object, Local<Value> vale) const
  {
   (cpp_object->*setter_)(ConvertFunctions::FromV8Value<S>(vale));
  }

 private:
  Signature setter_;
 };


 template<class R,
  class S1 = void,
  class S2 = void,
  class S3 = void,
  class S4 = void,
  class S5 = void,
  class S6 = void,
  class S7 = void,
  class S8 = void,
  class S9 = void,
  class S10 = void,
  class S11 = void,
  class S12 = void,
  class S13 = void,
  class S14 = void,
  class S15 = void,
  class S16 = void>
 class Invokeable
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {}

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]),
     ConvertFunctions::FromV8Value<S12>(args[11]),
     ConvertFunctions::FromV8Value<S13>(args[12]),
     ConvertFunctions::FromV8Value<S14>(args[13]),
     ConvertFunctions::FromV8Value<S15>(args[14]),
     ConvertFunctions::FromV8Value<S16>(args[15]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10,
  class S11,
  class S12,
  class S13,
  class S14,
  class S15>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {}

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]),
     ConvertFunctions::FromV8Value<S12>(args[11]),
     ConvertFunctions::FromV8Value<S13>(args[12]),
     ConvertFunctions::FromV8Value<S14>(args[13]),
     ConvertFunctions::FromV8Value<S15>(args[14]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10,
  class S11,
  class S12,
  class S13,
  class S14>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {}

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]),
     ConvertFunctions::FromV8Value<S12>(args[11]),
     ConvertFunctions::FromV8Value<S13>(args[12]),
     ConvertFunctions::FromV8Value<S14>(args[13]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10,
  class S11,
  class S12,
  class S13>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {}

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]),
     ConvertFunctions::FromV8Value<S12>(args[11]),
     ConvertFunctions::FromV8Value<S13>(args[12]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10,
  class S11,
  class S12>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]),
     ConvertFunctions::FromV8Value<S12>(args[11]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10,
  class S11>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]),
     ConvertFunctions::FromV8Value<S11>(args[10]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9,
  class S10>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]),
     ConvertFunctions::FromV8Value<S10>(args[9]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9, S10) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8,
  class S9>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]),
     ConvertFunctions::FromV8Value<S9>(args[8]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8, S9);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8, S9) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7,
  class S8>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]),
     ConvertFunctions::FromV8Value<S8>(args[7]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7, S8);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7, S8) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6,
  class S7>
 class Invokeable<R, S1, S2, S3, S4, S5, S6, S7>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]),
     ConvertFunctions::FromV8Value<S7>(args[6]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6, S7);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6, S7) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5,
  class S6>
 class Invokeable<R, S1, S2, S3, S4, S5, S6>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]),
     ConvertFunctions::FromV8Value<S6>(args[5]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5, S6);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5, S6) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4,
  class S5>
 class Invokeable<R, S1, S2, S3, S4, S5>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]),
     ConvertFunctions::FromV8Value<S5>(args[4]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4, S5);
  typedef R (T::*Signature2)(S1, S2, S3, S4, S5) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3,
  class S4>
 class Invokeable<R, S1, S2, S3, S4>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]),
     ConvertFunctions::FromV8Value<S4>(args[3]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3, S4);
  typedef R (T::*Signature2)(S1, S2, S3, S4) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2,
  class S3>
 class Invokeable<R, S1, S2, S3>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]),
     ConvertFunctions::FromV8Value<S3>(args[2]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2, S3);
  typedef R (T::*Signature2)(S1, S2, S3) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1,
  class S2>
 class Invokeable<R, S1, S2>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]),
     ConvertFunctions::FromV8Value<S2>(args[1]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1, S2);
  typedef R (T::*Signature2)(S1, S2) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R,
  class S1>
 class Invokeable<R, S1>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &args)
   {
    return CppWrapperInternal<R>::CallCallback1(cpp_object, callback_,
     ConvertFunctions::FromV8Value<S1>(args[0]));
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)(S1);
  typedef R (T::*Signature2)(S1) const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template<class R>
 class Invokeable<R>
 {
  template<class S>
  class Caller : public InvokeableBase
  {
  public:
   Caller(S callback) : callback_(callback) {};

   virtual Handle<Value> Call(T *cpp_object, const Arguments &)
   {
    return CppWrapperInternal<R>::CallCallback(cpp_object, callback_);
   }

  private:
   S callback_;
  };

 public:
  typedef R (T::*Signature1)();
  typedef R (T::*Signature2)() const;

  template <class S>
  static InvokeableBase *NewInvokeable(S callback)
  {
   return new Caller<S>(callback);
  }
 };


 template <class S>
 static void WeakExternalReferenceCallback(Persistent<Value>, void *parameter)
 {
  if (S *cpp_object = static_cast<S*>(parameter))
   delete cpp_object;
 }

 template <class S>
 static Persistent<External> NewWeakExternal(S* parameter)
 {
  Persistent<External> ret = Persistent<External>::New(External::New(parameter));
  ret.MakeWeak(parameter, WeakExternalReferenceCallback<S>);
  return ret;
 }

 template<class S>
 static inline S UnwrapExternal(Handle<Value> value)
 {
  return static_cast<S>(External::Cast(*value)->Value());
 }

 static inline T* UnwrapCppObject(const AccessorInfo &info)
 {
  return UnwrapExternal<T*>(info.This()->GetInternalField(0));
 }

 static inline GetSetAccessors *UnwrapGetSetAccessors(const AccessorInfo &info)
 {
  return UnwrapExternal<GetSetAccessors*>(info.Data());
 }


 static Handle<Value> PrototypeAccessorGetter(Local<String>, const AccessorInfo &info)
 {
  T* cpp_object = UnwrapCppObject(info);
  GetSetAccessors *accessors = UnwrapGetSetAccessors(info);
  return accessors->Get(cpp_object);
 }

 static void PrototypeAccessorSetter(Local<String>, Local<Value> value, const AccessorInfo& info)
 {
  T* cpp_object = UnwrapCppObject(info);
  GetSetAccessors *accessors = UnwrapGetSetAccessors(info);
  return accessors->Set(cpp_object, value);
 }

 static Handle<Value> PrototypeInvocationCallback(const Arguments &args)
 {
  T* cpp_object = UnwrapExternal<T*>(args.Holder()->GetInternalField(0));
  InvokeableBase *invokaeable = UnwrapExternal<InvokeableBase*>(args.Data());
  return invokaeable->Call(cpp_object, args);
 }


 template <class S>
 inline void SetPrototypeAccessor(const char *name, GetAccessorBase *getter, SetAccessorBase *setter = 0)
 {
  template_->PrototypeTemplate()->SetAccessor(String::New(name), PrototypeAccessorGetter, PrototypeAccessorSetter,
   NewWeakExternal(new GetSetAccessors(getter, setter)));
 }

 template <class R>
 inline void SetPrototypeFunction(const char *name, InvokeableBase *parameter)
 {
  template_->PrototypeTemplate()->Set(String::New(name), FunctionTemplate::New(PrototypeInvocationCallback, NewWeakExternal(parameter)));
 }

 typedef T* (*NewFunction)(const Arguments& args);

 static Handle<Value> FunctionInvocationCallback(const Arguments &args)
 {
  if (!args.IsConstructCall())
   return Undefined();

  NewFunction newFunction = UnwrapExternal<CppWrapper*>(args.Data())->new_function_;

  T *cpp_object = newFunction(args);

  if (!cpp_object)
   return ThrowException(String::New("Can not create Object in C++"));

  args.Holder()->SetInternalField(0, NewWeakExternal(cpp_object));

  return Undefined();
 }

 static T *DefaultNewFunction(const Arguments &)
 {
  return new T();
 }

 NewFunction new_function_;
 Handle<FunctionTemplate> template_;

public:
 CppWrapper(NewFunction new_function = DefaultNewFunction)
 {
  new_function_ = new_function;
  template_ = FunctionTemplate::New(&FunctionInvocationCallback, External::New(this));
  template_->InstanceTemplate()->SetInternalFieldCount(1);
 }

 inline Handle<FunctionTemplate> NewFunctionTemplate() { return template_; }

 Local<Object> NewInstance(T *pObject, bool make_weak = true)
 {
  Local<Object> ret = template_->InstanceTemplate()->NewInstance();

  Handle<Value> internalField;

  if (make_weak)
   internalField = NewWeakExternal(pObject);
  else
   internalField = External::New(pObject);

  ret->SetInternalField(0, internalField);

  return ret;
 }


 template <class S>
 inline void SetAccessor(const char *name, typename Getter1<S>::Signature getter, typename Setter1<S>::Signature setter)
 {
  SetPrototypeAccessor<S>(name, new Getter1<S>(getter), new Setter1<S>(setter));
 }

 template <class S>
 inline void SetAccessor(const char *name, typename Getter1<S>::Signature getter, typename Setter2<S>::Signature setter)
 {
  SetPrototypeAccessor<S>(name, new Getter1<S>(getter), new Setter2<S>(setter));
 }

 template <class S>
 inline void SetAccessor(const char *name, typename Getter2<S>::Signature getter, typename Setter1<S>::Signature setter)
 {
  SetPrototypeAccessor<S>(name, new Getter2<S>(getter), new Setter1<S>(setter));
 }

 template <class S>
 inline void SetAccessor(const char *name, typename Getter2<S>::Signature getter, typename Setter2<S>::Signature setter)
 {
  SetPrototypeAccessor<S>(name, new Getter2<S>(getter), new Setter2<S>(setter));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14, class S15, class S16>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14, class S15, class S16>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14, class S15>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14, class S15>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13, class S14>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12, class S13>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11, class S12>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10,
  class S11>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9, class S10>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8, class S9>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8, S9>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7, class S8>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7, S8>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6, class S7>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6, S7>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6, S7>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5, class S6>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5, S6>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5, S6>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4, class S5>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4, S5>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4, S5>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3, class S4>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3, S4>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3, S4>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2, class S3>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2, S3>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2, S3>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2>::NewInvokeable(callback));
 }

 template <class R, class S1, class S2>
 inline void SetFunction(const char *name, typename Invokeable<R, S1, S2>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1, S2>::NewInvokeable(callback));
 }

 template <class R, class S1>
 inline void SetFunction(const char *name, typename Invokeable<R, S1>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1>::NewInvokeable(callback));
 }

 template <class R, class S1>
 inline void SetFunction(const char *name, typename Invokeable<R, S1>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R, S1>::NewInvokeable(callback));
 }

 template <class R>
 inline void SetFunction(const char *name, typename Invokeable<R>::Signature1 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R>::NewInvokeable(callback));
 }

 template <class R>
 inline void SetFunction(const char *name, typename Invokeable<R>::Signature2 callback)
 {
  SetPrototypeFunction<R>(name, Invokeable<R>::NewInvokeable(callback));
 }
};

}  // namespace v8

#endif  // V8_CPPWRAPPER_H_
