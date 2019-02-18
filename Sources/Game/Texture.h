#pragma once

#include<vector>
#include<string>
#include<memory>

#include<wrl\client.h>
#include<d3d11_4.h>

#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	class Texture
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _impl;

	public:
		Texture(void);
		~Texture(void);

		void LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device>&, const std::string&);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV(void);

		const DirectX::SimpleMath::Vector2 GetTextureSize(void);
	};
}