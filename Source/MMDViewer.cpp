#include "MMDViewer.h"
#include "PathHelper.h"
#include "CeShi.h"
#include "Debug.h"

#include "MMD\MmdCommon.h"
#include "MMD\MmdReader.h"

MMDViewer::MMDViewer(D3DGraphicsContext* gc)
{
	motion_clip = nullptr;
	camera_clip = nullptr;
	current_model = nullptr;

	graphics_context = gc;

	/* 创建渲染器并制定摄像机 */
	renderer = new Renderer(gc);
	renderer->SetCamera(&camera);

	/* 设置摄像机默认位置 */
	camera.SetTarget(Vector3D(0, 8, 0));
	camera.SetDistance(30);

	/* 设置摄像机到动画控制器，执行镜头动画时使用 */
	animation_controller.SetOutputCamera(&camera);
}

MMDViewer::~MMDViewer()
{
	delete renderer;

	if ( current_model ) delete current_model;
	if ( motion_clip ) delete motion_clip;
	if ( camera_clip ) delete camera_clip;
}

void MMDViewer::LoadModel(const std::string& path)
{
	if ( path.size() == 0 || PathHelper::FileName(path).compare(model_name) == 0 ) return;
	this->StopAnimation();

	mmd::mmd_model* model = mmd::MMDReader::ReadModel(path);
	if ( model )
	{
		if ( current_model )
		{
			delete current_model;
		}
		current_model = this->LoadModel(path, model);
		current_model->UpdateModelBone();
		animation_controller.Reset();
		animation_controller.SetOutputModel(current_model);
		delete model;
	}
}

void MMDViewer::LoadMotion(const std::string& path)
{
	if ( path.size() == 0 || PathHelper::FileName(path).compare(bone_motion_name) == 0 ) return;
	this->StopAnimation();

	bone_motion_name = PathHelper::FileName(path);
	mmd::mmd_motion* motion = mmd::MMDReader::ReadMotion(path);
	if ( motion )
	{
		if ( motion_clip ) delete motion_clip;

		motion_clip = new AnimationClip(motion);
		animation_controller.Reset();
		animation_controller.SetBoneAnimationClip(motion_clip);
		delete motion;
	}
}

void MMDViewer::LoadCameraMotion(const std::string& path)
{
	if ( path.size() == 0 || PathHelper::FileName(path).compare(camera_motion_name) == 0 ) return;
	this->StopAnimation();

	camera_motion_name = PathHelper::FileName(path);
	mmd::mmd_motion* motion = mmd::MMDReader::ReadMotion(path);
	if ( motion )
	{
		if ( camera_clip ) delete camera_clip;

		camera_clip = new AnimationClip(motion);
		animation_controller.Reset();
		animation_controller.SetCameraAnimationClip(camera_clip);
		delete motion;
	}
}

Model* MMDViewer::LoadModel(const std::string& path, mmd::mmd_model* model)
{
	model_name = PathHelper::FileName(path);

	Model* m = new Model(graphics_context);
	m->shader = renderer->GetModelShader();
	m->LoadModel(PathHelper::FilePath(path), model);
	m->CreateBuffer();

	return m;
}

void MMDViewer::PlayAnimation()
{
	animation_controller.Play();
}

void MMDViewer::StopAnimation()
{
	animation_controller.Reset();
	if ( current_model )
	{
		current_model->Reset();
	}
}

void MMDViewer::Update(float dt)
{
	animation_controller.Update(dt);
}

void MMDViewer::Render()
{
	if ( current_model )
	{
		renderer->RenderModel(current_model);
	}

	renderer->DrawBox(camera.GetTarget(), 0.3, Color(1, 1, 1, 1));

	/* 绘制地面 */
	int n = 50;
	int offset = 4;
	int half = n * offset * 0.5f;

	for ( int i = 0; i < n + 1; i++ )
	{
		renderer->DrawLine(
			Vector3D(-half + i * offset, 0, -half),
			Vector3D(-half + i * offset, 0,  half), Color(1, 1, 1, 1));

		renderer->DrawLine(
			Vector3D(-half, 0, -half + i * offset),
			Vector3D( half, 0, -half + i * offset), Color(1, 1, 1, 1));
	}

	//renderer->DrawPlane(Vector3D(0, -0.02, 0), 500, Color(0.5, 0.5, 0.5, 1));

	/* 绘制坐标轴 */
	renderer->DrawLine(Vector3D(0, 0, 0), Vector3D(3000, 0, 0), Color(1, 0, 0, 1));
	renderer->DrawLine(Vector3D(0, 0, 0), Vector3D(0, 3000, 0), Color(0, 1, 0, 1));
	renderer->DrawLine(Vector3D(0, 0, 0), Vector3D(0, 0, 3000), Color(0, 0, 1, 1));

	renderer->RenderGeometry();
}