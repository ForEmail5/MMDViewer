#ifndef ANIMATION_CONTROLLER_H
#define ANIMATION_CONTROLLER_H

#include "AnimationClip.h"
#include "MorphController.h"

class Model;
class Camera;
class AnimationClip;

class AnimationController
{
public:
	AnimationController();

	~AnimationController();

	void SetOutputModel(Model* model);

	void SetOutputCamera(Camera* camera);

	void BindMotionToModel();

	void SetBoneAnimationClip(AnimationClip* anim_clip);

	void SetCameraAnimationClip(AnimationClip* anim_clip);

	void SetFPS(int fps);

	void Reset();

	void Play();

	void Update(float dt);

	void SetPlayFrame(int nframe);

private:
	int fps;
	int frame_num;
	int play_frame;

	bool is_stop;

	float frame_time;
	float duration_time;

	Model* model;
	Camera* camera;
	AnimationClip* bone_anim_clip;
	AnimationClip* camera_anim_clip;

	struct AnimToBone
	{
		KeyFrameAnimation* anim;
		Bone* bone;
	};

	struct AnimToMorph
	{
		KeyFrameAnimation* anim;
		MorphHandle* morph;
	};

	std::vector<AnimToBone> anim_to_bone;
	std::vector<AnimToMorph> anim_to_morph;
};

#endif