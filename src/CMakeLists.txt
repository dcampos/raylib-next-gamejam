add_executable(main)
# @NOTE: add more source files here
target_sources(main PRIVATE
    main.cpp
    game.cpp
    util.cpp
)

target_include_directories(main PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
target_link_libraries(main raylib)
if(NOT WIN32)
    target_link_libraries(main m)
endif()

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(main PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s FORCE_FILESYSTEM=1 -s WASM=1")

    set(web_link_flags)
    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(web_link_flags "${web_link_flags} -s ASSERTIONS=1")
    endif()
    set(web_link_flags "${web_link_flags} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../assets@assets --use-preload-plugins")
    set(web_link_flags "${web_link_flags} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/minshell.html")

    set_target_properties(main PROPERTIES LINK_FLAGS "${web_link_flags}")
endif()

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if(APPLE)
    target_link_libraries(main "-framework IOKit")
    target_link_libraries(main "-framework Cocoa")
    target_link_libraries(main "-framework OpenGL")
endif()

# misc
set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
# set the startup project for the "play" button in MSVC
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
