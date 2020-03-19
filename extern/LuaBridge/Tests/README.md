# LuaBridge Unit Tests

## Lunix and MacOS

Have CMake 3.5+ and a compiler supporting C++11 installed.

Run cmake to generate Makefiles:

```sh
mkdir build
cd build
cmake ..
```

Build the project:

```sh
make -j
```

Run tests from the `build` directory:

```sh
./Tests/LuaBridgeTests51
./Tests/LuaBridgeTests52
```

# Windows

Have CMake 3.5+ and MSVC 215 or 2017 installed.

Run cmake to generate MSVC solution and project files (run `cmake -G` to see all variants):

```cmd
mkdir build
cd build
# either
cmake -G Visual Studio 14 2015 ..
# or
cmake -G Visual Studio 15 2017 ..
```

Open the solution `LuaBridge.sln` in MSVC.

Set `LuaBridgeTests51` or `LuaBridgeTests52` as a startup project and run it.
