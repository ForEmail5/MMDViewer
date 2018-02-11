#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "D3D.h"
#include "Shader.h"
#include "MathLib.h"
#include "D3DState.h"
#include "Texture2D.h"

class D3DGraphicsContext;



//------------------------------------------------------------------------------
// ConstantBuffer
//------------------------------------------------------------------------------
class ConstantBuffer
{
public:
	ConstantBuffer(const BindingResource& br)
	{
		info = br.constant_buffer_info;
		buffer = new char[info.size];
		memset(buffer, 0, info.size);
	}

	~ConstantBuffer()
	{
		delete buffer;
	}

	int Size() { return info.size; }

	void* Buffer() { return buffer; }

	std::string Name() { return info.name; }

	bool SetValue(void* data, size_t size)
	{
		if ( size > info.size ) return false;

		memcpy(buffer, data, size);
		return true;
	}

	template<class Type>
	bool SetValue(const std::string& name, Type value)
	{
		auto it =  info.shader_veriants.find(name);
		if ( it == info.shader_veriants.end() ) return false;

		ShaderVariantInfo svd = it->second;
		*(( Type* ) (buffer + svd.start_offset)) = value;

		return true;
	}

private:
	char* buffer;
	ShaderConstantBufferInfo info;
};


struct TextureUnit
{
	int binding_points[3];
	Texture2D* texture_2d;
};

struct CBufferUnit
{
	int binding_points[3];
	ConstantBuffer* cb;
};


//------------------------------------------------------------------------------
// Material
//------------------------------------------------------------------------------
class Material
{
public:
	Material();

	~Material();

	void Clear();

	void BindMaterial(D3DGraphicsContext* gc);

	void SetShader(Shader* shader, const std::set<std::string>& skip_cb = std::set<std::string>());

	Shader* GetShader();

	void SetConstantBuffer(const BindingResource& rb);

	BlendState* GetBlendState() { return blend_state; }

	void SetBlendState(BlendState* state) { blend_state = state; }

	DepthStencilState* GetDepthStencilState() { return depth_stencil_state; }

	void SetDepthStencilState(DepthStencilState* state) { depth_stencil_state = state; }

	bool SetTexture(const std::string name, Texture2D* texture);

	bool SetInt(const std::string& name, int value)
	{
		return this->SetValue(name, value);
	}

	bool SetFloat(const std::string& name, float value)
	{
		return this->SetValue(name, value);
	}

	bool SetVector(const std::string& name, Vector4D value)
	{
		return this->SetValue(name, value);
	}

	bool SetMatrix(const std::string& name, Matrix4 matrix)
	{
		return this->SetValue(name, matrix);
	}

	bool SetColor(const std::string name, Color color)
	{
		return this->SetValue(name, color);
	}

	template<class Type>
	bool SetValue(const std::string& name, Type value)
	{
		auto it =  cb_index_table.find(name);
		if ( it != cb_index_table.end() )
		{
			constant_buffers[it->second].cb->SetValue<Type>(name, value);
			return true;
		}
		return false;
	}

private:
	Shader* shader;
	Shader::BindingResourceMap binding_resources;

	std::map<std::string, TextureUnit> textures;

	std::map<std::string, int> cb_index_table;
	std::vector<CBufferUnit> constant_buffers;

	BlendState* blend_state;
	DepthStencilState* depth_stencil_state;
};


//------------------------------------------------------------------------------
// ModelMaterialManager
//------------------------------------------------------------------------------
class ModelMaterialManager
{
public:
	ModelMaterialManager();

	~ModelMaterialManager();

	void SetShader(Shader* shader);

	Shader* GetShader();

	Material* NewMaterial(std::vector<std::string> unique_cb = std::vector<std::string>());

	Material* GetMaterial(int index);

	void BindUniqueConstantBuffer(D3DGraphicsContext* gc);

	ConstantBuffer* GetBuffer(const std::string& name);

private:
	Shader* shader;
	std::map<std::string, CBufferUnit> constant_buffers;
	std::vector<Material*> materials;
};

#endif