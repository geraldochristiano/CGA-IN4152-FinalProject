#include "abstraction.h"
#include "gpumesh.h"
#include "light.h"

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
#include <framework/mesh.h>
#include <functional>
#include <iostream>
#include <vector>
#include "application.h"
#include <tinyobjloader/tiny_obj_loader.h>
const int reflMapCubemapTextureUnit = 1;
const int skyboxCubemapTextureUnit = 2;
const glm::vec3 cameraStartingPosition = { 0,0,-3 };
const glm::vec3 cameraOriginalFocus = { 0,0,0 };

Application::Application()
    : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL45)
    , m_texture("resources/checkerboard.png")
    , m_mainCamera(m_window, cameraStartingPosition, cameraOriginalFocus, 50.0f, 0.1f, 30.0f)
    , m_skybox("resources/skybox/front.jpg",
            "resources/skybox/left.jpg",
            "resources/skybox/right.jpg",
            "resources/skybox/back.jpg",
            "resources/skybox/top.jpg",
            "resources/skybox/bottom.jpg",
            skyboxCubemapTextureUnit)
{
    
    registerCallbacks();
    loadMeshes();
    loadTextures();
    loadShaders();
    initLights();
    auto a = std::make_shared<Image>("resources/textures/Brick/brick-wall_normal-ogl.png");
    m_texture2 = new Texture(a.get());
    //glm::vec3 m_points[4] = { glm::vec3(3, 3, 3), glm::vec3(0, 1, 2) , glm::vec3(3, -2, 1) , glm::vec3(-3, -3, -3) };
    //std::cout << m_texture2;
    //prepareShaders();
    //dictionary.append("key", Texture("resources/checkerboard.png"));
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



void Application::loadTextures() {
    return; // m_gpuMeshes.
}

void Application::onKeyPressed(int key, int mods)
{
    if (key == GLFW_KEY_SPACE) {
        m_pointLights.push_back(PointLight(m_mainCamera.position(), { 1.0f,0.1f,0.1f }, {0.0, 0.6, 1.0}));
    }
}

void Application::onKeyReleased(int key, int mods)
{
    //std::cout << "Key released: " << key << std::endl;
}

void Application::onMouseMove(const glm::dvec2& cursorPos)
{
    //std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
}

void Application::onMouseClicked(int button, int mods)
{
    //std::cout << "Pressed mouse button: " << button << std::endl;
}

void Application::onMouseReleased(int button, int mods)
{
    //std::cout << "Released mouse button: " << button << std::endl;
}

void Application::loadMeshes() {
    Mesh dragon = mergeMeshes(loadMesh("resources/cube.obj")); 
    const Material& dragonMat = dragon.material;
    std::printf("Material: kd: %f, %f, %f, \nks: %f, %f, %f, \nshininess: %f, \ntransp: %f\n", dragonMat.kd.x, dragonMat.kd.y, dragonMat.kd.z, dragonMat.ks.x, dragonMat.ks.y, dragonMat.ks.z, dragonMat.shininess, dragonMat.transparency);
    
    Mesh unitCube = flatNormalUnitCube();
    const Material& cubeMat = unitCube.material;
    std::printf("Material: kd: %f, %f, %f, \nks: %f, %f, %f, \nshininess: %f, \ntransp: %f\n", cubeMat.kd.x, cubeMat.kd.y, cubeMat.kd.z, cubeMat.ks.x, cubeMat.ks.y, cubeMat.ks.z, cubeMat.shininess, cubeMat.transparency);
    
    m_gpuMeshes.emplace_back(dragon, DrawingMode::Opaque, glm::mat4(1.0f), MeshType::Static);

    m_gpuMeshes.emplace_back(unitCube, DrawingMode::ReflectionMap, glm::translate(glm::mat4(1.0f), {0, 0, 3}), MeshType::Static);
}


void Application::loadShaders() {
    try {
        ShaderBuilder defaultBuilder;
        defaultBuilder.addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl");
        defaultBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl");
        m_defaultShader = defaultBuilder.build();
        
        ShaderBuilder shadowBuilder;
        shadowBuilder.addStage(GL_VERTEX_SHADER, "shaders/shadow_vert.glsl");
        m_shadowShader = shadowBuilder.build();

        ShaderBuilder skyboxBuilder;
        skyboxBuilder.addStage(GL_VERTEX_SHADER, "shaders/skybox_vert.glsl");
        skyboxBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/skybox_frag.glsl");
        m_skyboxShader = skyboxBuilder.build();

        ShaderBuilder pointLightBlinnPhongBuilder;
        pointLightBlinnPhongBuilder.addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl");
        pointLightBlinnPhongBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/blinnphong_frag.glsl", "#define LIGHT_TYPE POINT_LIGHT");
        m_pointLightBlinnPhongShader = pointLightBlinnPhongBuilder.build();

        ShaderBuilder reflectionMapBuilder;
        reflectionMapBuilder.addStage(GL_VERTEX_SHADER, "shaders/reflectionmap_vert.glsl");
        reflectionMapBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/reflectionmap_frag.glsl");
        m_reflectionMapShader = reflectionMapBuilder.build();

        ShaderBuilder refractionMapBuilder;
        refractionMapBuilder.addStage(GL_VERTEX_SHADER, "shaders/reflectionmap_vert.glsl");
        refractionMapBuilder.addStage(GL_FRAGMENT_SHADER, "shaders/refractionmap_frag.glsl");
        m_refractionMapShader = refractionMapBuilder.build();
    }
    catch (ShaderLoadingException e) {
        std::cerr << e.what() << std::endl;
    }
}

void Application::initLights() {
    m_pointLights.push_back(PointLight(glm::vec3{ 3,3,3 }, glm::vec3{ 1,1,1 }, {0.1, 0.6, 1.0}));
}



void Application::update()
{   
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    auto prevTime = high_resolution_clock::now();

    int dummyInteger = 0;

    glEnable(GL_CULL_FACE); // Cull face, free optimization

    while (!m_window.shouldClose()) {

        m_window.updateInput();
        m_mainCamera.updateInput();

        // Use ImGui for easy input/output of ints, floats, strings, etc...
        ImGui::Begin("Window");
        ImGui::DragInt("This is an integer input", &dummyInteger); // Use ImGui::DragInt or ImGui::DragFloat for larger range of numbers.
        ImGui::Text("Value is: %i", dummyInteger); // Use C printf formatting rules (%i is a signed integer)
        ImGui::Checkbox("Use material if no texture", &m_useMaterial);
        ImGui::SliderFloat("Camera speed", &m_mainCamera.m_translationSpeed, 0.01f, 2.0f);
        ImGui::End();

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        for (auto& gpumesh : m_gpuMeshes) {
            GPUMesh& mesh = std::get<0>(gpumesh);
            const glm::mat4& modelMatrix = std::get<2>(gpumesh);
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

            //Shader& currentShader = m_defaultShader;
            //currentShader.bind();
            //currentShader.setUniformMatrix4("viewProjMatrix", m_mainCamera.viewProjMatrix());
            //currentShader.setUniformMatrix4("modelMatrix", m_modelMatrix);
            //currentShader.setUniformMatrix3("normalModelMatrix", normalModelMatrix);
           
            //if (mesh.hasTextureCoords()) {
            //    m_texture.bind(GL_TEXTURE0);
            //    //glUniform1i(3, 0);
            //    //glUniform1i(4, GL_TRUE);
            //    //glUniform1i(5, GL_FALSE);
            //    currentShader.setUniformInt("colorMap", 0);
            //    currentShader.setUniformBool("hasTexCoords", true);
            //    currentShader.setUniformBool("useMaterial", false);
            //} else {
            //    //glUniform1i(4, GL_FALSE);
            //    //glUniform1i(5, m_useMaterial);
            //    currentShader.setUniformBool("hasTexCoords", false);
            //    currentShader.setUniformBool("useMaterial", m_useMaterial);
            //}
            //mesh.draw(currentShader);

            switch (std::get<1>(gpumesh)) {
            case DrawingMode::Opaque: {
                glEnable(GL_BLEND); // Additive blending for multiple light sources

                // RENDERING POINT LIGHT
                Shader& currentShader = m_pointLightBlinnPhongShader;
                currentShader.bind();
                currentShader.setUniformMatrix4("viewProjMatrix", m_mainCamera.viewProjMatrix());
                currentShader.setUniformMatrix4("modelMatrix", modelMatrix);
                currentShader.setUniformMatrix3("normalModelMatrix", normalModelMatrix);
                currentShader.setUniformVec3("viewPos", m_mainCamera.position());
                currentShader.setUniformBool("useMaterial", true);
                currentShader.setUniformBool("useBlinnPhong", true);

                if (mesh.hasTextureCoords()) {
                    //std::cout << mesh.getTex();
                    //m_texture.bind(GL_TEXTURE0);
                    currentShader.setUniformBool("hasTexCoords", true);
                    mesh.kdTex->bind(GL_TEXTURE0);
                    currentShader.setUniformInt("kdtex", 0);
                    mesh.normalTex->bind(GL_TEXTURE1);
                    currentShader.setUniformInt("normaltex", 1);
                    mesh.roughnessTex->bind(GL_TEXTURE2);
                    currentShader.setUniformInt("roughnesstex", 2);
                }

                for (int i = 0; i < m_pointLights.size(); i++) {
                    // With opaque mesh, don't blend with background and applies ambient lighting only for the first light
                    if (i == 0) {
                        currentShader.setUniformBool("applyAmbientLighting", true);
                        glDepthFunc(GL_LESS);
                        glBlendFunc(GL_ONE, GL_ZERO);
                    }
                    else {
                        currentShader.setUniformBool("applyAmbientLighting", false);
                        glDepthFunc(GL_EQUAL);
                        glBlendFunc(GL_ONE, GL_ONE);
                    }

                    PointLight& pointLight = m_pointLights[i];
                    currentShader.setUniformVec3("lightColor", pointLight.lightColor);
                    currentShader.setUniformVec3("lightIntensities", pointLight.lightIntensities);
                    currentShader.setUniformVec3("lightPosition", pointLight.position);
                    mesh.draw(currentShader);
                }

                glDisable(GL_BLEND);
                break;
            }
            case DrawingMode::ReflectionMap: {
                Shader& currentShader = m_reflectionMapShader;
                currentShader.bind();
                currentShader.setUniformMatrix4("viewProjMatrix", m_mainCamera.viewProjMatrix());
                currentShader.setUniformMatrix4("modelMatrix", modelMatrix);
                currentShader.setUniformMatrix3("normalModelMatrix", normalModelMatrix);
                currentShader.setUniformVec3("viewPos", m_mainCamera.position());
                currentShader.setUniformInt("skybox", skyboxCubemapTextureUnit);

                mesh.draw(currentShader);
                break;
            }
            case DrawingMode::RefractionMap: {
                Shader& currentShader = m_refractionMapShader;
                currentShader.bind();
                currentShader.setUniformMatrix4("viewProjMatrix", m_mainCamera.viewProjMatrix());
                currentShader.setUniformMatrix4("modelMatrix", modelMatrix);
                currentShader.setUniformMatrix3("normalModelMatrix", normalModelMatrix);
                currentShader.setUniformVec3("viewPos", m_mainCamera.position());
                currentShader.setUniformInt("skybox", skyboxCubemapTextureUnit);
                currentShader.setUniformFloat("IOR", 1.00 / 1.52); // Air to glass

                mesh.draw(currentShader);
                break; 
            }
            default:
                break;
            }

        }

        // RENDERING SKYBOX
        glm::mat4 skyboxViewProj = m_mainCamera.projectionMatrix() * glm::mat4(glm::mat3(m_mainCamera.viewMatrix()));
        m_skybox.draw(m_skyboxShader,  skyboxViewProj);
        
        // Processes input and swaps the window buffer
        m_window.swapBuffers();
        
        //auto currTime = high_resolution_clock::now();
        //duration<double, std::milli> diff = currTime - prevTime;
        //std::cout << diff.count() << " ms" << std::endl;
        //prevTime = currTime;   
        //glm::vec3 a = glm::vec3(0.0f);
        glm::vec3 m_points[10] = { 
            glm::vec3(3, 3, 3), 
            glm::vec3(0, 1, 2) , glm::vec3(3, -2, 1) , glm::vec3(-3, -3, -3),
            glm::vec3(-3, -3, -3) , glm::vec3(3, -2, 1) , glm::vec3(2, 1, -3),
            glm::vec3(2, 1, -3) , glm::vec3(0, 1, 2) , glm::vec3(3, 3, 3)
        };
        m_pointLights[0].position = splines((dummyInteger*1.0f)/250.0f, m_points, 10);
        std::cout << splines(0.0f, m_points, 4).r;
        
    }
}

Mesh flatNormalUnitCube() {
    Mesh out;
    out.vertices = { // 8 vertices, each vertex repeated x3 with different normals
        // v1: - X, -Y, -Z (index 0,1,2)
        Vertex{glm::vec3{-0.5, -0.5, -0.5}, glm::vec3{-1, 0, 0}}, Vertex{glm::vec3{-0.5, -0.5, -0.5}, glm::vec3{0, -1, 0}}, Vertex{glm::vec3{-0.5, -0.5, -0.5}, glm::vec3{0, 0, -1} },
        // v2: X, -Y, -Z (index 3,4,5)
        Vertex{glm::vec3{0.5, -0.5, -0.5}, glm::vec3{1, 0, 0}}, Vertex{glm::vec3{0.5, -0.5, -0.5}, glm::vec3{0, -1, 0}}, Vertex{glm::vec3{0.5, -0.5, -0.5}, glm::vec3{0, 0, -1} },
        // v3: X, Y, -Z (index 6,7,8)
        Vertex{glm::vec3{0.5, 0.5, -0.5}, glm::vec3{1, 0, 0}}, Vertex{glm::vec3{0.5, 0.5, -0.5}, glm::vec3{0, 1, 0}}, Vertex{glm::vec3{0.5, 0.5, -0.5}, glm::vec3{0, 0, -1} },
        // v4: -X, Y, -Z (index 9,10,11)
        Vertex{glm::vec3{-0.5, 0.5, -0.5}, glm::vec3{-1, 0, 0}}, Vertex{glm::vec3{-0.5, 0.5, -0.5}, glm::vec3{0, 1, 0}}, Vertex{glm::vec3{-0.5, 0.5, -0.5}, glm::vec3{0, 0, -1} },
        // v5: - X, -Y, Z (index 12,13,14)
        Vertex{glm::vec3{-0.5, -0.5, 0.5}, glm::vec3{-1, 0, 0}}, Vertex{glm::vec3{-0.5, -0.5, 0.5}, glm::vec3{0, -1, 0}}, Vertex{glm::vec3{-0.5, -0.5, 0.5}, glm::vec3{0, 0, 1} },
        // v6: X, -Y, Z (index 15,16,17)
        Vertex{glm::vec3{0.5, -0.5, 0.5}, glm::vec3{1, 0, 0}}, Vertex{glm::vec3{0.5, -0.5, 0.5}, glm::vec3{0, -1, 0}}, Vertex{glm::vec3{0.5, -0.5, 0.5}, glm::vec3{0, 0, 1} },
        // v7: X, Y, Z (index 18,19,20)
        Vertex{glm::vec3{0.5, 0.5, 0.5}, glm::vec3{1, 0, 0}}, Vertex{glm::vec3{0.5, 0.5, 0.5}, glm::vec3{0, 1, 0}}, Vertex{glm::vec3{0.5, 0.5, 0.5}, glm::vec3{0, 0, 1} },
        // v8: -X, Y, Z (index 21,22,23)
        Vertex{glm::vec3{-0.5, 0.5, 0.5}, glm::vec3{-1, 0, 0}}, Vertex{glm::vec3{-0.5, 0.5, 0.5}, glm::vec3{0, 1, 0}}, Vertex{glm::vec3{-0.5, 0.5, 0.5}, glm::vec3{0, 0, 1} },
    };

    out.triangles = { 
        // Back
        glm::uvec3{11u, 5u, 2u}, glm::uvec3{11u, 8u, 5u},
        // Right
        glm::uvec3{6u, 15u, 3u}, glm::uvec3{6u, 18u, 15u},
        // Front
        glm::uvec3{14u, 17u, 23u}, glm::uvec3{17u, 20u, 23u},
        // Left
        glm::uvec3{9u, 0u, 12u}, glm::uvec3{9u, 12u, 21u},
        // Top
        glm::uvec3{7u, 10u, 22u}, glm::uvec3{19u, 7u, 22u},
        // Bottom
        glm::uvec3{1u, 4u, 13u}, glm::uvec3{13u, 4u, 16u},
    };
    return out;
}

//Cubic bezier
glm::vec3 Application::bezier3(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    float it = 1.0f - t;
    return (it * it * it * p0) + (3 * t * it * it * p1) + (3 * t * t * it * p2) + (t * t * t * p3);
}


glm::vec3 Application::splines(float t, glm::vec3* points, int size) {
    if (size < 4) {
        return glm::vec3(0.0f);
    }
    if ((size - 4) % 3 != 0) {
        return glm::vec3(0.0f);
    }
    int nSplines = (size - 1) / 3;
    float tval = fmod(t, nSplines);
    int index = (int)floor(tval);

    


    return bezier3((tval - index), points[0 + index * 3], points[1 + index * 3], points[2 + index * 3], points[3 + index * 3]);
}



int main()
{
    Application app;
    app.update();

    return 0;
}

