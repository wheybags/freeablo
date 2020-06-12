# Changelog

## v0.5 [?? ??? ????] 

- Added magic item generation (but only some effects actually work so far)
- Added accurate simulation of arrow damage
- Added game zoom with scroll wheel
- Added healing at Pepin
- Added healing and other potions
- Added ability to move through levels by clicking on stairs
- Added town portal spell
- Added debug grid that can be toggled with F11
- Refactored rendering, FPS greatly improved and there should be no stuttering now
- Fixed bug where arrows would miss stationary targets depending on the relative positions of shooter and target
- Fixed bug where player would stop moving if you clicked and held your mouse without wiggling it
- Fixed bug where game would crash if you pressed certain keys while on main menu
- Fixed bug where the player would walk at the target after firing an arrow

## v0.4 [6 Mar 2020]

- Added multiplayer
- Added save/load
- Added monster attack animations
- Added melee combat
- Added basic monster AI (walk to player and melee attack)
- Added town NPCs
- Added NPC dialogue system
- Added Griswold shop
- Added player inventory
- Added player gear equipping
- Added NPC loot drops
- Added pick up / put down items from / on the ground
- Added basic spells
- Added spellbook gui
- Added accurate movement timing
- Added dungeon generation for all dungeon levels
- Added nicer gui for Diablo image viewing program
- Added leveling / XP
- Added original Diablo fonts
- Added working character GUI
- Nicer launcher gui
- Many improvements to renderer
- Highlight hovered NPC
- Walls above doors now are now rendered
- New GUI system, and new main menu
- Added buying/selling items from/to Griswold
- Add support for GOG version
- Improved CEL/CL2 loading
- Much improved build/distribution process
- Many other backend improvements + minor bug fixes

## v0.3 [5 Aug 2015]

- Bugfix - monsters spawn on level stairs #112
- Threading refactor #107
- Hotkey selection GUI #106
- level 3 dungeon generation #103
- UI chroma key transparency #96
- UI animation #95
- Bugfix - gui clicks register as movement clicks #88
- Initial version of launcher #33
- sound #26
- better celview gui #134
- Bugfix - music memory leak #111
- Bugfix - exit when DIABDAT.MPQ not found #114, #115

## v0.2 [3 Dec 2014]

- Bugfix - MPQ filename case sensitivity #78, #76
- Bugfix - Deadlock on caertain command line args
- Bugfix - Player-npc clipping #97
- Bugfix - Reading freed memory in FAIO 6f098b1
- Config File parsing #81
- LibRocket Gui framework #82, #86
- Improved CEL rendering #85, #91
- Inventory and char menu placeholders #86
- Pause menu #90
- Main menu #94
- Keybinding configuration framework #98
- Music #99
- SDL1 support removed d4da725
- Dungeon level 2 generation #93

## v0.1 [24 Apr 2014]

- CEL / CL2 decoding
- Level rendering
- Level Generation
- Level switching
- Town NPCS
- Mouse movement
- Animation
- Doors
- Basic collision
