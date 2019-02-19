
#include<string>

#include"Shader.h"
#include"Resource.h"
#include"..\Utilities\Utils.h"
#include"..\Utilities\Log.h"
#include"..\Graphics\Graphics.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace Prizm
{
	// Resource directory from Shader.cpp
	const std::string SHADER_RESOURCE_DIR = RESOURCE_DIR + "Shaders/";
	constexpr const char* SHADER_COMPILER_VERSIONS[] = { "vs_5_0", "gs_5_0", "hs_5_0", "ds_5_0", "cs_5_0", "ps_5_0" };
	constexpr const char* SHADER_ENTRY_POINTS[] = { "VSMain", "GSMain", "DSMain", "HSMain", "CSMain", "PSMain" };
#ifdef _DEBUG
	constexpr UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	constexpr UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	class Shader::Impl
	{
	public:
		std::string name_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>   _vs;
		Microsoft::WRL::ComPtr<ID3D11PixelShader >   _ps;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> _gs;
		Microsoft::WRL::ComPtr<ID3D11HullShader>     _hs;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>   _ds;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>  _cs;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>    _input_layput;

		std::vector<ShaderTexture> _textures;

		Impl()
			: name_("")
			, _vs(nullptr)
			, _ps(nullptr)
			, _gs(nullptr)
			, _hs(nullptr)
			, _ds(nullptr)
			, _cs(nullptr)
			, _input_layput(nullptr){}
		
		Impl(const std::string& shader_file_name)
			: name_(shader_file_name)
			, _vs(nullptr)
			, _ps(nullptr)
			, _gs(nullptr)
			, _hs(nullptr)
			, _ds(nullptr)
			, _cs(nullptr)
			, _input_layput(nullptr){}

		bool CreateShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			ShaderType type, void* buffer, const size_t shader_binary_size)
		{
			switch (type)
			{
			case ShaderType::VS:
				if (failed(device->CreateVertexShader(buffer, shader_binary_size, nullptr, _vs.GetAddressOf())))
				{
					Log::Error("Error creating vertex shader program.");
					return false;
				}
				break;
			case ShaderType::PS:
				if (failed(device->CreatePixelShader(buffer, shader_binary_size, nullptr, _ps.GetAddressOf())))
				{
					Log::Error("Error creating pixel shader program.");
					return false;
				}
				break;
			case ShaderType::GS:
				if (failed(device->CreateGeometryShader(buffer, shader_binary_size, nullptr, _gs.GetAddressOf())))
				{
					Log::Error("Error creating pixel geometry program.");
					return false;
				}
				break;
			case ShaderType::HS:
				if (failed(device->CreateHullShader(buffer, shader_binary_size, nullptr, _hs.GetAddressOf())))
				{
					Log::Error("Error creating hull shader program.");
					return false;
				}
				break;
			case ShaderType::DS:
				if (failed(device->CreateDomainShader(buffer, shader_binary_size, nullptr, _ds.GetAddressOf())))
				{
					Log::Error("Error creating domain shader program.");
					return false;
				}
				break;
			case ShaderType::CS:
				if (failed(device->CreateComputeShader(buffer, shader_binary_size, nullptr, _cs.GetAddressOf())))
				{
					Log::Error("Error creating conpute shader program.");
					return false;
				}
				break;
			}
			return true;
		}

		bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc, Microsoft::WRL::ComPtr<ID3DBlob>& blob)
		{
			if (failed(device->CreateInputLayout(&element_desc[0], element_desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), _input_layput.GetAddressOf())))
			{
				Log::Error("Error creating input layout.");
				return false;
			}
			return true;
		}
	};

	Shader::Shader()
		: _impl(std::make_unique<Impl>()) {}

	Shader::Shader(const std::string& shader_file_name)
		: _impl(std::make_unique<Impl>(shader_file_name)) {}

	Shader::~Shader() = default;

	bool Shader::CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc)
	{
		std::string dir_path = SHADER_RESOURCE_DIR + _impl->name_;
		const std::wstring path = StrUtils::AsciiToUnicode(dir_path.c_str());

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

		if (failed(D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SHADER_ENTRY_POINTS[type],
			SHADER_COMPILER_VERSIONS[type],
			SHADER_COMPILE_FLAGS,
			0,
			blob.GetAddressOf(),
			error_blob.GetAddressOf())))
		{
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (blob)
		{
			if (failed(_impl->CreateShader(device, type, blob->GetBufferPointer(), blob->GetBufferSize())))
			{
				Log::Error("Do not create shader.");
				return false;
			}
		}
		else
		{
			Log::Error("ID3DBlob is nullptr.");
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (type == ShaderType::VS)
		{
			if (failed(_impl->CreateInputLayout(device, element_desc, blob)))
			{
				Log::Error("Do not create shader.");
			}
		}
		return true;
	}

	bool Shader::CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::string& filepath, const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc)
	{
		_impl->name_ = filepath;

		std::string dir_path = SHADER_RESOURCE_DIR + _impl->name_;
		const std::wstring path = StrUtils::AsciiToUnicode(dir_path.c_str());

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

		if (failed(D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SHADER_ENTRY_POINTS[type],
			SHADER_COMPILER_VERSIONS[type],
			SHADER_COMPILE_FLAGS,
			0,
			blob.GetAddressOf(),
			error_blob.GetAddressOf())))
		{
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (blob)
		{
			if (failed(_impl->CreateShader(device, type, blob->GetBufferPointer(), blob->GetBufferSize())))
			{
				Log::Error("Do not create shader.");
				return false;
			}
		}
		else
		{
			Log::Error("ID3DBlob is nullptr.");
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (type == ShaderType::VS)
		{
			if (failed(_impl->CreateInputLayout(device, element_desc, blob)))
			{
				Log::Error("Do not create shader.");
			}
		}
		return true;
	}

	void Shader::SetShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context, const ShaderType& type)
	{
		switch (type)
		{
		case ShaderType::VS:
			device_context->VSSetShader(_impl->_vs.Get(), nullptr, 0);
			break;

		case ShaderType::PS:
			device_context->PSSetShader(_impl->_ps.Get(), nullptr, 0);
			break;

		case ShaderType::GS:
			device_context->GSSetShader(_impl->_gs.Get(), nullptr, 0);
			break;

		case ShaderType::HS:
			device_context->HSSetShader(_impl->_hs.Get(), nullptr, 0);
			break;

		case ShaderType::DS:
			device_context->DSSetShader(_impl->_ds.Get(), nullptr, 0);
			break;

		case ShaderType::CS:
			device_context->CSSetShader(_impl->_cs.Get(), nullptr, 0);
			break;
		}
	}

	void Shader::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context)
	{
		device_context->IASetInputLayout(_impl->_input_layput.Get());
	}

	void Shader::SetShaderResources(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
		const ShaderType& type,	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& SRVs,
		unsigned int register_slot, unsigned int buffer_num)
	{
		switch (type)
		{
		case ShaderType::VS:
			device_context->VSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		case ShaderType::PS:
			device_context->PSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		case ShaderType::GS:
			device_context->GSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		case ShaderType::HS:
			device_context->HSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		case ShaderType::DS:
			device_context->DSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		case ShaderType::CS:
			device_context->CSSetShaderResources(register_slot, buffer_num, SRVs.GetAddressOf());
			break;
		}
	}

	void Shader::SetUnorderedAccessView(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uavs,
		const unsigned int* uav_inits,
		unsigned int register_slot, unsigned int buffer_num)
	{
		device_context->CSSetUnorderedAccessViews(register_slot, buffer_num,
			reinterpret_cast<ID3D11UnorderedAccessView* const*>(uavs.Get()), uav_inits);
	}

	void Shader::Dispatch(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
		unsigned int thread_group_x, unsigned int thread_group_y, unsigned int thread_group_z)
	{
		device_context->Dispatch(thread_group_x, thread_group_y, thread_group_z);
	}
}