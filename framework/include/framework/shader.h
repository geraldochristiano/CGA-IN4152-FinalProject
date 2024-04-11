#pragma once
#include "disable_all_warnings.h"
#include "opengl_includes.h"
DISABLE_WARNINGS_PUSH()
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <vector>
#include <unordered_map>

struct ShaderLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Shader {
public:
    Shader();
    Shader(const Shader&) = delete;
    Shader(Shader&&);
    ~Shader();

    Shader& operator=(Shader&&);

    // ... Feel free to add more methods here (e.g. for setting uniforms or keeping track of texture units) ...
    void bind() const;

    void setUniformMatrix4(const std::string& uniformName, const glm::mat4& value, bool transpose = false) const;
    void setUniformMatrix3(const std::string& uniformName, const glm::mat3& value, bool transpose = false) const;
    void setUniformVec4(const std::string& uniformName, const glm::vec4& value) const;
    void setUniformVec3(const std::string& uniformName, const glm::vec3& value) const;
    void setUniformFloat(const std::string& uniformName, const float& value) const;
    void setUniformInt(const std::string& uniformName, int value) const;
    void setUniformBool(const std::string& uniformName, bool value) const;

    // Bind the uniform define by the given name to the given location in its assigned block
    void bindUniformBlock(const std::string& blockName, GLuint bindingLocation) const;


private:
    friend class ShaderBuilder;
    Shader(GLuint program);
    GLint getUniformLocation(const std::string& uniformName) const;

private:
    GLuint m_program;
    mutable std::unordered_map<std::string, GLint> m_uniformLocCache;
};

class ShaderBuilder {
public:
    ShaderBuilder() = default;
    ShaderBuilder(const ShaderBuilder&) = delete;
    ShaderBuilder(ShaderBuilder&&) = default;
    ~ShaderBuilder();

    ShaderBuilder& addStage(GLuint shaderStage, std::filesystem::path shaderFile, const std::string& prependedString = "");
    Shader build();

private:
    void freeShaders();

private:
    std::vector<GLuint> m_shaders;
};
