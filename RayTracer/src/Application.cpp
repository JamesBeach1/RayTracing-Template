#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"

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

		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() });
		}
			
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Control Panel");

		ImGui::Text("Last render: %.3fms", lastRenderTime);
		ImGui::Text("FPS: %.1f", 1000 / lastRenderTime);

		ImGui::End();

		Render();
	}

	void Render() {

		Timer timer;

		if (!image || viewportWidth != image->GetWidth() || viewportHeight != image->GetHeight())
		{
			image = std::make_shared<Image>(viewportWidth, viewportHeight, ImageFormat::RGBA);
			delete[] imageData;
			imageData = new uint32_t[viewportWidth * viewportHeight];
		}

		for (uint32_t i = 0; i < viewportWidth * viewportHeight; i++)
		{
			imageData[i] = 0xffffffff;
		}

		image->SetData(imageData);

		lastRenderTime = timer.ElapsedMillis();
	}

private:
	std::shared_ptr<Image> image;
	uint32_t* imageData = nullptr;

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