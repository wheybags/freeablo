# CMakeLists.txt

# Copyright (C) 2007,2009-2016 Glenn Randers-Pehrson
# Written by Christian Ehrlicher, 2007
# Revised by Roger Lowman, 2009-2010
# Revised by Clifford Yapp, 2011-2012
# Revised by Roger Leigh, 2016
# Copyright (C) 2015 Ruslan Baratov

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h

cmake_minimum_required(VERSION 3.0)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

project(PNG VERSION 1.6.26 LANGUAGES C)

enable_testing()

set(PNGLIB_MAJOR ${PROJECT_VERSION_MAJOR})
set(PNGLIB_MINOR ${PROJECT_VERSION_MINOR})
set(PNGLIB_RELEASE ${PROJECT_VERSION_PATCH})
set(PNGLIB_NAME libpng${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_RELEASE})

# COMMAND LINE OPTIONS
option(PNG_TESTS  "Build libpng tests" NO)

# Many more configuration options could be added here
option(PNG_FRAMEWORK "Build OS X framework" OFF)
option(PNGARG        "Disable ANSI-C prototypes" OFF)

set(PNG_PREFIX "" CACHE STRING "Prefix to add to the API function names")
set(DFA_XTRA "" CACHE FILEPATH "File containing extra configuration settings")

option(PNG_DISABLE_AWK "Disable AWK based scripts code generation" YES)

# to distinguish between debug and release lib
if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

include(CheckCSourceCompiles)
option(ld-version-script "Enable linker version script" ON)
if(ld-version-script AND NOT APPLE)
  # Check if LD supports linker scripts.
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map" "VERS_1 {
        global: sym;
        local: *;
};

VERS_2 {
        global: sym2;
                main;
} VERS_1;
")
  set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} "-Wl,--version-script='${CMAKE_CURRENT_BINARY_DIR}/conftest.map'")
  check_c_source_compiles("void sym(void) {}
void sym2(void) {}
int main(void) {return 0;}
" HAVE_LD_VERSION_SCRIPT)
  if(NOT HAVE_LD_VERSION_SCRIPT)
    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE} "-Wl,-M -Wl,${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
  check_c_source_compiles("void sym(void) {}
void sym2(void) {}
int main(void) {return 0;}
" HAVE_SOLARIS_LD_VERSION_SCRIPT)
  endif()
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
  file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
endif()

# Find symbol prefix.  Likely obsolete and unnecessary with recent
# toolchains (it's not done in many other projects).
function(symbol_prefix)
  set(SYMBOL_PREFIX)

  execute_process(COMMAND "${CMAKE_C_COMPILER}" "-E" "-"
                  INPUT_FILE /dev/null
                  OUTPUT_VARIABLE OUT
                  RESULT_VARIABLE STATUS)

  if(CPP_FAIL)
    message(WARNING "Failed to run the C preprocessor")
  endif()

  string(REPLACE "\n" ";" OUT "${OUT}")
  foreach(line ${OUT})
    string(REGEX MATCH "^PREFIX=" found_match "${line}")
    if(found_match)
      STRING(REGEX REPLACE "^PREFIX=(.*\)" "\\1" prefix "${line}")
      string(REGEX MATCH "__USER_LABEL_PREFIX__" found_match "${prefix}")
      if(found_match)
        STRING(REGEX REPLACE "(.*)__USER_LABEL_PREFIX__(.*)" "\\1\\2" prefix "${prefix}")
      endif()
      set(SYMBOL_PREFIX "${prefix}")
    endif()
  endforeach()

    message(STATUS "Symbol prefix: ${SYMBOL_PREFIX}")
    set(SYMBOL_PREFIX "${SYMBOL_PREFIX}" PARENT_SCOPE)
endfunction()

if(UNIX)
  symbol_prefix()
endif()

find_program(AWK NAMES gawk awk)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(NOT AWK OR PNG_DISABLE_AWK)
  # No awk available to generate sources; use pre-built pnglibconf.h
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt
                 ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h)
  add_custom_target(genfiles) # Dummy
else()
  include(CMakeParseArguments)
  # Generate .chk from .out with awk
  # generate_chk(INPUT inputfile OUTPUT outputfile [DEPENDS dep1 [dep2...]])
  function(generate_chk)
    set(options)
    set(oneValueArgs INPUT OUTPUT)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(_GC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (NOT _GC_INPUT)
      message(FATAL_ERROR "Invalid arguments.  generate_out requires input.")
    endif()
    if (NOT _GC_OUTPUT)
      message(FATAL_ERROR "Invalid arguments.  generate_out requires output.")
    endif()

    add_custom_command(OUTPUT "${_GC_OUTPUT}"
                       COMMAND "${CMAKE_COMMAND}"
                               "-DINPUT=${_GC_INPUT}"
                               "-DOUTPUT=${_GC_OUTPUT}"
                               -P "${CMAKE_CURRENT_BINARY_DIR}/scripts/genchk.cmake"
                       DEPENDS "${_GC_INPUT}" ${_GC_DEPENDS}
                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endfunction()

  # Generate .out from .c with awk
  # generate_out(INPUT inputfile OUTPUT outputfile [DEPENDS dep1 [dep2...]])
  function(generate_out)
    set(options)
    set(oneValueArgs INPUT OUTPUT)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(_GO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (NOT _GO_INPUT)
      message(FATAL_ERROR "Invalid arguments.  generate_out requires input.")
    endif()
    if (NOT _GO_OUTPUT)
      message(FATAL_ERROR "Invalid arguments.  generate_out requires output.")
    endif()

    add_custom_command(OUTPUT "${_GO_OUTPUT}"
                       COMMAND "${CMAKE_COMMAND}"
                               "-DINPUT=${_GO_INPUT}"
                               "-DOUTPUT=${_GO_OUTPUT}"
                               -P "${CMAKE_CURRENT_BINARY_DIR}/scripts/genout.cmake"
                       DEPENDS "${_GO_INPUT}" ${_GO_DEPENDS}
                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endfunction()

  # Generate specific source file with awk
  # generate_source(OUTPUT outputfile [DEPENDS dep1 [dep2...]])
  function(generate_source)
    set(options)
    set(oneValueArgs OUTPUT)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(_GSO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (NOT _GSO_OUTPUT)
      message(FATAL_ERROR "Invalid arguments.  generate_source requires output.")
    endif()

    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${_GSO_OUTPUT}"
                       COMMAND "${CMAKE_COMMAND}"
                               "-DOUTPUT=${_GSO_OUTPUT}"
                               -P "${CMAKE_CURRENT_BINARY_DIR}/scripts/gensrc.cmake"
                       DEPENDS ${_GSO_DEPENDS}
                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endfunction()

  # Copy file
  function(generate_copy source destination)
    add_custom_command(OUTPUT "${destination}"
                       COMMAND "${CMAKE_COMMAND}" -E remove "${destination}"
                       COMMAND "${CMAKE_COMMAND}" -E copy "${source}"
                                                          "${destination}"
                       DEPENDS "${source}")
  endfunction()

  # Generate scripts/pnglibconf.h
  generate_source(OUTPUT "scripts/pnglibconf.c"
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.dfa"
                          "${CMAKE_CURRENT_SOURCE_DIR}/scripts/options.awk"
                          "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h")

  # Generate pnglibconf.c
  generate_source(OUTPUT "pnglibconf.c"
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.dfa"
                          "${CMAKE_CURRENT_SOURCE_DIR}/scripts/options.awk"
                          "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h")

  if(PNG_PREFIX)
    set(PNGLIBCONF_H_EXTRA_DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/prefix.out"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/macro.lst")
    set(PNGPREFIX_H_EXTRA_DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/intprefix.out")
  endif()

  generate_out(INPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out")

  # Generate pnglibconf.h
  generate_source(OUTPUT "pnglibconf.h"
                  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out"
                          ${PNGLIBCONF_H_EXTRA_DEPENDS})

  generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/intprefix.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/intprefix.out"
               DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

  generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/prefix.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/prefix.out"
               DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
                       "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
                       "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out")

  # Generate pngprefix.h
  generate_source(OUTPUT "pngprefix.h"
                  DEPENDS ${PNGPREFIX_H_EXTRA_DEPENDS})

  generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/sym.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/sym.out"
               DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

  generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/symbols.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.out"
               DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
                       "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
                       "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt")

  generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/vers.c"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/vers.out"
               DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
                       "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
                       "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

  generate_chk(INPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.out"
               OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.chk"
               DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/checksym.awk"
                       "${CMAKE_CURRENT_SOURCE_DIR}/scripts/symbols.def")

  add_custom_target(symbol-check DEPENDS
                    "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.chk")

  generate_copy("${CMAKE_CURRENT_BINARY_DIR}/scripts/sym.out"
                "${CMAKE_CURRENT_BINARY_DIR}/libpng.sym")
  generate_copy("${CMAKE_CURRENT_BINARY_DIR}/scripts/vers.out"
                "${CMAKE_CURRENT_BINARY_DIR}/libpng.vers")

  add_custom_target(genvers DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libpng.vers")
  add_custom_target(gensym DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libpng.sym")

  add_custom_target("genprebuilt"
                    COMMAND "${CMAKE_COMMAND}"
                            "-DOUTPUT=scripts/pnglibconf.h.prebuilt"
                            -P "${CMAKE_CURRENT_BINARY_DIR}/scripts/gensrc.cmake"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

  # A single target handles generation of all generated files.  If
  # they are dependend upon separately by multiple targets, this
  # confuses parallel make (it would require a separate top-level
  # target for each file to track the dependencies properly).
  add_custom_target(genfiles DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/libpng.sym"
    "${CMAKE_CURRENT_BINARY_DIR}/libpng.vers"
    "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.c"
    "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h"
    "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out"
    "${CMAKE_CURRENT_BINARY_DIR}/pngprefix.h"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/intprefix.out"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf.c"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/prefix.out"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/sym.out"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.chk"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/symbols.out"
    "${CMAKE_CURRENT_BINARY_DIR}/scripts/vers.out")
endif()

# OUR SOURCES
set(libpng_public_hdrs
  png.h
  pngconf.h
  "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h"
)
set(libpng_private_hdrs
  pngpriv.h
  pngdebug.h
  pnginfo.h
  pngstruct.h
)
if(AWK AND NOT PNG_DISABLE_AWK)
  list(APPEND libpng_private_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pngprefix.h")
endif()
set(libpng_sources
  ${libpng_public_hdrs}
  ${libpng_private_hdrs}
  png.c
  pngerror.c
  pngget.c
  pngmem.c
  pngpread.c
  pngread.c
  pngrio.c
  pngrtran.c
  pngrutil.c
  pngset.c
  pngtrans.c
  pngwio.c
  pngwrite.c
  pngwtran.c
  pngwutil.c
  arm/arm_init.c
  arm/filter_neon.S
  arm/filter_neon_intrinsics.c
)
set(pngtest_sources
  pngtest.c
)
set(pngvalid_sources
  contrib/libtests/pngvalid.c
)
set(pngstest_sources
  contrib/libtests/pngstest.c
)
set(pngunknown_sources
  contrib/libtests/pngunknown.c
)
set(pngimage_sources
  contrib/libtests/pngimage.c
)
set(pngfix_sources
  contrib/tools/pngfix.c
)
set(png_fix_itxt_sources
  contrib/tools/png-fix-itxt.c
)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

# NOW BUILD OUR TARGET
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(png ${libpng_sources})
target_include_directories(png PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
  # ExitProcess not available
  target_compile_definitions(png PRIVATE -DPNG_WINDOWS_STORE)
elseif(MSVC)
  # ExitProcess
  target_link_libraries(png PUBLIC Kernel32)
endif()

# Some platforms (e.g. Linux) need separate math library
include(CheckLibraryExists)
check_library_exists(m pow "" LIB_M_REQUIRED)
set(M_LIBRARY "")
if(LIB_M_REQUIRED)
  set(M_LIBRARY m)
endif()

if(PNG_FRAMEWORK)
  set(PNG_LIB_NAME_FRAMEWORK png_framework)
  add_library(png_framework SHARED ${libpng_sources})
  add_dependencies(png_framework genfiles)
  list(APPEND PNG_LIB_TARGETS png_framework)
  set_target_properties(png_framework PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION ${PNGLIB_VERSION}
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PNGLIB_MAJOR}.${PNGLIB_MINOR}
    MACOSX_FRAMEWORK_BUNDLE_VERSION ${PNGLIB_VERSION}
    MACOSX_FRAMEWORK_IDENTIFIER org.libpng.libpng
    XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
    PUBLIC_HEADER "${libpng_public_hdrs}"
    OUTPUT_NAME png)
  target_link_libraries(png_framework ${ZLIB_LIBRARY} ${M_LIBRARY})
endif()

target_link_libraries(png PRIVATE zlib ${M_LIBRARY})

target_compile_definitions(png PUBLIC "$<$<CONFIG:Debug>:PNG_DEBUG>")

if(BUILD_SHARED_LIBS AND WIN32)
  target_compile_definitions(png PUBLIC "PNG_BUILD_DLL")
endif()

function(png_add_test)
  set(options)
  set(oneValueArgs NAME COMMAND)
  set(multiValueArgs OPTIONS FILES)
  cmake_parse_arguments(_PAT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT _PAT_NAME)
    message(FATAL_ERROR "Invalid arguments.  png_add_test requires name.")
  endif()
  if (NOT _PAT_COMMAND)
    message(FATAL_ERROR "Invalid arguments.  png_add_test requires command.")
  endif()

  set(TEST_OPTIONS "${_PAT_OPTIONS}")
  set(TEST_FILES "${_PAT_FILES}")

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/test.cmake.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/tests/${_PAT_NAME}.cmake" @ONLY)
  if(CMAKE_MAJOR_VERSION GREATER 2) # have generator expressions
    add_test(NAME "${_PAT_NAME}"
             COMMAND "${CMAKE_COMMAND}"
             "-DLIBPNG=$<TARGET_FILE:png>"
             "-DTEST_COMMAND=$<TARGET_FILE:${_PAT_COMMAND}>"
             -P "${CMAKE_CURRENT_BINARY_DIR}/tests/${_PAT_NAME}.cmake")
  else() # old 2.x add_test; limited and won't work well on Windows
    # Note LIBPNG is a dummy value as there are no generator expressions
    add_test("${_PAT_NAME}" "${CMAKE_COMMAND}"
             "-DLIBPNG=${CMAKE_CURRENT_BINARY_DIR}/libpng.so"
             "-DTEST_COMMAND=./${_PAT_COMMAND}"
             -P "${CMAKE_CURRENT_BINARY_DIR}/tests/${_PAT_NAME}.cmake")
  endif()
endfunction()

if(PNG_TESTS)
  # Find test PNG files by globbing, but sort lists to ensure
  # consistency between different filesystems.
  file(GLOB PNGSUITE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/pngsuite/*.png")
  list(SORT PNGSUITE_PNGS)
  file(GLOB TEST_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/testpngs/*.png")
  list(SORT TEST_PNGS)

  set(PNGTEST_PNG "${CMAKE_CURRENT_SOURCE_DIR}/pngtest.png")

  add_executable(pngtest ${pngtest_sources})
  target_link_libraries(pngtest png)

  png_add_test(NAME pngtest COMMAND pngtest FILES "${PNGTEST_PNG}")

  add_executable(pngvalid ${pngvalid_sources})
  target_link_libraries(pngvalid png)

  png_add_test(NAME pngvalid-gamma-16-to-8
               COMMAND pngvalid OPTIONS --gamma-16-to-8)
  png_add_test(NAME pngvalid-gamma-alpha-mode
               COMMAND pngvalid OPTIONS --gamma-alpha-mode)
  png_add_test(NAME pngvalid-gamma-background
               COMMAND pngvalid OPTIONS --gamma-background)
  png_add_test(NAME pngvalid-gamma-expand16-alpha-mode
               COMMAND pngvalid OPTIONS --gamma-alpha-mode --expand16)
  png_add_test(NAME pngvalid-gamma-expand16-background
               COMMAND pngvalid OPTIONS --gamma-background --expand16)
  png_add_test(NAME pngvalid-gamma-expand16-transform
               COMMAND pngvalid OPTIONS --gamma-transform --expand16)
  png_add_test(NAME pngvalid-gamma-sbit
               COMMAND pngvalid OPTIONS --gamma-sbit)
  png_add_test(NAME pngvalid-gamma-threshold
               COMMAND pngvalid OPTIONS --gamma-threshold)
  png_add_test(NAME pngvalid-gamma-transform
               COMMAND pngvalid OPTIONS --gamma-transform)
  png_add_test(NAME pngvalid-progressive-interlace-standard
               COMMAND pngvalid OPTIONS --standard --progressive-read --interlace)
  png_add_test(NAME pngvalid-progressive-size
               COMMAND pngvalid OPTIONS --size --progressive-read)
  png_add_test(NAME pngvalid-progressive-standard
               COMMAND pngvalid OPTIONS --standard --progressive-read)
  png_add_test(NAME pngvalid-standard
               COMMAND pngvalid OPTIONS --standard)
  png_add_test(NAME pngvalid-transform
               COMMAND pngvalid OPTIONS --transform)

  add_executable(pngstest ${pngstest_sources})
  target_link_libraries(pngstest png)

  foreach(gamma_type 1.8 linear none sRGB)
    foreach(alpha_type none alpha)
      set(PNGSTEST_FILES)
      foreach(test_png ${TEST_PNGS})
        string(REGEX MATCH ".*-linear[-.].*" TEST_PNG_LINEAR "${test_png}")
        string(REGEX MATCH ".*-sRGB[-.].*" TEST_PNG_SRGB "${test_png}")
        string(REGEX MATCH ".*-1.8[-.].*" TEST_PNG_G18 "${test_png}")
        string(REGEX MATCH ".*-alpha-.*" TEST_PNG_ALPHA "${test_png}")

        set(TEST_PNG_VALID TRUE)

        if(TEST_PNG_ALPHA)
          if (NOT "${alpha_type}" STREQUAL "alpha")
            set(TEST_PNG_VALID FALSE)
          endif()
        else()
          if ("${alpha_type}" STREQUAL "alpha")
            set(TEST_PNG_VALID FALSE)
          endif()
        endif()

        if(TEST_PNG_LINEAR)
          if(NOT "${gamma_type}" STREQUAL "linear")
            set(TEST_PNG_VALID FALSE)
          endif()
        elseif(TEST_PNG_SRGB)
          if(NOT "${gamma_type}" STREQUAL "sRGB")
            set(TEST_PNG_VALID FALSE)
          endif()
        elseif(TEST_PNG_G18)
          if(NOT "${gamma_type}" STREQUAL "1.8")
            set(TEST_PNG_VALID FALSE)
          endif()
        else()
          if(NOT "${gamma_type}" STREQUAL "none")
            set(TEST_PNG_VALID FALSE)
          endif()
        endif()

        if(TEST_PNG_VALID)
          list(APPEND PNGSTEST_FILES "${test_png}")
        endif()
      endforeach()
      # Should already be sorted, but sort anyway to be certain.
      list(SORT PNGSTEST_FILES)
      png_add_test(NAME pngstest-${gamma_type}-${alpha_type}
                   COMMAND pngstest
                   OPTIONS --tmpfile "${gamma_type}-${alpha_type}-" --log
                   FILES ${PNGSTEST_FILES})
    endforeach()
  endforeach()

  add_executable(pngunknown ${pngunknown_sources})
  target_link_libraries(pngunknown png)

  png_add_test(NAME pngunknown-discard COMMAND pngunknown OPTIONS --strict default=discard FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-IDAT COMMAND pngunknown OPTIONS --strict default=discard IDAT=save FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-if-safe COMMAND pngunknown OPTIONS --strict default=if-safe FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-sAPI COMMAND pngunknown OPTIONS --strict bKGD=save cHRM=save gAMA=save all=discard iCCP=save sBIT=save sRGB=save FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-save COMMAND pngunknown OPTIONS --strict default=save FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-sTER COMMAND pngunknown OPTIONS --strict sTER=if-safe FILES "${PNGTEST_PNG}")
  png_add_test(NAME pngunknown-vpAg COMMAND pngunknown OPTIONS --strict vpAg=if-safe FILES "${PNGTEST_PNG}")

  add_executable(pngimage ${pngimage_sources})
  target_link_libraries(pngimage png)

  png_add_test(NAME pngimage-quick COMMAND pngimage OPTIONS --list-combos --log FILES ${PNGSUITE_PNGS})
  png_add_test(NAME pngimage-full COMMAND pngimage OPTIONS --exhaustive --list-combos --log FILES ${PNGSUITE_PNGS})
endif()

if(PNG_SHARED)
  add_executable(pngfix ${pngfix_sources})
  target_link_libraries(pngfix png)
  set(PNG_BIN_TARGETS pngfix)

  add_executable(png-fix-itxt ${png_fix_itxt_sources})
  target_link_libraries(png-fix-itxt ${ZLIB_LIBRARY} ${M_LIBRARY})
  list(APPEND PNG_BIN_TARGETS png-fix-itxt)
endif()

# Create source generation scripts.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/genchk.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/scripts/genchk.cmake @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/genout.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/scripts/genout.cmake @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/gensrc.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/scripts/gensrc.cmake @ONLY)
