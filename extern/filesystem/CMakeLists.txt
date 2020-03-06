add_library(Filesystem
    filesystem/fwd.h
	filesystem/path.h
	filesystem/path.cpp
	filesystem/resolver.h)
target_include_directories(Filesystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
