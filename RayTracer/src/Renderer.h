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

private:

	glm::vec4 getPixelColour(Ray& ray, const Scene& scene);

	const Scene* scene = nullptr;
	const Camera* camera = nullptr;

	std::shared_ptr<Walnut::Image> image;
	uint32_t* imageData = nullptr;

};
