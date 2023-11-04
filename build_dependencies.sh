#!/bin/bash

# Define the directory where you want to store the built libraries
DEPENDENCIES_DIR="dependencies"

# Create the dependencies directory if it doesn't exist
mkdir -p "$DEPENDENCIES_DIR"

# Build and install GLEW
echo "Building GLEW..."
cd external/glew
make
make install PREFIX="../$DEPENDENCIES_DIR/glew"
cd ../..

# Build GLFW
echo "Building GLFW..."
cd external/glfw
cmake .
make
make install DESTDIR="../$DEPENDENCIES_DIR/glfw"
cd ../..

# Build GLM (no build step required, just copy the headers)
echo "Copying GLM headers..."
cp -r external/glm/glm "$DEPENDENCIES_DIR/glm"

# Copy STB header files
echo "Copying STB headers..."
cp -r external/stb "$DEPENDENCIES_DIR/stb"

echo "Build and installation complete."
