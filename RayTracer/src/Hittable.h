#pragma once

#include <glm/glm.hpp>

#include "Ray.h"

struct Hittable {

	glm::vec3 position{ 0.0f, 0.0f, -2.0f };
	int materialIndex = 0;


};

struct Sphere : Hittable {
	
	float radius = 0.5f;

};