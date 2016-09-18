##Freeablo Architecture

Freeablo's source code is split up into two sections, apps and components. Apps
can depend on components, and components can depend on eachother, but components
cannot depend on apps. The idea of this arrangement is to separate out common
code that can be used in multiple programs into common libraries, so for example
the launcher and the main game executable can both use the mpq file IO library.

Tests live in the tests/ folder. See @ref md_docs_tests.

I won't try to list all the components here, as it's sure to go out of date, but
I'll gloss over a few of the most important ones.

### Components
- Cel, responsible for loading Diablo's CEL and CL2 image files into plain rgba
  buffers. There is some documentation available on these files here:
  https://github.com/wheybags/fyp (yes, freeablo was my final year college
  project originally :p), but it is somewhat out of date. Another good
  resource for these is https://github.com/mewrnd/blizzconv.

- Render, interface for rendering objects and levels. Also handles GUI rendering
  currently.

- FAIO, provides an fopen-style api for reading files from MPQ archives. Allows
  overriding files in DIABDAT.MPQ with files in the current directory.

- DiabloExe, interface for loading the portions of game data which were
  unfortunately hardcoded into the original game's executable.

- Serial, interface for serialising data into a bitstream (bit as opposed to
  byte-oriented). Based loosely on this blog series:
  http://gafferongames.com/building-a-game-network-protocol/ 

The freeablo project itself is actually just a single main.cpp, with all the
actual freeablo code technically arranged as a library in the same folder. This
was done so that unit tests can link to freeablo's internal classes.

Within the actual freeablo game codebase, the code is also split up into a
number of namespaces/directories. As I did with components, I'll try to list a
few of the most important ones here.

### freeablo namespaces
- FAWorld, basically holds the state for objects in the world, so levels,
  players, npcs, items, etc. FAWorld::FAWorld is the class that contains the
  whole lot, with FAWorld::Actor representing players, npcs and monsters with
  the subclasses FAWorld::Player, and FAWorld::Monster ([possibly out of date
  when you read this] NPCS are currently just instances of FAWorld::Actor
  directly, but this should be changed at some point as they do have their
  unique attributes [can talk to them, for one]).

- FARender, a higher-level interface for rendering that wraps the Render
  component. It handles spawning a separate thread dedicated to rendering, and
  sending the game state to that thread for rendering when needed. It wraps
  sprites to be drawn with a class (FARender::SpriteGroup) that is basically
  just an index into a cache of sprites (FARender::SpriteCache), that will be
  lazy-loaded by the render thread when they are needed for drawing. The indices
  from this cache are also used for synchronising sprites across multiplayer
  games, but this will be covered in more detail in the @ref md_docs_multiplayer
  section.

- FALevelGen, handles generation of levels. [may be out of date] Currently uses
  the awesome tinykeep dungeon generation algorithm
  ([algorithm](https://www.reddit.com/r/gamedev/comments/1dlwc4/procedural_dungeon_generation_algorithm_explained/),
  [demo](http://tinykeep.com/dungen/)). If you're looking at some fancy AI stuff, and
  thinking of adding flocking to monster AI, the algorithm includes an
  implementation of (reverse)flocking, so that might be of use.
 
