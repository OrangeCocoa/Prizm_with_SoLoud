#pragma once

#include<memory>

#include<Adx2le/cri_adx2le.h>
#include<DirectXTK\SimpleMath.h>

#include"..\..\Utilities\ResourcePool.h"

namespace Prizm
{
	class SoundSource
	{
	private:
		struct Source
		{
			CriAtomEx3dSourceHn source;
			CriAtomExVector source_pos;			/* sound source position */
			CriAtomExVector source_velocity;	/* sound source velocity */

			void Create(const CriAtomExPlayerHn& player)
			{
				source_pos.x = 0; source_pos.y = 0; source_pos.z = 0;
				source_velocity.x = 0; source_velocity.y = 0; source_velocity.z = 0;

				source = criAtomEx3dSource_Create(nullptr, nullptr, 0);
				criAtomExPlayer_Set3dSourceHn(player, source);

				criAtomEx3dSource_SetDopplerFactor(source, 1.f);			// doppler
				criAtomEx3dSource_SetPosition(source, &source_pos);
				criAtomEx3dSource_SetVelocity(source, &source_velocity);	// doppler
				criAtomEx3dSource_Update(source);
			}

			void Update(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
			{
				source_pos.x = pos.x;
				source_pos.y = pos.y;
				source_pos.z = pos.z;

				source_velocity.x += velocity.x;
				source_velocity.y += velocity.y;
				source_velocity.z += velocity.z;

				criAtomEx3dSource_SetPosition(source, &source_pos);
				criAtomEx3dSource_SetVelocity(source, &source_velocity);
				criAtomEx3dSource_Update(source);
			}

			void Destroy(void)
			{
				criAtomEx3dSource_Destroy(source);
			}
		};

		ResourcePool<Source> sound_sources_;

	public:
		// attatch 3d object
		unsigned int Create(const CriAtomExPlayerHn& player);

		void Update(unsigned int ss_id, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity);

		void Destroy(unsigned int ss_id);

		void DestroyAll(void);
	};
}