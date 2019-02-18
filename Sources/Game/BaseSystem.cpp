
#include <Windows.h>

#include"..\Input\Input.h"
#pragma comment(lib, "Input.lib")

#include"..\Utilities\Utils.h"
#include"..\Utilities\Log.h"
#pragma comment(lib, "Utilities.lib")

#pragma comment(lib, "Framework.lib")

#pragma comment(lib, "Graphics.lib")

#include"BaseSystem.h"
#include"GameManager.h"
#include"..\Graphics\Window.h"

namespace Prizm
{
	class BaseSystem::Impl
	{
	public:
		Impl() : app_exit_(false)
			   , input_module_(nullptr)
			   , game_manager_(std::make_unique<GameManager>()){}

		bool app_exit_;
		HMODULE input_module_;
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

	BaseSystem::BaseSystem() : _impl(std::make_unique<Impl>()) {}

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

		if (!_impl->game_manager_->Initialize(Window::GetWindowHandle())) return false;

		return true;
	}

	void BaseSystem::Run(void)
	{
		_impl->MessageLoop();
	}

	void BaseSystem::Finalize(void)
	{
		Log::Finalize();

		Window::Finalize();

		_impl->game_manager_->Finalize();
		_impl->game_manager_.reset();
	}
}