#include "AnimationController.h"
#include "Model.h"
#include "Debug.h"
#include "Camera.h"
#include "CeShi.h"

AnimationController::AnimationController()
{
	frame_num = 0;
	play_frame = 0;
	duration_time = 0;

	model = nullptr;
	camera = nullptr;
	bone_anim_clip = nullptr;
	camera_anim_clip = nullptr;

	is_stop = true;

	this->SetFPS(30);
}

AnimationController::~AnimationController()
{

}

void AnimationController::SetOutputModel(Model* model)
{
	this->model = model;
	this->BindMotionToModel();
}

void AnimationController::SetOutputCamera(Camera* camera)
{
	this->camera = camera;
}

void AnimationController::BindMotionToModel()
{
	if ( model && bone_anim_clip )
	{
		anim_to_bone.clear();
		anim_to_morph.clear();

		/* 匹配模型骨骼 */
		ModelBone* mb = model->GetModelBone();
		auto bone_animations = bone_anim_clip->GetBoneAnimations();

		int count = mb->GetBoneCount();
		for ( int i = 0; i < count; i++ )
		{
			Bone* bone = mb->GetBoneByIndex(i);
			auto it = bone_animations->find(bone->GetName());
			if ( it != bone_animations->end() )
			{
				AnimToBone atb;
				atb.anim = it->second;
				atb.bone = bone;
				anim_to_bone.push_back(atb);
			}
		}

		/* 匹配模型变形 */
		MorphController* mc = model->GetMorphController();
		auto morph_animations = bone_anim_clip->GetMorphAnimations();

		count = mc->GetMorphHandleCount();
		for ( int i = 0; i < count; i++ )
		{
			MorphHandle* mh = mc->GetMorphHandleByIndex(i);
			auto it = morph_animations->find(mh->GetName());
			if ( it != morph_animations->end() )
			{
				AnimToMorph atm;
				atm.anim = it->second;
				atm.morph = mh;
				anim_to_morph.push_back(atm);
			}
		}
	}
}

void AnimationController::SetBoneAnimationClip(AnimationClip* anim_clip)
{
	bone_anim_clip = anim_clip;
	this->BindMotionToModel();
}

void AnimationController::SetCameraAnimationClip(AnimationClip* anim_clip)
{
	camera_anim_clip = anim_clip;
}

void AnimationController::SetFPS(int fps)
{
	this->fps = fps;
	this->frame_time = 1000.0f / this->fps;
}

void AnimationController::Reset()
{
	play_frame = 0;
	is_stop = true;
}

void AnimationController::Play()
{
	is_stop = false;
}

void AnimationController::Update(float dt)
{
	if ( is_stop || model == nullptr || bone_anim_clip == nullptr ) return;

	duration_time += dt;
	if ( duration_time >= frame_time )
	{
		duration_time = 0;

		this->SetPlayFrame(play_frame++);

		/* 动画播放结束 */
		if ( play_frame >= bone_anim_clip->GetFrameCount() ) this->Reset();
	}
}

void AnimationController::SetPlayFrame(int nframe)
{
	if ( model )
	{
		for ( int i = 0; i < anim_to_bone.size(); i++ )
		{
			KeyFrameAnimation* anim = anim_to_bone[i].anim;
			Bone* bone = anim_to_bone[i].bone;
			bone->UpdateLocalMatrix(anim->GetKeyFrameData(nframe).ToMatrix());
		}

		for ( int i = 0; i < anim_to_morph.size(); i++ )
		{
			KeyFrameAnimation* anim = anim_to_morph[i].anim;
			MorphHandle* morph_handle = anim_to_morph[i].morph;
			morph_handle->SetValue(anim->GetKeyFrameData(nframe).position.x);
		}

		model->GetModelBone()->UpdateAllBoneIK();
		model->GetModelBone()->UpdateAllBoneMatrix();
		model->GetMorphController()->Update();
		model->UpdateModelBone();
	}

	if ( camera && camera_anim_clip )
	{
		KeyFrameAnimation* camera_anim = camera_anim_clip->GetCameraAnimation();
		if ( camera_anim )
		{
			KeyFrameData& data = camera_anim->GetKeyFrameData(nframe);
			camera->SetDistance(-data.focal_length);
			camera->SetTarget(data.position);
			camera->SetRotation(Quaternion::EulerXYZ(data.euler.x, -data.euler.y, -data.euler.z));
			camera->UpdateViewMatrix();
		}
	}
}