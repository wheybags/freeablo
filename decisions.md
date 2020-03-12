# Decisions taken over the project lifetime

## What is this?
See https://akazlou.com/posts-output/2015-11-09-every-project-should-have-decisions/
Bascially, the idea is to have a file in your projects repo, where you record the
decisions you make over the course of the project's lifetime.

When adding entries here, please place them at the top of the list, preferably
with the title of the entry being the output from `date`.

## Sun Feb  9 11:25:55 CET 2020
- Decided to stop using a package manager. We will just include our dependencies
in the repo. This is largely because the Hunter project kind of died, but also
because of some awkwardness around having libraries built before the actual
build of the game code. Hunter would build libs during the configure stage, and
sometimes ended up with mismacthed toolchains and pain. If we just vendor deps +
use cmake for everything, this is much easier.
- Actually, boost is not so great. Decided to not use boost any more, since most
of what we used it for is now in the stdlib in c++17. Also, it is a pain to build,
and we want to vendor all our deps now, as discussed in the point above.

## Thu 11 Apr 2019 09:25:16 AM CEST
- Decided to switch to adding entries to the changelog as we go, instead of trying
to look back at the github issue/git log to create one when preparing a release.
- Related, but I also decided not to bother adding github issue numbers to the
changelog anymore. If we add the changelog entry in the same PR, it should be
easy to use git blame to find it anyway.

## Mon Apr 30 08:41:55 CEST 2018
- Decided not to use floating point calculation in the game simulation in order to
  preserve determinism. It is possible achieve this while still using floats, but
  in my opinion it is easier and safer to just avoid it and use integer and fixed
  point maths instead.

## 19 Feb 2017 12:44:21
- No more unsigned ints. They just cause problems (just finished spending several 
  hours debugging a problem with a negative value being passed as a size_t).

## Tue Jan  2 17:39:15 CET 2018
- These were actually decided a while ago, but I never wrote them down.
- The existing multiplayer implementation was not working, decided to pursue
  deterministic lockstep instead.
- Hunter package manager is good, we shall use it (https://github.com/ruslo/hunter)
- Qt is annoying, we shall no longer use it (huge dependency for almost not gain,
  we barely used it).
- We'll start using \#pragma once instead of include guards.
- Using Nuklear (https://github.com/vurtun/nuklear) for gui, ditched librocket
  (https://github.com/libRocket/libRocket). Librocket is buggy and dead and 
  imgui systems are very appealing especially for games. Decided on nuklear
  over the competition because it allowed easy enough skinning to resemble the
  original diablo gui.
- Along with librocket, we aslo ditched python. No scripting for now, that can
  be reassessed later when we're ready for it.

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
