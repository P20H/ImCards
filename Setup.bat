@echo off

mkdir ThirdParty
cd ThirdParty

git clone --branch docking https://github.com/ocornut/imgui.git imgui
git clone --branch 2020.11-1 https://github.com/microsoft/vcpkg.git vcpkg
git clone https://github.com/CoreTrackProject/ImFramework.git
git clone https://github.com/juliettef/imgui_markdown.git

cd vcpkg

cmd /c call ./bootstrap-vcpkg.bat
cmd /c vcpkg.exe install glfw3 glew --triplet x64-windows

cd../..

mkdir Build
cd Build

cmake -G "Visual Studio 16 2019" ..






