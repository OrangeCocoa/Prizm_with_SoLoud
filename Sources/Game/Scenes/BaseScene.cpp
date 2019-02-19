
#include<unordered_map>
#include"BaseScene.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\SceneManager.h"
#include"..\..\Graphics\Window.h"

namespace Prizm
{
	SceneManager* BaseScene::_scene_manager = nullptr;

	BaseScene::BaseScene(void) 
	{
		// 2D shader
		_quad_shader = CreateShader("2D.hlsl");

		std::vector<D3D11_INPUT_ELEMENT_DESC> ui_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		CompileShader(_quad_shader, ShaderType::VS, ui_element);
		CompileShader(_quad_shader, ShaderType::PS, ui_element);

		_screen_quad = std::make_unique<Geometry>(GeometryGenerator::Quad2D(window_width<float>, window_height<float>, 0, 0));
	}

	void BaseScene::FadeIn(unsigned int curr_time)
	{

	}

	void BaseScene::FadeOut(unsigned int curr_time)
	{

	}

	unsigned int BaseScene::CreateShader(const std::string& file_name)
	{
		return _shaders.Load(std::make_shared<Shader>(file_name));
	}

	void BaseScene::CompileShader(const unsigned int index, const ShaderType type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc)
	{
		_shaders.Get(index)->CompileAndCreateFromFile(Graphics::GetDevice(), type, element_desc);
	}

	unsigned int BaseScene::LoadTexture(const std::string& tex_name)
	{
		auto index = _textures.Load(std::make_shared<Texture>());
		_textures.Get(index)->LoadTexture(Graphics::GetDevice(), tex_name);
		return index;
	}

	const std::shared_ptr<Shader>& BaseScene::GetShader(const unsigned int index)
	{
		return _shaders.Get(index);
	}

	const std::shared_ptr<Texture>& BaseScene::GetTexture(const unsigned int index)
	{
		return _textures.Get(index);
	}

	void BaseScene::ReleaseShader(const unsigned int index)
	{
		_shaders.Release(index);
	}

	void BaseScene::ReleaseTexture(const unsigned int index)
	{
		_textures.Release(index);
	}

	void BaseScene::RunEntities(void)
	{
		for (unsigned int i = 0; i < _back_ground.Size(); ++i)
		{
			if (_back_ground.Get(i))
				_back_ground.Get(i)->Run();
		}

		for (auto&& game_object : _game_objects_3d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Run();
			}
		}

		for (auto&& game_object : _game_objects_2d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Run();
			}
		}
	}

	void BaseScene::DrawEntities(void)
	{
		for (unsigned int i = 0; i < _back_ground.Size(); ++i)
		{
			if (_back_ground.Get(i))
				_back_ground.Get(i)->Draw();
		}

		for (auto&& game_object : _game_objects_3d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Draw();
			}
		}

		for (auto&& game_object : _game_objects_2d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Draw();
			}
		}
	}

	void BaseScene::FinalizeEntities(void)
	{
		for (unsigned int i = 0; i < _back_ground.Size(); ++i)
		{
			if (_back_ground.Get(i))
			{
				_back_ground.Get(i)->Finalize();
				_back_ground.Release(i);
			}
		}

		for (auto&& game_object : _game_objects_3d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
				{
					game_object.second.Get(index)->Finalize();
					game_object.second.Release(index);
				}
			}
		}

		for (auto&& game_object : _game_objects_2d)
		{
			for (auto index : _game_object_indices[game_object.first])
			{
				if (game_object.second.Get(index))
				{
					game_object.second.Get(index)->Finalize();
					game_object.second.Release(index);
				}
			}
		}
	}

	void BaseScene::SetSceneManager(SceneManager* sm)
	{
		_scene_manager = sm;
	}
}