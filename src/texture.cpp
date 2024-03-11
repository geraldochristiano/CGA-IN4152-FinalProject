#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>

#include <iostream>

Texture::Texture(std::filesystem::path filePath)
{
    // Load image from disk to CPU memory.
    // Image class is defined in <framework/image.h>
    Image cpuTexture { filePath };

    // Create a texture on the GPU
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

    // Define GPU texture parameters and upload corresponding data based on number of image channels
    switch (cpuTexture.channels) {
        case 1:
            glTextureStorage2D(m_texture, 1, GL_R8, cpuTexture.width, cpuTexture.height);
            glTextureSubImage2D(m_texture, 0, 0, 0, cpuTexture.width, cpuTexture.height, GL_RED, GL_UNSIGNED_BYTE, cpuTexture.pixels.data());
            break;
        case 3:
            glTextureStorage2D(m_texture, 1, GL_RGB8, cpuTexture.width, cpuTexture.height);
            glTextureSubImage2D(m_texture, 0, 0, 0, cpuTexture.width, cpuTexture.height, GL_RGB, GL_UNSIGNED_BYTE, cpuTexture.pixels.data());
            break;
        case 4:
            glTextureStorage2D(m_texture, 1, GL_RGBA8, cpuTexture.width, cpuTexture.height);
            glTextureSubImage2D(m_texture, 0, 0, 0, cpuTexture.width, cpuTexture.height, GL_RGBA, GL_UNSIGNED_BYTE, cpuTexture.pixels.data());
            break;
        default:
            std::cerr << "Number of channels read for texture is not supported" << std::endl;
            throw std::exception();
    }

    // Generate mip-maps
    glGenerateTextureMipmap(m_texture);

    // Set behavior for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set interpolation for texture sampling (bilinear interpolation across mip-maps).
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(Texture&& other)
    : m_texture(other.m_texture)
{
    other.m_texture = INVALID;
}

Texture::~Texture()
{
    if (m_texture != INVALID)
        glDeleteTextures(1, &m_texture);
}

void Texture::bind(GLint textureSlot)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
