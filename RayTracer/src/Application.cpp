#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Material.h"

using namespace Walnut;

class PrimaryLayer : public Walnut::Layer
{
public:

	PrimaryLayer() {

		Material* mat1 = new Lambertian();
		mat1->albedo = glm::vec3(1, 0, 0);
		scene.materials.push_back(mat1);

		Material* mat2 = new Lambertian();
		mat2->albedo = glm::vec3(0, 1, 0);
		scene.materials.push_back(mat2);

		Material* mat3 = new Metal(glm::vec3(0.7, 0.7, 0.7), 0.01f);
		scene.materials.push_back(mat3);

		Material* mat4 = new Dielectric();
		scene.materials.push_back(mat4);
		
		Sphere* sphere1 = new Sphere();
		sphere1->materialIndex = 2;
		scene.objects.push_back(sphere1);

		Sphere* sphere2 = new Sphere();
		sphere2->position = glm::vec3(0.0f, -100.5f, 0.0f);
		sphere2->radius = 100.0f;
		sphere2->materialIndex = 1;
		scene.objects.push_back(sphere2);

		Sphere* sphere3 = new Sphere();
		sphere3->materialIndex = 0;
		sphere3->position = glm::vec3(1.0f, 0.0f, 0.0f);
		scene.objects.push_back(sphere3);

		Sphere* sphere4 = new Sphere();
		sphere4->materialIndex = 3;
		sphere4->position = glm::vec3(-1.0f, 0.0f, 0.0f);
		scene.objects.push_back(sphere4);


	}

	virtual void OnUIRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = renderer.GetFinalImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
		}
			
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Control Panel");

		ImGui::Text("Last render: %.3fms", lastRenderTime);
		ImGui::Text("FPS: %.1f", 1000 / lastRenderTime);

		ImGui::End();

		Render();
	}

	virtual void OnUpdate(float ts) override
	{
		if (camera.onUpdate())
			renderer.resetSampleIndex();
	}


	void Render() {

		Timer timer;

		renderer.OnResize(viewportWidth, viewportHeight);
		camera.OnResize(viewportWidth, viewportHeight);
		renderer.Render(camera, scene);

		lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;
	Scene scene;

	uint32_t viewportWidth = 0;
	uint32_t viewportHeight = 0;

	float lastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing - Template";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<PrimaryLayer>();

	return app;
}