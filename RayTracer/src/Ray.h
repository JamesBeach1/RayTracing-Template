#pragma once

#include <glm/glm.hpp>

struct Ray {

	glm::vec3 origin;
	glm::vec3 direction;

};

struct RayPayload {

	glm::vec3 hitPosition{ -1.0f };
	glm::vec3 hitNormal{ -1.0f };

	float hitDistance = -1.0f;
	int hittableIndex = -1;
	bool frontFace = true;

	inline void setFaceNormal(const Ray& ray, const glm::vec3& outwardNormal) {
		frontFace = glm::dot(ray.direction, outwardNormal) < 0.0f;
		hitNormal = frontFace ? outwardNormal : -outwardNormal;
	}

};