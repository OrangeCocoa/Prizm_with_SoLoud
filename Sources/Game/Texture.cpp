
#include"Texture.h"
#include"Resource.h"
#include"..\Utilities\Utils.h"
#include"..\Utilities\Log.h"

#include"..\..\ThirdParty\Includes\DirectXTex\DirectXTex.h"
//#include"..\..\ThirdParty\Includes\stb\stb_image.h"

/* ToDo : stb image
JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
PNG 1/2/4/8/16-bit-per-channel
TGA (not sure what subset, if a subset)
BMP non-1bpp, non-RLE
PSD (composited view only, no extra channels, 8/16 bit-per-channel)
GIF (*comp always reports as 4-channel)
HDR (radiance rgbE format)
PIC (Softimage PIC)
PNM (PPM and PGM binary only)
*/

namespace Prizm
{
	// Resource directory from Texture.cpp 
	const std::string TEXTURE_DIR = RESOURCE_DIR + "Textures/";

	class Texture::Impl
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _tex_2d;
		unsigned _width, _height;

		std::string _file_name;

		Impl(void){}
	};

	Texture::Texture(void) : _impl(std::make_unique<Impl>()){}
	Texture::~Texture() = default;

	void Texture::LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string& filename)
	{
		if (filename.empty() || filename == "\"\"") return;

		_impl->_file_name = filename;
		std::string path = TEXTURE_DIR + filename;
		std::wstring wpath(path.begin(), path.end());

		std::unique_ptr<DirectX::ScratchImage> img = std::make_unique<DirectX::ScratchImage>();

		std::string extension = path.substr(path.find_last_of("."), path.size());

		if(extension == ".tga" || extension == ".TGA")
		{
			if (succeeded(LoadFromTGAFile(wpath.c_str(), nullptr, *img)))
			{
				CreateShaderResourceView(device.Get(), img->GetImages(), img->GetImageCount(), img->GetMetadata(), &_impl->_srv);

				// get srv from img
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				_impl->_srv->GetDesc(&srvDesc);

				// read width & height
				Microsoft::WRL::ComPtr<ID3D11Resource> resource;
				_impl->_srv->GetResource(&resource);

				if (succeeded(resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_impl->_tex_2d.GetAddressOf()))))
				{
					D3D11_TEXTURE2D_DESC desc;
					_impl->_tex_2d->GetDesc(&desc);
					_impl->_width = desc.Width;
					_impl->_height = desc.Height;
				}

				resource.Reset();
			}
		}
		else
		{
			if (succeeded(LoadFromWICFile(wpath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *img)))
			{
				CreateShaderResourceView(device.Get(), img->GetImages(), img->GetImageCount(), img->GetMetadata(), &_impl->_srv);

				// get srv from img
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				_impl->_srv->GetDesc(&srvDesc);

				// read width & height
				Microsoft::WRL::ComPtr<ID3D11Resource> resource;
				_impl->_srv->GetResource(&resource);

				if (succeeded(resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_impl->_tex_2d.GetAddressOf()))))
				{
					D3D11_TEXTURE2D_DESC desc;
					_impl->_tex_2d->GetDesc(&desc);
					_impl->_width = desc.Width;
					_impl->_height = desc.Height;
				}

				resource.Reset();
			}
		}
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Texture::GetSRV(void)
	{
		return _impl->_srv;
	}

	const DirectX::SimpleMath::Vector2 Texture::GetTextureSize(void)
	{
		return DirectX::SimpleMath::Vector2(static_cast<float>(_impl->_width), static_cast<float>(_impl->_height));
	}
}