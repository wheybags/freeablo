## Version 2.5

* Introduce isInstance method and a convenience function template.

## Version 2.4.1

* Do not call the object destructor then its constructor throws.

## Version 2.4

* String stack get specialization doesn't change the stack value anymore.
* Added namespace addProperty accepting C-functions.
* Introduce enableExceptions function.

## Version 2.3.2

* Fixed registration continuation for already registered class.

## Version 2.3.1

* Fixed registration continuation issues.

## Version 2.3

* Added class addFunction accepting proxy functions (C++11 only).
* Added class addFunction accepting std::function (C++11 only).
* Added class addProperty accepting functions with lua_State* parameter.
* Added class addProperty accepting std::function (C++11 only).
* Added stack traits for std::unordered_map (UnorderedMap.h).
* Now using lightuserdata for function pointers.

## Version 2.2.2

* Performance optimization.

## Version 2.2.1

* Refactored namespace and class handling.

## Version 2.2

* Refactored stack operations.
* Handle exceptions in stack operations.

## Version 2.1.2

* Added operator== and operator!= for RefCountedPtr template.

## Version 2.1.1

* Support for __stdcall function pointers.

## Version 2.1

* Added stack traits for std::vector (Vector.h), std::list (List.h) and std::map (Map.h).
* Added ability to use LuaRef objects as an std::map keys.
* Fixed some manual errata.

## Version 2.0

* Numerous bug fixes
* Feature Requests from Github Issues
* Added LuaRef object
* Rewritten documentation

## Version 1.1.0

* Split code up into several files
* Add Lua table and type representations (based on Nigel's code)
* Reformat documentation as external HTML file

## Version 1.0.3

* Pass nil to Lua when a null pointer is passed for objects with shared lifetime.

## Version 1.0.2

* Option to hide metatables selectable at runtime, default to true.
* addStaticMethod () renamed to addStaticFunction () for consistency.
* addMethod () renamed to addFunction() for consistency.
* addCFunction () registrations.
* Convert null pointers to and from nil.
* Small performance increase in class pointer extraction.

## Version 1.0.1

* Backward compatibility with Lua 5.1.x.

## Version 1.0

* Explicit lifetime management models.
* Generalized containers.
* Single header distribution.
