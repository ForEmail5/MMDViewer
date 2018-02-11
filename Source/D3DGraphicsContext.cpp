#include "D3DGraphicsContext.h"
#include "RenderWindow.h"
#include "Material.h"


Test::Test(ID3D11Device* device)
{
	ID3D11DeviceContext* device_context;
	device->GetImmediateContext(&device_context);

	//shader = new Shader(device,
	//	"G:\\MyProject\\C++\\MMDViewer\\Assets\\normal.vs",
	//	"G:\\MyProject\\C++\\MMDViewer\\Assets\\normal.ps");

	//shader->SetVertexAttribute(0, "POSITION", 0, 0, mmd_offsetof(Vertex, position));
	//shader->SetVertexAttribute(1, "TEXCOORD", 0, 0, mmd_offsetof(Vertex, texcoord));

	//shader->BindShader();

	/* 顶点数据 */
	//Vertex *vertices;
	//unsigned int *indices;

	//int vertexCount = 24;
	//int indexCount = 36;
	//vertices = new Vertex[vertexCount];
	//indices = new unsigned int[indexCount];

	//float val = -1;
	//vertices[0].position = Vector3D(-val, -val, -val);
	//vertices[0].texcoord = Vector2D(0, 0);

	//vertices[1].position = Vector3D(val, -val, -val);
	//vertices[1].texcoord = Vector2D(1, 0);

	//vertices[2].position = Vector3D(val, val, -val);
	//vertices[2].texcoord = Vector2D(1, 1);

	//vertices[3].position = Vector3D(-val, val, -val);
	//vertices[3].texcoord = Vector2D(0, 1);

	///* 正面 */
	//vertices[4].position = Vector3D(-val, -val, val);
	//vertices[4].texcoord = Vector2D(1, 1);

	//vertices[5].position = Vector3D(val, -val, val);
	//vertices[5].texcoord = Vector2D(0, 0);

	//vertices[6].position = Vector3D(val, val, val);
	//vertices[6].texcoord = Vector2D(0, 1);

	//vertices[7].position = Vector3D(-val, val, val);
	//vertices[7].texcoord = Vector2D(0, 0);

	///* 左面 */
	//vertices[8].position = Vector3D(-val, -val, -val);
	//vertices[8].texcoord = Vector2D(0, 0);

	//vertices[9].position = Vector3D(-val, -val, val);
	//vertices[9].texcoord = Vector2D(0, 1);

	//vertices[10].position = Vector3D(-val, val, val);
	//vertices[10].texcoord = Vector2D(1, 1);

	//vertices[11].position = Vector3D(-val, val, -val);
	//vertices[11].texcoord = Vector2D(1, 0);

	///* 右面 */
	//vertices[12].position = Vector3D(val, -val, -val);
	//vertices[12].texcoord = Vector2D(0, 0);

	//vertices[13].position = Vector3D(val, -val, val);
	//vertices[13].texcoord = Vector2D(0, 1);

	//vertices[14].position = Vector3D(val, val, val);
	//vertices[14].texcoord = Vector2D(1, 1);

	//vertices[15].position = Vector3D(val, val, -val);
	//vertices[15].texcoord = Vector2D(1, 0);

	///* 上面 */
	//vertices[16].position = Vector3D(-val, val, val);
	//vertices[16].texcoord = Vector2D(0, 1);

	//vertices[17].position = Vector3D(val, val, val);
	//vertices[17].texcoord = Vector2D(1, 1);

	//vertices[18].position = Vector3D(val, val, -val);
	//vertices[18].texcoord = Vector2D(1, 0);

	//vertices[19].position = Vector3D(-val, val, -val);
	//vertices[19].texcoord = Vector2D(0, 0);

	///* 下面 */
	//vertices[20].position = Vector3D(-val, -val, val);
	//vertices[20].texcoord = Vector2D(0, 1);

	//vertices[21].position = Vector3D(val, -val, val);
	//vertices[21].texcoord = Vector2D(1, 1);

	//vertices[22].position = Vector3D(val, -val, -val);
	//vertices[22].texcoord = Vector2D(1, 0);

	//vertices[23].position = Vector3D(-val, -val, -val);
	//vertices[23].texcoord = Vector2D(0, 0);

	//// 设置索引缓冲数据.
	//indices[0] = 0;  // 前面
	//indices[1] = 1;
	//indices[2] = 2;
	//indices[3] = 0;
	//indices[4] = 2;
	//indices[5] = 3;

	//indices[6] = 4;  // 后面
	//indices[7] = 5;
	//indices[8] = 6;
	//indices[9] = 4;
	//indices[10] = 6;
	//indices[11] = 7;

	//indices[12] = 8;  // 左面
	//indices[13] = 9;
	//indices[14] = 10;
	//indices[15] = 8;
	//indices[16] = 10;
	//indices[17] = 11;

	//indices[18] = 12;  //右面
	//indices[19] = 13;
	//indices[20] = 14;
	//indices[21] = 12;
	//indices[22] = 14;
	//indices[23] = 15;

	//indices[24] = 16;  // 上面
	//indices[25] = 17;
	//indices[26] = 18;
	//indices[27] = 16;
	//indices[28] = 18;
	//indices[29] = 19;

	//indices[30] = 20;  // 下面
	//indices[31] = 21;
	//indices[32] = 22;
	//indices[33] = 20;
	//indices[34] = 22;
	//indices[35] = 23;

	//vertex_buffer = VertexIndexBuffer::CreateVertexBuffer(device, vertices, sizeof( Vertex ) * vertexCount);
	//index_buffer = VertexIndexBuffer::CreateIndexBuffer(device, indices, sizeof( unsigned int ) * indexCount);

	//delete[] vertices;
	//delete[] indices;

	HRESULT result;

	//std::string name = "G:\\MyProject\\C++\\MMDViewer\\Model\\Nendoroid Miku\\NM_Body.tga";
	std::string name = "G:\\MyProject\\C++\\MMDViewer\\Assets\\image.png";

	// 读取纹理图像
	//result = D3DX11CreateShaderResourceViewFromFileA(device, "G:\\MyProject\\C++\\MMDViewer\\Assets\\image.png", 0, 0, &colorMap, 0);
	//if ( FAILED(result) ) return;

	//---------------------------------------------------------------------
	// 采样
	//---------------------------------------------------------------------
	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&colorMapDesc, &colorMapSample);
	if ( FAILED(result) ) return;

	//unsigned int stride = sizeof(Vertex);
	//unsigned int offset = 0;

	//ID3D11Buffer* vb = vertex_buffer->GetBuffer();
	//device_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

	//ID3D11Buffer* ib = index_buffer->GetBuffer();
	//device_context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	//device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//device_context->PSSetShaderResources(0, 1, &colorMap);
	device_context->PSSetSamplers(0, 1, &colorMapSample);

	//shader->GetBindingResourceByName("colorSampler_")->BindSampler(colorMapSample);

	/* MVP 矩阵 */
	//camera.SetDistance(10);
	//camera.SetTransform(Vector3D(0, 17, 0));

	//struct CBMatrix
	//{
	//	Matrix4 view;
	//	Matrix4 proj;
	//};

	//ID3D11Buffer* cb_buffer;

	//D3D11_BUFFER_DESC cb_buffer_desc;
	//cb_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cb_buffer_desc.ByteWidth = sizeof(CBMatrix);
	//cb_buffer_desc.CPUAccessFlags = 0;
	//cb_buffer_desc.MiscFlags = 0;
	//cb_buffer_desc.StructureByteStride = 0;
	//cb_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	//result = device->CreateBuffer(&cb_buffer_desc, 0, &cb_buffer);
	//if ( FAILED(result) ) return;

	//CBMatrix cb;
	//cb.view = camera.GetViewMatrix();
	//cb.proj = camera.GetProjMatrix();
	//device_context->UpdateSubresource(cb_buffer, 0, 0, &cb, 0, 0);
	//device_context->VSSetConstantBuffers(0, 1, &cb_buffer);
}


D3DGraphicsContext::D3DGraphicsContext(RenderWindow* rw)
	: render_window(rw)
{
	this->CreateDevice();
	this->CreateBlendState();
	this->CreateDepthStencil();
	this->CreateSamplerState();
	this->CreateViewPorts(render_window->GetRenderRect());
	this->CreateRasterizerState();
	this->CreateConstantBuffers();

	this->Reset();
}

D3DGraphicsContext::~D3DGraphicsContext()
{
	SAFE_RELEASE(device);
	SAFE_RELEASE(swap_chain);
	SAFE_RELEASE(device_context);
	SAFE_RELEASE(render_target_view);
	SAFE_RELEASE(depth_stencil_view);

	SAFE_RELEASE(raster_state);
	delete default_Blend_state;
	delete default_depth_stencil_state;

	for ( int i = 0; i < 24; i++ )
	{
		SAFE_RELEASE(d3d_cbuffer.buffers[i]);
		d3d_cbuffer.buffers[i] = nullptr;
	}
}

void D3DGraphicsContext::CreateDevice()
{
	D3D_FEATURE_LEVEL feature_level;
	D3D_FEATURE_LEVEL request_levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	/* 创建交换链描述 */
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = 0;
	swap_chain_desc.BufferDesc.Height = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = 4;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = render_window->GetHwnd();
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	/* [1] 创建设备、设备上下文、交换链 */
	HRESULT result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, request_levels, 3,
		D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, &feature_level, &device_context);
	assert(SUCCEEDED(result));

	/* [2] 创建渲染目标视图 */
	ID3D11Texture2D* back_buffer_texture;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), ( void** ) &back_buffer_texture);

	device->CreateRenderTargetView(back_buffer_texture, 0, &render_target_view);
	back_buffer_texture->Release();
}

void D3DGraphicsContext::CreateBlendState()
{
	default_Blend_state = BlendState::CreateAlphaState(this);
}

void D3DGraphicsContext::CreateDepthStencil()
{
	/* 创建纹理缓冲区 */
	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	swap_chain->GetDesc(&swap_chain_desc);

	depth_stencil_desc.Width = render_window->GetRenderWindowWidth();
	depth_stencil_desc.Height = render_window->GetRenderWindowHeight();
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc = swap_chain_desc.SampleDesc;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	HRESULT result = device->CreateTexture2D(&depth_stencil_desc, 0, &depth_stencil_tex);
	if ( FAILED(result) ) return;

	/* 创建深度模板视图 */
	result = device->CreateDepthStencilView(depth_stencil_tex, 0, &depth_stencil_view);
	if ( FAILED(result) ) return;

	/* 绑定到渲染管线 */
	device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	/* 创建深度模板状态 */
	default_depth_stencil_state = DepthStencilState::CreateDefualtState(this);
}

void D3DGraphicsContext::CreateSamplerState()
{
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));

	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = device->CreateSamplerState(&sampler_desc, &sampler_state);
	if ( FAILED(result) ) return;

	device_context->PSSetSamplers(0, 1, &sampler_state);
}

void D3DGraphicsContext::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = false;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	HRESULT result = device->CreateRasterizerState(&desc, &raster_state);
	if ( FAILED(result) ) return;

	device_context->RSSetState(raster_state);
}

void D3DGraphicsContext::CreateViewPorts(const Rect& rect)
{
	D3D11_VIEWPORT view_port;
	view_port.Width = rect.w;
	view_port.Height = rect.h;
	view_port.MinDepth = 0.0f;
	view_port.MaxDepth = 1.0f;
	view_port.TopLeftX = rect.x;
	view_port.TopLeftY = rect.y;

	device_context->RSSetViewports(1, &view_port);
}

void D3DGraphicsContext::CreateConstantBuffers()
{
	//D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 64;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	for ( int i = 0; i < 24; i++ )
	{
		d3d_cbuffer.buffer_size[i] = 64;
		HRESULT result = device->CreateBuffer(&desc, 0, &d3d_cbuffer.buffers[i]);
		assert(SUCCEEDED(result));
	}

	/* 绑定常量缓冲区到渲染管线 */
	device_context->VSSetConstantBuffers(0, 8, d3d_cbuffer.buffers);
	device_context->GSSetConstantBuffers(0, 8, d3d_cbuffer.buffers + 8);
	device_context->PSSetConstantBuffers(0, 8, d3d_cbuffer.buffers + 16);
}

void D3DGraphicsContext::UploadDataToConstantBuffer(int slot, void* data, int size, ShaderType type)
{
	slot = slot + ( int ) type * 8;

	/* 当缓冲区的空间不足时，为缓冲区扩展空间 */
	if ( size > this->d3d_cbuffer.buffer_size[slot] )
	{
		d3d_cbuffer.buffers[slot]->Release();
		d3d_cbuffer.buffer_size[slot] = size;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.MiscFlags = 0;
		desc.ByteWidth = size;
		desc.StructureByteStride = 0;

		HRESULT result = device->CreateBuffer(&desc, 0, &d3d_cbuffer.buffers[slot]);
		assert(SUCCEEDED(result));

		switch ( type )
		{
		case ShaderType_Vertex:
			device_context->VSSetConstantBuffers(slot, 1, &d3d_cbuffer.buffers[slot]);
			break;
		case ShaderType_Geometry:
			device_context->GSSetConstantBuffers(slot, 1, &d3d_cbuffer.buffers[slot]);
			break;
		case ShaderType_Pixel:
			device_context->PSSetConstantBuffers(slot, 1, &d3d_cbuffer.buffers[slot]);
			break;
		}
	}

	/* 更新缓冲区数据 */
	D3D11_MAPPED_SUBRESOURCE sub_resource;
	device_context->Map(d3d_cbuffer.buffers[slot], 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &sub_resource);
	memcpy(sub_resource.pData, data, size);
	device_context->Unmap(d3d_cbuffer.buffers[slot], 0);
}

void D3DGraphicsContext::SetShader(Shader* shader)
{
	if ( current_shader != shader )
	{
		current_shader = shader;
		current_shader->BindShader();
	}
}

void D3DGraphicsContext::SetMaterial(Material* mat)
{
	if ( current_material != mat )
	{
		current_material = mat;
		current_material->BindMaterial(this);
	}
}

void D3DGraphicsContext::SetBlendState(BlendState* state)
{
	if ( blend_state != state )
	{
		blend_state = state;
		blend_state->Bind();
	}
}

void D3DGraphicsContext::SetDepthStencilState(DepthStencilState* state)
{
	if ( depth_stencil_state != state )
	{
		depth_stencil_state = state;
		depth_stencil_state->Bind();
	}
}

void D3DGraphicsContext::Reset()
{
	draw_count = 0;
	blend_state = nullptr;
	current_shader = nullptr;
	current_material = nullptr;
	depth_stencil_state = nullptr;
}

void D3DGraphicsContext::Clear()
{
	this->Reset();

	//float color[4] = { 0, 0, 0, 0 };
	//float color[4] = { 1, 1, 1, 1 };
	float color[4] = { 0.2, 0.2, 0.2, 1 };
	device_context->ClearRenderTargetView(render_target_view, color);
	device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//struct CBMatrix
	//{
	//	Matrix4 view;
	//	Matrix4 proj;
	//};
	//CBMatrix cb;
	//cb.view = camera.GetViewMatrix();
	//cb.proj = camera.GetProjMatrix();
	//this->UploadDataToConstantBuffer(0, &cb, sizeof(cb), true);
}

void D3DGraphicsContext::DrawIndexed(unsigned int index_count, unsigned int start_index_location, int base_vertex_location)
{
	device_context->DrawIndexed(index_count, start_index_location, base_vertex_location);

	this->draw_count++;
}

void D3DGraphicsContext::Flush()
{
	swap_chain->Present(1, 0);
}