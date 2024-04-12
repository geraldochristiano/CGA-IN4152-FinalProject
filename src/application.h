#pragma once
#include "light.h"
#include "gpumesh.h"
#include "camera.h"
#include "texture.h"
#include "skybox.h"
#include "reflectionmap.h"
#include <map>

class Application {
public:
    Application();
    void update();

private:
    void registerCallbacks();
    void loadMeshes();
    void loadShaders();
    void loadTextures();
    void initLights();
    //void initEnvironmentMap();

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods);

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods);

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos);

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods);

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods);


   /* void drawImGUI();*/
private:
    Window m_window;
    Camera m_mainCamera;
    Skybox m_skybox;
    //std::map<GPUMesh*, ReflectionMap> m_environmentMap;

    // Shaders
    Shader m_defaultShader;
    Shader m_shadowShader;
    Shader m_skyboxShader;
    Shader m_reflectionMapShader;
    Shader m_refractionMapShader;
    Shader m_pointLightBlinnPhongShader;
    
    enum class DrawingMode {
        Opaque,
        ReflectionMap,
        RefractionMap,
    };

    enum class MeshType {
        Static,
        Dynamic
    };

    std::vector<std::tuple<GPUMesh, DrawingMode, glm::mat4, MeshType>> m_gpuMeshes;

    std::vector<PointLight> m_pointLights;
    Texture m_texture;

    Texture *m_texture2;
    bool m_useMaterial{ true };

    // Projection and view matrices for you to fill in and use
    /*glm::mat4 m_modelMatrix{ 1.0f };*/
};

Mesh flatNormalUnitCube();
