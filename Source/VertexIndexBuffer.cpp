#include "VertexIndexBuffer.h"
#include "Debug.h"

VertexIndexBuffer::VertexIndexBuffer(ID3D11Device* device, BufferUsage usage)
{
	this->device = device;
	this->usage = usage;
	this->buffer = nullptr;
	this->size = 0;
}

VertexIndexBuffer::~VertexIndexBuffer()
{
	SAFE_RELEASE(buffer);
}

void VertexIndexBuffer::Create(size_t size)
{
	this->size = size;
	static int flag[2] = { D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER };

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = flag[usage];
	buffer_desc.ByteWidth = size;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	if ( FAILED(device->CreateBuffer(&buffer_desc, 0, &buffer)) )
	{
		Debug::LogAndWrite("Faild to create buffer");
	}
}

void VertexIndexBuffer::Create(void* data, int size)
{
	this->size = size;
	static int flag[2] = { D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER };

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = flag[usage];
	buffer_desc.ByteWidth = size;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem = data;
	sub_data.SysMemPitch = 0;
	sub_data.SysMemSlicePitch = 0;

	if ( FAILED(device->CreateBuffer(&buffer_desc, &sub_data, &buffer)) )
	{
		Debug::LogAndWrite("Faild to create buffer");
	}
}

void VertexIndexBuffer::UploadData(size_t offset, void* data, size_t data_size)
{
	if ( offset < 0 || data_size < 0 || (data_size + offset) > size ) return;

	ID3D11DeviceContext* device_context = nullptr;
	device->GetImmediateContext(&device_context);

	D3D11_MAPPED_SUBRESOURCE sub;
	device_context->Map(buffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &sub);
	memcpy((( char* ) sub.pData) + offset, (( char* ) data) + offset, data_size);
	device_context->Unmap(buffer, 0);

}

VertexIndexBuffer* VertexIndexBuffer::CreateVertexBuffer(ID3D11Device* device, int size)
{
	VertexIndexBuffer* buffer = new VertexIndexBuffer(device, BufferUsage::Usage_VertexBuffer);
	buffer->Create(size);
	return buffer;
}

VertexIndexBuffer* VertexIndexBuffer::CreateVertexBuffer(ID3D11Device* device, void* data, int size)
{
	VertexIndexBuffer* buffer = new VertexIndexBuffer(device, BufferUsage::Usage_VertexBuffer);
	buffer->Create(data, size);
	return buffer;
}

VertexIndexBuffer* VertexIndexBuffer::CreateIndexBuffer(ID3D11Device* device, int size)
{
	VertexIndexBuffer* buffer = new VertexIndexBuffer(device, BufferUsage::Usage_IndexBuffer);
	buffer->Create(size);
	return buffer;
}

VertexIndexBuffer* VertexIndexBuffer::CreateIndexBuffer(ID3D11Device* device, void* data, int size)
{
	VertexIndexBuffer* buffer = new VertexIndexBuffer(device, BufferUsage::Usage_IndexBuffer);
	buffer->Create(data, size);
	return buffer;
}