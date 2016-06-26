set(FA_TESTS_ENABLED ON CACHE BOOL "enable tests")

if(FA_TESTS_ENABLED)
    # Setup gtest
	
	set(GTEST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest")
    set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
    set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(GTEST_LOCATION "${GTEST_PREFIX}/src/gtest-build")
    set(GTEST_INCLUDES "${GTEST_PREFIX}/src/gtest/include")
    set(GTEST_LIBRARY  "${GTEST_LOCATION}/${LIBPREFIX}gtest${LIBSUFFIX}")
    set(GTEST_MAINLIB  "${GTEST_LOCATION}/${LIBPREFIX}gtest_main${LIBSUFFIX}")
	set(GTEST_CMAKE_ARGS "")
	
	if(WIN32)
		set(GTEST_LIBRARY  "optimized;${GTEST_LOCATION}/Release/${LIBPREFIX}gtest${LIBSUFFIX};debug;${GTEST_LOCATION}/Debug/${LIBPREFIX}gtest${LIBSUFFIX}")
		set(GTEST_MAINLIB  "optimized;${GTEST_LOCATION}/Release/${LIBPREFIX}gtest_main${LIBSUFFIX};debug;${GTEST_LOCATION}/Debug/${LIBPREFIX}gtest_main${LIBSUFFIX}")
		set(GTEST_CMAKE_ARGS "-Dgtest_force_shared_crt=ON")
	endif()
    
    include(ExternalProject)
    ExternalProject_Add(gtest
      URL https://github.com/google/googletest/archive/release-1.7.0.zip
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
	  CMAKE_ARGS "${GTEST_CMAKE_ARGS}"
      INSTALL_COMMAND ""
    )

    
	set(all_tests)

    function(fa_add_test test_name link_libs in_fatest)
        add_executable("test_${test_name}" "${test_name}.cpp")

        add_dependencies("test_${test_name}" gtest)
        target_include_directories("test_${test_name}" PRIVATE ${GTEST_INCLUDES})
        target_compile_definitions("test_${test_name}" PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries("test_${test_name}" ${GTEST_LIBRARY} ${GTEST_MAINLIB} ${link_libs})
        
        set_target_properties("test_${test_name}" PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

		if(in_fatest)
			list(APPEND all_tests COMMAND "test_${test_name}")
			set(all_tests "${all_tests}" PARENT_SCOPE)
		endif()
    endfunction(fa_add_test)

    # actual tests go here
    fa_add_test(cel Cel No)
	fa_add_test(serial "Serial;freeablo_lib" Yes)

	
	add_custom_target(fatest ${all_tests})
	set_target_properties(fatest PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()