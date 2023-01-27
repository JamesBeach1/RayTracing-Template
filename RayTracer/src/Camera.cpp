#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Walnut/Input/Input.h"
#include <GLFW/glfw3.h>

using namespace Walnut;

bool Camera::onUpdate() {


	bool moved = false;

	// camera movement controls
	if (Input::IsKeyDown(KeyCode::W)) {
		cameraPosition += cameraSpeed * cameraFront;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::S)) {
		cameraPosition -= cameraSpeed * cameraFront;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::A)) {
		cameraPosition -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::D)) {
		cameraPosition += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::Space)) {
		cameraPosition.y += cameraSpeed;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::LeftControl)) {
		cameraPosition.y -= cameraSpeed;
		moved = true;
	}

	if (!Input::IsMouseButtonDown(MouseButton::Left))
	{
		Input::SetCursorMode(CursorMode::Normal);
	}

	if (Input::IsMouseButtonDown(MouseButton::Left) && !mouseDisabled) {

		glm::vec2 mousePosition = Input::GetMousePosition();
		glm::vec2 mouseDifference = glm::clamp(mousePosition - lastMousePosition, -100.0f, 100.0f);
		glm::vec2 delta = mouseDifference * 0.002f;
		lastMousePosition = mousePosition;


		// hide cursor while LMB is down
		Input::SetCursorMode(CursorMode::Locked);


		// Rotation
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			float pitchDelta = delta.y * cameraRotateSensitivity;
			float yawDelta = delta.x * cameraRotateSensitivity;

			glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, glm::cross(cameraFront, cameraUp)),
				glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
			cameraFront = glm::rotate(q, cameraFront);

			moved = true;
		}

	}

	if (moved) {
		RecalculateView();
		RecalculateRayDirections();
	}

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height) {

	if (width == viewportWidth && height == viewportHeight) {
		return;
	}

	viewportWidth = width;
	viewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();

}

void Camera::RecalculateProjection() {

	projection = glm::perspectiveFov(glm::radians(verticalFOV), (float)viewportWidth, (float)viewportHeight, nearClip, farClip);
	inverseProjection = glm::inverse(projection);

}

void Camera::RecalculateView() {

	view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, glm::vec3(0, 1, 0));
	inverseView = glm::inverse(view);

}

void Camera::RecalculateRayDirections() {

	rayDirections.resize(viewportWidth * viewportHeight);

	for (uint32_t y = 0; y < viewportHeight; y++)
	{
		for (uint32_t x = 0; x < viewportWidth; x++)
		{
			glm::vec2 coord = { (float)x / (float)viewportWidth, (float)y / (float)viewportHeight };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 target = inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
			rayDirections[x + y * viewportWidth] = rayDirection;
		}
	}

}
