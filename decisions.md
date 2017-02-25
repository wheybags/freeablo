# Decisions taken over the project lifetime

## What is this?
See http://akazlou.com/posts/2015-11-09-every-project-should-have-decisions.html
Bascially, the idea is to have a file in your projects repo, where you recod the
decisions you make over the course of the project's lifetime.

When adding entries here, please place them at the top of the list, preferably
with the title of the entry being the output from `date`.

## Sun Sep 18 19:43:49 IST 2016
Start of the decisions.md file, so I'll just list all the decisions I can think
of that have already been made.

- Using SDL2 for rendering. Probably going to switch to just using it for
  sound + input + window creation and do the actual rendering in opengl soon.
- Using librocket for GUI. Possibly a bad decision as it's a bit inactive and
  has a number of outstanding issues.
- Using two threads, one for rendering and one for game logic. Idea is to have
  the simulation speed be indepenedent of the graphical fps.
- Using a shit-ton of boost. Some people don't like his as they think boost is
  too heavy-weight. Life is too short for c++ without boost.
- Cmake as build system because this is a c++ project in the 21st century.
- Loading assets directly from their native diablo formats, instead of
  converting them first. This one was really just a personal preference over
  one-time conversion. The only thing that's absolutely off the table is having
  a dev convert the assets once and then shipping them to users with builds of
  the engine. That would be copyright infringement :p
- The engine code deals with sprites by using a simple reference index
  (FARender::SpriteGroup). The actual image data is lazy-loaded by the render
  thread when it is needed.
- using enet library for networking because it supports a mixture of reliable
  and unreliable channels.
- using googletest for tests because it seems to be the de-facto testing library
  for c/c++. Didn't really evaluate any others.
- using stormlib for mpq file reading becase a: it's the only option, b: it's
  actually really good. Only advantage is it's not packaged in mainstream
  distros.
- using qt for the launcher because it has good cross platform support (better
  than gtk), and besides, gtk is yucky. Also, gtk and qt are the only real
  options for cross platform gui libs in c/c++.
  
## 19 Feb 2017 12:44:21
No more unsigned ints. They just cause problems (just finished spending several 
hours debugging a problem with a negative value being passed as a size_t).