#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "Walnut/Random.h"
#include <glm/gtc/matrix_transform.hpp>
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

	delete[] accumulatedData;
	accumulatedData = new glm::vec4[width * height];
}

void Renderer::Render(const Camera& camera, const Scene& scene)
{

	activeCamera = &camera;
	activeScene = &scene;

	if (sampleIndex == 1) {
		memset(accumulatedData, 0, image->GetWidth() * image->GetHeight() * sizeof(glm::vec4));
	}

	std::for_each(std::execution::par, heightIterator.begin(), heightIterator.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, widthIterator.begin(), widthIterator.end(),
			[this, y](uint32_t x)
				{
					Ray ray;
					ray.origin = activeCamera->GetPosition();
					ray.direction = activeCamera->GetRayDirections()[x + y * image->GetWidth()];

					glm::vec4 color = glm::vec4(Colour(ray, 0), 1);

					accumulatedData[x + y * image->GetWidth()] += color;
					glm::vec4 accumulatedColour = accumulatedData[x + y * image->GetWidth()];
					accumulatedColour /= (float)sampleIndex;

					accumulatedColour = glm::clamp(accumulatedColour, glm::vec4(0.0f), glm::vec4(1.0f));

					imageData[x + y * image->GetWidth()] = Utils::ConvertToRGBA(accumulatedColour);
				});
		});

	image->SetData(imageData);

	if (accumulate) {
		sampleIndex++;
	}
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

glm::vec3 Renderer::Colour(Ray& r, int depth) {
	glm::vec3 attenuation(0.0f);
	RayPayload payload = traceRay(r);

	if (payload.hitDistance == -1.0f) {
		return activeScene->skyColour;
	}

	Hittable* object = activeScene->objects[payload.hittableIndex];
	Material* mat = activeScene->materials[object->materialIndex];
	
	if (depth < 50) {

		Ray scattered;
		
		mat->scatter(r, payload, attenuation, scattered);

		return attenuation * Colour(scattered, depth + 1);
	}

	return attenuation;

}

