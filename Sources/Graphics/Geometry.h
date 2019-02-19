#pragma once

#include<vector>
#include<wrl\client.h>
#include<DirectXTK/SimpleMath.h>

#include"Graphics.h"
#include"ConstantBuffer.h"
#include"Buffer.h"

namespace Prizm
{
	enum class TopologyType
	{
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,

		TOPOLOGY_MAX
	};

	class Geometry
	{
	private:
		Buffer	_vertex_buffer;
		Buffer	_index_buffer;
		TopologyType _topology;
		std::vector<VertexBuffer2D> _vertex_2d;

	public:
		template<class VertexBufferType>
		Geometry(const std::vector<VertexBufferType>& vertices, const std::vector<unsigned>& indices, TopologyType topology)
		{//3D
			Microsoft::WRL::ComPtr<ID3D11Device> device = Graphics::GetDevice();

			BufferDesc buffer_desc = {};

			buffer_desc.type = BufferType::VERTEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(vertices.size());
			buffer_desc.stride = sizeof(vertices[0]);

			_vertex_buffer = Buffer(buffer_desc);
			_vertex_buffer.Initialize(device.Get(), static_cast<const void*>(vertices.data()));

			buffer_desc.type = BufferType::INDEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(indices.size());
			buffer_desc.stride = sizeof(unsigned);

			_index_buffer = Buffer(buffer_desc);
			_index_buffer.Initialize(device.Get(), static_cast<const void*>(indices.data()));

			_topology = topology;
		}
		
		Geometry(const std::vector<VertexBuffer2D>& vertices, const std::vector<unsigned>& indices, BufferUsage vertex_usage)
		{//2D
			Microsoft::WRL::ComPtr<ID3D11Device> device = Graphics::GetDevice();

			BufferDesc buffer_desc = {};

			buffer_desc.type = BufferType::VERTEX_BUFFER;
			buffer_desc.usage = vertex_usage;
			buffer_desc.element_count = static_cast<unsigned>(vertices.size());
			buffer_desc.stride = sizeof(vertices[0]);

			_vertex_buffer = Buffer(buffer_desc);
			_vertex_buffer.Initialize(device.Get(), static_cast<const void*>(vertices.data()));

			_vertex_2d = vertices;

			buffer_desc.type = BufferType::INDEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(indices.size());
			buffer_desc.stride = sizeof(unsigned);

			_index_buffer = Buffer(buffer_desc);
			_index_buffer.Initialize(device.Get(), static_cast<const void*>(indices.data()));

			_topology = TopologyType::TRIANGLE_LIST;
		}

		void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&);
		void CleanUp(void);

		Buffer GetVertex(void) { return _vertex_buffer; }
		Buffer GetIndex(void) { return _index_buffer; }

		void MovePosition2DScreenToRatio(float x, float y);
	};
}