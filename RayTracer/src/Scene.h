#pragma once

#include "Hittable.h"
#include "Material.h"

#include <vector>

struct Scene {

	std::vector<Sphere> objects;
	std::vector<Material> materials;

};
