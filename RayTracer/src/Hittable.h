#pragma once

#include <glm/glm.hpp>

#include "Ray.h"

struct Hittable {

	glm::vec3 position{ 0.0f };
	int materialIndex = 0;

};

struct Sphere : Hittable {
	
	float radius = 1.0f;

};