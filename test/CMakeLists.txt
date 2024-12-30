include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
list(APPEND LINK_LIBS GTest::gtest_main)

enable_testing()


get_filename_component(TEST_PATH "." ABSOLUTE)

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS "${TEST_PATH}/*.cpp")

add_executable(
    tests
    ${LIB_SRC}
    ${TEST_SRC}
)
target_include_directories(
    tests
    PUBLIC
    ${LIB_SRC_PATH}
)
target_link_libraries(
    tests
    PUBLIC
    ${LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(tests)
