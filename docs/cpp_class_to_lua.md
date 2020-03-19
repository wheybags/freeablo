## Registering a C++ class into Lua

Suppose you have the following class:

```cpp
class Greeter
{
    const char* msg
public:
    Greeter() {}
    Greeter(const char *msg) {}
    void hello(const char* name) { std::cout << "Hello, " << name << "\n"; }
    void print() { std::cout << msg << "\n"; }
};
```

If you want to use it inside Lua, you should implement Script::LuaScript::registerType for it:

```cpp
namespace Script
{
    template <> void LuaScript::registerType<Greeter>()
    {
        luabridge::getGlobalNamespace(mState)
            .beginNamespace("example") // optionally, you can wrap the class inside a namespace
                .beginClass<Greeter>("Greeter")
                    .addConstructor<void (*)(void)>()
                    .addConstructor<void (*)(const char*)>()
                    .addFunction("hello", &Greeter::hello)
                    .addFunction("print", &Greeter::print)
                .endClass()
            .endNamespace();
    }
}
```

Make sure you call 'registerType' before using the class in Lua. Now, to use in Lua:

```lua
local greeter = example.Greeter()
greeter:hello("John Smith")

local greeter2 = example.Greeter("Hello world")
greeter2:print()
```

For more information, see [LuaBridge Reference](https://vinniefalco.github.io/LuaBridge/Manual.html).
