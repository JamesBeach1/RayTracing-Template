#pragma once

#include "Hittable.h"
#include "Material.h"

#include <vector>

struct Scene {

	std::vector<Hittable> objects;
	std::vector<Material> materials;

};
