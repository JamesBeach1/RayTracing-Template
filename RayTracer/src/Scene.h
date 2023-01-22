#pragma once

#include "Hittable.h"
#include "Material.h"

#include <vector>
#include <glm/glm.hpp>


struct Scene {

	glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));
	glm::vec3 skyColour = { 0.57f, 0.7f, 0.9f };

	std::vector<Hittable*> objects;
	std::vector<Material*> materials;

};
