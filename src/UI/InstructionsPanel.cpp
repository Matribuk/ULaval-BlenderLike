#include "UI/InstructionsPanel.hpp"

void InstructionsPanel::render()
{
    if (ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Application Guide");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Toolbar", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Select - Entity selection mode");
            ImGui::BulletText("Move - Camera movement mode");
            ImGui::BulletText("Ortho/Persp - Toggle orthographic/perspective projection");
            ImGui::BulletText("Import - Import files (images/3D models)");
            ImGui::BulletText("Export - Export scene or image sequences");
            ImGui::BulletText("Raytracing - Enable/disable raytracing renderer");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Mouse Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "SELECT Mode:");
            ImGui::BulletText("Left click - Select entity");
            ImGui::BulletText("CTRL + Left click - Multiple selection");
            ImGui::Spacing();

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "MOVE Mode:");
            ImGui::BulletText("Left click + drag - Pan camera laterally");
            ImGui::BulletText("Middle click + drag - Rotate camera");
            ImGui::BulletText("Right click + drag - Move camera forward/backward");
            ImGui::Spacing();

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "Assets Panel:");
            ImGui::BulletText("Drag & Drop - Place asset in scene at mouse position");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Keyboard Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("P - Toggle Ortho/Perspective projection");
            ImGui::BulletText("Z - Move camera forward");
            ImGui::BulletText("Q - Move camera left");
            ImGui::BulletText("S - Move camera backward");
            ImGui::BulletText("D - Move camera right");
            ImGui::BulletText("F - Toggle fullscreen mode");
            ImGui::BulletText("SPACE - Move camera up");
            ImGui::BulletText("SHIFT - Move camera down");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("3D Primitives")) {
            ImGui::BulletText("Cube - Create a cube");
            ImGui::BulletText("Sphere - Create a sphere");
            ImGui::BulletText("Plane - Create a plane");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Materials & Shaders")) {
            ImGui::BulletText("Lambert - Simple diffuse shader");
            ImGui::BulletText("Phong - Shader with specular highlights");
            ImGui::BulletText("Rainbow - Animated rainbow shader");
            ImGui::BulletText("Mirror - Mirror shader with cubemap reflections");
            ImGui::BulletText("Normal Mapping - Shader with normal map support");
            ImGui::BulletText("Procedural Texture - Perlin noise generation");
            ImGui::BulletText("Reflectivity - Control reflection intensity");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Lights")) {
            ImGui::BulletText("Point Light - Omnidirectional light source");
            ImGui::BulletText("Directional Light - Directional light (sun)");
            ImGui::BulletText("Spot Light - Cone-shaped light");
            ImGui::BulletText("Settings: Color, intensity, position, direction");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Skybox")) {
            ImGui::BulletText("Load cubemap from folder");
            ImGui::BulletText("Dynamic cubemap for reflections");
            ImGui::BulletText("Visible in both OpenGL and Raytracing modes");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Raytracing")) {
            ImGui::BulletText("Realistic rendering with shadows and reflections");
            ImGui::BulletText("Scene lights support");
            ImGui::BulletText("BVH acceleration for meshes");
            ImGui::BulletText("Materials: Lambertian, Metal, Dielectric");
            ImGui::BulletText("Skybox sampling for background");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Curves")) {
            ImGui::BulletText("Bezier - Cubic Bezier curves");
            ImGui::BulletText("Catmull-Rom - Interpolating curves");
            ImGui::BulletText("Add/remove control points");
            ImGui::BulletText("Parameters: Segments, closure");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Delaunay")) {
            ImGui::BulletText("2D Delaunay triangulation");
            ImGui::BulletText("Voronoi diagram");
            ImGui::BulletText("Random points generator");
            ImGui::BulletText("Triangle and cell visualization");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Transformation")) {
            ImGui::BulletText("Position (X, Y, Z)");
            ImGui::BulletText("Rotation (Pitch, Yaw, Roll)");
            ImGui::BulletText("Scale (X, Y, Z)");
            ImGui::BulletText("Bounding box visualization");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Import/Export")) {
            ImGui::BulletText("Import: Images (PNG, JPG), 3D Models (OBJ, FBX, etc.)");
            ImGui::BulletText("Export: Meshes in OBJ format");
            ImGui::BulletText("Export: Animated image sequences");
            ImGui::BulletText("Drag & drop from Assets Panel");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Tools")) {
            ImGui::BulletText("Eyedropper - Color picker tool");
            ImGui::BulletText("Color Picker - Color selection");
            ImGui::BulletText("Scene Manager - Entity management");
            ImGui::BulletText("Event Log - Event history");
            ImGui::Spacing();
        }

        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Tip:");
        ImGui::TextWrapped("Explore the different panels to discover all features!");
        ImGui::Spacing();
    }
    ImGui::End();
}