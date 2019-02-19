#pragma once

#include<memory>

#include"Scenes\BaseScene.h"
#include"..\Utilities\Log.h"

namespace Prizm
{
	class SceneManager
	{
	private:
		std::unique_ptr<BaseScene> _cur_scene;

	public:
		SceneManager(void)
		{
			_cur_scene = std::make_unique<BaseScene>();
			_cur_scene->SetSceneManager(this);
		}

		~SceneManager(void) = default;

		template<class SceneTypes>
		void SetNextScene(void)
		{
			if (_cur_scene)
			{
				_cur_scene->Finalize();
				_cur_scene.reset();
			}
			
			_cur_scene = std::make_unique<SceneTypes>();
			_cur_scene->LoadScene();

			Log::Info("Scene changed.");
		}

		bool Update(void)
		{
			return _cur_scene->Update();
		}

		void Draw(void)
		{
			_cur_scene->Draw();
		}

		void Finalize(void)
		{
			_cur_scene->Finalize();
		}
	};
}