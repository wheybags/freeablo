[![Join the chat at https://gitter.im/freeablo/Lobby](https://badges.gitter.im/freeablo/Lobby.svg)](https://gitter.im/freeablo/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Build Status](https://travis-ci.org/wheybags/freeablo.svg?branch=master)](https://travis-ci.org/wheybags/freeablo) [![Build status](https://ci.appveyor.com/api/projects/status/g0jl334givrw5b0d/branch/master?svg=true)](https://ci.appveyor.com/project/wheybags/freeablo/branch/master)


Hello my friend, stay a while and listen!

freeablo is a modern cross platform reimplementation of the game engine used in Diablo 1.
As it is just an engine, you will need the original data files to play the game.

Currently, you can run around town, players and NPCs animate, you can go through the dungeon and fight monsters with melee (and limited ranged and magic), monsters drop loot, you can buy and sell items, and a bunch more stuff.
This is just a base for a game so far, so if you can help, please see the contributing section below!


There is a website at http://freeablo.org

# Installation

Build the engine yourself (instructions below), or grab a prebuilt binary for your platform from https://github.com/wheybags/freeablo/releases

Copy DIABDAT.MPQ and Diablo.exe from your diablo install into the folder with the freeablo executable. (NOTE: only version 1.09 of diablo is currently supported, so please patch to that version)

# Controls

Mouse to walk around, just like the original, and click on doors to open them.

PgUp and PgDn keys will move up / down through dungeon levels.

n will turn off collision (player will be able to walk through walls).

q to quit

esc to open pause menu

The above will all be made configurable at some point.

# Documentation

[decisions.md](decisions.md) contains a list of significant decisions taken over the project's lifetime.

# Compilation

freeablo uses cmake and a c++ package manager called [hunter](https://github.com/ruslo/hunter), so it should be as simple as just compiling it like any normal cmake project.
During the intitial run of cmake, some third-party dependencies will be downloaded and compiled. They can take a while, but they will be cached in ~/.hunter (C:\\.hunter on windows) afterwards,
so you should only have to do that once.

## Detailed instructions

Get the source:

via git:

```bash
git clone https://github.com/wheybags/freeablo.git
```

or download from [https://github.com/wheybags/freeablo/releases](https://github.com/wheybags/freeablo/releases).

make a build directory:

```bash
cd freeablo
mkdir build
cd build
```

call cmake:
```bash
cmake ..
```

Now you're ready to go, cmake has generated a makefile/vs solution, just run make/open in vs to compile.

# Contributing

Have a look at the github issue tracker for something to do, send a pull request, and I'll probably accept it.
Having dabbled a bit in OpenMW (http://openmw.org), I have decided to use their coding standards for this project, which you can see here: https://wiki.openmw.org/index.php?title=Policies_and_Standards

## Bug reports

Please feel free to submit bug reports on the github issue tracker at https://github.com/wheybags/freeablo/issues


# Changelog
See [changelog.md](changelog.md)
