set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# https://github.com/libsdl-org/SDL
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL2_DISABLE_UNINSTALL ON)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(SDL)
set_target_properties(SDL2-static PROPERTIES FOLDER "ext")
set_target_properties(SDL2main PROPERTIES FOLDER "ext")
set_target_properties(sdl_headers_copy PROPERTIES FOLDER "ext")

# https://github.com/ocornut/imgui
find_package(OpenGL REQUIRED)
add_library(imgui_sdl2
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_sdl2 PUBLIC imgui imgui/backends)
target_compile_definitions(imgui_sdl2 PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(imgui_sdl2 SDL2::SDL2 SDL2main ${OPENGL_LIBRARY})
set_target_properties(imgui_sdl2 PROPERTIES FOLDER "ext")

