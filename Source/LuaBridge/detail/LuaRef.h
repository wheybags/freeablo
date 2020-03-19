//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge

  Copyright 2018, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2008, Nigel Atkinson <suprapilot+LuaCode@gmail.com>

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#pragma once

#include <LuaBridge/detail/LuaException.h>
#include <LuaBridge/detail/Stack.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace luabridge {

//------------------------------------------------------------------------------
/**
    Type tag for representing LUA_TNIL.

    Construct one of these using `Nil ()` to represent a Lua nil. This is faster
    than creating a reference in the registry to nil. Example:

        LuaRef t (LuaRef::createTable (L));
        ...
        t ["k"] = Nil (); // assign nil
*/
struct Nil
{
};


//------------------------------------------------------------------------------
/**
    Stack specialization for Nil.
*/
template <>
struct Stack <Nil>
{
  static void push (lua_State* L, Nil)
  {
    lua_pushnil (L);
  }

  static bool isInstance (lua_State* L, int index)
  {
    return lua_type (L, index) == LUA_TTABLE;
  }
};

/**
 * Base class for LuaRef and table value proxy classes.
 */
template <class Impl, class LuaRef>
class LuaRefBase
{
protected:
  //----------------------------------------------------------------------------
  /**
      Pop the Lua stack.

      Pops the specified number of stack items on destruction. We use this
      when returning objects, to avoid an explicit temporary variable, since
      the destructor executes after the return statement. For example:

          template <class U>
          U cast (lua_State* L)
          {
            StackPop p (L, 1);
            ...
            return U (); // dtor called after this line
          }

      @note The `StackPop` object must always be a named local variable.
  */
  class StackPop
  {
  public:
    /** Create a StackPop object.

        @param count The number of stack entries to pop on destruction.
    */
    StackPop (lua_State* L, int count)
      : m_L (L)
      , m_count (count)
    {
    }

    ~StackPop ()
    {
      lua_pop (m_L, m_count);
    }

  private:
    lua_State* m_L;
    int m_count;
  };

  friend struct Stack <LuaRef>;

  //----------------------------------------------------------------------------
  /**
      Type tag for stack construction.
  */
  struct FromStack { };

  LuaRefBase (lua_State* L)
    : m_L (L)
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to this ref.

      This is used internally.
  */
  int createRef () const
  {
    impl ().push ();
    return luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

public:
  //----------------------------------------------------------------------------
  /**
      converts to a string using luas tostring function
  */
  std::string tostring () const
  {
    lua_getglobal (m_L, "tostring");
    impl ().push ();
    lua_call (m_L, 1, 1);
    const char* str = lua_tostring (m_L, -1);
    lua_pop (m_L, 1);
    return str;
  }

  //----------------------------------------------------------------------------
  /**
      Print a text description of the value to a stream.

      This is used for diagnostics.
  */
  void print (std::ostream& os) const
  {
    switch (type ())
    {
    case LUA_TNIL:
      os << "nil";
      break;

    case LUA_TNUMBER:
      os << cast <lua_Number> ();
      break;

    case LUA_TBOOLEAN:
      os << (cast <bool> () ? "true" : "false");
      break;

    case LUA_TSTRING:
      os << '"' << cast <std::string> () << '"';
      break;

    case LUA_TTABLE:
      os << "table: " << tostring ();
      break;

    case LUA_TFUNCTION:
      os << "function: " << tostring ();
      break;

    case LUA_TUSERDATA:
      os << "userdata: " << tostring ();
      break;

    case LUA_TTHREAD:
      os << "thread: " << tostring ();
      break;

    case LUA_TLIGHTUSERDATA:
      os << "lightuserdata: " << tostring ();
      break;

    default:
      os << "unknown";
      break;
    }
  }

  //------------------------------------------------------------------------------
  /**
      Write a LuaRef to a stream.

      This allows LuaRef and table proxies to work with streams.
  */
  friend std::ostream& operator<< (std::ostream& os, LuaRefBase const& ref)
  {
    ref.print (os);
    return os;
  }

  //============================================================================
  //
  // This group of member functions is mirrored in Proxy
  //

  /** Retrieve the lua_State associated with the reference.
  */
  lua_State* state () const
  {
    return m_L;
  }

  //----------------------------------------------------------------------------
  /**
      Place the object onto the Lua stack.
  */
  void push (lua_State* L) const
  {
    assert (equalstates (L, m_L));
    (void) L;
    impl ().push ();
  }

  //----------------------------------------------------------------------------
  /**
      Pop the top of Lua stack and assign the ref to m_ref
  */
  void pop (lua_State* L)
  {
    assert (equalstates (L, m_L));
    (void) L;
    impl ().pop ();
  }

  //----------------------------------------------------------------------------
  /**
      Determine the object type.

      The return values are the same as for `lua_type`.
  */
  /** @{ */
  int type () const
  {
    impl ().push ();
    StackPop p (m_L, 1);
    return lua_type (m_L, -1);
  }

  // should never happen
  // bool isNone () const { return m_ref == LUA_NOREF; }

  bool isNil () const { return type () == LUA_TNIL; }
  bool isBool () const { return type () == LUA_TBOOLEAN; }
  bool isNumber () const { return type () == LUA_TNUMBER; }
  bool isString () const { return type () == LUA_TSTRING; }
  bool isTable () const { return type () == LUA_TTABLE; }
  bool isFunction () const { return type () == LUA_TFUNCTION; }
  bool isUserdata () const { return type () == LUA_TUSERDATA; }
  bool isThread () const { return type () == LUA_TTHREAD; }
  bool isLightUserdata () const { return type () == LUA_TLIGHTUSERDATA; }

  /** @} */

  //----------------------------------------------------------------------------
  /**
      Perform an explicit conversion.
  */
  template <class T>
  T cast () const
  {
    StackPop p (m_L, 1);
    impl ().push ();
    return Stack <T>::get (m_L, -1);
  }

  //----------------------------------------------------------------------------
  /**
      Universal implicit conversion operator.

      NOTE: Visual Studio 2010 and 2012 have a bug where this function
            is not used. See:

      http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/e30b2664-a92d-445c-9db2-e8e0fbde2014
      https://connect.microsoft.com/VisualStudio/feedback/details/771509/correct-code-doesnt-compile

          // This code snippet fails to compile in vs2010,vs2012
          struct S {
            template <class T> operator T () const { return T (); }
          };
          int main () {
            S () || false;
            return 0;
          }
  */
  template <class T>
  operator T () const
  {
    return cast <T> ();
  }

  //----------------------------------------------------------------------------
  /**
      Universal comparison operators.
  */
  /** @{ */
  template <class T>
  bool operator== (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();
    Stack <T>::push (m_L, rhs);
    return lua_compare (m_L, -2, -1, LUA_OPEQ) == 1;
  }

  template <class T>
  bool operator< (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType < rhsType;
    }
    return lua_compare (m_L, -2, -1, LUA_OPLT) == 1;
  }

  template <class T>
  bool operator<= (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType <= rhsType;
    }
    return lua_compare (m_L, -2, -1, LUA_OPLE) == 1;
  }

  template <class T>
  bool operator> (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType > rhsType;
    }
    return lua_compare (m_L, -1, -2, LUA_OPLT) == 1;
  }

  template <class T>
  bool operator>= (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    int lhsType = lua_type (m_L, -2);
    int rhsType = lua_type (m_L, -1);
    if (lhsType != rhsType)
    {
      return lhsType >= rhsType;
    }
    return lua_compare (m_L, -1, -2, LUA_OPLE) == 1;
  }

  template <class T>
  bool rawequal (T rhs) const
  {
    StackPop p (m_L, 2);
    impl ().push ();;
    Stack <T>::push (m_L, rhs);
    return lua_rawequal (m_L, -1, -2) == 1;
  }
  /** @} */

  //----------------------------------------------------------------------------
  /**
      Append a value to the table.

      If the table is a sequence this will add another element to it.
  */
  template <class T>
  void append (T v) const
  {
    impl ().push ();;
    Stack <T>::push (m_L, v);
    luaL_ref (m_L, -2);
    lua_pop (m_L, 1);
  }

  //----------------------------------------------------------------------------
  /**
      Call the length operator.

      This is identical to applying the Lua # operator.
  */
  int length () const
  {
    StackPop p (m_L, 1);
    impl ().push ();;
    return get_length (m_L, -1);
  }

  //----------------------------------------------------------------------------
  /**
      Call Lua code.

      These overloads allow Lua code to be called with up to 8 parameters.
      The return value is provided as a LuaRef (which may be LUA_REFNIL).
      If an error occurs, a LuaException is thrown.
  */
  /** @{ */
  LuaRef operator() () const
  {
    impl ().push ();;
    LuaException::pcall (m_L, 0, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1>
  LuaRef operator() (P1 p1) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    LuaException::pcall (m_L, 1, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2>
  LuaRef operator() (P1 p1, P2 p2) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    LuaException::pcall (m_L, 2, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3>
  LuaRef operator() (P1 p1, P2 p2, P3 p3) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    LuaException::pcall (m_L, 3, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    LuaException::pcall (m_L, 4, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    LuaException::pcall (m_L, 5, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    LuaException::pcall (m_L, 6, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
  {
    impl ().push ();;
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    Stack <P7>::push (m_L, p7);
    LuaException::pcall (m_L, 7, 1);
    return LuaRef::fromStack (m_L);
  }

  template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
  LuaRef operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) const
  {
    impl ().push ();
    Stack <P1>::push (m_L, p1);
    Stack <P2>::push (m_L, p2);
    Stack <P3>::push (m_L, p3);
    Stack <P4>::push (m_L, p4);
    Stack <P5>::push (m_L, p5);
    Stack <P6>::push (m_L, p6);
    Stack <P7>::push (m_L, p7);
    Stack <P8>::push (m_L, p8);
    LuaException::pcall (m_L, 8, 1);
    return LuaRef::fromStack (m_L);
  }
  /** @} */

  //============================================================================

protected:
  lua_State* m_L;

private:
  const Impl& impl () const
  {
    return static_cast <const Impl&> (*this);
  }

  Impl& impl ()
  {
    return static_cast <Impl&> (*this);
  }
};

//------------------------------------------------------------------------------
/**
    Lightweight reference to a Lua object.

    The reference is maintained for the lifetime of the C++ object.
*/
class LuaRef : public LuaRefBase <LuaRef, LuaRef>
{
  //----------------------------------------------------------------------------
  /**
      A proxy for representing table values.
  */
  class Proxy : public LuaRefBase <Proxy, LuaRef>
  {
    friend class LuaRef;

  public:
    //--------------------------------------------------------------------------
    /**
        Construct a Proxy from a table value.

        The table is in the registry, and the key is at the top of the stack.
        The key is popped off the stack.
    */
    Proxy (lua_State* L, int tableRef)
      : LuaRefBase (L)
      , m_tableRef (LUA_NOREF)
      , m_keyRef (luaL_ref (L, LUA_REGISTRYINDEX))
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, tableRef);
      m_tableRef = luaL_ref (L, LUA_REGISTRYINDEX);
    }

    //--------------------------------------------------------------------------
    /**
        Create a Proxy via copy constructor.

        It is best to avoid code paths that invoke this, because it creates
        an extra temporary Lua reference. Typically this is done by passing
        the Proxy parameter as a `const` reference.
    */
    Proxy (Proxy const& other)
      : LuaRefBase (other.m_L)
      , m_tableRef (LUA_NOREF)
      , m_keyRef (LUA_NOREF)
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, other.m_tableRef);
      m_tableRef = luaL_ref (m_L, LUA_REGISTRYINDEX);

      lua_rawgeti (m_L, LUA_REGISTRYINDEX, other.m_keyRef);
      m_keyRef = luaL_ref (m_L, LUA_REGISTRYINDEX);
    }

    //--------------------------------------------------------------------------
    /**
        Destroy the proxy.

        This does not destroy the table value.
    */
    ~Proxy ()
    {
      luaL_unref (m_L, LUA_REGISTRYINDEX, m_keyRef);
      luaL_unref (m_L, LUA_REGISTRYINDEX, m_tableRef);
    }

    //--------------------------------------------------------------------------
    /**
        Assign a new value to this table key.

        This may invoke metamethods.
    */
    template <class T>
    Proxy& operator= (T v)
    {
      StackPop p (m_L, 1);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      Stack <T>::push (m_L, v);
      lua_settable (m_L, -3);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Assign a new value to this table key.

        The assignment is raw, no metamethods are invoked.
    */
    template <class T>
    Proxy& rawset (T v)
    {
      StackPop p (m_L, 1);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      Stack <T>::push (m_L, v);
      lua_rawset (m_L, -3);
      return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Push the value onto the Lua stack.
    */
    using LuaRefBase::push;

    void push () const
    {
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_tableRef);
      lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_keyRef);
      lua_gettable (m_L, -2);
      lua_remove (m_L, -2); // remove the table
    }

    //--------------------------------------------------------------------------
    /**
        Access a table value using a key.

        This invokes metamethods.
    */
    template <class T>
    Proxy operator[] (T key) const
    {
      return LuaRef (*this) [key];
    }

    //--------------------------------------------------------------------------
    /**
        Access a table value using a key.

        The operation is raw, metamethods are not invoked. The result is
        passed by value and may not be modified.
    */
    template <class T>
    LuaRef rawget (T key) const
    {
      return LuaRef (*this).rawget (key);
    }

  private:
    int m_tableRef;
    int m_keyRef;
  };

  friend struct Stack <Proxy>;
  friend struct Stack <Proxy&>;

  //----------------------------------------------------------------------------
  /**
      Create a reference to an object at the top of the Lua stack and pop it.

      This constructor is private and not invoked directly.
      Instead, use the `fromStack` function.

      @note The object is popped.
  */
  LuaRef (lua_State* L, FromStack)
    : LuaRefBase (L)
    , m_ref (luaL_ref (m_L, LUA_REGISTRYINDEX))
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to an object on the Lua stack.

      This constructor is private and not invoked directly.
      Instead, use the `fromStack` function.

      @note The object is not popped.
  */
  LuaRef (lua_State* L, int index, FromStack)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
    lua_pushvalue (m_L, index);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }


public:
  //----------------------------------------------------------------------------
  /**
      Create a nil reference.

      The LuaRef may be assigned later.
  */
  LuaRef (lua_State* L)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to a value.
  */
  template <class T>
  LuaRef (lua_State* L, T v)
    : LuaRefBase (L)
    , m_ref (LUA_NOREF)
  {
    Stack <T>::push (m_L, v);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

  //----------------------------------------------------------------------------
  /**
      Create a reference to a table value.
  */
  LuaRef (Proxy const& v)
    : LuaRefBase (v.state ())
    , m_ref (v.createRef ())
  {
  }

  //----------------------------------------------------------------------------
  /**
      Create a new reference to an existing reference.
  */
  LuaRef (LuaRef const& other)
    : LuaRefBase (other.m_L)
    , m_ref (other.createRef ())
  {
  }

  //----------------------------------------------------------------------------
  /**
      Destroy a reference.

      The corresponding Lua registry reference will be released.

      @note If the state refers to a thread, it is the responsibility of the
            caller to ensure that the thread still exists when the LuaRef
            is destroyed.
  */
  ~LuaRef ()
  {
    luaL_unref (m_L, LUA_REGISTRYINDEX, m_ref);
  }

  //----------------------------------------------------------------------------
  /**
      Return a LuaRef from a top stack item.

      The stack item is not popped.
  */
  static LuaRef fromStack (lua_State* L)
  {
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Return a LuaRef from a stack item.

      The stack item is not popped.
  */
  static LuaRef fromStack (lua_State* L, int index)
  {
    lua_pushvalue (L, index);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Create a new empty table and return a reference to it.

      It is also possible to use the free function `newTable`.

      @see ::luabridge::newTable
  */
  static LuaRef newTable (lua_State* L)
  {
    lua_newtable (L);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Return a reference to a named global.

      It is also possible to use the free function `getGlobal`.

      @see ::luabridge::getGlobal
  */
  static LuaRef getGlobal (lua_State *L, char const* name)
  {
    lua_getglobal (L, name);
    return LuaRef (L, FromStack ());
  }

  //----------------------------------------------------------------------------
  /**
      Assign another LuaRef to this LuaRef.
  */
  LuaRef& operator= (LuaRef const& rhs)
  {
    LuaRef ref (rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Assign Proxy to this LuaRef.
  */
  LuaRef& operator= (LuaRef::Proxy const& rhs)
  {
    LuaRef ref (rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
  Assign nil to this LuaRef.
  */
  LuaRef& operator= (Nil const&)
  {
    LuaRef ref (m_L);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Assign a different value to this LuaRef.
  */
  template <class T>
  LuaRef& operator= (T rhs)
  {
    LuaRef ref (m_L, rhs);
    swap (ref);
    return *this;
  }

  //----------------------------------------------------------------------------
  /**
      Place the object onto the Lua stack.
  */
  using LuaRefBase::push;

  void push () const
  {
    lua_rawgeti (m_L, LUA_REGISTRYINDEX, m_ref);
  }

  //----------------------------------------------------------------------------
  /**
      Pop the top of Lua stack and assign the ref to m_ref
  */
  void pop ()
  {
    luaL_unref (m_L, LUA_REGISTRYINDEX, m_ref);
    m_ref = luaL_ref (m_L, LUA_REGISTRYINDEX);
  }

  //----------------------------------------------------------------------------
  /**
      Access a table value using a key.

      This invokes metamethods.
  */
  template <class T>
  Proxy operator[] (T key) const
  {
    Stack <T>::push (m_L, key);
    return Proxy (m_L, m_ref);
  }

    //--------------------------------------------------------------------------
    /**
        Access a table value using a key.

        The operation is raw, metamethods are not invoked. The result is
        passed by value and may not be modified.
    */
    template <class T>
    LuaRef rawget (T key) const
    {
      StackPop (m_L, 1);
      push (m_L);
      Stack <T>::push (m_L, key);
      lua_rawget (m_L, -2);
      return LuaRef (m_L, FromStack ());
    }

private:
  void swap (LuaRef& other)
  {
    std::swap (m_L, other.m_L);
    std::swap (m_ref, other.m_ref);
  }

  int m_ref;
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `LuaRef`.
 */
template <>
struct Stack <LuaRef>
{
  // The value is const& to prevent a copy construction.
  //
  static void push (lua_State* L, LuaRef const& v)
  {
    v.push (L);
  }

  static LuaRef get (lua_State* L, int index)
  {
    return LuaRef::fromStack (L, index);
  }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `Proxy`.
 */
template <>
struct Stack <LuaRef::Proxy>
{
  // The value is const& to prevent a copy construction.
  //
  static void push (lua_State* L, LuaRef::Proxy const& v)
  {
    v.push (L);
  }
};

//------------------------------------------------------------------------------
/**
    Create a reference to a new, empty table.

    This is a syntactic abbreviation for LuaRef::newTable ().
*/
inline LuaRef newTable (lua_State* L)
{
  return LuaRef::newTable (L);
}

//------------------------------------------------------------------------------
/**
    Create a reference to a value in the global table.

    This is a syntactic abbreviation for LuaRef::getGlobal ().
*/
inline LuaRef getGlobal (lua_State *L, char const* name)
{
  return LuaRef::getGlobal (L, name);
}

//------------------------------------------------------------------------------

// more C++-like cast syntax
//
template <class T>
T LuaRef_cast (LuaRef const& lr)
{
  return lr.cast <T> ();
}

} // namespace luabridge
