Hello my friend, stay a while and listen!

freeablo is a modern cross platform reimplementation of the game engine used in Diablo 1.
As it is just an engine, you will need the original data files to play the game.

Currently, you can run around town, players and npcs animate, and you can go down into the first few levels of the dungeon (which is randomly generated).
This is just a base for a game so far, so if you can help, please see the contributing section below!

There is a website at http://freeablo.org

#Installation
Build the engine yourself (instructions below), or grab a prebuilt binary for your platform from https://github.com/wheybags/freeablo/releases
Copy DIABDAT.MPQ and Diablo.exe from your diablo install into the folder with the freeablo executable. (NOTE: only version 1.09 of diablo is currently supported, so please patch to that version)

#Controls
Mouse to walk around, just like the original, and click on doors to open them.

o and p keys will move up / down through dungeon levels.

n will turn off collision (player will be able to walk through walls).

q to quit

esc to open pause menu

The above will all be made configurable at some point.

#Compilation

## Windows
If you are using windows, please see https://github.com/wheybags/freeablo-windows-build/tree/master for instructions on building with Visual Studio 2010

=======
## OSX
```
brew install boost lbzip2 zlib sdl2 freetype libtomcrypt
```
and continue on with the Linux/Unix section

##Linux/Unix
freeablo uses cmake, and depends on boost >= 1.54, libbz2, zlib, and SDL2, sdl2_image, and libRocket, so make sure to have those installed.
libRocket (http://github.com/libRocket/libRocket) isn't packaged in most distros, so you'll have to install it from source (this isn't very hard)

libRocket installation:

```
$ git clone https://github.com/libRocket/libRocket.git
$ cd libRocket
$ mkdir buildDir
$ cd buildDir
$ cmake ../Build -DBUILD_PYTHON_BINDINGS=On
$ make
$ sudo make install
```

libRocket depends on boost::python and freetype

Get the source:

via git: 

```
$ git clone https://github.com/wheybags/freeablo.git
$ cd freeablo 
$ git submodule update --init
```

or download from [https://github.com/wheybags/freeablo/releases](https://github.com/wheybags/freeablo/releases).

make a build directory:

```
$ mkdir build
$ cd build
```

symlink the resources folder:
```
$ ln -s ../freeablo/resources
```

call cmake: 
```
$ cmake ../freeablo
```

Now you're ready to go, cmake has generated a makefile, just run make to compile

#Contributing
Have a look at the github issue tracker for something to do, send a pull request, and I'll probably accept it.
Having dabbled a bit in OpenMW (http://openmw.org), I have decided to use their coding standards for this project, which you can see here: https://wiki.openmw.org/index.php?title=Policies_and_Standards

##Bug reports
Please feel free to submit bug reports on the github issue tracker at https://github.com/wheybags/freeablo/issues


#Changelog
##v0.1
- CEL / CL2 decoding
- Level rendering
- Level Generation
- Level switching
- Town NPCS
- Mouse movement
- Animation
- Doors
- Basic collision
