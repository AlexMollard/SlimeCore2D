@echo off
setlocal

REM Define the dependencies directory
set "DEPENDENCIES_DIR=dependencies"

REM Create the dependencies directory if it doesn't exist
if not exist "%DEPENDENCIES_DIR%" mkdir "%DEPENDENCIES_DIR%"

REM Build GLEW using Makefile
echo Building GLEW...
cd external\glew
mingw32-make

REM Copy GLEW library and header files
xcopy /E /Y bin\Release\Win32\glew32.dll ..\..\%DEPENDENCIES_DIR%\glew
xcopy /E /Y include\GL ..\..\%DEPENDENCIES_DIR%\glew\include\GL

cd ..\..

REM Build GLFW using CMake
echo Building GLFW...
cd external\glfw
mkdir build
cd build
cmake -G "MinGW Makefiles" -DGLFW_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX=../../%DEPENDENCIES_DIR%/glfw ..
mingw32-make
mingw32-make install

REM Copy GLFW library and header files
xcopy /E /Y src\Release\glfw3.dll ..\..\..\%DEPENDENCIES_DIR%\glfw
xcopy /E /Y include\GL ..\..\..\%DEPENDENCIES_DIR%\glfw\include\GL

cd ..\..

echo Build and installation complete.
endlocal
