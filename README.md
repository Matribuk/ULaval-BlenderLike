# ULaval-BlenderLike

⚠️ **PROPRIETARY CODE - VIEWING ONLY**

This repository is for educational/portfolio purposes. The code is viewable but NOT licensed for use, modification, or distribution. See LICENSE file for details.

## 📖 About This Project

**ULaval-BlenderLike** is a comprehensive computer graphics application developed as an academic project for the course **IFT3100 Infographie** at Université Laval. The project implements a Blender-inspired 3D modeling and rendering environment, built from scratch using **C++** and **openFrameworks**.

This application demonstrates fundamental and advanced concepts in computer graphics, including 2D/3D primitives, scene management, lighting models, texturing, ray tracing, and interactive user interfaces.

---

## ✨ Key Features

### 🎨 **1. Import/Export & Color Management**
- **Image Import**: Load images interactively and display them in the scene
- **Image Export**: Export rendered scene sequences with customizable time intervals
- **Color Palette**: Custom palette with at least 4 colors for visual element styling

### 🖱️ **2. Tools and Interaction**
- **Dynamic Cursors**: 5+ cursor representations that change based on application state (draw, select, transform)
- **Vector Primitives**: Create 6 types of 2D primitives (point, line, square, rectangle, circle, triangle)
- **Interactive UI**: Visual feedback elements and controls for application state management

### 🧠 **3. Selection and Transformation**
- **Scene Graph**: Hierarchical organization of visual elements with add/delete/select/edit capabilities
- **Multi-Selection**: Select and modify multiple elements simultaneously
- **Transformations**: Interactive translation, rotation, and scaling

### 🧱 **4. 3D Geometry**
- **Bounding Boxes**: Automatic bounding box generation for all 3D models
- **Procedural Primitives**: Generate cubes and spheres procedurally
- **3D Model Import**: Support for OBJ, PLY, STL, and other 3D formats

### 🎥 **5. Cameras and Navigation**
- **Interactive Camera**: Intuitive orbit, pan, and zoom controls
- **Multiple Cameras**: Support for multiple cameras with individual attributes
- **Perspective & Orthographic**: Toggle between projection modes
- **Auto-Focus**: Automatic focus and framing on selected objects

### 🖼️ **6. Texturing**
- **UV Mapping**: Proper texture coordinates for 3D meshes
- **Cubemaps**: Environment mapping with cubemap textures
- **Procedural Textures**: Generate 2+ types of procedural textures

### 💡 **7. Classic Lighting**
- **Illumination Models**: Lambert and Phong shading models
- **Materials**: 4+ different material presets
- **Light Types**: Ambient, directional, point, and spotlight support

### 🔷 **8. Topology**
- **Triangulation**: Algorithm for complex 2D shape triangulation
- **Parametric Curves**: Bézier or Catmull-Rom splines with 5+ control points
- **Relief Effects**: Displacement and normal mapping for surface detail

### 🌟 **9. Ray Tracing**
- **Ray-Primitive Intersection**: Calculate intersections with spheres, boxes, and planes
- **Reflections**: Mirror-like reflective surfaces using ray tracing principles
- **Shadows**: Shadow casting with ray-traced shadow calculations

---

## 🏗️ Project Structure

```
ULaval-BlenderLike/
├── src/
│   ├── Algorithms/       # Geometric and rendering algorithms
│   ├── Components/       # Entity Component System components
│   ├── Core/            # Core application logic
│   ├── Events/          # Event handling system
│   ├── Manager/         # Resource and state managers
│   ├── Raytracing/      # Ray tracing implementation
│   ├── Systems/         # ECS systems
│   ├── UI/              # User interface components
│   ├── main.cpp         # Application entry point
│   └── ofApp.cpp        # Main application class
├── include/             # Header files
├── bin/                 # Compiled binaries
├── README_OBJECTIF.md   # Detailed feature checklist
├── devlog.md           # Development log
└── Makefile            # Build configuration
```

---

## 🛠️ Technologies Used

- **Language**: C++ (primary language composition)
- **Framework**: openFrameworks
- **Graphics API**: OpenGL
- **Build System**: Make

---

## 🚀 Getting Started

### Prerequisites
- C++ compiler (GCC/Clang)
- openFrameworks installed and configured
- OpenGL support

### Building the Project
```bash
# Clone the repository
git clone https://github.com/Matribuk/ULaval-BlenderLike.git

# Navigate to project directory
cd ULaval-BlenderLike

# Build using Make
make

# Run the application
make run
```

---

## 📋 Implementation Status

For a detailed breakdown of implemented features and progress, see [README_OBJECTIF.md](README_OBJECTIF.md).

**Completion Status**: 90%+ of planned features implemented
- ✅ All core features (import/export, primitives, transformations)
- ✅ Advanced 3D rendering (lighting, textures, materials)
- ✅ Ray tracing implementation
- ⏳ PBR materials (in progress)

---

## 📝 Development Log

The project's development journey, challenges, and solutions are documented in [devlog.md](devlog.md).

---

## 🎓 Academic Context

**Course**: IFT3100 - Infographie  
**Institution**: Université Laval  
**Project Type**: Comprehensive computer graphics application  
**Focus Areas**: 2D/3D graphics, rendering pipelines, interactive applications

---

## 📜 License

This code is proprietary and provided for viewing purposes only. No license is granted for use, modification, or distribution. See [LICENSE](LICENSE) for full details.

---

## 👥 Authors

- [@Matribuk](https://github.com/Matribuk)
- [@Richonn](https://github.com/Richonn)
- [@Krio18](https://github.com/Krio18)
- [@THORINKAUFFMANN](https://github.com/THORINKAUFFMANN)
- [@Maskalito](https://github.com/Maskalito)

---

## 🙏 Acknowledgments

- Université Laval - IFT3100 Infographie course
- openFrameworks community
- Computer graphics resources and references used during development