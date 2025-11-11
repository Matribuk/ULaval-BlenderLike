#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofImage.h"
#include <string>
#include <iostream>
#include <unordered_map>

class ResourceManager {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        ofMesh& loadMesh(std::string path);
        ofTexture& loadTexture(std::string path);
        ofTexture& storeTexture(const std::string& name, const ofTexture& texture);
        ofShader& loadShader(std::string vertexPath, std::string fragmentPath);
        std::string getMeshPath(ofMesh& target);
        std::string getTexturePath(ofTexture& target);
        std::string getShaderPath(ofShader& target);
    private:
        std::unordered_map<std::string, ofTexture> _textures;
        std::unordered_map<std::string, ofShader> _shaders;
        std::unordered_map<std::string, ofMesh> _meshes;

};
