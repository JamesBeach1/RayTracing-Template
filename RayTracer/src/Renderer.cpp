#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "Walnut/Random.h"
#include <glm/gtc/matrix_transform.hpp>

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

	delete[] imageData;
	imageData = new uint32_t[width * height];
}

void Renderer::Render(const Camera& camera, const Scene& scene)
{

	activeCamera = &camera;
	activeScene = &scene;

	for (uint32_t y = 0; y < image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < image->GetWidth(); x++)
		{

			glm::vec4 color = getPixelColour(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			imageData[x + y * image->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	image->SetData(imageData);
}

glm::vec4 Renderer::getPixelColour(uint32_t x, uint32_t y)
{

	Ray ray;
	ray.origin = activeCamera->GetPosition();
	ray.direction = activeCamera->GetRayDirections()[x + y * image->GetWidth()];

	glm::vec3 lightDir = activeScene->lightDirection;
	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	for(int i = 0; i < 5; i++){
		RayPayload closestHit = traceRay(ray);

		if (closestHit.hitDistance == -1.0f) {
			color += activeScene->skyColour * multiplier;
			break;
		}

		float intensity = glm::max(glm::dot(closestHit.hitNormal, -lightDir), 0.0f);

		Hittable* object = activeScene->objects[closestHit.hittableIndex];
		glm::vec3 objectColour = activeScene->materials[object->materialIndex]->albedo;
		objectColour *= intensity;

		color += objectColour * multiplier;
		multiplier *= 0.5f;

		ray.origin = closestHit.hitPosition + closestHit.hitNormal * 0.0001f;
		ray.direction = glm::reflect(ray.direction, closestHit.hitNormal);
	}

	return glm::vec4(color, 1);
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
