#include "Manager/ResourceManager.hpp"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ofMesh& ResourceManager::loadMesh(std::string path) {

    if (this->_meshes.find(path) == this->_meshes.end()) {
        ofxAssimpModelLoader model;
        ofMesh mesh;

        model.load(path);
        mesh = model.getMesh(0);
        size_t vcount = mesh.getNumVertices();
        size_t tcount = mesh.getNumTexCoords();
        ofVec3f minb(FLT_MAX, FLT_MAX, FLT_MAX), maxb(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto &v : mesh.getVertices()) {
            minb.x = std::min(minb.x, v.x);
            minb.y = std::min(minb.y, v.y);
            minb.z = std::min(minb.z, v.z);
            maxb.x = std::max(maxb.x, v.x);
            maxb.y = std::max(maxb.y, v.y);
            maxb.z = std::max(maxb.z, v.z);
        }
        ofLogNotice("ResourceManager") << "Loaded mesh '" << path << "' vertices=" << vcount << " texcoords=" << tcount
                                       << " bounds=[(" << minb.x << "," << minb.y << "," << minb.z << ") - ("
                                       << maxb.x << "," << maxb.y << "," << maxb.z << ")]";

        float targetSize = 5.0f;
        glm::vec3 size = glm::vec3(maxb.x - minb.x, maxb.y - minb.y, maxb.z - minb.z);
        float maxDim = std::max({size.x, size.y, size.z});
        if (maxDim > 0.0f) {
            float scaleFactor = (maxDim != 0.f) ? (targetSize / maxDim) : 1.0f;
            if (fabs(scaleFactor - 1.0f) > 1e-6) {
                ofLogNotice("ResourceManager") << "Scaling mesh '" << path << "' by factor " << scaleFactor << " to fit targetSize=" << targetSize;
                auto &verts = mesh.getVertices();
                for (auto &v : verts) {
                    v.x *= scaleFactor;
                    v.y *= scaleFactor;
                    v.z *= scaleFactor;
                }

                minb = ofVec3f(FLT_MAX, FLT_MAX, FLT_MAX);
                maxb = ofVec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                for (auto &v : mesh.getVertices()) {
                    minb.x = std::min(minb.x, v.x);
                    minb.y = std::min(minb.y, v.y);
                    minb.z = std::min(minb.z, v.z);
                    maxb.x = std::max(maxb.x, v.x);
                    maxb.y = std::max(maxb.y, v.y);
                    maxb.z = std::max(maxb.z, v.z);
                }
            }
        }

        if (tcount == 0 && vcount > 0) {
            ofLogWarning("ResourceManager") << "Mesh has no texcoords - generating planar UVs (XZ projection)";
            mesh.clearTexCoords();
            float spanX = maxb.x - minb.x;
            float spanZ = maxb.z - minb.z;
            if (spanX <= 0.f) spanX = 1.f;
            if (spanZ <= 0.f) spanZ = 1.f;
            for (auto &v : mesh.getVertices()) {
                float u = (v.x - minb.x) / spanX;
                float vcoord = (v.z - minb.z) / spanZ;
                mesh.addTexCoord(ofVec2f(u, vcoord));
            }
        }
        this->_meshes[path] = mesh;
    }
    return this->_meshes[path];
}

ofTexture& ResourceManager::loadTexture(std::string path) {

    ofDisableArbTex();
    auto it = this->_textures.find(path);
    if (it != this->_textures.end()) {
        return it->second;
    }

    ofImage img;
    if (!img.load(path)) {
        ofLogError() << "Failed to load texture: " << path;
        static ofTexture dummy;
        return dummy;
    }

    auto [insertedIt, success] = this->_textures.emplace(path, ofTexture());
    ofTexture& tex = insertedIt->second;

    tex.allocate(img.getWidth(), img.getHeight(), GL_RGBA);
    tex.loadData(img.getPixels());

    return tex;
}

std::string ResourceManager::getTexturePath(ofTexture& target) {
    for (const auto& [path, tex] : this->_textures) {
        if (&tex == &target) {
            return path;
        }
    }
    return "";
}

std::string ResourceManager::getShaderPath(ofShader& target) {
    for (const auto& [path, shader] : this->_shaders) {
        if (&shader == &target) {
            return path;
        }
    }
    return "";
}

std::string ResourceManager::getMeshPath(ofMesh& target) {
    for (const auto& [path, mesh] : this->_meshes) {
        if (&mesh == &target) {
            return path;
        }
    }
    return "";
}

ofShader& ResourceManager::loadShader(std::string vertexPath, std::string fragmentPath) {
    std::string key = vertexPath + "|" + fragmentPath;

    if (this->_shaders.find(key) == this->_shaders.end()) {
        ofShader shader;
        shader.setupShaderFromFile(GL_VERTEX_SHADER, vertexPath);
        shader.setupShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath);

        shader.bindDefaults();
        shader.linkProgram();
        if (!shader.isLoaded()) {
            ofLogError() << "Shader failed to load!";
        }
        this->_shaders[key] = shader;
    }

    return this->_shaders[key];
}
