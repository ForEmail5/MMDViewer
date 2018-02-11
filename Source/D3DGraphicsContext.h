#ifndef D3D_GRAPHICS_CONTEXT_H
#define D3D_GRAPHICS_CONTEXT_H

#include <dxgi.h>
#include <d3dcommon.h>

#include "D3D.h"
#include "Camera.h"
#include "Shader.h"
#include "MathLib.h"
#include "D3DState.h"
#include "VertexIndexBuffer.h"
#include "Texture2D.h"


class Test
{
public:
	Test(ID3D11Device* device);

	~Test()
	{
		//delete shader;
		//delete vertex_buffer;
		//delete index_buffer;
		SAFE_RELEASE(colorMapSample);
		//SAFE_RELEASE(colorMap);
	}

	void Draw();

public:
	Shader* shader;

	VertexIndexBuffer* vertex_buffer;
	VertexIndexBuffer* index_buffer;

	ID3D11SamplerState* colorMapSample;
	ID3D11ShaderResourceView* colorMap;
};

class Material;
class RenderWindow;

class D3DGraphicsContext
{
	struct D3DCBuffer
	{
		ID3D11Buffer* buffers[16];
		int buffer_size[16];
	};

public:
	D3DGraphicsContext(RenderWindow* rw);

	~D3DGraphicsContext();

	void SetShader(Shader* shader);

	void SetMaterial(Material* mat);

	void SetBlendState(BlendState* state);

	void SetDepthStencilState(DepthStencilState* state);

	void Reset();

	void Clear();

	void DrawIndexed(unsigned int index_count, unsigned int start_index_location, int base_vertex_location);

	void Flush();

	void UploadDataToConstantBuffer(int slot, void* data, int size, ShaderType type);

	int GetDrawCount() { return draw_count; }

	ID3D11Device* GetDevice() { return device; }

	IDXGISwapChain* GetSwapChain() { return swap_chain; }

	ID3D11DeviceContext* GetDeviceContext() { return device_context; }

	ID3D11DepthStencilView* GetDepthStencilView() { return depth_stencil_view; }

	ID3D11RenderTargetView* GetRenderTargetView() { return render_target_view; }

	BlendState* GetDefaultBlendState() { return default_Blend_state; }

	DepthStencilState* GetDefaultDepthStencilState() { return default_depth_stencil_state; }

private:
	void CreateDevice();

	void CreateBlendState();

	void CreateDepthStencil();

	void CreateSamplerState();

	void CreateRasterizerState();

	void CreateViewPorts(const Rect& rect);

	void CreateConstantBuffers();

private:
	RenderWindow* render_window;

	/* 渲染管线相关 */
	ID3D11Device* device;
	IDXGISwapChain* swap_chain;
	ID3D11DeviceContext* device_context;

	/* 帧缓冲相关 */
	ID3D11RenderTargetView* render_target_view;

	/* 深度测试和模板测试相关 */
	ID3D11Texture2D* depth_stencil_tex;
	ID3D11DepthStencilView* depth_stencil_view;

	DepthStencilState* depth_stencil_state;
	DepthStencilState* default_depth_stencil_state;

	/* 混合状态相关 */
	BlendState* blend_state;
	BlendState* default_Blend_state;

	/* 光栅化 */
	ID3D11RasterizerState* raster_state;

	/* 纹理采样状态 */
	ID3D11SamplerState* sampler_state;

	/* 默认常量缓冲区 */
	struct D3DCBuffer_
	{
		ID3D11Buffer* buffers[24];
		int buffer_size[24];
	} d3d_cbuffer;

	Test* test;
	int draw_count;

	ID3D11Buffer* cb_buffer;

	Shader* current_shader;
	Material* current_material;
};

#endif