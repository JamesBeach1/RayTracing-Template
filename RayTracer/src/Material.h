#pragma once

#include <glm/glm.hpp>

struct Material {

	glm::vec3 albedo{ 1.0f };
	float roughness = 0.0f;
	float metalness = 0.0f;

};
