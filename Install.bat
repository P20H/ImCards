@echo off

cd Build

cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release --target install






