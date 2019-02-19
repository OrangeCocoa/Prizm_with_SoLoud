#pragma once

#include<vector>
#include<string>

#include<Adx2le/cri_adx2le.h>
#pragma comment(lib, "Adx2le/cri_ware_pcx86_le_import.lib")

#include<DirectXTK\SimpleMath.h>
#pragma comment(lib, "DirectXTK/DirectXTK.lib")

namespace Prizm
{
	enum AtomPlayerStatus
	{
		S_ERROR,
		S_STOP,
		S_PREP,
		S_PLAYNG,
		S_PLAYEND
	};

	class Adx2leWrapper
	{
	private:
		/* default sound */
		CriAtomExPlayerHn           _player;
		CriAtomExVoicePoolHn        _std_voice_pool;    /* voice pool(ADX/HCA codic) */
		CriAtomExVoicePoolHn        _hcamx_voice_pool;  /* voice pool(HCA-MX) */
		std::vector<CriAtomExAcbHn> _acb_hn;            /* ACB handle */
		CriAtomDbasId               _dbas_id;           /* D-BAS id*/

		/* 3d sound */
		// sample source
		CriAtomEx3dSourceHn _source;
		CriAtomExVector     _source_pos;        /* sound source position */
		CriAtomExVector     _source_velocity;   /* sound source velocity */

		/* listener */
		CriAtomEx3dListenerHn _listener;
		CriAtomExVector       _listener_pos;    /* listener position */
		CriAtomExVector       _listener_front;  /* listener front vector */
		CriAtomExVector       _listener_top;    /* listener up vector */

		/* interactive sound(cross fade) */
		CriAtomExAisacControlId  _aisac_control_id;

		/* all */
		CriAtomExPlaybackId     _playback_id;    /* VoiceキューのプレイバックID(再生開始時に保持) */
		unsigned int            _current_scene_id;
		float                   _pitch;

	public:
		Adx2leWrapper(){}
		~Adx2leWrapper() = default;

		/* call */
		bool PositioningSoundInitialize(void);
		void PositioningSoundFinalize(void);

		bool InteractiveSoundInitialize(void);
		void InteractiveSoundFinalize(void);

		void ResetAisac(void);

		/* all */
		bool Initialize(std::string& acf_file_path);
		AtomPlayerStatus Update(void);
		void Finalize(void);

		/* 3d sound */
		void PositioningSoundSourceUpdate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity);
		void PositioningSoundListenerUpdate(DirectX::SimpleMath::Vector3& pos
			, DirectX::SimpleMath::Vector3& front, DirectX::SimpleMath::Vector3& up);

		/* interactive sound */
		void DecreaseAisac(void);
		void IncreaseAisac(void);

		// シーンごとにキューシートを切り替えするといい
		void LoadAcb(const std::string& acb_file_name, const std::string& awb_file_name, unsigned int scene);
		void ReleaseAcb(unsigned int scene);
		void ReleaseAcbAll(void);

		// CriAtomExPlaybackId(unsined long型)のキューIDを保存して使う
		CriAtomExPlaybackId PlayGeneralCue(CriAtomExCueId cue_id);
		CriAtomExPlaybackId PlayCurrentCue(CriAtomExCueId cue_id);
		void StopCue(CriAtomExPlaybackId id);
		void Stop(void);

		// is_paused はポーズフラグ(int型)を XOR(^=1)でスイッチングして使う
		void PauseCue(CriAtomExPlaybackId cue_id, int is_paused);
		void Pause(int is_paused);

		void UpPitchAll(void);
		void UpPitch(CriAtomExPlaybackId cue_id);
		void DownPitchAll(void);
		void DownPitch(CriAtomExPlaybackId cue_id);
		void RestorePitchAll(void);
		void RestorePitch(CriAtomExPlaybackId cue_id);

		CriAtomExPlayerHn& GetPlayer(void);
		int64_t GetTimeMS(CriAtomExPlaybackId);
	};
}