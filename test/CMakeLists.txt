set(FA_TESTS_ENABLED ON CACHE BOOL "enable tests")

if(FA_TESTS_ENABLED)
    set(all_tests)

    function(fa_add_test test_name link_libs in_fatest)
        add_executable("test_${test_name}" "${test_name}.cpp")

        target_compile_definitions("test_${test_name}" PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries("test_${test_name}" gtest_main ${link_libs})

        set(flags "${FA_COMPILER_FLAGS}")
        if (UNIX)
            set(flags "${flags} -Wno-keyword-macro")
        endif()
        set_target_properties("test_${test_name}" PROPERTIES COMPILE_FLAGS "${flags}")

        if(in_fatest)
            list(APPEND all_tests COMMAND "test_${test_name}")
            set(all_tests "${all_tests}" PARENT_SCOPE)
        endif()
    endfunction(fa_add_test)

    # TODO: simplify this (and maybe travis?)
    fa_add_test(cel "Cel;SDL2;Misc;SDL_image;Filesystem" No)

    add_custom_target(fatest ${all_tests})
    set_target_properties(fatest PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()

add_subdirectory(unit)
