
#include"MainGameScene.h"
#include"..\GameManager.h"
#include"..\SceneManager.h"
#include"..\ImguiManager.h"
#include"..\Entity\BackGround.h"
#include"..\Entity\UI.h"
#include"..\Entity\Player2D.h"
#include"..\Entity\Enemy.h"
#include"..\..\Input\Input.h"
#include"..\..\Graphics\Window.h"
#include"..\..\Framework\Entity.h"

//#include"..\..\Framework\SoLoud\SoloudWrapper.h"

#include "SoLoud\soloud.h"
#include "SoLoud\soloud_sfxr.h"
#include "SoLoud\soloud_speech.h"
#pragma comment(lib, "SoLoud/soloud_static.lib")

#include"ImGui/imgui.h"

namespace Prizm
{
	class MainGameScene::Impl
	{
	public:
		// sound id

		// texture id
		unsigned int _bg_tex;
		unsigned int _player_tex;
		unsigned int _enemy_tex;

		// game object
		unsigned int _player_obj;
		unsigned int _enemy_obj;

		//SoloudWrapper _soloud;

		SoLoud::Soloud _soloud;
		SoLoud::Sfxr _sfx_mouse, _sfx_orbit;
		SoLoud::Speech _sfx_crazy;

		int _sound_handle_mouse = 0;
		int _sound_handle_orbit = 0;
		int _sound_handle_crazy = 0;
	};

	MainGameScene::MainGameScene(void) : _impl(std::make_unique<Impl>()){}
	MainGameScene::~MainGameScene(void) = default;

	void MainGameScene::LoadScene(void)
	{
		_impl->_bg_tex = this->LoadTexture("simple_bg00.jpg");
		_impl->_player_tex = this->LoadTexture("green.png");
		_impl->_enemy_tex = this->LoadTexture("yellow.png");

		// game object initialize
		auto bg_id = this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(bg_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetBackGround<BackGround>(bg_id)->LoadTexture(this->GetTexture(_impl->_bg_tex));

		_impl->_player_obj = this->AddGameObject2D<Player2D>();
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->LoadTexture(this->GetTexture(_impl->_player_tex));
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->MovePosition(0, 0);

		_impl->_enemy_obj = this->AddGameObject2D<Enemy>();
		this->GetGameObject2D<Enemy>(_impl->_enemy_obj)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<Enemy>(_impl->_enemy_obj)->LoadTexture(this->GetTexture(_impl->_enemy_tex));
		this->GetGameObject2D<Enemy>(_impl->_enemy_obj)->MovePosition(200, 0);

		//_impl->_soloud.Initialize();

		_impl->_soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION);
		_impl->_soloud.setGlobalVolume(4);

		_impl->_sfx_mouse.loadPreset(SoLoud::Sfxr::LASER, 3);
		_impl->_sfx_mouse.setLooping(1);
		_impl->_sfx_mouse.set3dMinMaxDistance(1, 200);
		_impl->_sfx_mouse.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5);
		_impl->_sound_handle_mouse = _impl->_soloud.play3d(_impl->_sfx_mouse, 100, 0, 0);

		_impl->_sfx_orbit.loadPreset(SoLoud::Sfxr::COIN, 3);
		_impl->_sfx_orbit.setLooping(1);
		_impl->_sfx_orbit.set3dMinMaxDistance(1, 200);
		_impl->_sfx_orbit.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5);
		_impl->_sound_handle_orbit = _impl->_soloud.play3d(_impl->_sfx_orbit, 50, 0, 0);

		_impl->_sfx_crazy.setText("sasaki tomotaka is fatman.");
		_impl->_sfx_crazy.setLooping(1);
		_impl->_sfx_crazy.set3dMinMaxDistance(1, 400);
		_impl->_sfx_crazy.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.25);
		//_impl->_sound_handle_crazy = _impl->_soloud.play3d(_impl->_sfx_crazy, 50, 0, 0);
	}

	bool MainGameScene::Update(void)
	{
		auto& player_pos = this->GetGameObject2D<Player2D>(_impl->_enemy_obj)->GetPosition();

		//_impl->_soloud.set3dListenerPosition(player_pos.x, 0, player_pos.y);

		//_impl->_soloud.set3dSourceAttenuation(_impl->_sound_handle_crazy, );

		auto& enemy_pos = this->GetGameObject2D<Enemy>(_impl->_enemy_obj)->GetPosition();
		_impl->_soloud.set3dSourceParameters(_impl->_sound_handle_crazy, enemy_pos.x, 0, enemy_pos.y, 0, 0, 0);

		_impl->_soloud.set3dSourceParameters(_impl->_sound_handle_orbit, player_pos.x, 0, player_pos.y, 0, 0, 0);

		_impl->_soloud.set3dSourcePosition(_impl->_sound_handle_mouse, enemy_pos.x, enemy_pos.y, 0);

		_impl->_soloud.update3dAudio();

		this->RunEntities();

		float *buf = _impl->_soloud.getWave();
		float *fft = _impl->_soloud.calcFFT();

		//ImGui::SetNextWindowPos(ImVec2(500, 20));
		ImGui::Begin("Output");
		ImGui::PlotLines("##Wave", buf, 256, 0, "Wave", -1, 1, ImVec2(264, 80));
		ImGui::PlotHistogram("##FFT", fft, 256 / 2, 0, "FFT", 0, 10, ImVec2(264, 80), 8);
		ImGui::Text("Active voices    : %d", _impl->_soloud.getActiveVoiceCount());
		ImGui::End();

		return true;
	}

	void MainGameScene::Draw(void)
	{
		this->DrawEntities();
	}

	void MainGameScene::Finalize(void)
	{
		this->FinalizeEntities();
	}
}