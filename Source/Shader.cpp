#include "Shader.h"
#include "Debug.h"
#include "Texture2D.h"

#include <D3Dcompiler.h>


//------------------------------------------------------------------------------
// Shader
//------------------------------------------------------------------------------
Shader::Shader(ID3D11Device* device, const std::string& vs, const std::string& ps)
{
	this->device = device;
	this->input_layout = nullptr;
	this->pixel_shader = nullptr;
	this->vertex_shader = nullptr;
	this->geometry_shader = nullptr;

	this->CreateShader(vs, ShaderType::ShaderType_Vertex);
	this->CreateShader(ps, ShaderType::ShaderType_Pixel);
}

Shader::Shader(ID3D11Device* device, const std::string& vs, const std::string& gs, const std::string& ps)
{
	this->device = device;
	this->input_layout = nullptr;
	this->pixel_shader = nullptr;
	this->vertex_shader = nullptr;
	this->geometry_shader = nullptr;

	this->CreateShader(vs, ShaderType::ShaderType_Vertex);
	this->CreateShader(gs, ShaderType::ShaderType_Geometry);
	this->CreateShader(ps, ShaderType::ShaderType_Pixel);
}

Shader::~Shader()
{
	SAFE_RELEASE(input_layout);
	SAFE_RELEASE(pixel_shader);
	SAFE_RELEASE(vertex_shader);
	SAFE_RELEASE(geometry_shader);

	binding_resources.clear();
}

Shader::BindingResourceMap Shader::GetBindingResources()
{
	return this->binding_resources;
}

void Shader::SetVertexAttribute(int index, const std::string& semantic, int semantic_index, VertexDataType type, int slot, int offset)
{
	VertexAttribute va;
	va.semantic = semantic;
	va.semantic_index = semantic_index;
	va.type = type;
	va.slot = slot;
	va.offset = offset;

	if ( input_descs.size() <= index )
	{
		input_descs.resize(index + 1);
	}
	input_descs[index] = va;
}

void Shader::CreateInputLayout()
{
	static DXGI_FORMAT convert[] = {
		/* signed int */
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32B32A32_SINT,

		/* unsigned int */
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32A32_UINT,

		/* float */
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> tmp;
	for ( VertexAttribute &va : input_descs )
	{
		D3D11_INPUT_ELEMENT_DESC input_ele_desc;
		input_ele_desc.SemanticName = va.semantic.c_str();
		input_ele_desc.SemanticIndex = va.semantic_index;
		input_ele_desc.Format = convert[va.type];
		input_ele_desc.InputSlot = va.slot;
		input_ele_desc.AlignedByteOffset = va.offset;
		input_ele_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		input_ele_desc.InstanceDataStepRate = 0;

		tmp.push_back(input_ele_desc);
	}

	HRESULT result = device->CreateInputLayout(&tmp[0], tmp.size(), &vertex_code[0], vertex_code.size(), &input_layout);
	if ( FAILED(result) )
	{
		Debug::LogAndWrite("Faild to create input layout");
	}
}

void Shader::CreateShader(const std::string& filename, ShaderType type)
{
	static std::string profiles[3] = { "vs_5_0", "gs_5_0", "ps_5_0" };

	/* 编译着色程序 */
	ID3D10Blob* shader_code = this->CompileShader(type, filename, profiles[type]);

	/* 创建着色器 */
	if ( shader_code )
	{
		HRESULT result;
		switch ( type )
		{
		case ShaderType::ShaderType_Vertex:
			result = device->CreateVertexShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), 0, &vertex_shader);
			vertex_code.append(( char* ) shader_code->GetBufferPointer(), shader_code->GetBufferSize());
			break;
		case ShaderType::ShaderType_Geometry:
			result = device->CreateGeometryShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), 0, &geometry_shader);
			break;
		case ShaderType::ShaderType_Pixel:
			result = device->CreatePixelShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), 0, &pixel_shader);
			break;
		}
		SAFE_RELEASE(shader_code);
	}
}

ID3D10Blob* Shader::CompileShader(ShaderType type, const std::string& filename, const std::string& profile)
{
	ID3D10Blob* shader_buffer;
	ID3D10Blob* error_message;

	HRESULT result = D3DX11CompileFromFileA(filename.c_str(), 0, 0, "main", profile.c_str(),
		D3D10_SHADER_ENABLE_STRICTNESS, 0, 0, &shader_buffer, &error_message, 0);

	if ( FAILED(result) )
	{
		if ( error_message )
		{
			Debug::LogAndWrite(( char* ) error_message->GetBufferPointer());
		}
		else
		{
			Debug::LogAndWrite("Faild to open shader file: %s", filename.c_str());
		}
		return nullptr;
	}
	this->FindBindingResource(type, shader_buffer);

	return shader_buffer;
}

void Shader::FindBindingResource(ShaderType type, ID3DBlob* shader_code)
{
	HRESULT result;
	ID3D11ShaderReflection* shader_refletion;

	result = D3DReflect(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), IID_ID3D11ShaderReflection, ( void** ) &shader_refletion);
	if ( FAILED(result) ) return;

	D3D11_SHADER_DESC shader_desc;
	result = shader_refletion->GetDesc(&shader_desc);

	for ( int i = 0; i < shader_desc.BoundResources; i++ )
	{
		/* 获取着色器中的绑定资源 */
		D3D11_SHADER_INPUT_BIND_DESC bind_desc;
		shader_refletion->GetResourceBindingDesc(i, &bind_desc);

		/* 获取绑定资源 */
		BindingResource br;
		br.name = bind_desc.Name;

		switch ( bind_desc.Type )
		{
		case D3D_SIT_CBUFFER:
			br.resource_type = ResourceType::RT_ConstantBuffer;
			br.SetBindingPoint(type, bind_desc.BindPoint);
			br.constant_buffer_info = this->GetConstantBufferInfo(shader_refletion, bind_desc.Name);
			break;
		case D3D_SIT_TEXTURE:
			br.resource_type = ResourceType::RT_Texture;
			br.SetBindingPoint(type, bind_desc.BindPoint);
			break;
		case D3D_SIT_SAMPLER:
			br.resource_type = ResourceType::RT_Sampler;
			br.SetBindingPoint(type, bind_desc.BindPoint);
			break;
		}
		this->binding_resources.insert(std::make_pair(br.name, br));
	}
}

static ShaderVariantType ToShaderVariantType(D3D11_SHADER_TYPE_DESC desc)
{
	switch ( desc.Class )
	{
	case D3D_SVC_SCALAR:
		if ( desc.Type == D3D_SVT_INT )
		{
			return ShaderVariantType::SVT_Int;
		}
		else if ( desc.Type == D3D_SVT_FLOAT )
		{
			return ShaderVariantType::SVT_Float;
		}
		break;
	case D3D_SVC_VECTOR: return ShaderVariantType::SVT_Vector;
	case D3D_SVC_MATRIX_COLUMNS: return ShaderVariantType::SVT_Matrix;
	}
	return ShaderVariantType::SVT_Unknown;
}

ShaderConstantBufferInfo Shader::GetConstantBufferInfo(ID3D11ShaderReflection* shader_refletion, const char* name)
{
	/* 遍历所有常量缓冲区 */
	ID3D11ShaderReflectionConstantBuffer* srcb = shader_refletion->GetConstantBufferByName(name);

	/* 获取缓冲区信息 */
	D3D11_SHADER_BUFFER_DESC constant_buffer_desc;
	srcb->GetDesc(&constant_buffer_desc);

	ShaderConstantBufferInfo cbi;
	cbi.name = constant_buffer_desc.Name;
	cbi.size = constant_buffer_desc.Size;

	/* 遍历缓冲区中的变量 */
	for ( int v = 0; v < constant_buffer_desc.Variables; v++ )
	{
		ID3D11ShaderReflectionVariable* srv = srcb->GetVariableByIndex(v);

		D3D11_SHADER_VARIABLE_DESC variable_desc;
		srv->GetDesc(&variable_desc);

		/* 跳过没有被使用的变量 */
		//if ( !(variable_desc.uFlags & D3D_SVF_USED) ) continue;

		/* 获取变量的数据类型 */
		D3D11_SHADER_TYPE_DESC type_desc;
		srv->GetType()->GetDesc(&type_desc);

		ShaderVariantInfo svd;
		svd.name = variable_desc.Name;
		svd.size = variable_desc.Size;
		svd.elements = type_desc.Elements;
		svd.start_offset = variable_desc.StartOffset;
		svd.type = ToShaderVariantType(type_desc);

		cbi.shader_veriants.insert(std::make_pair(svd.name, svd));
	}
	return cbi;
}

void Shader::BindShader()
{
	ID3D11DeviceContext* device_context = nullptr;
	device->GetImmediateContext(&device_context);

	/* 创建顶点输入布局 */
	if ( input_layout == nullptr )
	{
		this->CreateInputLayout();
	}

	/* 设置顶点输入布局到渲染管线  */
	device_context->IASetInputLayout(input_layout);

	/* 设置着色器到渲染管线 */
	device_context->VSSetShader(vertex_shader, nullptr, 0);
	device_context->GSSetShader(geometry_shader, nullptr, 0);
	device_context->PSSetShader(pixel_shader, nullptr, 0);
}