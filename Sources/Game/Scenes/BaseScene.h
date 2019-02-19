#pragma once

#include<memory>
#include<vector>
#include<unordered_map>
#include<string>

#include"..\Shader.h"
#include"..\Texture.h"
#include"..\..\Framework\Entity.h"
#include"..\..\Utilities\ResourcePool.h"
#include"..\..\Graphics\Geometry.h"

namespace Prizm
{
	class SceneManager;

	class BaseScene
	{
	private:
		ResourcePool<Entity> _back_ground;
		std::unordered_map<std::string, ResourcePool<Entity>> _game_objects_2d;		// UI
		std::unordered_map<std::string, ResourcePool<Entity>> _game_objects_3d;		// objects

		ResourcePool<Shader> _shaders;
		ResourcePool<Texture> _textures;

		std::unique_ptr<Geometry> _screen_quad;
		
	protected:
		static SceneManager* _scene_manager;
		std::unordered_map<std::string, std::vector<unsigned int>> _game_object_indices;
		int _score;

		unsigned int _quad_shader;

		SceneManager* GetSceneManager(void) { return _scene_manager; }

		void FadeIn(unsigned int);

		void FadeOut(unsigned int);

		unsigned int CreateShader(const std::string&);

		void CompileShader(const unsigned int, const ShaderType, const std::vector<D3D11_INPUT_ELEMENT_DESC>&);

		unsigned int LoadTexture(const std::string&);

		const std::shared_ptr<Shader>& GetShader(const unsigned int);

		const std::shared_ptr<Texture>& GetTexture(const unsigned int);

		void ReleaseShader(const unsigned int);

		void ReleaseTexture(const unsigned int);

	public:
		BaseScene(void);
		virtual ~BaseScene(void) = default;
		virtual void LoadScene(void) {}
		virtual bool Update(void) { return false; }
		virtual void Draw(void) {}
		virtual void Finalize(void) {}

		template<class _Type>
		unsigned int AddBackGround(void)
		{
			auto game_object_index = _back_ground.Load(std::make_shared<_Type>());
			_game_object_indices[typeid(_Type).name()].emplace_back(game_object_index);
			_back_ground.Get(game_object_index)->Initialize();
			return game_object_index;
		}

		template<class _Type>
		std::shared_ptr<_Type> GetBackGround(unsigned int index)
		{
			auto game_object = _back_ground.Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		template<class _Type>
		unsigned int AddGameObject2D(void)
		{
			auto game_object_index = _game_objects_2d[typeid(_Type).name()].Load(std::make_shared<_Type>());
			_game_object_indices[typeid(_Type).name()].emplace_back(game_object_index);
			_game_objects_2d[typeid(_Type).name()].Get(game_object_index)->Initialize();
			return game_object_index;
		}
		
		template<class _Type>
		std::shared_ptr<_Type> GetGameObject2D(unsigned int index)
		{
			auto game_object = _game_objects_2d[typeid(_Type).name()].Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		template<class _Type>
		unsigned int AddGameObject3D(void)
		{
			auto game_object_index = _game_objects_3d[typeid(_Type).name()].Load(std::make_shared<_Type>());
			_game_object_indices[typeid(_Type).name()].emplace_back(game_object_index);
			_game_objects_3d[typeid(_Type).name()].Get(game_object_index)->Initialize();
			return game_object_index;
		}

		template<class _Type>
		std::shared_ptr<_Type> GetGameObject3D(unsigned int index)
		{
			auto game_object = _game_objects_3d[typeid(_Type).name()].Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		// all game objects function
		void RunEntities(void);
		void DrawEntities(void);
		void FinalizeEntities(void);

		static void SetSceneManager(SceneManager* sm);
	};
}