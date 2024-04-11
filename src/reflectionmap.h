#pragma once
#include <framework/opengl_includes.h>
#include <glm/vec3.hpp>
#include "gpumesh.h"
#include <framework/shader.h>
#include "camera.h"

class ReflectionMap {
public:
    ReflectionMap(GLuint textureUnit, GLsizei cubemapTextureSize, const glm::vec3& position);

    // Non copy
    ReflectionMap(const ReflectionMap&) = delete;
    ReflectionMap& operator=(const ReflectionMap&) = delete;

    GLuint getFramebuffer();
    glm::mat4 reflMapViewProj();

    ~ReflectionMap();

private:
    GLuint m_framebuffer;
    GLuint m_cubemapTexture;
    GLuint m_textureUnit;
    GLuint m_depthBuffer;

    glm::vec3 m_cameraPos;

public:
    static constexpr float cubeFOV = glm::radians(90.0f);
    static constexpr float nearPlane = 0.1f;
    static constexpr float farPlane = 100.0f;
};