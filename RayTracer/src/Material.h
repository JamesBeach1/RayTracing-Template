#pragma once

#include "Ray.h"

#include <glm/glm.hpp>

class Material {
public:
	glm::vec3 albedo{ 1.0f };

	virtual glm::vec3 scatter(Ray& ray, RayPayload payload) {
		return glm::vec3(-1.0f);
	}

};

class Lambertian : public Material {

	virtual glm::vec3 scatter(Ray& ray, RayPayload payload) override {
		return albedo;
	}

};
