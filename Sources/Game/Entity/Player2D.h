#pragma once

#include<memory>
#include"../../Framework/Entity.h"

#include<DirectXTK\SimpleMath.h>

namespace Prizm
{
	class Shader;
	class Texture;

	class Player2D : public Entity
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _impl;

	public:
		Player2D(void);
		~Player2D(void);

		bool Initialize() override;
		void Run(void) override;
		void Draw(void) override;
		void Finalize(void) override;

		void LoadShader(const std::shared_ptr<Shader>&);
		void LoadTexture(const std::shared_ptr<Texture>&);

		void MovePosition(float x, float y);
		DirectX::SimpleMath::Vector2& GetPosition(void);
	};
}