##Tests

Tests live in the tests/ folder, and use googletest (https://github.com/google/googletest).
To run all tests, use tour build system to invoke the fatest target. On linux,
this woulbe just be a make fatest. In visual studio, you would right click->
build the fatest project.

There are multiple "test files", each of which generates an executable binary
that runs the tests contained in it. To add new tests, use the fa\_add\_test
function in tests/CMakeLists.txt.

To run an individual group of tests, just run the egenrated executable for it.
It should just be sitting there in your build dir.
