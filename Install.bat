@echo off

cd Build

cmake -G "Visual Studio 16 2019" ..
cmake --build . --target install






