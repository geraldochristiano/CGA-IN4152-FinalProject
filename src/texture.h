#pragma once
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>

struct ImageLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Texture {
public:
    Texture(std::filesystem::path filePath);
    Texture(const Texture&) = delete;
    Texture(Texture&&);
    ~Texture();

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = default;

    void bind(GLint textureSlot);

private:
    static constexpr GLuint INVALID = 0xFFFFFFFF;
    GLuint m_texture { INVALID };
};
