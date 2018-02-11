#ifndef VERTEX_INDEX_BUFFER_H
#define VERTEX_INDEX_BUFFER_H

#include "D3D.h"


enum BufferUsage
{
	Usage_VertexBuffer,
	Usage_IndexBuffer
};


class VertexIndexBuffer
{
public:
	VertexIndexBuffer(ID3D11Device* device, BufferUsage usage);

	~VertexIndexBuffer();

	void Create(size_t size);

	void Create(void* data, int size);

	void UploadData(size_t offset, void* data, size_t data_size);

	ID3D11Buffer* GetBuffer() { return buffer; }

	static VertexIndexBuffer* CreateVertexBuffer(ID3D11Device* device, int size);
	static VertexIndexBuffer* CreateVertexBuffer(ID3D11Device* device, void* data, int size);

	static VertexIndexBuffer* CreateIndexBuffer(ID3D11Device* device, int size);
	static VertexIndexBuffer* CreateIndexBuffer(ID3D11Device* device, void* data, int size);

private:
	ID3D11Device* device;
	ID3D11Buffer* buffer;

	int size;
	BufferUsage usage;
};


#endif