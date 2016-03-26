set(FA_TESTS_ENABLED ON CACHE BOOL "enable tests")

if(FA_TESTS_ENABLED)
    # Setup gtest
    
    include(ExternalProject)
    ExternalProject_Add(gtest
      URL https://github.com/google/googletest/archive/release-1.7.0.zip
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
      INSTALL_COMMAND ""
    )
    
    set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

    set(GTEST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest")
    set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
    set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(GTEST_LOCATION "${GTEST_PREFIX}/src/gtest-build")
    set(GTEST_INCLUDES "${GTEST_PREFIX}/src/gtest/include")
    set(GTEST_LIBRARY  "${GTEST_LOCATION}/${LIBPREFIX}gtest${LIBSUFFIX}")
    set(GTEST_MAINLIB  "${GTEST_LOCATION}/${LIBPREFIX}gtest_main${LIBSUFFIX}")

    add_custom_target(fatest)

    function(fa_add_test test_name link_libs in_default)
        add_executable("test_${test_name}" "${test_name}.cpp")

        add_dependencies("test_${test_name}" gtest)
        target_include_directories("test_${test_name}" PRIVATE ${GTEST_INCLUDES})
        target_compile_definitions("test_${test_name}" PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries("test_${test_name}" ${GTEST_LIBRARY} ${GTEST_MAINLIB} ${link_libs})

        if(in_default)
            add_custom_target("FATEST_${test_name}" COMMAND "${CMAKE_BINARY_DIR}/test_${test_name}" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
            add_dependencies("FATEST_${test_name}" "test_${test_name}")
            add_dependencies(fatest "FATEST_${test_name}")
        endif()
    endfunction(fa_add_test)

    # actual tests go here
    fa_add_test(cel Cel No)
endif()
