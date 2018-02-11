#ifndef MMD_VIEWER_H
#define MMD_VIEWER_H

#include <map>

#include "Model.h"
#include "Camera.h"
#include "Renderer.h"
#include "D3DGraphicsContext.h"
#include "AnimationClip.h"
#include "AnimationController.h"

class MMDViewer
{
public:
	MMDViewer(D3DGraphicsContext* gc);

	~MMDViewer();

	void LoadModel(const std::string& path);

	void LoadMotion(const std::string& path);

	void LoadCameraMotion(const std::string& path);

	Model* LoadModel(const std::string& path, mmd::mmd_model* model);

	void PlayAnimation();

	void StopAnimation();

	void Update(float dt);

	void Render();

	Camera* GetCamera() { return &camera; }

	Model* GetModel() { return current_model; }

	bool HasModel() { return current_model != nullptr; }

	bool HasBoneMotion() { return motion_clip != nullptr; }

	bool HasCameraMotion() { return camera_clip != nullptr; }

	std::string GetModelName() { return model_name; }

	std::string GetBoneMotionName() { return bone_motion_name; }

	std::string GetCameraMotionName() { return camera_motion_name; }

private:
	Renderer* renderer;
	D3DGraphicsContext* graphics_context;

	Model* current_model;
	std::string model_name;

	std::string bone_motion_name;
	std::string camera_motion_name;

	ThirdPersonCamera camera;

	AnimationClip* motion_clip;
	AnimationClip* camera_clip;
	AnimationController animation_controller;
};

#endif