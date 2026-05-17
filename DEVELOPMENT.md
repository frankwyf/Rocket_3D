# SkyRocket 3D - Developer Documentation

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Code Organization](#code-organization)
3. [Core Systems](#core-systems)
4. [Building and Compiling](#building-and-compiling)
5. [Debugging](#debugging)
6. [Performance Profiling](#performance-profiling)
7. [Adding New Features](#adding-new-features)
8. [Graphics Pipeline](#graphics-pipeline)
9. [Physics System](#physics-system)
10. [Troubleshooting](#troubleshooting)

## Architecture Overview

SkyRocket 3D follows a modular architecture with clear separation of concerns:

```
©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
©¦        Application Main Loop            ©¦
©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
                   ©¦
        ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
        ©¦                     ©¦
    ©°©¤©¤©¤¨‹©¤©¤©¤©¤©´          ©°©¤©¤©¤©¤¨‹©¤©¤©¤©¤©´
    ©¦ Graphics©¦          ©¦ Game    ©¦
    ©¦ System  ©¦          ©¦ Logic   ©¦
    ©¸©¤©¤©¤©Ð©¤©¤©¤©¤©¼          ©¸©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¼
        ©¦                     ©¦
©°©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
©¦                  ©¦                  ©¦           ©¦
¨‹                  ¨‹                  ¨‹           ¨‹
Shader          Camera           Physics      Input
System          System           System      System
```

## Code Organization

### Directory Structure

```
main/
©À©¤©¤ main.cpp              # Application entry point and main loop
©À©¤©¤ rocket.cpp/hpp        # Rocket model and rendering
©À©¤©¤ simple_mesh.cpp/hpp   # Basic mesh primitives (cylinder, cone, etc.)
©À©¤©¤ loadobj.cpp/hpp       # OBJ file parsing and loading
©À©¤©¤ load_texture.cpp/hpp  # Texture loading and management
©À©¤©¤ defaults.hpp          # Default constants and settings
©À©¤©¤ game_mechanics.hpp    # Game systems (particles, scoring, challenges)
©À©¤©¤ game_config.hpp       # Configuration management
©À©¤©¤ audio_system.hpp      # Audio system interface
©¸©¤©¤ ui_system.hpp         # UI and HUD components
```

## Core Systems

### 1. Rendering System

**Files:** `main.cpp`, `rocket.cpp`, shader programs

**Key Components:**
- VAO/VBO management for 3D meshes
- Texture binding and material system
- Shader compilation and linking
- Camera transformation matrices

**Example:**
```cpp
// Rendering a mesh
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, nullptr);
```

### 2. Physics System

**Files:** `main.cpp`, `rocket.cpp`

**Features:**
- Position and velocity tracking
- Gravity simulation
- Basic collision detection
- Movement constraints

**Example:**
```cpp
// Update rocket position
position += velocity * deltaTime;
velocity.y -= gravity * deltaTime;
```

### 3. Input System

**Files:** `main.cpp` (callback functions)

**Input Types:**
- Keyboard (W, A, S, D, SPACE, CTRL, SHIFT, Q, R, ESC)
- Mouse (movement, click)
- Window events (resize, close)

**Callback Structure:**
```cpp
void glfw_callback_key_(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_callback_motion_(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
```

### 4. Asset Loading System

**Files:** `loadobj.cpp`, `load_texture.cpp`

**Supported Formats:**
- OBJ (3D models)
- MTL (Material definitions)
- PNG, JPG (Textures)

**Loading Process:**
1. Parse file format
2. Create vertex/index buffers
3. Bind to VAO
4. Transfer to GPU

### 5. Game Mechanics System

**Files:** `game_mechanics.hpp`

**Subsystems:**
- **Particle Emitter:** Dynamic particle effects
- **Score Manager:** Game scoring and statistics
- **Power-up Manager:** Collectible power-up system
- **Challenge Manager:** Mission/objective system
- **Waypoint Navigator:** Path navigation

## Building and Compiling

### Prerequisites

- Visual Studio 2019 or later
- Windows 10/11 x64
- Standard C++17 compiler features

### Build Steps

```bash
# 1. Generate project files
premake5.exe vs2019

# 2. Open solution in Visual Studio
start SkyRocket3D.sln

# 3. Build configuration
# - Debug: Full debugging symbols, slower performance
# - Release: Optimizations enabled, stripped symbols

# 4. Command line build
msbuild SkyRocket3D.sln /p:Configuration=Release /p:Platform=x64
```

### Compiler Flags

- **Warning Level:** W4
- **C++ Standard:** C++17
- **Optimization:** O2 (Release)
- **Debug Info:** Full (Debug)

## Debugging

### Visual Studio Debugging

1. **Set Breakpoints:**
   - Click on line number in editor
   - Breakpoint appears as red circle

2. **Start Debugging:**
   - Press `F5` or Debug ¡ú Start Debugging
   - Application pauses at breakpoints

3. **Inspect Variables:**
   - Hover over variables to see values
   - Use Watch window (Debug ¡ú Windows ¡ú Watch)

4. **Step Through Code:**
   - `F10`: Step over
   - `F11`: Step into
   - `Shift+F11`: Step out

### OpenGL Debugging

**Debug Output:**
```cpp
#if !defined(NDEBUG)
    setup_gl_debug_output();
#endif
```

**Common Issues:**
- Shader compilation errors (check console)
- Texture binding problems (black/missing textures)
- Mesh rendering issues (invisible objects)

## Performance Profiling

### Frame Rate Monitoring

```cpp
// Simple FPS counter
static float frameTime = 0.0f;
static int frameCount = 0;
frameTime += deltaTime;
frameCount++;

if (frameTime > 1.0f)
{
    printf("FPS: %d\n", frameCount);
    frameCount = 0;
    frameTime = 0.0f;
}
```

### GPU Performance

- Monitor fill rate and vertex throughput
- Use NVIDIA/AMD profiling tools
- Check shader performance with GPU timers

### Memory Usage

- Monitor GPU memory allocation
- Track CPU memory usage
- Profile particle system impact

## Adding New Features

### Example: Add a New Rocket Type

1. **Create New Model File:**
   ```
   assets/models/rocket_variant.obj
   assets/textures/rocket_variant.png
   ```

2. **Load in main.cpp:**
   ```cpp
   SimpleMeshData rocketVariant = load_obj_file("assets/rocket_variant.obj");
   GLuint rocketVAO = bind_vao(rocketVariant);
   ```

3. **Add Rendering Code:**
   ```cpp
   // In main render loop
   glBindVertexArray(rocketVAO);
   glDrawElements(GL_TRIANGLES, rocketVertexCount, GL_UNSIGNED_INT, nullptr);
   ```

4. **Handle Selection:**
   ```cpp
   // Add to input handling
   if (key == GLFW_KEY_1) {
       currentRocket = ROCKET_STANDARD;
   } else if (key == GLFW_KEY_2) {
       currentRocket = ROCKET_VARIANT;
   }
   ```

### Example: Add a New Shader Effect

1. **Create Shader Files:**
   ```
   assets/shaders/effect.vert
   assets/shaders/effect.frag
   ```

2. **Compile Shader Program:**
   ```cpp
   ShaderProgram program("assets/shaders/effect.vert", 
                         "assets/shaders/effect.frag");
   ```

3. **Use in Rendering:**
   ```cpp
   program.use();
   glBindVertexArray(vao);
   glDrawElements(...);
   ```

## Graphics Pipeline

### Rendering Flow

```
Input Processing
    ¡ý
Camera Update
    ¡ý
Frustum Culling
    ¡ý
Render Scene
    ©À¡ú Render Rocket
    ©À¡ú Render Landing Pad
    ©À¡ú Render Particles
    ©¸¡ú Render UI
    ¡ý
Post-Processing (optional)
    ¡ý
Buffer Swap
```

### Shader Uniforms

Common uniforms used across shaders:
- `projection`: Camera projection matrix
- `view`: Camera view matrix
- `model`: Model transformation matrix
- `texture0`: Texture sampler
- `time`: Elapsed time (for animations)

## Physics System

### Collision Detection

Simple AABB (Axis-Aligned Bounding Box) collision:

```cpp
bool checkCollision(Vec3f pos1, Vec3f size1, Vec3f pos2, Vec3f size2)
{
    return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x) &&
           (pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y) &&
           (pos1.z < pos2.z + size2.z && pos1.z + size1.z > pos2.z);
}
```

### Velocity Update

```cpp
// Simple Euler integration
position += velocity * deltaTime;
velocity *= damping; // Air resistance
velocity.y -= gravity * deltaTime; // Gravity
```

## Troubleshooting

### Black Screen

**Causes:**
- Shaders not compiled
- Texture not bound
- Projection matrix incorrect

**Solution:**
1. Check shader compilation errors
2. Verify texture loading
3. Debug projection matrix

### Crashes on Startup

**Causes:**
- Asset files missing
- GLFW initialization failed
- GPU driver issues

**Solution:**
1. Verify asset files exist
2. Check GLFW error messages
3. Update GPU drivers

### Low Performance

**Causes:**
- Too many particles
- Inefficient shader
- Vertex count too high

**Solution:**
1. Reduce particle count
2. Profile and optimize shaders
3. Use LOD (Level of Detail)

### Input Not Working

**Causes:**
- Callback not registered
- Window not focused
- Key codes incorrect

**Solution:**
1. Verify callback registration
2. Check window focus
3. Verify key constants

## Additional Resources

- OpenGL Documentation: https://khronos.org/opengl/
- GLFW Documentation: https://www.glfw.org/
- C++ Reference: https://cppreference.com/
- Graphics Programming Books: "Real-Time Rendering", "Game Engine Architecture"

---

For more help, check the README.md or open an issue on GitHub.
