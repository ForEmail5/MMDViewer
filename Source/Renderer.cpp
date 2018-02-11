#include "Renderer.h"
#include "Model.h"
#include "Debug.h"
#include "PathHelper.h"
#include "CeShi.h"

Renderer::Renderer(D3DGraphicsContext* gc)
{
	graphics_context = gc;

	CeShi::GetInstance()->SetRenderer(this);

	this->CreateShader();
	this->CreateBuffer();
	this->Reset();
}

Renderer::~Renderer()
{
	delete color_shader;
	delete color_material;

	delete line_ibuffer;
	delete line_vbuffer;

	delete triangle_ibuffer;
	delete triangle_vbuffer;

	delete model_shader;
}

void Renderer::CreateBuffer()
{
	ID3D11Device* device = graphics_context->GetDevice();

	RenderUnit unit;
	unit.index_num = 0;
	unit.index_offset = 0;
	unit.material = color_material;

	/* 线绘制所需缓冲区 */
	memset(line_vertex_buffer, 0, sizeof( char ) *line_buffer_size);
	line_vbuffer = VertexIndexBuffer::CreateVertexBuffer(device, line_buffer_size);
	line_ibuffer = VertexIndexBuffer::CreateIndexBuffer(device, sizeof( unsigned int ) * line_index_size);

	VertexData vdl;
	vdl.offset = 0;
	vdl.slot = 0;
	vdl.stride = sizeof(ColorVertex);
	vdl.vertex_buffer = line_vbuffer;

	line_render_data.index_buffer = line_ibuffer;
	line_render_data.prim_type = PrimType::PT_Linelist;
	line_render_data.vertex_datas.push_back(vdl);
	line_render_data.render_units.push_back(unit);

	/* 三角形绘制所需缓冲区 */
	memset(triangle_vertex_buffer, 0, sizeof( char ) *triangle_buffer_size);
	triangle_vbuffer = VertexIndexBuffer::CreateVertexBuffer(device, triangle_buffer_size);
	triangle_ibuffer = VertexIndexBuffer::CreateIndexBuffer(device, sizeof( unsigned int ) * triangle_index_size);

	VertexData vdt;
	vdt.offset = 0;
	vdt.slot = 0;
	vdt.stride = sizeof(ColorVertex);
	vdt.vertex_buffer = triangle_vbuffer;

	triangle_render_data.index_buffer = triangle_ibuffer;
	triangle_render_data.prim_type = PrimType::PT_Trianglelist;
	triangle_render_data.vertex_datas.push_back(vdt);
	triangle_render_data.render_units.push_back(unit);
}

void Renderer::CreateShader()
{
	color_shader = new Shader(
		graphics_context->GetDevice(), 
		PathHelper::ShaderFullPath("Color_vs.hlsl"), 
		PathHelper::ShaderFullPath("Color_ps.hlsl"));
	color_shader->SetVertexAttribute(0, "POSITION", 0, 0, mmd_offsetof(ColorVertex, position));
	color_shader->SetVertexAttribute(1, "COLOR", 0, 0, mmd_offsetof(ColorVertex, color));

	color_material = new Material;
	color_material->SetShader(color_shader);
	color_material->SetBlendState(graphics_context->GetDefaultBlendState());
	//color_material->SetDepthStencilState(DepthStencilState::CreateBoneRenderState(graphics_context));
	color_material->SetDepthStencilState(graphics_context->GetDefaultDepthStencilState());

	model_shader = new Shader(
		graphics_context->GetDevice(), 
		PathHelper::ShaderFullPath("Skinning_vs.hlsl"), 
		PathHelper::ShaderFullPath("Skinning_ps.hlsl"));

	model_shader->SetVertexAttribute(0, "POSITION", 0, 0, mmd_offsetof(ModelPosition, position));
	model_shader->SetVertexAttribute(1, "NORMAL", 0, 1, mmd_offsetof(ModelVertex, normal));
	model_shader->SetVertexAttribute(2, "TEXCOORD", 0, 1, mmd_offsetof(ModelVertex, texcoord));
	model_shader->SetVertexAttribute(3, "WEIGHTS", 0, 1, mmd_offsetof(ModelVertex, weights));
	model_shader->SetVertexAttribute(4, "BONES", 0, 1, mmd_offsetof(ModelVertex, bones));
}

void Renderer::Reset()
{
	line_vertex_count = 0;
	line_index_count = 0;

	triangle_vertex_count = 0;
	triangle_index_count = 0;
}

void Renderer::DrawLine(const Vector3D& v1, const Vector3D& v2, const Color& color)
{
	if ( line_vertex_count + 2 >= line_index_size )
	{
		this->RenderGeometry();
		line_index_count = 0;
		line_vertex_count = 0;
	}

	ColorVertex* vp = ( ColorVertex* ) line_vertex_buffer;
	vp[line_vertex_count + 0].position = v1;
	vp[line_vertex_count + 0].color = color;
	vp[line_vertex_count + 1].position = v2;
	vp[line_vertex_count + 1].color = color;
	line_vertex_count += 2;

	unsigned int* ip = ( unsigned int* ) line_index_buffer;
	ip[line_index_count + 0] = line_index_count + 0;
	ip[line_index_count + 1] = line_index_count + 1;
	line_index_count += 2;
}

void Renderer::DrawPlane(const Vector3D& position, float size, const Color& color)
{
	if ( sizeof( ColorVertex ) * triangle_vertex_count + 4 >= triangle_buffer_size )
	{
		this->RenderGeometry();
		triangle_vertex_count = 0;
		triangle_index_count = 0;
	}
	float half_size = size * 0.5f;
	ColorVertex* vp = ( ColorVertex* ) triangle_vertex_buffer;
	vp[triangle_vertex_count + 0].position.Set(-half_size, 0, -half_size);
	vp[triangle_vertex_count + 1].position.Set( half_size, 0, -half_size);
	vp[triangle_vertex_count + 2].position.Set( half_size, 0,  half_size);
	vp[triangle_vertex_count + 3].position.Set(-half_size, 0,  half_size);

	for ( int i = 0; i < 8; i++ )
	{
		vp[triangle_vertex_count + i].position = position + vp[triangle_vertex_count + i].position;
	}
	vp[triangle_vertex_count + 0].color = color;
	vp[triangle_vertex_count + 1].color = color;
	vp[triangle_vertex_count + 2].color = color;
	vp[triangle_vertex_count + 3].color = color;

	unsigned int* ip = ( unsigned int* ) triangle_index_buffer;
	ip[triangle_index_count + 0] = triangle_vertex_count + 0;
	ip[triangle_index_count + 1] = triangle_vertex_count + 1;
	ip[triangle_index_count + 2] = triangle_vertex_count + 2;
	ip[triangle_index_count + 3] = triangle_vertex_count + 0;
	ip[triangle_index_count + 4] = triangle_vertex_count + 2;
	ip[triangle_index_count + 5] = triangle_vertex_count + 3;

	triangle_vertex_count += 4;
	triangle_index_count += 6;
}

void Renderer::DrawBox(const Vector3D& position, float size, const Color& color)
{
	if ( sizeof( ColorVertex ) *triangle_vertex_count + 8 >= triangle_buffer_size )
	{
		this->RenderGeometry();
		triangle_vertex_count = 0;
		triangle_index_count = 0;
	}

	float half_size = size * 0.5f;
	ColorVertex* vp = ( ColorVertex* ) triangle_vertex_buffer;
	vp[triangle_vertex_count + 0].position.Set(-half_size,  half_size, -half_size);
	vp[triangle_vertex_count + 1].position.Set( half_size,  half_size, -half_size);
	vp[triangle_vertex_count + 2].position.Set( half_size,  half_size,  half_size);
	vp[triangle_vertex_count + 3].position.Set(-half_size,  half_size,  half_size);
	vp[triangle_vertex_count + 4].position.Set(-half_size, -half_size, -half_size);
	vp[triangle_vertex_count + 5].position.Set( half_size, -half_size, -half_size);
	vp[triangle_vertex_count + 6].position.Set( half_size, -half_size,  half_size);
	vp[triangle_vertex_count + 7].position.Set(-half_size, -half_size,  half_size);
	for ( int i = 0; i < 8; i++ )
	{
		vp[triangle_vertex_count + i].position = position + vp[triangle_vertex_count + i].position;
	}

	vp[triangle_vertex_count + 0].color = color;
	vp[triangle_vertex_count + 1].color = color;
	vp[triangle_vertex_count + 2].color = color;
	vp[triangle_vertex_count + 3].color = color;
	vp[triangle_vertex_count + 4].color = color;
	vp[triangle_vertex_count + 5].color = color;
	vp[triangle_vertex_count + 6].color = color;
	vp[triangle_vertex_count + 7].color = color;

	static unsigned int idx[36] = {
		0, 2, 3, 0, 1, 2,
		4, 1, 0, 4, 5, 1,
		5, 2, 1, 5, 6, 2,
		7, 0, 3, 7, 4, 0,
		7, 6, 4, 7, 5, 6,
		6, 3, 2, 6, 1, 3
	};

	unsigned int* ip = ( unsigned int* ) triangle_index_buffer;
	for ( int i = 0; i < 36; i++ )
	{
		ip[triangle_index_count + i] = triangle_vertex_count + idx[i];
	}
	triangle_vertex_count += 8;
	triangle_index_count += 36;
}

void Renderer::RenderModel(Model* model)
{
	RenderData& render_data = model->GetRenderData();
	model->Bind();

	this->Render(&render_data, camera->GetVPMatrix(), camera->GetPosition());
}

void Renderer::RenderGeometry()
{
	if ( line_vertex_count > 0 )
	{
		/* 更新缓冲区数据 */
		line_vbuffer->UploadData(0, line_vertex_buffer, sizeof( ColorVertex ) * line_vertex_count);
		line_ibuffer->UploadData(0, line_index_buffer, sizeof( unsigned int ) * line_index_count);

		line_render_data.render_units[0].index_num = line_index_count;
		this->Render(&line_render_data, camera->GetVPMatrix(), camera->GetPosition());
	}

	if ( triangle_vertex_count > 0 )
	{
		/* 更新缓冲区数据 */
		triangle_vbuffer->UploadData(0, triangle_vertex_buffer, sizeof( ColorVertex ) * triangle_vertex_count);
		triangle_ibuffer->UploadData(0, triangle_index_buffer, sizeof( unsigned int ) * triangle_index_count);

		triangle_render_data.render_units[0].index_num = triangle_index_count;
		this->Render(&triangle_render_data, camera->GetVPMatrix(), camera->GetPosition());
	}
	this->Reset();
}

void Renderer::Render(RenderData* render_data, const Matrix4& mat, const Vector3D& camera_pos)
{
	ID3D11DeviceContext* device_context = graphics_context->GetDeviceContext();

	/* [1] 绑定顶点缓冲区到渲染管线 */
	for ( int i = 0; i < render_data->vertex_datas.size(); i++ )
	{
		VertexData vd = render_data->vertex_datas[i];
		ID3D11Buffer* vb = vd.vertex_buffer->GetBuffer();
		device_context->IASetVertexBuffers(vd.slot, 1, &vb, &vd.stride, &vd.offset);
	}

	/* [2] 绑定索引缓冲区到渲染管线 */
	ID3D11Buffer* ib = render_data->index_buffer->GetBuffer();
	device_context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	/* [3] 设置图元 */
	device_context->IASetPrimitiveTopology(( D3D11_PRIMITIVE_TOPOLOGY ) render_data->prim_type);

	/* [4] 绘制 */
	for ( int i = 0; i < render_data->render_units.size(); i++ )
	{
		RenderUnit& unit = render_data->render_units[i];
		if ( unit.material )
		{
			Material* material = unit.material;
			material->SetMatrix("mvp_matrix", mat);
			material->SetVector("camera_pos", Vector4D(camera_pos));

			graphics_context->SetShader(material->GetShader());
			graphics_context->SetBlendState(material->GetBlendState());
			graphics_context->SetDepthStencilState(material->GetDepthStencilState());
			graphics_context->SetMaterial(material);
		}
		graphics_context->DrawIndexed(unit.index_num, unit.index_offset, 0);
	}
}
