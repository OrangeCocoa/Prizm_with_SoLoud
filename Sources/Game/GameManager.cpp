
#include"GameManager.h"
#include"ImguiManager.h"
#include"..\Graphics\Graphics.h"
#include"SceneManager.h"
#include"Scenes\MainGameScene.h"
//#include"Adx2le\AudioDriver_Adx2le.h"
#include"..\Graphics\Window.h"

#include"..\Utilities\Log.h"
#include"..\Input\Input.h"

namespace Prizm
{
	class GameManager::Impl
	{
	public:
		bool want_exit_;
		std::unique_ptr<SceneManager> _scene_manager;
		std::unique_ptr<ImguiManager> _imgui_manager;

		Impl() : want_exit_(false){}
	};

	GameManager::GameManager() : _impl(std::make_unique<Impl>()){}

	GameManager::~GameManager() = default;

	bool GameManager::Initialize(HWND window_handle)
	{
		if (!Graphics::Initialize(window_width<int>, window_height<int>, false, window_handle, false))
			return false;

		_impl->_scene_manager = std::make_unique<SceneManager>();
		_impl->_scene_manager->SetNextScene<MainGameScene>();

		_impl->_imgui_manager = std::make_unique<ImguiManager>();
		_impl->_imgui_manager->Initialize();

		return true;
	}

	bool GameManager::Run(void)
	{
		if (Input::IsKeyTriggered("F9"))
		{// change fullscreen
			//Graphics::ChangeWindowMode();
		}

		// scene update
		_impl->_imgui_manager->BeginFrame();

		if (_impl->_scene_manager->Update())
		{
			// scene draw
			Graphics::BeginFrame();
			_impl->_scene_manager->Draw();
			_impl->_imgui_manager->EndFrame();
			Graphics::EndFrame();
		}

		return _impl->want_exit_;
	}

	void GameManager::Finalize(void)
	{
		_impl->_imgui_manager->Finalize();
		_impl->_scene_manager->Finalize();
		Graphics::Finalize();
	}
}