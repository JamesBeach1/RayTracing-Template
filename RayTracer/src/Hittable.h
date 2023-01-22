#pragma once

#include <glm/glm.hpp>

#include "Ray.h"

class Hittable {
public:

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	int materialIndex = 0;

	virtual RayPayload Hit(Ray& ray, int idx) { 
		RayPayload payload;
		return payload; 
	}
};

class Sphere : public Hittable {
public:
	float radius = 0.5f;

	virtual RayPayload Hit(Ray& ray, int idx) override {

		RayPayload payload;

		glm::vec3 origin = ray.origin - position;

		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - radius * radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f) {
			return payload;
		}

		float hitDist0 = (-b + glm::sqrt(discriminant)) / (2 * a);
		float hitDist1 = (-b - glm::sqrt(discriminant)) / (2 * a);

		float closestHit = glm::min(hitDist0, hitDist1);

		glm::vec3 hitPoint = glm::vec3(origin + ray.direction * closestHit);
		glm::vec3 normal = glm::normalize(hitPoint);

		payload.hitDistance = closestHit;
		payload.hitPosition = hitPoint + position;
		payload.hitNormal = normal;
		payload.hittableIndex = idx;
		payload.setFaceNormal(ray, normal);

		return payload;

	}

};