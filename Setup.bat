@echo off

if not exist ThirdParty (
     mkdir ThirdParty
)
cd ThirdParty

if not exist vcpkg (
     git clone --branch 2020.11-1 https://github.com/microsoft/vcpkg.git vcpkg
     cd vcpkg

     cmd /c call ./bootstrap-vcpkg.bat
     cmd /c vcpkg.exe install glfw3 glew --triplet x64-windows

     cd..
)

if not exist ImFramework (
     git clone https://github.com/CoreTrackProject/ImFramework.git
) else (
     cd ImFramework
     git pull
     cd ..
)

if not exist imgui_markdown (
     git clone https://github.com/juliettef/imgui_markdown.git
) else (
     cd imgui_markdown
     git pull
     cd ..
)

cd ..





mkdir Build
cd Build

cmake -G "Visual Studio 16 2019" ..






