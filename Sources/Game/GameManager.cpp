
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
		std::unique_ptr<SceneManager> scene_manager_;
		std::unique_ptr<ImguiManager> imgui_manager_;

		Impl() : want_exit_(false){}
	};

	GameManager::GameManager() : _impl(std::make_unique<Impl>()){}

	GameManager::~GameManager() = default;

	bool GameManager::Initialize(HWND window_handle)
	{
		if (!Graphics::Initialize(window_width<int>, window_height<int>, false, window_handle, false))
			return false;

		_impl->scene_manager_ = std::make_unique<SceneManager>();
		_impl->scene_manager_->SetNextScene<MainGameScene>();

		_impl->imgui_manager_ = std::make_unique<ImguiManager>();
		_impl->imgui_manager_->Initialize();

		return true;
	}

	bool GameManager::Run(void)
	{
		if (Input::IsKeyTriggered("F9"))
		{// change fullscreen
			//Graphics::ChangeWindowMode();
		}

		// scene update
		_impl->imgui_manager_->BeginFrame();

		if (_impl->scene_manager_->Update())
		{
			// scene draw
			Graphics::BeginFrame();
			_impl->scene_manager_->Draw();
			_impl->imgui_manager_->EndFrame();
			Graphics::EndFrame();
		}

		return _impl->want_exit_;
	}

	void GameManager::Finalize(void)
	{
		_impl->imgui_manager_->Finalize();
		_impl->scene_manager_->Finalize();
		Graphics::Finalize();
	}
}