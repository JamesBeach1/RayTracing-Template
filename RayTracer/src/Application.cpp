#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"

#include "Renderer.h"
#include "Camera.h"

using namespace Walnut;

class PrimaryLayer : public Walnut::Layer
{
public:
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
		camera.onUpdate();
	}


	void Render() {

		Timer timer;

		renderer.OnResize(viewportWidth, viewportHeight);
		camera.OnResize(viewportWidth, viewportHeight);
		renderer.Render(camera);

		lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;

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