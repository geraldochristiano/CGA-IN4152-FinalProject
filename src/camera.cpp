#include "abstraction.h"
#include "camera.h"
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <framework/window.h>


Camera::Camera(Window& p_window, Degree fovy, float nearPlaneDist, float farPlaneDist)
{
    glm::ivec2 windowsize = p_window.getWindowSize();
    m_perspective = PerspectiveProjection(glm::radians(fovy), windowsize.x, windowsize.y, nearPlaneDist, farPlaneDist);
    m_perspective.recalcProjectionMatrix();
    m_glfwPtr = p_window.getGLFWWindow();

    // Register callbacks
    p_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
        cameraKeyCallback(key, scancode, action, mods);
        });
    p_window.registerMouseMoveCallback([this](const glm::vec2& cursorPos) {
        cameraMouseMoveCallback(cursorPos);
        });
    p_window.registerMouseButtonCallback([this](int button, int action, int mods) {
        cameraMouseButtonCallback(button, action, mods);
        });
}

void PerspectiveProjection::recalcProjectionMatrix() {
    m_projectionMat = glm::perspective(m_fovy, m_width / m_height, m_nearDistance, m_farDistance);
}

glm::mat4 Camera::projectionMatrix() {
    return m_perspective.m_projectionMat;
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(m_camPosition, m_camPosition + m_camForward, m_WorldUpAxis);
}

glm::mat4 Camera::viewProjMatrix() {
    return projectionMatrix() * viewMatrix();
}

void Camera::changeTranslationSpeed(float newTranslationSpeed) {
    m_translationSpeed = newTranslationSpeed;
}

void Camera::changeRotationSpeed(Degree newRotationSpeed) {
    m_rotationSpeed = glm::radians(newRotationSpeed);
}

void Camera::cameraKeyCallback(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) {
            m_camPosition += m_translationSpeed * m_camForward;
        };
        if (key == GLFW_KEY_A) {
            const glm::vec3 norm_left = -normalize(cross(m_camForward, m_WorldUpAxis));
            m_camPosition += m_translationSpeed * norm_left;
        };
        if (key == GLFW_KEY_S) {
            m_camPosition -= m_translationSpeed * m_camForward;
        }
        if (key == GLFW_KEY_D) {
            const glm::vec3 norm_right = normalize(cross(m_camForward, m_WorldUpAxis));
            m_camPosition += m_translationSpeed * norm_right;
        }
    }
}

void Camera::cameraMouseButtonCallback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        m_receiveInput = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        m_receiveInput = false;
        firstMouseEnter = true;
    }
}

void Camera::cameraMouseMoveCallback(const glm::vec2& cursorPos) {
    if (!m_receiveInput) return;

    if (firstMouseEnter) {
        m_lastCursorPosX = cursorPos.x;
        m_lastCursorPosY = cursorPos.y;
        firstMouseEnter = false;
        return;
    }

    double xoffset = cursorPos.x - m_lastCursorPosX;
    double yoffset = cursorPos.y - m_lastCursorPosY;
    m_lastCursorPosX = cursorPos.x;
    m_lastCursorPosY = cursorPos.y;

    xoffset *= m_rotationSpeed;
    yoffset *= m_rotationSpeed;

    m_currentYaw += xoffset;
    m_currentPitch += yoffset;

    // Limit pitch
    if (m_currentPitch > 89.0) {
        m_currentPitch = 89.0f;
    }
    else if (m_currentPitch < -89.0f) {
        m_currentPitch = -89.0f;
    }

    glm::vec3 dir = glm::vec3{
        cos(glm::radians(m_currentYaw)) * cos(glm::radians(m_currentPitch)),
        sin(glm::radians(m_currentPitch)),
        sin(glm::radians(m_currentYaw))* cos(glm::radians(m_currentPitch)) 
        };
    m_camForward = glm::normalize(dir);
}


