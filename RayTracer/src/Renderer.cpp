#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "Walnut/Random.h"
#include <glm/gtc/matrix_transform.hpp>
#include <corecrt_math_defines.h>
#include <execution>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	glm::vec3 random_in_unit_sphere() {
		glm::vec3 p;
		do {
			p = 2.0f * Walnut::Random::Vec3() - 1.0f;
		} while (glm::dot(p, p) >= 1.0f);
		return p;
	}

	// spherical coordinates
	glm::vec3 random_in_unit_sphere_spherical() {
		float r1 = Walnut::Random::Float();
		float r2 = Walnut::Random::Float();
		float z = 1.0f - 2.0f * r1;
		float r = sqrt(1.0f - z * z);
		float phi = 2.0f * M_PI * r2;
		float x = r * cos(phi);
		float y = r * sin(phi);
		return glm::vec3(x, y, z);
	}

	// golden ratio
	glm::vec3 random_in_unit_sphere_golden() {
		float theta = Walnut::Random::Float() * 2.0f * M_PI;
		float phi = acos(2.0f * Walnut::Random::Float() - 1.0f);
		float x = cos(theta) * sin(phi);
		float y = sin(theta) * sin(phi);
		float z = cos(phi);
		return glm::vec3(x, y, z);
	}

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (image)
	{
		if (image->GetWidth() == width && image->GetHeight() == height)
			return;

		image->Resize(width, height);
	}
	else
	{
		image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	widthIterator.resize(width);
	heightIterator.resize(height);
	for (uint32_t i = 0; i < width; i++)
		widthIterator[i] = i;
	for (uint32_t i = 0; i < height; i++)
		heightIterator[i] = i;

	delete[] imageData;
	imageData = new uint32_t[width * height];
}

void Renderer::Render(const Camera& camera, const Scene& scene)
{

	activeCamera = &camera;
	activeScene = &scene;

	std::for_each(std::execution::par, heightIterator.begin(), heightIterator.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, widthIterator.begin(), widthIterator.end(),
			[this, y](uint32_t x)
				{
					Ray ray;
					ray.origin = activeCamera->GetPosition();
					ray.direction = activeCamera->GetRayDirections()[x + y * image->GetWidth()];

					glm::vec4 color = glm::vec4(Color(ray, 0), 1);
					color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

					imageData[x + y * image->GetWidth()] = Utils::ConvertToRGBA(color);
				});
		});

	image->SetData(imageData);
}

RayPayload Renderer::traceRay(Ray& ray)
{
	RayPayload closestHit;
	float minHitDistance = FLT_MAX;

	for (int i = 0; i < activeScene->objects.size(); i++) {
		Hittable* object = activeScene->objects[i];
		RayPayload payload = object->Hit(ray, i);

		if (payload.hitDistance < minHitDistance && payload.hitDistance > 0) {
			minHitDistance = payload.hitDistance;
			closestHit = payload;
		}
	}

	return closestHit;
}

glm::vec3 Renderer::Color(Ray& r, int depth) {
	glm::vec3 attenuation;
	RayPayload payload = traceRay(r);

	if (payload.hitDistance == -1.0f) {
		return activeScene->skyColour;
	}

	Hittable* object = activeScene->objects[payload.hittableIndex];
	Material* mat = activeScene->materials[object->materialIndex];
	
	if (depth < 2) {

		attenuation = mat->scatter(r, payload);

		Ray scattered;;
		glm::vec3 target = payload.hitPosition + payload.hitNormal + Utils::random_in_unit_sphere_spherical();
		scattered.direction = target - payload.hitPosition;
		scattered.origin = payload.hitPosition;

		return attenuation * Color(scattered, depth + 1);
	}

}

