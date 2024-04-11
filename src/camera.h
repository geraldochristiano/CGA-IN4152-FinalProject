#pragma once
#include "glm/glm.hpp"
#include "framework/window.h"
#include "abstraction.h"

class Camera {
private: //Data to construct projection matrix
	Radian m_fovy;
	float m_width;
	float m_height;
	float m_nearDistance;
	float m_farDistance;
	glm::mat4 m_projectionMat; // Cache projection matrix, recalculate when necessary

	void recalcProjectionMatrix();

private:
	static constexpr glm::vec3 s_yAxis{ 0, 1, 0 };
	glm::vec3 m_camPosition{ 0 };
	glm::vec3 m_camForward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_camUp{ 0.0f, 1.0f, 0.0f };
	Window* m_window;

	glm::dvec2 m_lastCursorPos{ 0 };

public:
	float m_translationSpeed = 0.02f;
	float m_rotationSpeed = 0.002f;

public:
	Camera(Window& p_window, const glm::vec3& pos, const glm::vec3& focusAt,
		Degree fovy, float nearPlaneDist, float farPlaneDist);

	inline glm::mat4 viewMatrix() const;
	inline glm::mat4 projectionMatrix() const;
	glm::mat4 viewProjMatrix() const;
	glm::vec3 position() const;

	void updateInput();

	void cameraWindowResizeCallback(const glm::ivec2& size);

};



