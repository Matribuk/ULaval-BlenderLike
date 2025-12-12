#include "Core/Cubemap.hpp"

Cubemap::Cubemap()
    : _textureID(0)
    , _loaded(false)
{}

Cubemap::~Cubemap()
{
    if (this->_loaded && this->_textureID != 0) {
        glDeleteTextures(1, &this->_textureID);
    }
}

bool Cubemap::load(
    const std::string& right,
    const std::string& left,
    const std::string& top,
    const std::string& bottom,
    const std::string& front,
    const std::string& back
)
{
    glGenTextures(1, &this->_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_textureID);

    bool success = true;
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, front);
    success &= this->_loadFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, back);

    if (success) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        this->_loaded = true;
    } else {
        std::cout << "[CUBEMAP] Failed to load one or more faces" << std::endl;
        glDeleteTextures(1, &this->_textureID);
        this->_textureID = 0;
        this->_loaded = false;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return success;
}

bool Cubemap::allocateRenderTarget(int size)
{
    if (this->_textureID != 0) {
        glDeleteTextures(1, &this->_textureID);
        this->_textureID = 0;
    }

    glGenTextures(1, &this->_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_textureID);

    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    if (this->_fbo == 0) glGenFramebuffers(1, &this->_fbo);
    if (this->_rbo == 0) glGenRenderbuffers(1, &this->_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->_size = size;
    this->_loaded = true;
    return true;
}

void Cubemap::beginFaceRender(GLenum face)
{
    if (!this->_loaded || this->_fbo == 0) return;

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, this->_textureID, 0);
    GLenum drawBufs[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBufs);
    glViewport(0, 0, this->_size, this->_size);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Cubemap::endFaceRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Cubemap::loadFromFolder(const std::string& folderPath, const std::string& extension)
{
    std::string right  = folderPath + "/right."  + extension;
    std::string left   = folderPath + "/left."   + extension;
    std::string top    = folderPath + "/top."    + extension;
    std::string bottom = folderPath + "/bottom." + extension;
    std::string front  = folderPath + "/front."  + extension;
    std::string back   = folderPath + "/back."   + extension;

    ofFile testFile(right);
    if (!testFile.exists()) {
        std::string altExt = (extension == "jpg") ? "png" : "jpg";
        right  = folderPath + "/right."  + altExt;
        left   = folderPath + "/left."   + altExt;
        top    = folderPath + "/top."    + altExt;
        bottom = folderPath + "/bottom." + altExt;
        front  = folderPath + "/front."  + altExt;
        back   = folderPath + "/back."   + altExt;
    }

    return this->load(right, left, top, bottom, front, back);
}

bool Cubemap::_loadFace(GLenum target, const std::string& path)
{
    ofImage img;
    if (!img.load(path)) {
        std::cout << "[CUBEMAP] Failed to load: " << path << std::endl;
        return false;
    }

    GLenum format;
    switch (img.getPixels().getNumChannels()) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            std::cout << "[CUBEMAP] Unsupported channel count: " << img.getPixels().getNumChannels() << std::endl;
            return false;
    }

    glTexImage2D(
        target,
        0,
        format,
        img.getWidth(),
        img.getHeight(),
        0,
        format,
        GL_UNSIGNED_BYTE,
        img.getPixels().getData()
    );

    return true;
}

void Cubemap::bind(int textureUnit) const
{
    if (this->_loaded) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->_textureID);
    }
}

void Cubemap::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

bool Cubemap::isLoaded() const
{
    return this->_loaded;
}

GLuint Cubemap::getTextureID() const
{
    return this->_textureID;
}
