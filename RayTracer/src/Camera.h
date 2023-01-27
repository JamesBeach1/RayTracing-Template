#pragma once

#include <glm/glm.hpp>
#include <vector>


class Camera {

public:
	bool onUpdate();
	void OnResize(uint32_t width, uint32_t height);

	const glm::vec3& GetPosition() const { return cameraPosition; }
	const glm::vec3& GetFront() const { return cameraFront; }
	const glm::mat4& GetProjection() const { return projection; }
	const glm::mat4& GetInverseProjection() const { return inverseProjection; }
	const glm::mat4& GetView() const { return view; }
	const glm::mat4& GetInverseView() const { return inverseView; }

	const std::vector<glm::vec3>& GetRayDirections() const { return rayDirections; }

	bool mouseDisabled = false;


private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);		// Where is our camera
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);			// Camera front vector
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);				// Camera up vector
	glm::mat4 projection{ 1.0f };
	glm::mat4 view{ 1.0f };
	glm::mat4 inverseProjection{ 1.0f };
	glm::mat4 inverseView{ 1.0f };
	std::vector<glm::vec3> rayDirections;
	glm::vec2 lastMousePosition{ 0.0f, 0.0f };
	uint32_t viewportWidth = 0;
	uint32_t viewportHeight = 0;
	float verticalFOV = 45.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;
	float cameraSpeed = 0.05f;										// Camera movement speed
	float cameraRotateSensitivity = 0.3f;							// Camera Rotation Speed
	bool mouseHeld = false;

};