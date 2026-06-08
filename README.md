# 🛏️ 3D Bedroom Simulation using OpenGL

A fully interactive 3D Bedroom environment built using C++ and the OpenGL/GLUT library. This project showcases advanced computer graphics techniques, including custom 3D object modeling, real-time lighting control, dynamic camera positioning, and interactive animations.

---

## 🚀 Project Overview
This project simulates a modern bedroom with detailed interior components. Users can navigate through the 3D space, interact with the environment, toggle multiple light sources, change room aesthetics (wall and floor colors), and experience a dynamic **Night Mode** with animated moon phases.

---

## 🎮 Keyboard Controls & Interaction

Run the application and use the following keys to interact with the 3D room:

### 🎥 Camera & Navigation
* `w` / `s` : Move Camera Forward / Backward
* `a` / `d` : Move Camera Left / Right
* `q` / `e` : Move Camera Up / Down
* `i` / `j` / `k` / `l` : Look Around (Pitch & Yaw)

### 🎨 Customization (Presets)
* `c` : Cycle Wall Colors (8 presets: Warm, Blue, Green, Purple, Orange, Pink, Teal, Linen)
* `f` : Cycle Floor Textures/Colors (4 presets: Wood, Dark Wood, Marble, Gray)

### 🌙 Night Mode & Moon Animations
* `y` : Toggle Night Mode (Turns off main lights, activates dark ambiance)
* `k` : Toggle Moon Animation (Rotate / Slide outside the window)
* `p` : Cycle Moon Phases (Full Moon, Half Moon, Crescent Moon)

### 💡 Lighting Control
* `1` : Toggle Ceiling Light 1  |  `4` / `5` / `6` : Toggle Ambient / Diffuse / Specular of Light 1
* `2` : Toggle Ceiling Light 2  |  `7` / `8` / `9` : Toggle Ambient / Diffuse / Specular of Light 2
* `3` : Toggle Bedside Lamp     |  `e` / `r` / `t` : Toggle Ambient / Diffuse / Specular of Lamp

* `ESC` : Quit the application

---

## 🛠️ Tech Stack & Requirements
* **Language:** C++
* **Graphics API:** OpenGL
* **Utility Toolkit:** GLUT (FreeGLUT)
* **Mathematical Operations:** Standard Math Libraries (for sin/cos wave animations)

---

## 📂 Key Features Implemented
1. **Hierarchical 3D Modeling:** Custom-built 3D furniture including a bed, wardrobe, bedside table, windows, and ceiling structures using basic OpenGL primitives.
2. **Advanced Lighting & Shading:** Implements multiple independent light sources (positional and directional) with adjustable ambient, diffuse, and specular components.
3. **Dynamic Camera Control:** A smooth first-person style camera control matrix for seamless room exploration.
4. **Interactive Texturing & Materials:** Real-time material property switching to simulate different architectural surfaces like marble, wood, and paint.

---

## 🔧 How to Compile and Run

### On Windows (using Code::Blocks or Visual Studio)
1. Install **FreeGLUT** and configure the include (`GL/glut.h`) and library paths.
2. Link the following libraries in your compiler build settings:
   ```text
   -lfreeglut -lopengl32 -lglu32
