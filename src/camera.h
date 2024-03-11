#pragma once
#include "glm/glm.hpp"
#include "framework/window.h"
#include "abstraction.h"


struct PerspectiveProjection {
	Radian m_fovy;
	float m_width;
	float m_height;
	float m_nearDistance;
	float m_farDistance;

	// Cache projection matrix, recalculate when necessary
	glm::mat4 m_projectionMat;
	void recalcProjectionMatrix();
};

class Camera {

private:
	glm::vec3 m_camPosition{ 0 };
	glm::vec3 m_camForward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_WorldUpAxis{ 0.0f, 1.0f, 0.0f };
	float m_translationSpeed = 0.02f;
	Radian m_rotationSpeed = glm::radians(5.0f);
	GLFWwindow* m_glfwPtr;

	PerspectiveProjection m_perspective;

	// Input handling
	double m_lastCursorPosX = 0;
	double m_lastCursorPosY = 0;
	bool firstMouseEnter = true;
	bool m_receiveInput = false;
	double m_currentYaw = - 90.0f;
	double m_currentPitch = 0;


public:
	Camera(Window& p_window, Degree fovy = 45.0f, float nearPlaneDist = 0.1f, float farPlaneDist = 100.0f);

	glm::mat4 viewMatrix();
	glm::mat4 projectionMatrix();
	glm::mat4 viewProjMatrix();

	void changeTranslationSpeed(float newTranslationSpeed);
	void changeRotationSpeed(Degree newRotationSpeed);

	void cameraKeyCallback(int key, int scancode, int action, int mods);
	void cameraMouseMoveCallback(const glm::vec2& cursorPos);
	void cameraMouseButtonCallback(int button, int action, int mods);
	void cameraWindowResizeCallback(const glm::ivec2& size);

};



