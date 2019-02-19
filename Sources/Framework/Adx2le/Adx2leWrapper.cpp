
#include"Adx2leWrapper.h"

#include"..\..\Utilities\Log.h"

namespace Prizm
{
	/* MAX_VOICE is concurrent play music num */
	constexpr unsigned int MAX_VOICE = 24;
	constexpr unsigned int MAX_VIRTUAL_VOICE = 64;
	constexpr unsigned int MAX_CRIFS_LOADER = 64;

	/* Max sampling rate（include pitch change） */
	constexpr unsigned int MAX_SAMPLING_RATE = 48000 * 2;

	/* Sampling rate of HCA-MX codic */
	constexpr unsigned int SAMPLINGRATE_HCAMX = 32000;

	constexpr unsigned int X_POS_OFFSET_FOR_MAP_DRAW = 20;
	constexpr float PITCH_CHANGE_VALUE = 0.2f;
	constexpr float MAX_PITCH_VALUE = 1200.0f;

	/* Aisac control delta */
	constexpr float DELTA_AISAC_VALUE = .02f;

	/* need for initialize */
	static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
	{
		const CriChar8 *errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
		Log::Error(reinterpret_cast<const char*>(errmsg));
		return;
	}

	void* user_alloc_func(void *obj, CriUint32 size)
	{
		void *ptr;
		ptr = malloc(size);
		return ptr;
	}

	void user_free_func(void *obj, void *ptr)
	{
		free(ptr);
	}

	bool Adx2leWrapper::PositioningSoundInitialize(void)
	{
		/* 3d sound parameter */
		_source_pos.x = 0; _source_pos.y = 0; _source_pos.z = 0;
		_source_velocity.x = 0; _source_velocity.y = 0; _source_velocity.z = 0;
		_listener_pos.x = 0; _listener_pos.y = 0; _listener_pos.z = 0;
		_listener_front.x = 0; _listener_front.y = 0; _listener_front.z = 1;
		_listener_top.x = 0; _listener_top.y = 1; _listener_top.z = 0;

		/* create object */
		_source = criAtomEx3dSource_Create(nullptr, nullptr, 0);
		_listener = criAtomEx3dListener_Create(nullptr, nullptr, 0);

		/* set object */
		criAtomExPlayer_Set3dSourceHn(_player, _source);
		criAtomExPlayer_Set3dListenerHn(_player, _listener);

		/* source initialize */
		criAtomEx3dSource_SetPosition(_source, &_source_pos);
		criAtomEx3dSource_SetVelocity(_source, &_source_velocity);
		criAtomEx3dSource_Update(_source);

		/* listener initialize */
		criAtomEx3dListener_SetPosition(_listener, &_listener_pos);
		criAtomEx3dListener_SetOrientation(_listener, &_listener_front, &_listener_top);
		criAtomEx3dListener_Update(_listener);

		return true;
	}

	void Adx2leWrapper::PositioningSoundFinalize(void)
	{
	}

	bool Adx2leWrapper::InteractiveSoundInitialize(void)
	{
		/* AISAC control initialize CRI_XXXX_AISACCONTROL_ANY = 0 */
		_aisac_control_id = 0;

		ResetAisac();

		return true;
	}

	void Adx2leWrapper::InteractiveSoundFinalize(void)
	{
		ResetAisac();
	}

	void Adx2leWrapper::ResetAisac(void)
	{
		criAtomExPlayer_SetAisacById(_player, _aisac_control_id, .0f);
	}

	bool Adx2leWrapper::Initialize(std::string& acf_file_path)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		_playback_id = 0;
		_pitch = 0;
		_current_scene_id = 0;

		criErr_SetCallback(user_error_callback_func);

		/* メモリアロケータの登録 */
		criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, nullptr);

		/* ライブラリの初期化（最大ボイス数変更） */
		CriAtomExConfig_WASAPI init_config;
		CriFsConfig fs_config;
		criAtomEx_SetDefaultConfig_WASAPI(&init_config);
		criFs_SetDefaultConfig(&fs_config);
		init_config.atom_ex.max_virtual_voices = MAX_VIRTUAL_VOICE;
		init_config.hca_mx.output_sampling_rate = SAMPLINGRATE_HCAMX;
		fs_config.num_loaders = MAX_CRIFS_LOADER;
		init_config.atom_ex.fs_config = &fs_config;
		criAtomEx_Initialize_WASAPI(&init_config, nullptr, 0);

		/* D-Basの作成（最大ストリーム数はここで決まります） */
		_dbas_id = criAtomDbas_Create(nullptr, nullptr, 0);

		/* ACFファイルの読み込みと登録 */
		criAtomEx_RegisterAcfFile(nullptr, acf_file_path.c_str(), nullptr, 0);

		/* DSP attatch */
		criAtomEx_AttachDspBusSetting("DspBusSetting_0", nullptr, 0);

		/* standard voice pool create（最大ボイス数変更／最大ピッチ変更／ストリーム再生対応） */
		CriAtomExStandardVoicePoolConfig std_vpool_config;
		criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&std_vpool_config);
		std_vpool_config.num_voices = MAX_VOICE;
		std_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
		std_vpool_config.player_config.streaming_flag = CRI_TRUE;
		_std_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&std_vpool_config, nullptr, 0);

		/* HCA-MX：voice pool create */
		CriAtomExHcaMxVoicePoolConfig hcamx_vpool_config;
		criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamx_vpool_config);
		hcamx_vpool_config.num_voices = MAX_VOICE;
		hcamx_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
		hcamx_vpool_config.player_config.streaming_flag = CRI_TRUE;
		_hcamx_voice_pool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamx_vpool_config, nullptr, 0);

		/* create player */
		_player = criAtomExPlayer_Create(nullptr, nullptr, 0);

		PositioningSoundInitialize();
		InteractiveSoundInitialize();

		// 最大ピッチを設定していると、範囲内のピッチ変更ならピッチ変更時のバッファリングなしに変更してくれるっぽい
		criAtomExPlayer_SetMaxPitch(_player, MAX_PITCH_VALUE);

		return true;
	}

	AtomPlayerStatus Adx2leWrapper::Update(void)
	{
		criAtomEx_ExecuteMain();

#if defined(USE_INGAME_PREVIEW)
		/* バス解析情報の取得 */
		CriAtomExAsrBusAnalyzerInfo analyze_info;
		CriSint32 bus_no;
		for (bus_no = 0; bus_no < 8; bus_no++) {
			criAtomExAsr_GetBusAnalyzerInfo(bus_no, &analyze_info);
		}
#endif
		/* get current status */
		CriAtomExPlayerStatus playerstatus;
		playerstatus = criAtomExPlayer_GetStatus(_player);

		switch (playerstatus)
		{
		case CRIATOMEXPLAYER_STATUS_ERROR:
			return S_ERROR;

		case CRIATOMEXPLAYER_STATUS_PREP:
			return S_PREP;

		case CRIATOMEXPLAYER_STATUS_PLAYING:
			return S_PLAYNG;

		case CRIATOMEXPLAYER_STATUS_PLAYEND:
			return S_PLAYEND;
		}

		return S_STOP;
	}

	void Adx2leWrapper::Finalize(void)
	{
		criAtomEx_DetachDspBusSetting();

		criAtomExPlayer_Destroy(_player);

		criAtomExVoicePool_Free(_hcamx_voice_pool);
		criAtomExVoicePool_Free(_std_voice_pool);

		criAtomExAcb_ReleaseAll();

		criAtomEx_UnregisterAcf();

#if defined(USE_INGAME_PREVIEW)
		/* インゲームプレビュー関連機能の終了処理 */
		CriSint32 bus_no;
		for (bus_no = 0; bus_no < 8; bus_no++)
		{
			criAtomExAsr_DetachBusAnalyzer(bus_no);
		}
		criAtomExMonitor_Finalize();
#endif
		criAtomDbas_Destroy(_dbas_id);

		criAtomEx_Finalize_WASAPI();

		CoUninitialize();
	}

	void Adx2leWrapper::PositioningSoundSourceUpdate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
	{
		_source_pos.x += pos.x;
		_source_pos.y += pos.y;
		_source_pos.z += pos.z;

		_source_velocity.x += velocity.x;
		_source_velocity.y += velocity.y;
		_source_velocity.z += velocity.z;

		criAtomEx3dSource_SetPosition(_source, &_source_pos);
		criAtomEx3dSource_SetVelocity(_source, &_source_velocity);
		criAtomEx3dSource_Update(_source);
	}

	void Adx2leWrapper::PositioningSoundListenerUpdate(DirectX::SimpleMath::Vector3& pos
		, DirectX::SimpleMath::Vector3& front, DirectX::SimpleMath::Vector3& up)
	{
		_listener_pos.x = pos.x;
		_listener_pos.y = pos.y;
		_listener_pos.z = pos.z;

		_listener_front.x = front.x;
		_listener_front.y = front.y;
		_listener_front.z = front.z;

		_listener_top.x = up.x;
		_listener_top.y = up.y;
		_listener_top.z = up.z;

		criAtomEx3dListener_SetPosition(_listener, &_listener_pos);
		criAtomEx3dListener_SetOrientation(_listener, &_listener_front, &_listener_top);
		criAtomEx3dListener_Update(_listener);
	}

	void Adx2leWrapper::DecreaseAisac(void)
	{
		CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(_player, _aisac_control_id) - DELTA_AISAC_VALUE;
		criAtomExPlayer_SetAisacById(_player, _aisac_control_id, aisac_val);
		criAtomExPlayer_UpdateAll(_player);
	}

	void Adx2leWrapper::IncreaseAisac(void)
	{
		CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(_player, _aisac_control_id) + DELTA_AISAC_VALUE;
		criAtomExPlayer_SetAisacById(_player, _aisac_control_id, aisac_val);
		criAtomExPlayer_UpdateAll(_player);
	}

	void Adx2leWrapper::LoadAcb(const std::string& acb_file_path, const std::string& awb_file_path, unsigned int scene)
	{
		_acb_hn[scene] = criAtomExAcb_LoadAcbFile(nullptr, acb_file_path.c_str(), nullptr, awb_file_path.c_str(), nullptr, 0);

		_current_scene_id = scene;
	}

	void Adx2leWrapper::ReleaseAcb(unsigned int scene)
	{
		criAtomExAcb_Release(_acb_hn[scene]);
		_current_scene_id = 0;
	}

	void Adx2leWrapper::ReleaseAcbAll(void)
	{
		criAtomExAcb_ReleaseAll();
	}

	CriAtomExPlaybackId Adx2leWrapper::PlayGeneralCue(CriAtomExCueId start_cue_id)
	{
		criAtomExPlayer_SetCueId(_player, _acb_hn[0], start_cue_id);

		return _playback_id = criAtomExPlayer_Start(_player);
	}

	CriAtomExPlaybackId Adx2leWrapper::PlayCurrentCue(CriAtomExCueId start_cue_id)
	{
		criAtomExPlayer_SetCueId(_player, _acb_hn[_current_scene_id], start_cue_id);

		return _playback_id = criAtomExPlayer_Start(_player);
	}

	void Adx2leWrapper::StopCue(CriAtomExPlaybackId cue_id)
	{
		criAtomExPlayback_Stop(cue_id);

		/* ToDo */
		ResetAisac();
	}

	void Adx2leWrapper::Stop(void)
	{
		criAtomExPlayer_Stop(_player);
		ResetAisac();
	}

	void Adx2leWrapper::PauseCue(CriAtomExPlaybackId cue_id, int is_paused)
	{
		criAtomExPlayback_Pause(cue_id, is_paused);
	}

	void Adx2leWrapper::Pause(int is_paused)
	{
		criAtomExPlayer_Pause(_player, is_paused);
	}

	void Adx2leWrapper::UpPitchAll(void)
	{
		if (_pitch < MAX_PITCH_VALUE)
			_pitch += PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(_player, _pitch);
		criAtomExPlayer_UpdateAll(_player);
	}

	void Adx2leWrapper::UpPitch(CriAtomExPlaybackId cue_id)
	{
		if (_pitch < MAX_PITCH_VALUE)
			_pitch += PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(_player, _pitch);
		criAtomExPlayer_Update(_player, cue_id);
	}

	void Adx2leWrapper::DownPitchAll(void)
	{
		if (_pitch > -MAX_PITCH_VALUE)
			_pitch -= PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(_player, _pitch);
		criAtomExPlayer_UpdateAll(_player);
	}

	void Adx2leWrapper::DownPitch(CriAtomExPlaybackId cue_id)
	{
		if (_pitch > -MAX_PITCH_VALUE)
			_pitch -= PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(_player, _pitch);
		criAtomExPlayer_Update(_player, cue_id);
	}

	void Adx2leWrapper::RestorePitchAll(void)
	{
		criAtomExPlayer_SetPitch(_player, 0);
		criAtomExPlayer_UpdateAll(_player);
	}

	void Adx2leWrapper::RestorePitch(CriAtomExPlaybackId cue_id)
	{
		criAtomExPlayer_SetPitch(_player, 0);
		criAtomExPlayer_Update(_player, cue_id);
	}

	CriAtomExPlayerHn& Adx2leWrapper::GetPlayer(void)
	{
		return _player;
	}

	int64_t Adx2leWrapper::GetTimeMS(CriAtomExPlaybackId playback_id)
	{
		return criAtomExPlayback_GetTime(playback_id);
	}
}