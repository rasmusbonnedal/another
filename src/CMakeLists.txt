add_executable(another
    memlist.cpp
    memlist.h
    main.cpp
    unpack.c
    sound.cpp
    sound.h
)

target_compile_definitions(another PRIVATE _CRT_SECURE_NO_WARNINGS=1)
target_link_libraries(another imgui_sdl2)
