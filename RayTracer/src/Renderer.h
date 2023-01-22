#pragma once

#include "Walnut/Image.h"
#include "Camera.h"
#include "Scene.h"
#include "Ray.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera, const Scene& scene);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return image; }

	bool accumulate = true;
	void resetSampleIndex() { sampleIndex = 1; }

private:

	RayPayload traceRay(Ray& ray);
	glm::vec3 Colour(Ray& r, int depth);

	std::vector<uint32_t> widthIterator, heightIterator;

	const Scene* activeScene = nullptr;
	const Camera* activeCamera = nullptr;

	std::shared_ptr<Walnut::Image> image;
	uint32_t* imageData = nullptr;

	glm::vec4* accumulatedData = nullptr;
	uint32_t sampleIndex = 1;

};
