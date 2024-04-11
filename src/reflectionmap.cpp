#include "reflectionmap.h"
#include <glm/ext/matrix_clip_space.hpp>

ReflectionMap::ReflectionMap(GLuint textureUnit, GLsizei cubemapTextureSize, const glm::vec3& position) 
    : m_textureUnit(textureUnit)
    , m_cameraPos(position)
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_cubemapTexture);
    glBindTextureUnit(m_textureUnit, m_cubemapTexture);
    glTextureStorage2D(m_cubemapTexture, 1, GL_RGBA8, cubemapTextureSize, cubemapTextureSize);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glCreateFramebuffers(1, &m_framebuffer);
    glCreateRenderbuffers(1, &m_depthBuffer);
    glNamedRenderbufferStorage(m_depthBuffer, GL_DEPTH_COMPONENT, cubemapTextureSize, cubemapTextureSize);

    glNamedFramebufferRenderbuffer(m_framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
    glNamedFramebufferTexture(m_framebuffer, GL_COLOR_ATTACHMENT0, m_cubemapTexture, 0);
}


GLuint ReflectionMap::getFramebuffer()
{
    return m_framebuffer;
}

glm::mat4 ReflectionMap::reflMapViewProj()
{
    return glm::perspective(ReflectionMap::cubeFOV, 1.0f, ReflectionMap::nearPlane, ReflectionMap::farPlane);
}

ReflectionMap::~ReflectionMap()
{   
    glDeleteRenderbuffers(1, &m_depthBuffer);
    glDeleteTextures(1, &m_cubemapTexture);
    glDeleteFramebuffers(1, &m_framebuffer);
}
