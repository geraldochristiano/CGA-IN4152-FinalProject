#include "abstraction.h"
#include "camera.h"
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <framework/window.h>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <glm/gtx/euler_angles.hpp>
#include <imgui/imgui.h>


Camera::Camera(Window& window, const glm::vec3& pos, const glm::vec3& focusAt, 
    Degree fovy, float nearPlaneDist, float farPlaneDist)
{
    m_window = &window;
    glm::ivec2 windowsize = window.getWindowSize();
    m_fovy = fovy;
    m_width = windowsize.x;
    m_height = windowsize.y;
    m_nearDistance = nearPlaneDist;
    m_farDistance = farPlaneDist;
    recalcProjectionMatrix();
    
    m_camPosition = pos;
    m_camForward = glm::normalize(focusAt - pos);
}

void Camera::recalcProjectionMatrix() {
    m_projectionMat = glm::perspective(glm::radians(m_fovy), m_width / m_height, m_nearDistance, m_farDistance);
}

inline glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_camPosition, m_camPosition + m_camForward, m_camUp);
}

inline glm::mat4 Camera::projectionMatrix() const {
    return m_projectionMat;
}

glm::mat4 Camera::viewProjMatrix() const {
    return projectionMatrix() * viewMatrix();
}

glm::vec3 Camera::position() const {
    return m_camPosition;
}

void Camera::updateInput(){
    const glm::vec3 right = glm::normalize(glm::cross(m_camForward, m_camUp));
    if (m_window->isKeyPressed(GLFW_KEY_A))
        m_camPosition -= m_translationSpeed * right;
    if (m_window->isKeyPressed(GLFW_KEY_D))
        m_camPosition += m_translationSpeed * right;
    if (m_window->isKeyPressed(GLFW_KEY_W))
        m_camPosition += m_translationSpeed * m_camForward;
    if (m_window->isKeyPressed(GLFW_KEY_S))
        m_camPosition -= m_translationSpeed * m_camForward;
    if (m_window->isKeyPressed(GLFW_KEY_Q))
        m_camPosition += m_translationSpeed * m_camUp;
    if (m_window->isKeyPressed(GLFW_KEY_E))
        m_camPosition -= m_translationSpeed * m_camUp;

    const glm::dvec2 cursorPos = m_window->getCursorPos();
    const glm::vec2 delta = m_rotationSpeed * glm::vec2(cursorPos - m_lastCursorPos);
    m_lastCursorPos = cursorPos;

    if (m_window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
        if (delta.x != 0.0f) {
            const glm::vec3 horAxis = glm::cross(s_yAxis, m_camForward);
            m_camForward = glm::normalize(glm::angleAxis(delta.x, s_yAxis) * m_camForward);
            m_camUp = glm::normalize(glm::cross(m_camForward, horAxis));
        }

        if (delta.y != 0.0f) {
            const glm::vec3 horAxis = glm::cross(s_yAxis, m_camForward);
            m_camForward = glm::normalize(glm::angleAxis(delta.y, horAxis) * m_camForward);
            m_camUp = glm::normalize(glm::cross(m_camForward, horAxis));
        }
    }

}


