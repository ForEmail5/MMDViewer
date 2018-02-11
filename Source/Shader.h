#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <string>
#include <vector>
#include <D3D11Shader.h>

#include "D3D.h"
#include "MathLib.h"

enum VertexDataType
{
	VDT_Int,
	VDT_Int2,
	VDT_Int4,
	VDT_Uint,
	VDT_Uint2,
	VDT_Uint4,
	VDT_Float,
	VDT_Float2,
	VDT_Float3,
	VDT_Float4
};

enum ShaderType
{
	ShaderType_Vertex,
	ShaderType_Geometry,
	ShaderType_Pixel
};

enum ResourceType
{
	RT_Texture,
	RT_Sampler,
	RT_ConstantBuffer,
	RT_Unknown
};


enum ShaderVariantType
{
	SVT_Int,
	SVT_Float,
	SVT_Vector,
	SVT_Matrix,
	SVT_Unknown
};

//------------------------------------------------------------------------------
// ShaderVariantInfo
//------------------------------------------------------------------------------
struct ShaderVariantInfo
{
	std::string name;
	ShaderVariantType type;

	int start_offset;
	int size;
	int elements;
};

//------------------------------------------------------------------------------
// ShaderConstantBufferInfo
//------------------------------------------------------------------------------
struct ShaderConstantBufferInfo
{
	std::string name;
	int size;
	std::map<std::string, ShaderVariantInfo> shader_veriants;
};

//------------------------------------------------------------------------------
// BindingResource
//------------------------------------------------------------------------------
struct BindingResource
{
	BindingResource()
	{
		this->resource_type = RT_Unknown;
		this->binding_points[0] = -1;
		this->binding_points[1] = -1;
		this->binding_points[2] = -1;
	}

	void SetBindingPoint(ShaderType shader_type, int binding_point)
	{
		this->binding_points[shader_type] = binding_point;
	}

	std::string name;
	ResourceType resource_type;

	int binding_points[3];
	ShaderConstantBufferInfo constant_buffer_info;
};


//------------------------------------------------------------------------------
// Shader
//------------------------------------------------------------------------------
class Shader
{
public:
	typedef std::map<std::string, BindingResource> BindingResourceMap;

	struct VertexAttribute
	{
		std::string semantic;
		int semantic_index;
		VertexDataType type;
		int slot;
		int offset;
	};

public:
	Shader(ID3D11Device* device, const std::string& vs, const std::string& ps);

	Shader(ID3D11Device* device, const std::string& vs, const std::string& gs, const std::string& ps);

	~Shader();

	void CreateShader(const std::string& filename, ShaderType type);

	void BindShader();

	BindingResourceMap GetBindingResources();

	/* 设置顶点属性 */

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, VertexDataType type, int slot, int offset);

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, int* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Int, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector2Di* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Int2, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector4Di* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Int4, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, unsigned int* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Uint, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector2Dui* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Uint2, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector4Dui* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Uint4, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, float* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Float, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector2D* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Float2, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector3D* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Float3, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Vector4D* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Float4, slot, size_t(offset));
	}

	void SetVertexAttribute(int index, const std::string& semantic, int semantic_index, int slot, Color* offset)
	{
		SetVertexAttribute(index, semantic, semantic_index, VertexDataType::VDT_Float4, slot, size_t(offset));
	}

private:
	/* 编译着色程序 */
	ID3D10Blob* CompileShader(ShaderType type, const std::string& filename, const std::string& profile);

	/* 创建顶点输入布局，调用 SetVertexAttribute 函数来设置顶点属性后才可用 */
	void CreateInputLayout();

	/* 获取着色器中的绑定资源，用于将资源绑定到着色器 */
	void FindBindingResource(ShaderType type, ID3DBlob* shader_code);

	ShaderConstantBufferInfo GetConstantBufferInfo(ID3D11ShaderReflection* shader_refletion, const char* name);

private:
	ID3D11Device* device;

	ID3D11VertexShader* vertex_shader;
	ID3D11GeometryShader* geometry_shader;
	ID3D11PixelShader* pixel_shader;

	std::string vertex_code;

	ID3D11InputLayout* input_layout;
	std::vector<VertexAttribute> input_descs;

	BindingResourceMap binding_resources;
};

#endif