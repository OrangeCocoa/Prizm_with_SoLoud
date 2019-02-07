
#define DLL_EXPORT __declspec(dllexport)

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
		/*auto temp = std::make_tuple(source_pos.x, source_pos.y, source_pos.z);
		temp = std::forward_as_tuple(.0f, .0f, .0f);

		temp = std::make_tuple(source_velocity.x, source_velocity.y, source_velocity.z);
		temp = std::forward_as_tuple(.0f, .0f, .0f);

		temp = std::make_tuple(listener_pos.x, listener_pos.y, listener_pos.z);
		temp = std::forward_as_tuple(.0f, .0f, .0f);

		temp = std::make_tuple(listener_front.x, listener_front.y, listener_front.z);
		temp = std::forward_as_tuple(.0f, .0f, 1.f);

		temp = std::make_tuple(listener_top.x, listener_top.y, listener_top.z);
		temp = std::forward_as_tuple(.0f, 1.f, .0f);*/

		source_pos_.x = 0; source_pos_.y = 0; source_pos_.z = 0;
		source_velocity_.x = 0; source_velocity_.y = 0; source_velocity_.z = 0;
		listener_pos_.x = 0; listener_pos_.y = 0; listener_pos_.z = 0;
		listener_front_.x = 0; listener_front_.y = 0; listener_front_.z = 1;
		listener_top_.x = 0; listener_top_.y = 1; listener_top_.z = 0;

		/* create object */
		source_ = criAtomEx3dSource_Create(nullptr, nullptr, 0);
		listener_ = criAtomEx3dListener_Create(nullptr, nullptr, 0);

		/* set object */
		criAtomExPlayer_Set3dSourceHn(player_, source_);
		criAtomExPlayer_Set3dListenerHn(player_, listener_);

		/* source initialize */
		criAtomEx3dSource_SetPosition(source_, &source_pos_);
		criAtomEx3dSource_SetVelocity(source_, &source_velocity_);
		criAtomEx3dSource_Update(source_);

		/* listener initialize */
		criAtomEx3dListener_SetPosition(listener_, &listener_pos_);
		criAtomEx3dListener_SetOrientation(listener_, &listener_front_, &listener_top_);
		criAtomEx3dListener_Update(listener_);

		return true;
	}

	void Adx2leWrapper::PositioningSoundFinalize(void)
	{
	}

	bool Adx2leWrapper::InteractiveSoundInitialize(void)
	{
		/* AISAC control initialize CRI_XXXX_AISACCONTROL_ANY = 0 */
		aisac_control_id_ = 0;

		ResetAisac();

		return true;
	}

	void Adx2leWrapper::InteractiveSoundFinalize(void)
	{
		ResetAisac();
	}

	void Adx2leWrapper::ResetAisac(void)
	{
		criAtomExPlayer_SetAisacById(player_, aisac_control_id_, .0f);
	}

	bool Adx2leWrapper::Initialize(std::string& acf_file_path)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		playback_id_ = 0;
		pitch_ = 0;
		current_scene_id_ = 0;

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
		dbas_id_ = criAtomDbas_Create(nullptr, nullptr, 0);

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
		std_voice_pool_ = criAtomExVoicePool_AllocateStandardVoicePool(&std_vpool_config, nullptr, 0);

		/* HCA-MX：voice pool create */
		CriAtomExHcaMxVoicePoolConfig hcamx_vpool_config;
		criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamx_vpool_config);
		hcamx_vpool_config.num_voices = MAX_VOICE;
		hcamx_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
		hcamx_vpool_config.player_config.streaming_flag = CRI_TRUE;
		hcamx_voice_pool_ = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamx_vpool_config, nullptr, 0);

		/* create player */
		player_ = criAtomExPlayer_Create(nullptr, nullptr, 0);

		PositioningSoundInitialize();
		InteractiveSoundInitialize();

		// 最大ピッチを設定していると、範囲内のピッチ変更ならピッチ変更時のバッファリングなしに変更してくれるっぽい
		criAtomExPlayer_SetMaxPitch(player_, MAX_PITCH_VALUE);

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
		playerstatus = criAtomExPlayer_GetStatus(player_);

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

		criAtomExPlayer_Destroy(player_);

		criAtomExVoicePool_Free(hcamx_voice_pool_);
		criAtomExVoicePool_Free(std_voice_pool_);

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
		criAtomDbas_Destroy(dbas_id_);

		criAtomEx_Finalize_WASAPI();

		CoUninitialize();
	}

	void Adx2leWrapper::PositioningSoundSourceUpdate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
	{
		source_pos_.x += pos.x;
		source_pos_.y += pos.y;
		source_pos_.z += pos.z;

		source_velocity_.x += velocity.x;
		source_velocity_.y += velocity.y;
		source_velocity_.z += velocity.z;

		criAtomEx3dSource_SetPosition(source_, &source_pos_);
		criAtomEx3dSource_SetVelocity(source_, &source_velocity_);
		criAtomEx3dSource_Update(source_);
	}

	void Adx2leWrapper::PositioningSoundListenerUpdate(DirectX::SimpleMath::Vector3& pos
		, DirectX::SimpleMath::Vector3& front, DirectX::SimpleMath::Vector3& up)
	{
		listener_pos_.x = pos.x;
		listener_pos_.y = pos.y;
		listener_pos_.z = pos.z;

		listener_front_.x = front.x;
		listener_front_.y = front.y;
		listener_front_.z = front.z;

		listener_top_.x = up.x;
		listener_top_.y = up.y;
		listener_top_.z = up.z;

		criAtomEx3dListener_SetPosition(listener_, &listener_pos_);
		criAtomEx3dListener_SetOrientation(listener_, &listener_front_, &listener_top_);
		criAtomEx3dListener_Update(listener_);
	}

	void Adx2leWrapper::DecreaseAisac(void)
	{
		CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(player_, aisac_control_id_) - DELTA_AISAC_VALUE;
		criAtomExPlayer_SetAisacById(player_, aisac_control_id_, aisac_val);
		criAtomExPlayer_UpdateAll(player_);
	}

	void Adx2leWrapper::IncreaseAisac(void)
	{
		CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(player_, aisac_control_id_) + DELTA_AISAC_VALUE;
		criAtomExPlayer_SetAisacById(player_, aisac_control_id_, aisac_val);
		criAtomExPlayer_UpdateAll(player_);
	}

	void Adx2leWrapper::LoadAcb(const std::string& acb_file_path, const std::string& awb_file_path, unsigned int scene)
	{
		acb_hn_[scene] = criAtomExAcb_LoadAcbFile(nullptr, acb_file_path.c_str(), nullptr, awb_file_path.c_str(), nullptr, 0);

		current_scene_id_ = scene;
	}

	void Adx2leWrapper::ReleaseAcb(unsigned int scene)
	{
		criAtomExAcb_Release(acb_hn_[scene]);
		current_scene_id_ = 0;
	}

	void Adx2leWrapper::ReleaseAcbAll(void)
	{
		criAtomExAcb_ReleaseAll();
	}

	CriAtomExPlaybackId Adx2leWrapper::PlayGeneralCue(CriAtomExCueId start_cue_id)
	{
		criAtomExPlayer_SetCueId(player_, acb_hn_[0], start_cue_id);

		return playback_id_ = criAtomExPlayer_Start(player_);
	}

	CriAtomExPlaybackId Adx2leWrapper::PlayCurrentCue(CriAtomExCueId start_cue_id)
	{
		criAtomExPlayer_SetCueId(player_, acb_hn_[current_scene_id_], start_cue_id);

		return playback_id_ = criAtomExPlayer_Start(player_);
	}

	void Adx2leWrapper::StopCue(CriAtomExPlaybackId cue_id)
	{
		criAtomExPlayback_Stop(cue_id);

		/* ToDo */
		ResetAisac();
	}

	void Adx2leWrapper::Stop(void)
	{
		criAtomExPlayer_Stop(player_);
		ResetAisac();
	}

	void Adx2leWrapper::PauseCue(CriAtomExPlaybackId cue_id, int is_paused)
	{
		criAtomExPlayback_Pause(cue_id, is_paused);
	}

	void Adx2leWrapper::Pause(int is_paused)
	{
		criAtomExPlayer_Pause(player_, is_paused);
	}

	void Adx2leWrapper::UpPitchAll(void)
	{
		if (pitch_ < MAX_PITCH_VALUE)
			pitch_ += PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(player_, pitch_);
		criAtomExPlayer_UpdateAll(player_);
	}

	void Adx2leWrapper::UpPitch(CriAtomExPlaybackId cue_id)
	{
		if (pitch_ < MAX_PITCH_VALUE)
			pitch_ += PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(player_, pitch_);
		criAtomExPlayer_Update(player_, cue_id);
	}

	void Adx2leWrapper::DownPitchAll(void)
	{
		if (pitch_ > -MAX_PITCH_VALUE)
			pitch_ -= PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(player_, pitch_);
		criAtomExPlayer_UpdateAll(player_);
	}

	void Adx2leWrapper::DownPitch(CriAtomExPlaybackId cue_id)
	{
		if (pitch_ > -MAX_PITCH_VALUE)
			pitch_ -= PITCH_CHANGE_VALUE;

		criAtomExPlayer_SetPitch(player_, pitch_);
		criAtomExPlayer_Update(player_, cue_id);
	}

	void Adx2leWrapper::RestorePitchAll(void)
	{
		criAtomExPlayer_SetPitch(player_, 0);
		criAtomExPlayer_UpdateAll(player_);
	}

	void Adx2leWrapper::RestorePitch(CriAtomExPlaybackId cue_id)
	{
		criAtomExPlayer_SetPitch(player_, 0);
		criAtomExPlayer_Update(player_, cue_id);
	}

	CriAtomExPlayerHn& Adx2leWrapper::GetPlayer(void)
	{
		return player_;
	}

	int64_t Adx2leWrapper::GetTimeMS(CriAtomExPlaybackId playback_id)
	{
		return criAtomExPlayback_GetTime(playback_id);
	}
}