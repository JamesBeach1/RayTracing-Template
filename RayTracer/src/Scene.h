#pragma once

#include "Hittable.h"
#include "Material.h"

#include <vector>

struct scene {

	std::vector<Hittable> objects;
	std::vector<Material> materials;

};
