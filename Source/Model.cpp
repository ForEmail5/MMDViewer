#include "Model.h"
#include "Debug.h"
#include "Camera.h"
#include "Renderer.h"
#include "CeShi.h"

#include <algorithm>

Model::Model(D3DGraphicsContext* gc)
{
	this->gc = gc;
	render_data.index_buffer = nullptr;
	render_data.prim_type = PrimType::PT_Trianglelist;

	scale = Vector3D::ONE;
	transform = Matrix4::IDENTITY;
	position = Vector3D::ZERO;
	rotation = Quaternion::Identity();

	arc_ball = new ArcBall(this);

	morph_controller = new MorphController(this);

	CeShi::GetInstance()->model = this;
}

Model::~Model()
{
	/* 释放缓冲区 */
	delete render_data.index_buffer;
	for ( int i = 0; i < render_data.vertex_datas.size(); i++ )
	{
		delete render_data.vertex_datas[i].vertex_buffer;
	}

	/* 释放纹理 */
	for ( auto it = textures.begin(); it != textures.end(); ++it )
	{
		delete it->second;
	}
	textures.clear();

	delete arc_ball;
	delete model_bone;
	delete morph_controller;
}

void Model::Reset()
{
	model_bone->Reset();
	this->UpdateModelBone();
}

void Model::LoadModel(const std::string& path, mmd::mmd_model* model)
{
	this->path = path;

	this->LoadVertices(model);
	this->LoadMaterials(model);

	/* 创建模型骨骼 */
	model_bone = new ModelBone(model->bones);

	/* 加载表情 */
	morph_controller->LoadMorph(model->morphs);
}

void Model::LoadVertices(mmd::mmd_model* model)
{
	/* 设置顶点数据 */
	int vertex_count = model->vertices.size();
	for ( int i = 0; i < vertex_count; i++ )
	{
		mmd::mmd_vertex mv = model->vertices[i];

		/* 将位置坐标单独储存在一个缓冲区中 */
		Vector3D pos(mv.position.x, mv.position.y, mv.position.z);
		positions.push_back(pos);

		/* 另一个缓冲区储存法线、纹理坐标、骨骼信息 */
		ModelVertex vertex;
		vertex.normal.Set(mv.normal.x, mv.normal.y, mv.normal.z);
		vertex.texcoord.Set(mv.texcoord.x, 1 - mv.texcoord.y);

		memcpy(&vertex.weights, &mv.weights, sizeof( float ) * 4);
		memcpy(&vertex.bones, &mv.bone_indices, sizeof( unsigned int ) * 4);

		vertices.push_back(vertex);
	}

	/* 设置索引数据 */
	indices.resize(model->indices.size());
	memcpy(&indices[0], &model->indices[0], sizeof( unsigned int ) * model->indices.size());
}

void Model::LoadMaterials(mmd::mmd_model* model)
{
	/* 获取材质 */
	int material_count = model->materials.size();
	int offset = 0;

	model_material_manager.SetShader(shader);
	std::vector<std::string> skip;
	skip.push_back("AnimMatrices");

	/* 加载纹理 */
	for ( int i = 0; i < model->textures.size(); i++ )
	{
		this->LoadTexture(model->textures[i]);
	}

	/* 加载材质 */
	for ( int i = 0; i < material_count; i++ )
	{
		RenderUnit unit;
		unit.material = nullptr;
		unit.index_offset = offset;
		unit.index_num = model->materials[i].index_num;

		const mmd::mmd_material& mat = model->materials[i];
		Texture2D* sub_tex = nullptr;
		Texture2D* main_tex = nullptr;

		if ( mat.main_texture.size() > 0 )
		{
			main_tex = this->GetTexture(mat.main_texture);
		}
		if ( mat.sub_texture.size() > 0 )
		{
			sub_tex = this->GetTexture(mat.sub_texture);
		}

		if ( main_tex )
		{
			Material* material = model_material_manager.NewMaterial(skip);
			material->SetBlendState(gc->GetDefaultBlendState());
			material->SetDepthStencilState(gc->GetDefaultDepthStencilState());

			material->SetTexture("main_tex", main_tex);
			if ( mat.toon >= 0 && mat.toon < textures.size() )
			{
				material->SetTexture("toon_tex", this->GetTexture(model->textures[mat.toon]));
				material->SetInt("has_toon", 1);
			}
			else
			{
				material->SetInt("has_toon", 0);
			}
			if ( sub_tex )
			{
				material->SetTexture("sub_tex", sub_tex);
				material->SetInt("use_sub_tex", 1);
			}
			else
			{
				material->SetInt("use_sub_tex", 0);
			}
			material->SetFloat("shininess", mat.shininess);
			material->SetFloat("edge_size", mat.edge_size);
			material->SetColor("embient", Color(mat.embient.x, mat.embient.y, mat.embient.z, 1.0f));
			material->SetColor("diffuse", Color(mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w));
			unit.material = material;
		}
		offset += unit.index_num;
		render_data.render_units.push_back(unit);
	}
}

bool Model::LoadTexture(const std::string& filename)
{
	if ( filename[0] == 0 ) return false;

	size_t pos = filename.find('*');
	if ( pos != std::string::npos )
	{
		std::string tex_name = filename.substr(0, pos);
		std::string spx_name= filename.substr(pos + 1);

		if ( tex_name.size() > 0 )
		{
			std::string fullpath = path + "\\" + tex_name;
			this->AddTexture(fullpath, tex_name);
		}
		if ( spx_name.size() > 0 )
		{
			std::string fullpath = path + "\\" + spx_name;
			this->AddTexture(fullpath, spx_name);
		}
	}
	else
	{
		std::string fullpath = path + "\\" + filename;
		this->AddTexture(fullpath, filename);
	}
}

void Model::AddTexture(const std::string& fullpath, std::string texname)
{
	/* 查找是否存在纹理？不存在则创建纹理 */
	Texture2D* texture = this->GetTexture(texname);

	if ( texture == nullptr )
	{
		texture = Texture2D::CreateTexture2D(fullpath, gc->GetDevice());
		if ( texture )
		{
			std::transform(texname.begin(), texname.end(), texname.begin(), ::tolower);
			textures.insert(std::make_pair(texname, texture));
		}
	}
}

Texture2D* Model::GetTexture(std::string filename)
{
	std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

	auto it = textures.find(filename);
	if ( it != textures.end() ) return it->second;

	return nullptr;
}

void Model::UpdateVectices()
{
	/* 执行变形后更新顶点 */
	render_data.vertex_datas[0].vertex_buffer->UploadData(0, &positions[0], sizeof( ModelPosition ) * positions.size());
}

void Model::UpdateModelBone()
{
	model_material_manager.GetBuffer("AnimMatrices")->SetValue(&model_bone->final_matries[0], sizeof( Matrix4 ) * model_bone->final_matries.size());
}

void Model::CreateBuffer()
{
	/* 将顶点的 position 单独储存在一个顶点缓冲区中，在执行变形动画时只更新位置信息，减少拷贝时间 */
	VertexData vd1;
	vd1.slot = 0;
	vd1.stride = sizeof(ModelPosition);
	vd1.offset = 0;
	vd1.vertex_buffer = VertexIndexBuffer::CreateVertexBuffer(gc->GetDevice(), &positions[0], sizeof( ModelPosition ) * positions.size());

	VertexData vd2;
	vd2.slot = 1;
	vd2.stride = sizeof(ModelVertex);
	vd2.offset = 0;
	vd2.vertex_buffer = VertexIndexBuffer::CreateVertexBuffer(gc->GetDevice(), &vertices[0], sizeof( ModelVertex ) * vertices.size());

	render_data.vertex_datas.push_back(vd1);
	render_data.vertex_datas.push_back(vd2);
	render_data.index_buffer = VertexIndexBuffer::CreateIndexBuffer(gc->GetDevice(), &indices[0], sizeof( unsigned int ) * indices.size());

	indices.reserve(0);
}

Matrix4 Model::GetMatrix()
{
	rotation = arc_ball->GetQuaternionNow();
	//transform = Matrix4::MakeTransform(position, scale) * rotation.ToRotationMatrx();
	transform = Matrix4::MakeTransform(position, scale);
	return transform;
}