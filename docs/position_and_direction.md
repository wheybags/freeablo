## Position and Direction

### Tiles
Tiles are layed out in an isometric style. i.e. isometric North is North East (top right) when relative to the screen.
Also keep in mind that y is inverted, i.e. positive y is South, this has implications on direction.
![Image](ScreenshotShowingTiles.png?raw=true "Title")

### Direction
Direction is mostly used for player movement, item drop direction and missiles.
When a tile on the map is clicked a vector between the source tile and the destination tile can be calculated as `destPoint(x, y) - sourcePoint(x, y) = Point(destX - sourceX, destY - sourceY)`.
A direction/angle can then be calculated from this vector using trigonometry `angle = atan(y/x)` (actually `atan2(y, x)` is used as this can also recover the correct quadrant).

The coordinates of the clicked tile are isometric (x, y), so the calculated angle is isometric radians/degrees. **The coordinates used by most of the engine are normal (non isometric) angle and South based i.e. South (down) is 0° relative to your screen.** `gameDegrees = isometricDegrees + (45 - 90)`. Adding 45° converts from isometric to normal direction, subtracting 90° converts from East to South based.

**Since y is inverted positive degrees is clockwise rotation**.

Some Diablo assets have an 8 direction resolution `gameDegrees * 8 / 360` e.g. player movement animations can only be toward one of the 8 surrounding tiles.

Other Diablo assets have a 16 direction resolution `gameDegrees * 16 / 360` e.g. some missiles animations including arrows can be fired in any direction, so require more sprites to ensure they don't appear skewed when fired in certian directions.

### Position
Currently the Position class maintains a position and a direction. It can also update position periodically, i.e. also handles movement.

There are two base types of movement:
* GridLocked ([Chebyshev](https://en.wikipedia.org/wiki/Chebyshev_distance)) movement with a destination tile.
GridLocked (Chebyshev) movement means movement to any neighboring tile takes the same time.
This is used for actor (player/monster/NPC) movement to match the original game.
Not sure if this was a bug in the original game or not but we'll just go with it.
This means you can move ~1.4 (sqrt(2)) faster across angled tiles than parallel tiles (i.e. horizontal/vertical movement relative to your screen is faster).

* FreeMovement
Free (Euclidean) movement in any direction.
This is used for missiles, e.g. arrows etc fly with a "real world" movement in any direction.

##### Position Resolution
The Position class also contain `mFractionalPos`, which is sub-tile position in 100ths. This is used for moving objects to give sub-tile resolution, as oposed to objects/items that can only be on a specific tile. `mCurrent` and `mFractionalPos` may be combined into `FixedPoint` numbers to tidy this up in the future.
