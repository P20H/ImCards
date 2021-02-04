cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(ImCards)


set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Install/")



add_subdirectory(ThirdParty/ImFramework)


add_executable(ImCards
	Source/Main.cpp
)

target_link_libraries(ImCards PUBLIC
    ImFramework
)

target_include_directories(ImCards PRIVATE 
	ThirdParty/imgui_markdown
)



install(TARGETS ImCards
	CONFIGURATIONS Debug
        RUNTIME
                DESTINATION "${CMAKE_SOURCE_DIR}/Install/Debug"
)

install(TARGETS ImCards
	CONFIGURATIONS Release
        RUNTIME
                DESTINATION "${CMAKE_SOURCE_DIR}/Install/Release"
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Install")

