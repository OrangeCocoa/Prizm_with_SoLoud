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
		CriAtomExPlayerHn           player_;
		CriAtomExVoicePoolHn        std_voice_pool_;    /* voice pool(ADX/HCA codic) */
		CriAtomExVoicePoolHn        hcamx_voice_pool_;  /* voice pool(HCA-MX) */
		std::vector<CriAtomExAcbHn> acb_hn_;            /* ACB handle */
		CriAtomDbasId               dbas_id_;           /* D-BAS id*/

		/* 3d sound */
		// sample source
		CriAtomEx3dSourceHn source_;
		CriAtomExVector source_pos_;        /* sound source position */
		CriAtomExVector source_velocity_;   /* sound source velocity */

		/* listener */
		CriAtomEx3dListenerHn listener_;
		CriAtomExVector listener_pos_;    /* listener position */
		CriAtomExVector listener_front_;  /* listener front vector */
		CriAtomExVector listener_top_;    /* listener up vector */

		/* interactive sound(cross fade) */
		CriAtomExAisacControlId  aisac_control_id_;

		/* all */
		CriAtomExPlaybackId     playback_id_;    /* Voice�L���[�̃v���C�o�b�NID(�Đ��J�n���ɕێ�) */
		unsigned int current_scene_id_;
		float pitch_;

	public:
		Adx2leWrapper(){}
		~Adx2leWrapper() = default;

		/* call */
		bool DLL_EXPORT PositioningSoundInitialize(void);
		void DLL_EXPORT PositioningSoundFinalize(void);

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

		// �V�[�����ƂɃL���[�V�[�g��؂�ւ�����Ƃ���
		void LoadAcb(const std::string& acb_file_name, const std::string& awb_file_name, unsigned int scene);
		void ReleaseAcb(unsigned int scene);
		void ReleaseAcbAll(void);

		// CriAtomExPlaybackId(unsined long�^)�̃L���[ID��ۑ����Ďg��
		CriAtomExPlaybackId PlayGeneralCue(CriAtomExCueId cue_id);
		CriAtomExPlaybackId PlayCurrentCue(CriAtomExCueId cue_id);
		void StopCue(CriAtomExPlaybackId id);
		void Stop(void);

		// is_paused �̓|�[�Y�t���O(int�^)�� XOR(^=1)�ŃX�C�b�`���O���Ďg��
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