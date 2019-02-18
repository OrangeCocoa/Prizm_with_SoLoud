
#include"ImguiManager.h"
#include"ImGui/imgui.h"
#include"ImGui/imgui_impl_win32.h"
#include"ImGui/imgui_impl_dx11.h"
#include"Resource.h"
#include"..\Graphics\Graphics.h"
#include"..\Input\Input.h"

namespace Prizm
{
	class ImguiManager::Impl
	{
	public:
		Impl()
		: clear_color_(ImVec4(0.45f, 0.55f, 0.60f, 1.00f))
		{}

		ImVec4 clear_color_;
	};

	ImguiManager::ImguiManager() : _impl(std::make_unique<Impl>()){}
	ImguiManager::~ImguiManager() = default;

	void ImguiManager::Initialize(void)
	{
		// Setup ImGui binding
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		io.IniFilename = 0;

		std::string font_path = RESOURCE_DIR + "Fonts/ABDUCTIO.ttf";
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), 30.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

		ImGui_ImplWin32_Init(Graphics::GetWindowHandle());
		ImGui_ImplDX11_Init(Graphics::GetDevice().Get(), Graphics::GetDeviceContext().Get());

		// Setup style
		ImGui::StyleColorsClassic();
	}

	void ImguiManager::BeginFrame(void)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImguiManager::EndFrame(void)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImguiManager::ResizeBegin(void)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void ImguiManager::ResizeEnd(void)
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void ImguiManager::Finalize(void)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}