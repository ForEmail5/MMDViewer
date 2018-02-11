#include "Material.h"
#include "D3DGraphicsContext.h"

Material::Material()
{
	blend_state = nullptr;
	depth_stencil_state = nullptr;
}

Material::~Material()
{
	this->Clear();
}

void Material::Clear()
{
	this->textures.clear();

	for ( int i = 0; i < constant_buffers.size(); i++ )
	{
		delete constant_buffers[i].cb;
	}
	this->constant_buffers.clear();
}

Shader* Material::GetShader()
{
	return this->shader;
}

void Material::SetShader(Shader* shader, const std::set<std::string>& skip_cb)
{
	this->Clear();
	this->shader = shader;

	this->binding_resources = shader->GetBindingResources();
	for ( auto& it : binding_resources )
	{
		std::string name = it.first;
		BindingResource br = it.second;

		if ( br.resource_type != ResourceType::RT_ConstantBuffer ) continue;
		if ( skip_cb.find(name) != skip_cb.end() ) continue;

		this->SetConstantBuffer(br);
	}
}

void Material::SetConstantBuffer(const BindingResource& br)
{
	CBufferUnit unit;
	unit.binding_points[0] = br.binding_points[0];
	unit.binding_points[1] = br.binding_points[1];
	unit.binding_points[2] = br.binding_points[2];
	unit.cb = new ConstantBuffer(br);

	constant_buffers.push_back(unit);

	/* ͨ���������ڵı�������������������������Ҫ����һ�ű������Ӧ������������������ */
	for ( auto var : br.constant_buffer_info.shader_veriants )
	{
		cb_index_table.insert(std::make_pair(var.first, constant_buffers.size() - 1));
	}
}

bool Material::SetTexture(const std::string name, Texture2D* texture)
{
	auto br_it =  this->binding_resources.find(name);
	if ( br_it != this->binding_resources.end() )
	{
		auto tex_it =  textures.find(name);
		if ( tex_it == textures.end() )
		{
			TextureUnit unit;
			unit.binding_points[0] = br_it->second.binding_points[0];
			unit.binding_points[1] = br_it->second.binding_points[1];
			unit.binding_points[2] = br_it->second.binding_points[2];
			unit.texture_2d = texture;

			textures.insert(std::make_pair(name, unit));
		}
		else
		{
			tex_it->second.texture_2d = texture;
		}
		return true;
	}
	return false;
}

void Material::BindMaterial(D3DGraphicsContext* gc)
{
	ID3D11DeviceContext* device_context = gc->GetDeviceContext();

	/* ���������� */
	for ( auto tex : textures )
	{
		TextureUnit unit = tex.second;
		if ( unit.binding_points[0] != -1 )
		{
			ID3D11ShaderResourceView* srv = unit.texture_2d->GetShaderResourceView();
			device_context->VSSetShaderResources(unit.binding_points[0], 1, &srv);
		}
		if ( unit.binding_points[1] != -1 )
		{
			ID3D11ShaderResourceView* srv = unit.texture_2d->GetShaderResourceView();
			device_context->GSSetShaderResources(unit.binding_points[1], 1, &srv);
		}
		if ( unit.binding_points[2] != -1 )
		{
			ID3D11ShaderResourceView* srv = unit.texture_2d->GetShaderResourceView();
			device_context->PSSetShaderResources(unit.binding_points[2], 1, &srv);
		}
	}

	/* �ϴ����ݵ�ָ���۵Ļ����� */
	for ( int i = 0; i < constant_buffers.size(); i++ )
	{
		CBufferUnit unit = constant_buffers[i];
		if ( unit.binding_points[0] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[0], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Vertex);
		}
		if ( unit.binding_points[1] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[1], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Geometry);
		}
		if ( unit.binding_points[2] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[2], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Pixel);
		}
	}
}


//------------------------------------------------------------------------------
// ModelMaterialManager
//------------------------------------------------------------------------------
ModelMaterialManager::ModelMaterialManager()
{
	this->shader = nullptr;
}

ModelMaterialManager::~ModelMaterialManager()
{
	for ( auto it : constant_buffers ) delete it.second.cb;
	constant_buffers.clear();

	for ( int i = 0; i < materials.size(); i++ ) delete materials[i];
	materials.clear();
}

void ModelMaterialManager::SetShader(Shader* shader)
{
	this->shader = shader;
}

Shader* ModelMaterialManager::GetShader()
{
	return this->shader;
}

Material* ModelMaterialManager::NewMaterial(std::vector<std::string> unique_cb)
{
	/* һ��ģ�ʹ��ڶ�����ʣ���ʱ��Ҫ������ʹ���һ���������������������ת������
	��Щ����Ļ����������� constant_buffers �У������ǲ����� */
	std::set<std::string> skip_names;

	for ( int i = 0; i < unique_cb.size(); i++ )
	{
		auto it =  constant_buffers.find(unique_cb[i]);
		if ( it == constant_buffers.end() )
		{
			/* �����ڹ��õĳ������������򴴽� */
			Shader::BindingResourceMap br = shader->GetBindingResources();
			auto br_it =  br.find(unique_cb[i]);
			if ( br_it == br.end() ) continue;

			CBufferUnit unit;
			unit.binding_points[0] = br_it->second.binding_points[0];
			unit.binding_points[1] = br_it->second.binding_points[1];
			unit.binding_points[2] = br_it->second.binding_points[2];
			unit.cb = new ConstantBuffer(br_it->second);

			constant_buffers.insert(std::make_pair(unit.cb->Name(), unit));
		}
		skip_names.insert(unique_cb[i]);
	}

	Material* mat = new Material();
	mat->SetShader(shader, skip_names);
	materials.push_back(mat);

	return mat;
}

Material* ModelMaterialManager::GetMaterial(int index)
{
	if ( index >= materials.size() ) return nullptr;
	return materials[index];
}

ConstantBuffer* ModelMaterialManager::GetBuffer(const std::string& name)
{
	auto it =  constant_buffers.find(name);
	if ( it != constant_buffers.end() )
	{
		return it->second.cb;
	}
	return nullptr;
}

void ModelMaterialManager::BindUniqueConstantBuffer(D3DGraphicsContext* gc)
{
	for ( auto& it : constant_buffers )
	{
		CBufferUnit unit = it.second;
		if ( unit.binding_points[0] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[0], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Vertex);
		}
		if ( unit.binding_points[1] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[1], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Geometry);
		}
		if ( unit.binding_points[2] != -1 )
		{
			gc->UploadDataToConstantBuffer(unit.binding_points[2], unit.cb->Buffer(), unit.cb->Size(), ShaderType::ShaderType_Pixel);
		}
	}
}