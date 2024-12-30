get_filename_component(EXE_SRC_PATH "." ABSOLUTE)
file(GLOB_RECURSE EXE_SRC CONFIGURE_DEPENDS "${EXE_SRC_PATH}/*.cpp")

add_executable(kaleidoscope ${LIB_SRC} ${EXE_SRC})
target_include_directories(kaleidoscope PUBLIC ${LIB_SRC_PATH} ${EXE_SRC_PATH})
target_link_libraries(kaleidoscope PRIVATE ${LINK_LIBS})
