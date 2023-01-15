#pragma once

#include <glm/glm.hpp>

struct Ray {

	glm::vec3 origin;
	glm::vec3 direction;

};

struct RayPayload {

	glm::vec3 hitPosition;
	glm::vec3 hitNormal;

	float hitDistance = -1.0f;
	int hittableIndex = -1;

};