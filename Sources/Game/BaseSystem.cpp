
#include <Windows.h>

#include"BaseSystem.h"
#include"GameManager.h"
#include"..\Input\Input.h"
#include"..\Utilities\Utils.h"
#include"..\Utilities\Log.h"
#include"..\Graphics\Window.h"

#pragma comment(lib, "Input.lib")
#pragma comment(lib, "Utilities.lib")
#pragma comment(lib, "Framework.lib")
#pragma comment(lib, "Graphics.lib")

#include <crtdbg.h>

namespace Prizm
{
	class BaseSystem::Impl
	{
	public:
		Impl() : _app_exit(false)
			   , _input_module(nullptr)
			   , _game_manager(std::make_unique<GameManager>()){}

		bool _app_exit;
		HMODULE _input_module;
		std::unique_ptr<GameManager> _game_manager;

		void MessageLoop(void)
		{
			MSG msg = {};

			while (!_app_exit)
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
							_app_exit = true;
						}
					}
				}

				if (msg.message == WM_QUIT)
				{
					_app_exit = true;
				}
				else
				{
					_app_exit |= _game_manager->Run();
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
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		std::string workspace_directory = DirectoryUtils::GetSpecialFolderPath(DirectoryUtils::FolderType::APPDATA) + "\\PrizmEngine";

#ifdef _DEBUG
		Log::Initialize(Log::LogMode::CONSOLE, workspace_directory);
#endif
		Input::Initialize();

		if (!Window::Initialize()) return false;

		if (!_impl->_game_manager->Initialize(Window::GetWindowHandle())) return false;

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

		_impl->_game_manager->Finalize();
		_impl->_game_manager.reset();
	}
}