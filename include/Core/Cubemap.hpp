#pragma once

#include "ofMain.h"
#include <string>

#include "ofImage.h"
#include <iostream>

class Cubemap {
    public:
        Cubemap();
        ~Cubemap();

        bool load(
            const std::string& right,
            const std::string& left,
            const std::string& top,
            const std::string& bottom,
            const std::string& front,
            const std::string& back
        );

        bool loadFromFolder(const std::string& folderPath, const std::string& extension = "jpg");
        void bind(int textureUnit = 0) const;
        void unbind() const;
        bool isLoaded() const;
        GLuint getTextureID() const;
        bool allocateRenderTarget(int size);
        void beginFaceRender(GLenum face);
        void endFaceRender();
        int getSize() const { return _size; }

    private:
        GLuint _textureID;
        bool _loaded;
        GLuint _fbo = 0;
        GLuint _rbo = 0;
        int _size = 0;

        bool _loadFace(GLenum target, const std::string& path);
};
