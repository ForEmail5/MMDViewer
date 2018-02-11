#ifndef D3D_STATE_H
#define D3D_STATE_H

#include "D3D.h"

/* 混合操作 */
enum BlendOp
{
	BlendOp_Add			= D3D11_BLEND_OP_ADD,
	BlendOp_Subtract	= D3D11_BLEND_OP_SUBTRACT,
	BlendOp_RevSubract	= D3D11_BLEND_OP_REV_SUBTRACT,
	BlendOp_Min			= D3D11_BLEND_OP_MIN,
	BlendOp_Max			= D3D11_BLEND_OP_MAX
};

/* 混合因子 */
enum BlendFactor
{
	BlendFactor_Zero				= D3D11_BLEND_ZERO,
	BlendFactor_One					= D3D11_BLEND_ONE,
	BlendFactor_SrcColor			= D3D11_BLEND_SRC_COLOR,
	BlendFactor_OneMinusSrcColor	= D3D11_BLEND_INV_SRC_COLOR,
	BlendFactor_SrcAlpha			= D3D11_BLEND_SRC_ALPHA,
	BlendFactor_OneMinusSrcAlpha	= D3D11_BLEND_INV_SRC_ALPHA,
	BlendFactor_DestAlpha			= D3D11_BLEND_DEST_ALPHA,
	BlendFactor_OneMinusDestAlpha	= D3D11_BLEND_INV_DEST_ALPHA,
	BlendFactor_DestColor			= D3D11_BLEND_DEST_COLOR,
	BlendFactor_OneMinusDestColor	= D3D11_BLEND_INV_DEST_COLOR,
	BlendFactor_SrcAlphaSat			= D3D11_BLEND_SRC_ALPHA_SAT,
	BlendFactor_BlendFactor			= D3D11_BLEND_BLEND_FACTOR,
	BlendFactor_OneMinusBlendFactor = D3D11_BLEND_INV_BLEND_FACTOR,
	BlendFactor_Src1Color			= D3D11_BLEND_SRC1_COLOR,
	BlendFactor_OneMinusSrc1Color	= D3D11_BLEND_INV_SRC1_COLOR,
	BlendFactor_Src1Alpha			= D3D11_BLEND_SRC1_ALPHA,
	BlendFactor_OneMinusSrc1Alpha	= D3D11_BLEND_INV_SRC1_ALPHA
};

/* 比较函数 */
enum CompareFunc
{
	CompareFunc_Never		= D3D11_COMPARISON_NEVER,
	CompareFunc_Less		= D3D11_COMPARISON_LESS,
	CompareFunc_Equal		= D3D11_COMPARISON_EQUAL,
	CompareFunc_LessEqual	= D3D11_COMPARISON_LESS_EQUAL,
	CompareFunc_Grater		= D3D11_COMPARISON_GREATER,
	CompareFunc_NotEqual	= D3D11_COMPARISON_NOT_EQUAL,
	CompareFunc_GraterEqual = D3D11_COMPARISON_GREATER_EQUAL,
	CompareFunc_Always		= D3D11_COMPARISON_ALWAYS
};

/* 模板操作 */
enum StencilOp
{
	StencilOp_Keep		= D3D11_STENCIL_OP_KEEP,
	StencilOp_Zero		= D3D11_STENCIL_OP_ZERO,
	StencilOp_Replace	= D3D11_STENCIL_OP_REPLACE,
	StencilOp_IncrSat	= D3D11_STENCIL_OP_INCR_SAT,
	StencilOp_DecrSat	= D3D11_STENCIL_OP_DECR_SAT,
	StencilOp_Invert	= D3D11_STENCIL_OP_INVERT,
	StencilOp_Incr		= D3D11_STENCIL_OP_INCR,
	StencilOp_Decr		= D3D11_STENCIL_OP_DECR
};

struct DepthStencilDesc
{
	int stencil_ref;
	bool enable_depth;
	bool enable_stencil;
	CompareFunc depth_func;
	CompareFunc stencil_func;
	StencilOp stencil_faild;
	StencilOp stencil_pass_depth_faild;
	StencilOp stencil_pass_depth_pass;
};

struct BlendStateDesc
{
	bool enable_alpha;
	BlendOp color;
	BlendOp alpha;

	BlendFactor src;
	BlendFactor src_alpha;
	BlendFactor dest;
	BlendFactor dest_alpha;
};


class D3DGraphicsContext;

//---------------------------------------------------------------
// DepthStencilState
//---------------------------------------------------------------
class DepthStencilState
{
public:
	DepthStencilState(D3DGraphicsContext* gc, const DepthStencilDesc& desc);

	~DepthStencilState();

	void Bind();

	static DepthStencilState* CreateDefualtState(D3DGraphicsContext* gc);

	static DepthStencilState* CreateBoneRenderState(D3DGraphicsContext* gc);

private:
	D3DGraphicsContext* gc;
	ID3D11DepthStencilState* state;

	int stencil_ref;
};


//---------------------------------------------------------------
// BlendState
//---------------------------------------------------------------
class BlendState
{
public:
	BlendState(D3DGraphicsContext* gc, const BlendStateDesc& desc);

	~BlendState();

	void Bind();

	static BlendState* CreateDefaultState(D3DGraphicsContext* gc);

	static BlendState* CreateAlphaState(D3DGraphicsContext* gc);

private:
	D3DGraphicsContext* gc;
	ID3D11BlendState* state;
};

#endif