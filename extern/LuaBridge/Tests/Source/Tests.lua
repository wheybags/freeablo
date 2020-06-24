-- test lua script to be run with the luabridge test program

print("Running LuaBridge tests:");

-- enum from C++
FN_CTOR = 0
FN_DTOR = 1
FN_STATIC = 2
FN_VIRTUAL = 3
FN_PROPGET = 4
FN_PROPSET = 5
FN_STATIC_PROPGET = 6
FN_STATIC_PROPSET = 7
FN_OPERATOR = 8
NUM_FN_TYPES = 9

-- function to print contents of a table
function printtable (t)
  for k, v in pairs(t) do
    if (type(v) == "table") then
      print(k .. " =>", "(table)");
    elseif (type(v) == "function") then
      print(k .. " =>", "(function)");
    elseif (type(v) == "userdata") then
      print(k .. " =>", "(userdata)");
    else
      print(k .. " =>", v);
    end
  end
end

function assert (expr)
  if (not expr) then error("assert failed", 2) end
end

-- test functions registered from C++

assert(testSucceeded());
assert(testRetInt() == 47);
assert(testRetFloat() == 47.0);
assert(testRetConstCharPtr() == "Hello, world");
assert(testRetStdString() == "Hello, world");

testParamInt(47);                       assert(testSucceeded());
testParamBool(true);                    assert(testSucceeded());
testParamFloat(47.0);                   assert(testSucceeded());
testParamConstCharPtr("Hello, world");  assert(testSucceeded());
testParamStdString("Hello, world");     assert(testSucceeded());
testParamStdStringRef("Hello, world");  assert(testSucceeded());

-- test static methods of classes registered from C++

A.testStatic();             assert(testAFnCalled(FN_STATIC));
B.testStatic();             assert(testAFnCalled(FN_STATIC));
B.testStatic2();            assert(testBFnCalled(FN_STATIC));

-- test static properties of classes registered from C++

assert(A.testStaticProp == 47);
assert(A.testStaticProp2 == 47);assert(testAFnCalled(FN_STATIC_PROPGET));
A.testStaticProp = 48;          assert(A.testStaticProp == 48);
A.testStaticProp2 = 49;         assert(testAFnCalled(FN_STATIC_PROPSET) and A.testStaticProp2 == 49);

-- test classes registered from C++

object1 = A("object1");          assert(testAFnCalled(FN_CTOR));
object1:testVirtual();           assert(testAFnCalled(FN_VIRTUAL));

object2 = B("object2");         assert(testAFnCalled(FN_CTOR) and testBFnCalled(FN_CTOR));
object2:testVirtual();          assert(testBFnCalled(FN_VIRTUAL) and not testAFnCalled(FN_VIRTUAL));

-- test functions taking and returning objects

testParamAPtr(object1);          assert(object1:testSucceeded());
testParamAPtrConst(object1);     assert(object1:testSucceeded());
testParamConstAPtr(object1);     assert(object1:testSucceeded());
testParamSharedPtrA(object1);    assert(object1:testSucceeded());

testParamAPtr(object2);          assert(object2:testSucceeded());
testParamAPtrConst(object2);     assert(object2:testSucceeded());
testParamConstAPtr(object2);     assert(object2:testSucceeded());
testParamSharedPtrA(object2);    assert(object2:testSucceeded());

result = testRetSharedPtrA();    assert(result:getName() == "from C");

-- test constness

constA = testRetSharedPtrConstA();    assert(constA:getName() == "const A");
assert(constA.testVirtual == nil);
testParamConstAPtr(constA);        assert(constA:testSucceeded());
assert(pcall(testParamAPtr, constA) == false, "attempt to call nil value");

-- test properties

assert(object1.testProp == 47);
assert(object1.testProp2 == 47);    assert(testAFnCalled(FN_PROPGET));
assert(object2.testProp == 47);
assert(object2.testProp2 == 47);    assert(testAFnCalled(FN_PROPGET));

object1.testProp = 48;          assert(object1.testProp == 48);
object1.testProp2 = 49;          assert(testAFnCalled(FN_PROPSET) and object1.testProp2 == 49);

-- test operator overload
object1a = object1 + object1;      assert(testAFnCalled(FN_OPERATOR));
assert(object1a:getName() == "object1 + object1");

print("All tests succeeded.");
