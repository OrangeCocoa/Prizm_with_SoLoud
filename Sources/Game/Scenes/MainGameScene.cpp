
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
		unsigned int _enemy1_tex;
		unsigned int _enemy2_tex;

		// game object
		unsigned int _player_obj;
		unsigned int _enemy1_obj;
		unsigned int _enemy2_obj;

		//SoloudWrapper _soloud;

		SoLoud::Soloud _soloud;
		SoLoud::Sfxr _sfx_enemy2, _sfx_enemy1;
		SoLoud::Speech _sfx_speech;

		int _sound_handle_enemy1 = 0;
		int _sound_handle_enemy2 = 0;
		int _sound_handle_speech = 0;
	};

	MainGameScene::MainGameScene(void) : _impl(std::make_unique<Impl>()){}
	MainGameScene::~MainGameScene(void) = default;

	void MainGameScene::LoadScene(void)
	{
		_impl->_bg_tex = this->LoadTexture("simple_bg00.jpg");
		_impl->_player_tex = this->LoadTexture("green.png");
		_impl->_enemy1_tex = this->LoadTexture("yellow.png");
		_impl->_enemy2_tex = this->LoadTexture("skyblue.png");

		// game object initialize
		auto bg_id = this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(bg_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetBackGround<BackGround>(bg_id)->LoadTexture(this->GetTexture(_impl->_bg_tex));

		_impl->_player_obj = this->AddGameObject2D<Player2D>();
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->LoadTexture(this->GetTexture(_impl->_player_tex));
		this->GetGameObject2D<Player2D>(_impl->_player_obj)->MovePosition(0, -100);

		_impl->_enemy1_obj = this->AddGameObject2D<Enemy>();
		this->GetGameObject2D<Enemy>(_impl->_enemy1_obj)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<Enemy>(_impl->_enemy1_obj)->LoadTexture(this->GetTexture(_impl->_enemy1_tex));
		this->GetGameObject2D<Enemy>(_impl->_enemy1_obj)->MovePosition(200, 0);

		_impl->_enemy2_obj = this->AddGameObject2D<Enemy>();
		this->GetGameObject2D<Enemy>(_impl->_enemy2_obj)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<Enemy>(_impl->_enemy2_obj)->LoadTexture(this->GetTexture(_impl->_enemy2_tex));
		this->GetGameObject2D<Enemy>(_impl->_enemy2_obj)->MovePosition(0, 0);

		//_impl->_soloud.Initialize();

		_impl->_soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION);
		_impl->_soloud.setGlobalVolume(4);

		_impl->_sfx_enemy1.loadPreset(SoLoud::Sfxr::COIN, 3);
		_impl->_sfx_enemy1.setLooping(1);
		_impl->_sfx_enemy1.set3dMinMaxDistance(1, 200);
		_impl->_sfx_enemy1.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5);
		//_impl->_sound_handle_enemy1 = _impl->_soloud.play3d(_impl->_sfx_enemy1, 50, 0, 0);

		_impl->_sfx_enemy2.loadPreset(SoLoud::Sfxr::LASER, 3);
		_impl->_sfx_enemy2.setLooping(1);
		_impl->_sfx_enemy2.set3dMinMaxDistance(1, 200);
		_impl->_sfx_enemy2.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5);
		_impl->_sound_handle_enemy2 = _impl->_soloud.play3d(_impl->_sfx_enemy2, 100, 0, 0);

		_impl->_sfx_speech.setText("Default speech.");
		_impl->_sfx_speech.setLooping(1);
		_impl->_sfx_speech.set3dMinMaxDistance(1, 400);
		_impl->_sfx_speech.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.25);
		_impl->_sound_handle_speech = _impl->_soloud.play3d(_impl->_sfx_speech, 50, 0, 0);
	}

	bool MainGameScene::Update(void)
	{
		auto& player_pos = this->GetGameObject2D<Player2D>(_impl->_player_obj)->GetPosition();

		_impl->_soloud.set3dListenerPosition(player_pos.x, player_pos.y, 0);

		//_impl->_soloud.set3dSourceAttenuation(_impl->_sound_handle_speech, );

		auto enemy_pos = this->GetGameObject2D<Enemy>(_impl->_enemy1_obj)->GetPosition();
		_impl->_soloud.set3dSourceParameters(_impl->_sound_handle_speech, enemy_pos.x, enemy_pos.y, 0, 5, 0, 0);

		//_impl->_soloud.set3dSourceParameters(_impl->_sound_handle_enemy1, enemy_pos.x, enemy_pos.y, 0, 5, 0, 0);

		enemy_pos = this->GetGameObject2D<Enemy>(_impl->_enemy2_obj)->GetPosition();
		_impl->_soloud.set3dSourceParameters(_impl->_sound_handle_enemy2, enemy_pos.x, enemy_pos.y, 0, 5, 0, 0);

		_impl->_soloud.update3dAudio();

		this->RunEntities();

		float *buf = _impl->_soloud.getWave();
		float *fft = _impl->_soloud.calcFFT();

		ImGui::SetNextWindowPos(ImVec2(50, 20));
		ImGui::Begin("Listener");
		ImGui::SetWindowSize(ImVec2(420, 420));
		ImGui::PlotLines("Wave", buf, 256, 0, "Wave", -1, 1, ImVec2(300, 160));
		ImGui::PlotHistogram("FFT", fft, 256 / 2, 0, "FFT", 0, 10, ImVec2(300, 160), 8);
		ImGui::Text("Active voices    : %d", _impl->_soloud.getActiveVoiceCount());
		ImGui::End();

		static char text[256] = "";

		ImGui::SetNextWindowPos(ImVec2(50, 500));
		ImGui::Begin("Speech Control");
		ImGui::SetWindowSize(ImVec2(420, 200));
		ImGui::InputText("", text, sizeof(text));
		if (ImGui::Button("join"))
		{
			_impl->_sfx_speech.setText(text);
			_impl->_sound_handle_speech = _impl->_soloud.play3d(_impl->_sfx_speech, 50, 0, 0);
		}

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