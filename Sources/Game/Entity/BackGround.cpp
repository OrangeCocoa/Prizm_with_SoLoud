
#include"BackGround.h"
#include"..\Texture.h"
#include"..\Shader.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Window.h"

namespace Prizm
{
	class BackGround::Impl
	{
	public:
		// ToDo
		std::unique_ptr<Geometry> _geometry;
		std::shared_ptr<Shader> _shader;
		std::shared_ptr<Texture> _texture;
	};

	BackGround::BackGround(void) : _impl(std::make_unique<Impl>()) {}
	BackGround::~BackGround() = default;

	bool BackGround::Initialize(void)
	{
		_impl->_geometry = std::make_unique<Geometry>(GeometryGenerator::Quad2D(window_width<float>, window_height<float>, 0, 0));

		return true;
	}

	void BackGround::Run(void)
	{

	}

	void BackGround::Draw(void)
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

	void BackGround::Finalize(void)
	{
		_impl->_geometry.reset();
		_impl->_shader.reset();
		_impl->_texture.reset();
	}

	void BackGround::LoadShader(const std::shared_ptr<Shader>& shader)
	{
		_impl->_shader = shader;
	}

	void BackGround::LoadTexture(const std::shared_ptr<Texture>& texture)
	{
		_impl->_texture = texture;
	}
}