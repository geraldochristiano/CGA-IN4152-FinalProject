#include "shader.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

static constexpr GLuint invalid = 0xFFFFFFFF;

static bool checkShaderErrors(GLuint shader);
static bool checkProgramErrors(GLuint program);
static std::string readFile(std::filesystem::path filePath);

Shader::Shader(GLuint program)
    : m_program(program)
{
}

Shader::Shader()
    : m_program(invalid)
{
}

Shader::Shader(Shader&& other)
{
    m_program = other.m_program;
    other.m_program = invalid;
}

Shader::~Shader()
{
    if (m_program != invalid)
        glDeleteProgram(m_program);
}

Shader& Shader::operator=(Shader&& other)
{
    if (m_program != invalid)
        glDeleteProgram(m_program);

    m_program = other.m_program;
    other.m_program = invalid;
    return *this;
}

void Shader::bind() const
{
    assert(m_program != invalid);
    glUseProgram(m_program);
}

void Shader::bindUniformBlock(const std::string& blockName, GLuint bindingLocation) const
{
    GLuint blockIdx = glGetUniformBlockIndex(m_program, blockName.data());
    glUniformBlockBinding(m_program, blockIdx, bindingLocation);
}

ShaderBuilder::~ShaderBuilder()
{
    freeShaders();
}

ShaderBuilder& ShaderBuilder::addStage(GLuint shaderStage, std::filesystem::path shaderFile, const std::string& prependedString)
{
    if (!std::filesystem::exists(shaderFile)) {
        throw ShaderLoadingException(fmt::format("File {} does not exist", shaderFile.string().c_str()));
    }

    std::string shaderSource = readFile(shaderFile);
    if (!prependedString.empty()) { 
        if (size_t from = shaderSource.find("//$define_string"); from != std::string::npos) {
            shaderSource.replace(from, 16, prependedString + "\n");
        }    
    }
    
    const GLuint shader = glCreateShader(shaderStage);
    const char* shaderSourcePtr = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderSourcePtr, nullptr);
    glCompileShader(shader);
    if (!checkShaderErrors(shader)) {
        glDeleteShader(shader);
        throw ShaderLoadingException(fmt::format("Failed to compile shader {}", shaderFile.string().c_str()));
    }

    m_shaders.push_back(shader);
    return *this;
}

Shader ShaderBuilder::build()
{
    // Combine vertex and fragment shaders into a single shader program.
    GLuint program = glCreateProgram();
    for (GLuint shader : m_shaders)
        glAttachShader(program, shader);
    glLinkProgram(program);
    freeShaders();

    if (!checkProgramErrors(program)) {
        throw ShaderLoadingException("Shader program failed to link");
    }

    return Shader(program);
}

void ShaderBuilder::freeShaders()
{
    for (GLuint shader : m_shaders)
        glDeleteShader(shader);
}

static std::string readFile(std::filesystem::path filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static bool checkShaderErrors(GLuint shader)
{
    // Check if the shader compiled successfully.
    GLint compileSuccessful;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccessful);

    // If it didn't, then read and print the compile log.
    if (!compileSuccessful) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::string logBuffer;
        logBuffer.resize(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, nullptr, logBuffer.data());

        std::cerr << logBuffer << std::endl;
        return false;
    } else {
        return true;
    }
}

static bool checkProgramErrors(GLuint program)
{
    // Check if the program linked successfully
    GLint linkSuccessful;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccessful);

    // If it didn't, then read and print the link log
    if (!linkSuccessful) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::string logBuffer;
        logBuffer.resize(static_cast<size_t>(logLength));
        glGetProgramInfoLog(program, logLength, nullptr, logBuffer.data());

        std::cerr << logBuffer << std::endl;
        return false;
    } else {
        return true;
    }
}

GLint Shader::getUniformLocation(const std::string& uniformName) const
{
    if (m_uniformLocCache.find(uniformName) != m_uniformLocCache.end()) {
        return m_uniformLocCache[uniformName];
    }

    GLint location = glGetUniformLocation(m_program, uniformName.c_str());
    if (location == -1)
        std::cout << "location for " << uniformName << " is invalid" << std::endl;
    m_uniformLocCache[uniformName] = location;
    return location;
}

void Shader::setUniformMatrix4(const std::string& uniformName, const glm::mat4& value, bool transpose) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniformMatrix4fv(m_program, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformMatrix3(const std::string& uniformName, const glm::mat3& value, bool transpose) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniformMatrix3fv(m_program, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformVec3(const std::string& uniformName, const glm::vec3& value) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniform3f(m_program, location, value.x, value.y, value.z);
}

void Shader::setUniformVec4(const std::string& uniformName, const glm::vec4& value) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniform4f(m_program, location, value.x, value.y, value.z, value.w);
}

void Shader::setUniformFloat(const std::string& uniformName, const float& value) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniform1f(m_program, location, value);
}

void Shader::setUniformInt(const std::string& uniformName, int value) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniform1i(m_program, location, value);
}

void Shader::setUniformBool(const std::string& uniformName, bool value) const
{
    GLint location = getUniformLocation(uniformName);
    glProgramUniform1i(m_program, location, value ? GL_TRUE : GL_FALSE);
}



