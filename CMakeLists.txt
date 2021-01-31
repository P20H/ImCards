cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(ImCards)


set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Install/")


find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

set(IMGUI_VERSION "imgui")
set(IMGUI_SRC
    ThirdParty/${IMGUI_VERSION}/imgui.h
    ThirdParty/${IMGUI_VERSION}/imgui.cpp

    ThirdParty/${IMGUI_VERSION}/imgui_internal.h
    ThirdParty/${IMGUI_VERSION}/imconfig.h

    ThirdParty/${IMGUI_VERSION}/imgui_demo.cpp
    ThirdParty/${IMGUI_VERSION}/imgui_draw.cpp

    ThirdParty/${IMGUI_VERSION}/imgui_widgets.cpp
    ThirdParty/${IMGUI_VERSION}/imgui_tables.cpp

    ThirdParty/${IMGUI_VERSION}/imstb_rectpack.h 
    ThirdParty/${IMGUI_VERSION}/imstb_textedit.h
    ThirdParty/${IMGUI_VERSION}/imstb_truetype.h

    ThirdParty/${IMGUI_VERSION}/backends/imgui_impl_glfw.h
    ThirdParty/${IMGUI_VERSION}/backends/imgui_impl_glfw.cpp

    ThirdParty/${IMGUI_VERSION}/backends/imgui_impl_opengl3.h
    ThirdParty/${IMGUI_VERSION}/backends/imgui_impl_opengl3.cpp
)

add_library(ImGui ${IMGUI_SRC})
target_include_directories(ImGui
    PUBLIC
    ./ThirdParty/imgui
)
target_link_libraries(ImGui PRIVATE glfw GLEW::GLEW)


add_subdirectory(ThirdParty/ImFramework)
target_include_directories(ImFramework
    PUBLIC
    ./ThirdParty/imgui
)

add_executable(ImCards
	Source/Main.cpp
)

target_link_libraries(ImCards PUBLIC
    ImGui
    ImFramework
)

target_include_directories(ImCards PRIVATE 
	ThirdParty/imgui_markdown
)



install(TARGETS ImCards
        RUNTIME
                DESTINATION "${CMAKE_SOURCE_DIR}/Install/Debug"
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Install/Debug" )



#install(TARGETS ImCards glfw3 GLEW
#        CONFIGURATIONS Release
#        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/Install/Release)
