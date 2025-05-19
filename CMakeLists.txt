cmake_minimum_required(VERSION 3.31)
project(
        yomitan_dictionary_builder
        VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# Glaze (json library)
FetchContent_Declare(
        glaze
        GIT_REPOSITORY https://github.com/stephenberry/glaze.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)

# Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_library(yomitan_dictionary_builder_lib STATIC
        src/core/dicentry.cpp
        src/core/html_element.cpp
        src/core/yomitan_dictionary.cpp
        src/utils/jptools/kanji_utils.cpp
        src/utils/jptools/kana_convert.cpp
)

target_include_directories(yomitan_dictionary_builder_lib PRIVATE
        include
        ${glaze_SOURCE_DIR}/include
)

# Main Executable
add_executable(yomitan_dictionary_builder src/main.cpp)

target_link_libraries(yomitan_dictionary_builder PRIVATE
        yomitan_dictionary_builder_lib
        glaze::glaze
        gtest
)


# Tests executable
enable_testing()
add_executable(yomitan_dictionary_tests
        test/html_element_test.cpp
        test/json_test.cpp
        test/kanji_utils_test.cpp
        test/kana_convert_test.cpp
)

target_include_directories(yomitan_dictionary_tests PRIVATE
        include
        ${glaze_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}/include
)

target_link_libraries(yomitan_dictionary_tests PRIVATE
        yomitan_dictionary_builder_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(yomitan_dictionary_tests)
