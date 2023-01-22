#pragma once

#include "Ray.h"

#include <glm/glm.hpp>
#include <Walnut/Random.h>

namespace Utils {
	static glm::vec3 random_in_unit_sphere() {
		while (true) {
			auto p = Walnut::Random::Vec3(-1.0f, 1.0f);
			if (glm::dot(p, p) >= 1.0f) continue;
			return p;
		}
	}
}


class Material {
public:
	glm::vec3 albedo{ 1.0f };

	virtual bool scatter(const Ray& ray, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const = 0;

};

class Lambertian : public Material {

	virtual bool scatter (const Ray& ray, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override {

		scattered.direction = payload.hitNormal + Utils::random_in_unit_sphere();;
		scattered.origin = payload.hitPosition;

		attenuation = albedo;

		return true;
	}

};

class Metal : public Material {
public:

	float fuzz = 0.0f;

	Metal(const glm::vec3& a, float f){
		albedo = a;
		fuzz = f;
	}

	Metal() = default;

	virtual bool scatter(const Ray& ray, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override {
		glm::vec3 reflected = glm::reflect(ray.direction, payload.hitNormal);
		scattered.origin = payload.hitPosition;
		scattered.direction = reflected + fuzz * Utils::random_in_unit_sphere();
		attenuation = albedo;
		return (glm::dot(scattered.direction, payload.hitNormal) > 0.0f);
	}

};

class Dielectric : public Material {
public:
	float refractiveIndex = 1.52f;

	Dielectric(float ir) : refractiveIndex(ir) {}
	Dielectric() = default;

	virtual bool scatter(const Ray& ray, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override {

		attenuation = glm::vec3(1.0f);
		float refractionRatio = payload.frontFace ? (1.0f / refractiveIndex) : refractiveIndex;

		float cosTheta = glm::min(glm::dot(-ray.direction, payload.hitNormal), 1.0f);
		float sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = refractionRatio * sinTheta > 1.0f;
		glm::vec3 direction;

		if (cannotRefract || reflectance(cosTheta, refractionRatio) > Walnut::Random::Float())
			direction = glm::reflect(ray.direction, payload.hitNormal);
		else
			direction = glm::refract(ray.direction, payload.hitNormal, refractionRatio);

		scattered.origin = payload.hitPosition;
		scattered.direction = direction;
		return true;

	}

private:
	static float reflectance(float cosine, float ref_idx) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

