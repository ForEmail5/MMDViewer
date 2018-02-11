#ifndef MODEL_H
#define MODEL_H

#include "MMD\PmdReader.h"

#include "D3D.h"
#include "Camera.h"
#include "Material.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "MorphController.h"
#include "ModelBone.h"
#include "VertexIndexBuffer.h"
#include "D3DGraphicsContext.h"


struct ModelPosition
{
	Vector3D position;
};

struct ModelVertex
{
	Vector3D normal;
	Vector2D texcoord;
	Vector4D weights;
	Vector4Dui bones;
};

class MorphController;

//---------------------------------------------------------------------------
// Transform
//---------------------------------------------------------------------------
class Transform
{
public:
	Matrix4 matrix;

	Vector3D scale;
	Vector3D position;
	Quaternion rotation;
};

//---------------------------------------------------------------------------
// Model
//---------------------------------------------------------------------------
class Model
{
public:
	Model(D3DGraphicsContext* gc);

	~Model();

	void Reset();

	void LoadModel(const std::string& path, mmd::mmd_model* model);

	void LoadVertices(mmd::mmd_model* model);

	void LoadMaterials(mmd::mmd_model* model);

	bool LoadTexture(const std::string& filename);

	void AddTexture(const std::string& fullpath, std::string texname);

	Texture2D* GetTexture(std::string filename);

	void CreateBuffer();

	void UpdateVectices();

	void UpdateModelBone();

	Matrix4 GetMatrix();

	void Bind()
	{
		model_material_manager.BindUniqueConstantBuffer(gc);
	}

	std::vector<Vector3D>* GetPositions() { return &positions; }

	void SetPosition(const Vector3D& pos) { position = pos; }

	Quaternion GetRotation() { return rotation; }

	RenderData GetRenderData() { return render_data; }

	ModelBone* GetModelBone() { return model_bone; }

	MorphController* GetMorphController() { return morph_controller; }

	Shader* shader;

	/* ¹Ç÷ÀÊý¾Ý */
	ModelBone* model_bone;

private:
	D3DGraphicsContext* gc;

	MorphController* morph_controller;

	RenderData render_data;

	std::vector<Vector3D> positions;
	ModelMaterialManager model_material_manager;

	std::vector<ModelVertex> vertices;
	std::vector<unsigned int> indices;

	std::map<std::string, Texture2D*> textures;

	std::string path;
	ArcBall* arc_ball;

	Matrix4 transform;
	Vector3D position;
	Vector3D scale;
	Quaternion rotation;
};

#endif