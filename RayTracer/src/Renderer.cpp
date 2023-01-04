#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"

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
		// No resize necessary
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

void Renderer::Render(const Camera& camera)
{

	Ray ray;
	ray.origin = camera.GetPosition();

	for (uint32_t y = 0; y < image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < image->GetWidth(); x++)
		{

			ray.direction = camera.GetRayDirections()[x + y * image->GetWidth()];

			glm::vec4 color = getPixelColour(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			imageData[x + y * image->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	image->SetData(imageData);
}

glm::vec4 Renderer::getPixelColour(Ray& ray)
{

	float radius = 0.5f;

	float a = glm::dot(ray.direction, ray.direction);
	float b = 2.0f * glm::dot(ray.origin, ray.direction);
	float c = glm::dot(ray.origin, ray.origin) - radius * radius;

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(glm::vec3(0.0f), 1.0f);

	float hitDist0 = (-b + glm::sqrt(discriminant)) / (2 * a);
	float hitDist1 = (-b - glm::sqrt(discriminant)) / (2 * a);

	float minHitDistance = glm::min(hitDist0, hitDist1);
	glm::vec3 hitPoint = glm::vec3(ray.origin + ray.direction * minHitDistance);

	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float intensity = glm::max(glm::dot(normal, -lightDir), 0.0f);

	glm::vec3 sphereColour = glm::vec3(1, 0, 1);
	sphereColour *= intensity;

	return glm::vec4(sphereColour,1);
}