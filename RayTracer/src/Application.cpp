#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"
#include "Walnut/Input/Input.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Material.h"

#include <glm/gtc/type_ptr.hpp>

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

		if (ImGui::IsWindowFocused() && ImGui::IsItemActive()) {
			camera.mouseDisabled = false;
		}
		else
			camera.mouseDisabled = true;

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = renderer.GetFinalImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
		}
			
		ImGui::End();
		ImGui::PopStyleVar();


		ImGui::Begin("Control Panel");

		if (ImGui::CollapsingHeader("Objects")) {
			for (size_t i = 0; i < scene.objects.size(); i++) {
				ImGui::PushID(i);

				Hittable* object = scene.objects[i];
				ImGui::Text("Object %d", i + 1);
				ImGui::DragFloat3("Position", glm::value_ptr(object->position), 0.01f);
				ImGui::DragInt("Material", &object->materialIndex, 0.05f, 0, (int)scene.materials.size() - 1);

				if (dynamic_cast<Sphere*>(object) != nullptr) {
					ImGui::DragFloat("Radius", &(static_cast<Sphere*>(object)->radius), 0.01f, 0.0f, 1.0f);
				}
				ImGui::Separator();

				ImGui::PopID();
			}
		}

		if (ImGui::CollapsingHeader("Materials")) {
			for (size_t i = 0; i < scene.materials.size(); i++) {

				ImGui::PushID(i);

				Material* material = scene.materials[i];
				ImGui::Text("Material %d", i + 1);
				ImGui::ColorEdit3("Albedo", glm::value_ptr(material->albedo));

				if (dynamic_cast<Metal*>(material) != nullptr) {
					ImGui::DragFloat("Fuzz", &(static_cast<Metal*>(material)->fuzz), 0.01f, 0.0f, 1.0f);
				}

				if (dynamic_cast<Dielectric*>(material) != nullptr) {
					ImGui::DragFloat("Refractive Index", &(static_cast<Dielectric*>(material)->refractiveIndex), 0.01f, 0.0f, 3.0f);
				}

				ImGui::Separator();

				ImGui::PopID();
			}
		}

		ImGui::End();


		ImGui::Begin("Performance");

		ImGui::Text("Last render: %.3fms", lastRenderTime);
		ImGui::Text("FPS: %.1f", 1000 / lastRenderTime);

		if (frames.size() > 0) {
			ImGui::Text("FPS Graph");
			ImGui::PlotLines("", frames.data(), 100, 0, 0, 0.0f, 144.0f, ImVec2(500, 100));
		}

		ImGui::End();

		if (Input::IsMouseButtonDown(MouseButton::Left)) // I hate this but ImGui currently doesn't have an easy way of checking for state changes
			renderer.resetSampleIndex();

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
		addFrame( 1000 / lastRenderTime);
	}

private:
	Renderer renderer;
	Camera camera;
	Scene scene;

	uint32_t viewportWidth = 0;
	uint32_t viewportHeight = 0;

	float lastRenderTime = 0.0f;
	std::vector<float> frames;

	void addFrame(float v) {
		if (frames.size() > 100) //Max seconds to show
		{
			for (size_t i = 1; i < frames.size(); i++)
			{
				frames[i - 1] = frames[i];
			}

			frames[frames.size() - 1] = v;
		}
		else
		{
			frames.push_back(v);
		}
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing - Template";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<PrimaryLayer>();

	return app;
}