
#include"Geometry.h"
#include"Window.h"

namespace Prizm
{
	void Geometry::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dc)
	{
		UINT offset[] = { 0 };
		dc->IASetVertexBuffers(0, 1, _vertex_buffer.buffer_data.GetAddressOf(), &_vertex_buffer.desc.stride, offset);

		dc->IASetIndexBuffer(_index_buffer.buffer_data.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(_topology));

		dc->DrawIndexed(_index_buffer.desc.element_count, 0, 0);
	}

	void Geometry::CleanUp(void)
	{
		_vertex_buffer.CleanUp();
		_index_buffer.CleanUp();
	}
	void Geometry::MovePosition2DScreenToRatio(float x, float y)
	{
		for (auto& vtx : _vertex_2d)
		{
			vtx.position.x += x / (window_width<float> / 2);
			vtx.position.y += y / (window_height<float> / 2);
		}

		_vertex_buffer.Update(Graphics::GetDeviceContext(), static_cast<const void*>(_vertex_2d.data()));
	}
}