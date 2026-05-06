# Real-Time-3D-Graphics-Engine
Real-time 3D rendering engine built with C++ and WebGPU.
# Real-Time 3D Graphics Engine

## Overview

Real-time 3D graphics engine developed in C++ using WebGPU.

This project demonstrates GPU-based rendering, hierarchical transformations, shader programming, texture mapping, and interactive graphics systems through multiple real-time simulations.

The engine was developed as part of advanced computer graphics coursework focused on real-time rendering systems and graphics pipeline architecture.

---

## Features

- Real-time GPU rendering
- Hierarchical transformations
- Shader programming with WGSL
- Texture mapping
- Interactive camera movement
- Solar system simulation
- Analog clock simulation
- Frame-rate independent animation
- Object rotation and orbital systems

---

## Solar System Simulation
  <img width="1280" height="720" alt="transform1_test" src="https://github.com/user-attachments/assets/f02adb6a-a8f3-4a23-ba72-20bc8a221778" />

Implemented a hierarchical transformation system using push_matrix() and pop_matrix() to create parent-child relationships between celestial bodies.

### Included Objects

- Sun
- Venus
- Earth
- Moon



### Functionality

- Planet orbit systems
- Self-axis rotation
- Texture-based rendering
- Real-time animation updates
- Camera movement using keyboard controls

---

## Analog Clock Simulation
<img width="1280" height="720" alt="transform2_test" src="https://github.com/user-attachments/assets/5567c2b5-063a-45aa-b444-199282a69748" />


Developed a real-time analog clock using system time and transformation-based rendering.

### Included Features

- Dynamic hour, minute, and second hands
- Circular second markers
- Quarter markers and hour markers
- Rotational transformations
- Animated clock face
- Central sphere axis

---

## Technologies Used

- C++
- WebGPU
- WGSL Shaders
- CMake
- Graphics Programming
- Real-Time Rendering

---

## Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run the executable from the build directory after compilation.

---

## Concepts Implemented

- Transformation matrices
- Hierarchical modeling
- GPU rendering pipeline
- Shader-based rendering
- Real-time animation
- Texture mapping
- Camera systems
- Frame-rate independent motion

---

## Project Structure

- transform1_test.cpp → Solar system simulation
- transform2_test.cpp → Analog clock simulation
- shader.wgsl → GPU shader programs
- textures/ → Planet textures and assets
  

---
