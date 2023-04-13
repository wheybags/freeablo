-- $Id: testes/cstack.lua $
-- See Copyright Notice in file all.lua

local debug = require "debug"

print"testing C-stack overflow detection"
print"If this test crashes, see its file ('cstack.lua')"

-- Segmentation faults in these tests probably result from a C-stack
-- overflow. To avoid these errors, you can use the function
-- 'debug.setcstacklimit' to set a smaller limit for the use of
-- C stack by Lua. After finding a reliable limit, you might want
-- to recompile Lua with this limit as the value for
-- the constant 'LUAI_MAXCCALLS', which defines the default limit.
-- (The default limit is printed by this test.)
-- Alternatively, you can ensure a larger stack for the program.

-- For Linux, a limit up to 30_000 seems Ok. Windows cannot go much
-- higher than 2_000.


-- get and print original limit
local origlimit = debug.setcstacklimit(400)
print("default stack limit: " .. origlimit)

-- Do the tests using the original limit. Or else you may want to change
-- 'currentlimit' to lower values to avoid a seg. fault or to higher
-- values to check whether they are reliable.
local currentlimit = origlimit
debug.setcstacklimit(currentlimit)
print("current stack limit: " .. currentlimit)


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end

-- auxiliary function to keep 'count' on the screen even if the program
-- crashes.
local count
local back = string.rep("\b", 8)
local function progress ()
  count = count + 1
  local n = string.format("%-8d", count)
  io.stderr:write(back, n)   -- erase previous value and write new one
end


do    print("testing simple recursion:")
  count = 0
  local function foo ()
    progress()
    foo()   -- do recursive calls until a stack error (or crash)
  end
  checkerror("stack overflow", foo)
  print("\tfinal count: ", count)
end


do  print("testing stack overflow in message handling")
  count = 0
  local function loop (x, y, z)
    progress()
    return 1 + loop(x, y, z)
  end
  local res, msg = xpcall(loop, loop)
  assert(msg == "error in error handling")
  print("\tfinal count: ", count)
end


-- bug since 2.5 (C-stack overflow in recursion inside pattern matching)
do  print("testing recursion inside pattern matching")
  local function f (size)
    local s = string.rep("a", size)
    local p = string.rep(".?", size)
    return string.match(s, p)
  end
  local m = f(80)
  assert(#m == 80)
  checkerror("too complex", f, 200000)
end


do  print("testing stack-overflow in recursive 'gsub'")
  count = 0
  local function foo ()
    progress()
    string.gsub("a", ".", foo)
  end
  checkerror("stack overflow", foo)
  print("\tfinal count: ", count)

  print("testing stack-overflow in recursive 'gsub' with metatables")
  count = 0
  local t = setmetatable({}, {__index = foo})
  foo = function ()
    count = count + 1
    progress(count)
    string.gsub("a", ".", t)
  end
  checkerror("stack overflow", foo)
  print("\tfinal count: ", count)
end


do  print("testing changes in C-stack limit")

  assert(not debug.setcstacklimit(0))        -- limit too small
  assert(not debug.setcstacklimit(50000))    -- limit too large
  local co = coroutine.wrap (function ()
               return debug.setcstacklimit(400)
             end)
  assert(co() == false)         -- cannot change C stack inside coroutine

  local n
  local function foo () n = n + 1; foo () end

  local function check ()
    n = 0
    pcall(foo)
    return n
  end

  -- set limit to 400
  assert(debug.setcstacklimit(400) == currentlimit)
  local lim400 = check()
  -- set a very low limit (given that there are already several active
  -- calls to arrive here)
  local lowlimit = 38
  assert(debug.setcstacklimit(lowlimit) == 400)
  assert(check() < lowlimit - 30)
  assert(debug.setcstacklimit(600) == lowlimit)
  local lim600 = check()
  assert(lim600 == lim400 + 200)


  -- 'setcstacklimit' works inside protected calls. (The new stack
  -- limit is kept when 'pcall' returns.)
  assert(pcall(function ()
    assert(debug.setcstacklimit(400) == 600)
    assert(check() <= lim400)
  end))

  assert(check() == lim400)
  assert(debug.setcstacklimit(origlimit) == 400)   -- restore original limit
end


print'OK'
