#include "skybox.h"
#include <framework/image.h>
#include <stb/stb_image.h>

Skybox::Skybox(const std::string& frontFileName, 
        const std::string& leftFileName, 
        const std::string& rightFileName, 
        const std::string& backFileName, 
        const std::string& topFileName, 
        const std::string& bottomFileName,
        GLuint textureUnit
    ) : m_textureUnit(textureUnit)
{   
    Image right{ rightFileName };
    Image left{ leftFileName };
    Image top{ topFileName };
    Image bottom{ bottomFileName };
    Image front{ frontFileName };
    Image back{ backFileName };

    auto getFormat = [](int channels) {return channels == 4 ? GL_RGBA : GL_RGB; };
    auto getInternalFormat = [](int channels) { return channels == 4 ? GL_RGBA8 : GL_RGB8; };
    
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_cubemapTexture);
    glBindTextureUnit(m_textureUnit, m_cubemapTexture);

    glTextureStorage2D(m_cubemapTexture, 1, getInternalFormat(front.channels), front.width, front.height);
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 0, right.width, right.height, 1, getFormat(right.channels), GL_UNSIGNED_BYTE, right.pixels.data());
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 1, left.width, left.height, 1, getFormat(left.channels), GL_UNSIGNED_BYTE, left.pixels.data());
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 2, top.width, top.height, 1, getFormat(top.channels), GL_UNSIGNED_BYTE, top.pixels.data());
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 3, bottom.width, bottom.height, 1, getFormat(bottom.channels), GL_UNSIGNED_BYTE, bottom.pixels.data());
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 4, front.width, front.height, 1, getFormat(front.channels), GL_UNSIGNED_BYTE, front.pixels.data());
    glTextureSubImage3D(m_cubemapTexture, 0, 0, 0, 5, back.width, back.height, 1, getFormat(back.channels), GL_UNSIGNED_BYTE, back.pixels.data());

    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemapTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
 
    // Set up VAO and VBO for positions
    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);
    glBindVertexArray(m_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
}

void Skybox::draw(Shader& shader, const glm::mat4& viewProjMatrix) {
    shader.bind();
    shader.setUniformMatrix4("viewProjMatrix", viewProjMatrix);
    shader.setUniformInt("skybox", m_textureUnit);
    
    GLint oldDepthMode;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthMode);

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(m_skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(oldDepthMode);
}

