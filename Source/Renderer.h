#ifndef RENDERER_H
#define RENDERER_H

#include "MathLib.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Texture2D.h"
#include "D3DGraphicsContext.h"


/* ����ͼԪ */
enum PrimType
{
	PT_Linelist		= D3D11_PRIMITIVE_TOPOLOGY_LINELIST,		// ������
	PT_Pointlist	= D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,		// ���Ƶ�
	PT_Trianglelist	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST		// ����������
};

struct VertexData
{
	unsigned int slot;
	unsigned int stride;
	unsigned int offset;
	VertexIndexBuffer* vertex_buffer;
};

/* ��Ⱦ��Ԫ����Ӧһ�� DrawCall */
struct RenderUnit
{
	unsigned int index_num;
	unsigned int index_offset;

	Material* material;
};

/* ��Ⱦ���� */
struct RenderData
{
	PrimType prim_type;						/* ��ȾͼԪ */
	VertexIndexBuffer* index_buffer;		/* ����������   */
	std::vector<RenderUnit> render_units;	/* ��Ⱦ��Ԫ���� */
	std::vector<VertexData> vertex_datas;	/* ������������ */
};

struct ColorVertex
{
	Vector3D position;
	Color color;
};

class Model;

//---------------------------------------------------------------------------
// Renderer
//---------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(D3DGraphicsContext* gc);

	~Renderer();

	void CreateBuffer();

	void CreateShader();

	void Reset();

	void DrawLine(const Vector3D& v1, const Vector3D& v2, const Color& color);

	void DrawPlane(const Vector3D& position, float size, const Color& color);

	void DrawBox(const Vector3D& position, float size, const Color& color);

	void RenderGeometry();

	void RenderModel(Model* model);

	void Render(RenderData* render_data, const Matrix4& mat, const Vector3D& camera_pos);

	void SetCamera(Camera* cam) { camera = cam; }

	Shader* GetModelShader() { return model_shader; }

private:
	D3DGraphicsContext* graphics_context;

	/****************************** �߻��� ******************************/
	int line_index_count;
	int line_vertex_count;

	/* ���㻺���������������� */
	static const int line_buffer_size = 1024 * 1024;
	static const int line_index_size = line_buffer_size / sizeof(ColorVertex);

	char line_vertex_buffer[line_buffer_size];
	unsigned int line_index_buffer[line_index_size];

	VertexIndexBuffer* line_vbuffer;
	VertexIndexBuffer* line_ibuffer;

	RenderData line_render_data;

	/****************************** �����λ��� ******************************/
	int triangle_index_count;
	int triangle_vertex_count;

	/* ���㻺���������������� */
	static const int triangle_buffer_size = 1024 * 1024;
	static const int triangle_index_size = triangle_buffer_size / sizeof( ColorVertex ) * 1.5;

	char triangle_vertex_buffer[triangle_buffer_size];
	unsigned int triangle_index_buffer[triangle_index_size];

	VertexIndexBuffer* triangle_vbuffer;
	VertexIndexBuffer* triangle_ibuffer;

	RenderData triangle_render_data;

	/****************************** ģ�ͻ��� ******************************/
	Shader* color_shader;
	Material* color_material;

	Camera* camera;
	Shader* model_shader;
};

#endif