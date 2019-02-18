
#include"UI.h"
#include"..\Texture.h"
#include"..\Shader.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Window.h"

namespace Prizm
{
	class UI::Impl
	{
	public:
		// ToDo
		std::unique_ptr<Geometry> _geometry;
		std::shared_ptr<Shader> _shader;
		std::shared_ptr<Texture> _texture;

		DirectX::SimpleMath::Vector2 _position;
	};

	UI::UI(void) : _impl(std::make_unique<Impl>()) {}
	UI::~UI(void) = default;

	bool UI::Initialize()
	{
		_impl->_geometry = std::make_unique<Geometry>(GeometryGenerator::Quad2D(256.f, 256.f, 0, 0));
		_impl->_position = DirectX::SimpleMath::Vector2(0, 0);

		return true;
	}

	void UI::Run(void)
	{

	}

	void UI::Draw(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		_impl->_shader->SetInputLayout(device_context);
		_impl->_shader->SetShader(device_context, ShaderType::VS);
		_impl->_shader->SetShader(device_context, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_NONE);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_STENCIL_DISABLED);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		Graphics::SetPSTexture(0, 1, _impl->_texture->GetSRV());

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		_impl->_geometry->Draw(device_context);
	}

	void UI::Finalize(void)
	{
		_impl->_geometry.reset();
		_impl->_shader.reset();
		_impl->_texture.reset();
	}

	void UI::LoadShader(const std::shared_ptr<Shader>& shader)
	{
		_impl->_shader = shader;
	}

	void UI::LoadTexture(const std::shared_ptr<Texture>& texture)
	{
		_impl->_texture = texture;
	}

	void UI::MovePosition(float x, float y)
	{
		_impl->_geometry->MovePosition2DScreenToRatio(x, y);
		_impl->_position.x += x;
		_impl->_position.y += y;
	}
	DirectX::SimpleMath::Vector2 & UI::GetPosition(void)
	{
		return _impl->_position;
	}
}