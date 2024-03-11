//#include "Image.h"
#include "abstraction.h"
#include "gpumesh.h"
#include "texture.h"
#include "camera.h"
// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>
#include "application.h"

Application::Application()
    : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL45)
    , m_texture("resources/checkerboard.png")
    , m_mainCamera(m_window)
{
    registerCallbacks();
    loadMeshes();
    prepareShaders();
}

void Application::registerCallbacks() {
    m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS)
            onKeyPressed(key, mods);
        else if (action == GLFW_RELEASE)
            onKeyReleased(key, mods);
        });
    m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
    m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
        if (action == GLFW_PRESS)
            onMouseClicked(button, mods);
        else if (action == GLFW_RELEASE)
            onMouseReleased(button, mods);
        });
}

void Application::onKeyPressed(int key, int mods)
{
    std::cout << "Key pressed: " << key << std::endl;
}

void Application::onKeyReleased(int key, int mods)
{
    std::cout << "Key released: " << key << std::endl;
}

void Application::onMouseMove(const glm::dvec2& cursorPos)
{
    std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
}

void Application::onMouseClicked(int button, int mods)
{
    std::cout << "Pressed mouse button: " << button << std::endl;
}

void Application::onMouseReleased(int button, int mods)
{
    std::cout << "Released mouse button: " << button << std::endl;
}

void Application::loadMeshes() {
    m_meshes = GPUMesh::loadMeshGPU("resources/dragon.obj");
}

void Application::prepareShaders() {
    try {
        ShaderBuilder defaultBuilder;
        defaultBuilder.addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl");
        defaultBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl");
        m_defaultShader = defaultBuilder.build();

        ShaderBuilder shadowBuilder;
        shadowBuilder.addStage(GL_VERTEX_SHADER, "shaders/shadow_vert.glsl");
        m_shadowShader = shadowBuilder.build();

        // Any new shaders can be added below in similar fashion.
        // ==> Don't forget to reconfigure CMake when you do!
        //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
        //     VS Code: ctrl + shift + p => CMake: Configure => enter
        // ....
    }
    catch (ShaderLoadingException e) {
        std::cerr << e.what() << std::endl;
    }
}

void Application::update()
{
    int dummyInteger = 0; // Initialized to 0
    while (!m_window.shouldClose()) {

        m_window.updateInput();

        // Use ImGui for easy input/output of ints, floats, strings, etc...
        ImGui::Begin("Window");
        ImGui::InputInt("This is an integer input", &dummyInteger); // Use ImGui::DragInt or ImGui::DragFloat for larger range of numbers.
        ImGui::Text("Value is: %i", dummyInteger); // Use C printf formatting rules (%i is a signed integer)
        ImGui::Checkbox("Use material if no texture", &m_useMaterial);
        ImGui::End();

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ...
        glEnable(GL_DEPTH_TEST);

        const glm::mat4 mvpMatrix = m_mainCamera.viewProjMatrix() * m_modelMatrix;
        // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
        // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
        const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));

        for (GPUMesh& mesh : m_meshes) {
            m_defaultShader.bind();
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
            glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
            if (mesh.hasTextureCoords()) {
                m_texture.bind(GL_TEXTURE0);
                glUniform1i(3, 0);
                glUniform1i(4, GL_TRUE);
                glUniform1i(5, GL_FALSE);
            } else {
                glUniform1i(4, GL_FALSE);
                glUniform1i(5, m_useMaterial);
            }
            mesh.draw(m_defaultShader);
        }

        // Processes input and swaps the window buffer
        m_window.swapBuffers();
    }
}


int main()
{
    Application app;
    app.update();

    return 0;
}
