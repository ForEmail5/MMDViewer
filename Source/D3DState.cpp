#include "D3DState.h"
#include "D3DGraphicsContext.h"
#include "Debug.h"


//---------------------------------------------------------------
// DepthStencilState
//---------------------------------------------------------------
DepthStencilState::DepthStencilState(D3DGraphicsContext* gc, const DepthStencilDesc& desc) 
: gc(gc)
, state(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC d3d_desc;
	d3d_desc.DepthEnable = desc.enable_depth;
	d3d_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3d_desc.DepthFunc = ( D3D11_COMPARISON_FUNC ) desc.depth_func;

	d3d_desc.StencilEnable = desc.enable_stencil;
	d3d_desc.StencilReadMask = 0xFF;
	d3d_desc.StencilWriteMask = 0xFF;

	d3d_desc.FrontFace.StencilFailOp = ( D3D11_STENCIL_OP ) desc.stencil_faild;
	d3d_desc.FrontFace.StencilDepthFailOp = ( D3D11_STENCIL_OP ) desc.stencil_pass_depth_faild;
	d3d_desc.FrontFace.StencilPassOp = ( D3D11_STENCIL_OP ) desc.stencil_pass_depth_pass;
	d3d_desc.FrontFace.StencilFunc = ( D3D11_COMPARISON_FUNC ) desc.stencil_func;

	d3d_desc.BackFace.StencilFailOp = ( D3D11_STENCIL_OP ) desc.stencil_faild;
	d3d_desc.BackFace.StencilDepthFailOp = ( D3D11_STENCIL_OP ) desc.stencil_pass_depth_faild;
	d3d_desc.BackFace.StencilPassOp = ( D3D11_STENCIL_OP ) desc.stencil_pass_depth_pass;
	d3d_desc.BackFace.StencilFunc = ( D3D11_COMPARISON_FUNC ) desc.stencil_func;

	HRESULT result = gc->GetDevice()->CreateDepthStencilState(&d3d_desc, &state);
	if ( FAILED(result) ) return;
}

DepthStencilState::~DepthStencilState()
{
	SAFE_RELEASE(state);
}

void DepthStencilState::Bind()
{
	if ( state == false )
	{
		LOG_DEBUG("DepthStencilState is null");
		return;
	}
	gc->GetDeviceContext()->OMSetDepthStencilState(state, stencil_ref);
}

DepthStencilState* DepthStencilState::CreateDefualtState(D3DGraphicsContext* gc)
{
	DepthStencilDesc desc;
	desc.enable_depth = true;
	desc.depth_func = CompareFunc::CompareFunc_Less;

	desc.stencil_ref = 1;
	desc.enable_stencil = false;
	desc.stencil_func = CompareFunc::CompareFunc_Always;
	desc.stencil_faild = StencilOp::StencilOp_Keep;
	desc.stencil_pass_depth_faild = StencilOp::StencilOp_Keep;
	desc.stencil_pass_depth_pass = StencilOp::StencilOp_Keep;

	return new DepthStencilState(gc, desc);
}

DepthStencilState* DepthStencilState::CreateBoneRenderState(D3DGraphicsContext* gc)
{
	DepthStencilDesc desc;
	desc.enable_depth = true;
	desc.depth_func = CompareFunc::CompareFunc_Always;

	desc.stencil_ref = 1;
	desc.enable_stencil = false;
	desc.stencil_func = CompareFunc::CompareFunc_Always;
	desc.stencil_faild = StencilOp::StencilOp_Keep;
	desc.stencil_pass_depth_faild = StencilOp::StencilOp_Keep;
	desc.stencil_pass_depth_pass = StencilOp::StencilOp_Keep;

	return new DepthStencilState(gc, desc);
}

//---------------------------------------------------------------
// BlendState
//---------------------------------------------------------------
BlendState::BlendState(D3DGraphicsContext* gc, const BlendStateDesc& desc)
: gc(gc)
, state(nullptr)
{
	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;	/* 是否对不同的渲染对象使用不同的混合状态 */

	for ( int i = 0; i < 8; i++ )
	{
		blend_desc.RenderTarget[i].BlendEnable = desc.enable_alpha;
		blend_desc.RenderTarget[i].BlendOp = ( D3D11_BLEND_OP ) desc.color;
		blend_desc.RenderTarget[i].BlendOpAlpha = ( D3D11_BLEND_OP ) desc.alpha;
		blend_desc.RenderTarget[i].SrcBlend = ( D3D11_BLEND ) desc.src;
		blend_desc.RenderTarget[i].DestBlend = ( D3D11_BLEND ) desc.dest;
		blend_desc.RenderTarget[i].SrcBlendAlpha = ( D3D11_BLEND ) desc.src_alpha;
		blend_desc.RenderTarget[i].DestBlendAlpha = ( D3D11_BLEND ) desc.dest_alpha;
		blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	HRESULT result = gc->GetDevice()->CreateBlendState(&blend_desc, &state);
	if ( FAILED(result) ) return;
}

BlendState::~BlendState()
{
	SAFE_RELEASE(state);
}

void BlendState::Bind()
{
	if ( state == nullptr )
	{
		LOG_DEBUG("BlendState is null");
		return;
	}

	float blend_factor[4] = { 1, 1, 1, 1 };
	gc->GetDeviceContext()->OMSetBlendState(state, blend_factor, 0xFFFFFFFF);
}

BlendState* BlendState::CreateDefaultState(D3DGraphicsContext* gc)
{
	BlendStateDesc desc;
	desc.enable_alpha = false;
	desc.color = BlendOp::BlendOp_Add;
	desc.alpha = BlendOp::BlendOp_Add;
	desc.src = BlendFactor::BlendFactor_One;
	desc.dest = BlendFactor::BlendFactor_Zero;
	desc.src_alpha = BlendFactor::BlendFactor_One;
	desc.dest_alpha = BlendFactor::BlendFactor_Zero;

	return new BlendState(gc, desc);
}

BlendState* BlendState::CreateAlphaState(D3DGraphicsContext* gc)
{
	BlendStateDesc desc;
	desc.enable_alpha = true;
	desc.color = BlendOp::BlendOp_Add;
	desc.alpha = BlendOp::BlendOp_Add;
	desc.src = BlendFactor::BlendFactor_SrcAlpha;
	desc.dest = BlendFactor::BlendFactor_OneMinusSrcAlpha;
	desc.src_alpha = BlendFactor::BlendFactor_One;
	desc.dest_alpha = BlendFactor::BlendFactor_Zero;

	return new BlendState(gc, desc);
}