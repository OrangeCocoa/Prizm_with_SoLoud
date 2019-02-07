
#include <windows.h>

#define DLL_EXPORT __declspec(dllimport)

#include"..\Input\Input.h"
#pragma comment(lib, "Input.lib")

#include"..\Window\Window.h"
#pragma comment(lib, "Window.lib")

#include"..\Utilities\Utils.h"
#include"..\Utilities\Log.h"
#pragma comment(lib, "Utilities.lib")

#pragma comment(lib, "Framework.lib")

//#pragma comment(lib, "Graphics.lib")

#include"BaseSystem.h"
#include"GameManager.h"

namespace Prizm
{
	class BaseSystem::Impl
	{
	public:
		Impl() : app_exit_(false)
			   , game_manager_(std::make_unique<GameManager>()){}

		bool app_exit_;
		std::unique_ptr<GameManager> game_manager_;

		void MessageLoop(void)
		{
			MSG msg = {};

			while (!app_exit_)
			{
				if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}

				if (Input::IsKeyTriggered("escape"))
				{
					if (Input::IsMouseCaptured())
					{
						Input::CaptureMouse(Window::GetWindowHandle(), false);
					}
					else
					{
						if (MessageBoxA(Window::GetWindowHandle(), "Quit ?", "User Notification", MB_YESNO | MB_DEFBUTTON2) == IDYES)
						{
							Log::Info("[EXIT] KEY DOWN ESC");
							app_exit_ = true;
						}
					}
				}

				if (msg.message == WM_QUIT)
				{
					app_exit_ = true;
				}
				else
				{
					app_exit_ |= game_manager_->Run();
				}

				Input::PostStateUpdate();
			}
		}
	};

	BaseSystem::BaseSystem() : impl_(std::make_unique<Impl>()) {}

	BaseSystem::~BaseSystem() { Log::Info("~BaseSystem()"); }
	// = default;

	bool BaseSystem::Initialize(void)
	{
		std::string workspace_directory = DirectoryUtils::GetSpecialFolderPath(DirectoryUtils::FolderType::APPDATA) + "\\PrizmEngine";

#ifdef _DEBUG
		Log::Initialize(Log::LogMode::CONSOLE, workspace_directory);
#endif
		Input::Initialize();

		if (!Window::Initialize()) return false;

		if (!impl_->game_manager_->Initialize(Window::GetWindowHandle())) return false;

		return true;
	}

	void BaseSystem::Run(void)
	{
		impl_->MessageLoop();
	}

	void BaseSystem::Finalize(void)
	{
		Log::Finalize();

		Window::Finalize();

		impl_->game_manager_->Finalize();
		impl_->game_manager_.reset();
	}
}