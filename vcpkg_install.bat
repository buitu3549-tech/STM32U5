@echo off
set "PATH=C:\Program Files\CMake\bin;%PATH%"
set "VCPKG_ROOT=E:\vcpkg"
set "WindowsSdkDir=C:\Program Files (x86)\Windows Kits\10"
set "WindowsSdkVersion=10.0.22621.0"
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64;%PATH%"
set "PATH=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64;%PATH%"

E:\vcpkg\vcpkg.exe install qtbase:x64-windows qtdeclarative:x64-windows portaudio:x64-windows onnxruntime:x64-windows spdlog:x64-windows
