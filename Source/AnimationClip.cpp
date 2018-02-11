#include "AnimationClip.h"
#include "ModelBone.h"
#include "Debug.h"


//-------------------------------------------------------------------
// 浮点型关键帧
// FloatKeyFrame
//-------------------------------------------------------------------
FloatKeyFrame::FloatKeyFrame(int nframe) : KeyFrame(nframe)
{

}

void FloatKeyFrame::SetValue(float value)
{
	this->value = value;
}

float FloatKeyFrame::GetValue()
{
	return value;
}

KeyFrameData FloatKeyFrame::GetData()
{
	KeyFrameData kfd;
	kfd.position.x = value;
	return kfd;
}

KeyFrameData FloatKeyFrame::Slerp(KeyFrame* kf, float t)
{
	KeyFrameData kfd;

	FloatKeyFrame* fkf = static_cast< FloatKeyFrame* >(kf);
	kfd.position.x = value + (fkf->value - value) * t;
	return kfd;
}



//-------------------------------------------------------------------
// 动作关键帧
// MotionKeyFrame
//-------------------------------------------------------------------
MotionKeyFrame::MotionKeyFrame(int nframe) : KeyFrame(nframe)
{

}

void MotionKeyFrame::SetPosition(float x, float y, float z)
{
	position.Set(x, y, z);
}

void MotionKeyFrame::SetRotation(float x, float y, float z, float w)
{
	rotation = Quaternion(x, y, z, w);
}

void MotionKeyFrame::SetBezier(char* interpolator)
{
	float inv = 1.0f / 127.0f;

	cx_0.x = interpolator[16 * 0 + 8 * 0 + 0] * inv;
	cx_0.y = interpolator[16 * 0 + 8 * 0 + 4] * inv;
	cx_1.x = interpolator[16 * 0 + 8 * 1 + 0] * inv;
	cx_1.y = interpolator[16 * 0 + 8 * 1 + 4] * inv;

	cy_0.x = interpolator[16 * 1 + 8 * 0 + 0] * inv;
	cy_0.y = interpolator[16 * 1 + 8 * 0 + 4] * inv;
	cy_1.x = interpolator[16 * 1 + 8 * 1 + 0] * inv;
	cy_1.y = interpolator[16 * 1 + 8 * 1 + 4] * inv;

	cz_0.x = interpolator[16 * 2 + 8 * 0 + 0] * inv;
	cz_0.y = interpolator[16 * 2 + 8 * 0 + 4] * inv;
	cz_1.x = interpolator[16 * 2 + 8 * 1 + 0] * inv;
	cz_1.y = interpolator[16 * 2 + 8 * 1 + 4] * inv;

	cr_0.x = interpolator[16 * 3 + 8 * 0 + 0] * inv;
	cr_0.y = interpolator[16 * 3 + 8 * 0 + 4] * inv;
	cr_1.x = interpolator[16 * 3 + 8 * 1 + 0] * inv;
	cr_1.y = interpolator[16 * 3 + 8 * 1 + 4] * inv;
}

KeyFrameData MotionKeyFrame::GetData()
{
	KeyFrameData kfd;
	kfd.position = position;
	kfd.rotation = rotation;
	return kfd;
}

KeyFrameData MotionKeyFrame::Slerp(KeyFrame* kf, float t)
{
	KeyFrameData kfd;
	MotionKeyFrame* mkf = static_cast< MotionKeyFrame* >(kf);
	Vector3D vt = Vector3D::ZERO;
	Vector3D new_pos = mkf->position;

	/* 在贝塞尔曲线上采样 */
	if ( position.x != 0 || new_pos.x != 0 )
	{
		vt.x = this->IsLinear(cx_0, cx_1) ? t : SampleBezier(cx_0, cx_1, t);
	}
	if ( position.y != 0 || new_pos.y != 0 )
	{
		vt.y = this->IsLinear(cy_0, cy_1) ? t : SampleBezier(cy_0, cy_1, t);
	}
	if ( position.z != 0 || new_pos.z != 0 )
	{
		vt.z = this->IsLinear(cz_0, cz_1) ? t : SampleBezier(cz_0, cz_1, t);
	}
	/* 对位置插值 */
	kfd.position = Vector3D::Lerp(position, new_pos, vt);

	/* 对旋转插值 */
	float rt = IsLinear(cr_0, cr_1) ? t : SampleBezier(cr_0, cr_1, t);
	kfd.rotation = Quaternion::Slerp(rotation, mkf->rotation, rt, true);

	return kfd;
}


//-------------------------------------------------------------------
// 摄像机动作关键帧
// MotionKeyFrame
//-------------------------------------------------------------------
CameraMotionKeyFrame::CameraMotionKeyFrame(int nframe) : KeyFrame(nframe)
{

}

void CameraMotionKeyFrame::SetFov(float fov)
{
	this->fov = fov;
}

void CameraMotionKeyFrame::SetFocalLength(float length)
{
	this->focal_length = length;
}

void CameraMotionKeyFrame::SetPosition(float x, float y, float z)
{
	position.Set(x, y, z);
}

void CameraMotionKeyFrame::SetRotation(float x, float y, float z)
{
	rotation.Set(x, y, z);
}

void CameraMotionKeyFrame::SetBezier(char* interpolator)
{
	float inv = 1.0f / 127.0f;

	cx_0.x = interpolator[0] * inv;
	cx_0.y = interpolator[1] * inv;
	cx_1.x = interpolator[2] * inv;
	cx_1.y = interpolator[3] * inv;

	cy_0.x = interpolator[4] * inv;
	cy_0.y = interpolator[5] * inv;
	cy_1.x = interpolator[6] * inv;
	cy_1.y = interpolator[7] * inv;

	cz_0.x = interpolator[8] * inv;
	cz_0.y = interpolator[9] * inv;
	cz_1.x = interpolator[10] * inv;
	cz_1.y = interpolator[11] * inv;

	crx_0.x = interpolator[12] * inv;
	crx_0.y = interpolator[13] * inv;
	crx_1.x = interpolator[14] * inv;
	crx_1.y = interpolator[15] * inv;

	cry_0.x = interpolator[16] * inv;
	cry_0.y = interpolator[17] * inv;
	cry_1.x = interpolator[18] * inv;
	cry_1.y = interpolator[19] * inv;

	crz_0.x = interpolator[20] * inv;
	crz_0.y = interpolator[21] * inv;
	crz_1.x = interpolator[22] * inv;
	crz_1.y = interpolator[23] * inv;
}

KeyFrameData CameraMotionKeyFrame::GetData()
{
	KeyFrameData kfd;
	kfd.fov = fov;
	kfd.euler = rotation;
	kfd.position = position;
	return kfd;
}

KeyFrameData CameraMotionKeyFrame::Slerp(KeyFrame* kf, float t)
{
	KeyFrameData kfd;
	CameraMotionKeyFrame* cmkf = static_cast< CameraMotionKeyFrame* >(kf);
	Vector3D vt = Vector3D::ZERO;
	Vector3D new_pos = cmkf->position;

	/* 在贝塞尔曲线上采样 */
	vt.x = this->IsLinear(cx_0, cx_1) ? t : this->SampleBezier(cx_0, cx_1, t);
	vt.y = this->IsLinear(cy_0, cy_1) ? t : this->SampleBezier(cy_0, cy_1, t);
	vt.z = this->IsLinear(cz_0, cz_1) ? t : this->SampleBezier(cz_0, cz_1, t);

	/* 对位置插值 */
	kfd.position = Vector3D::Lerp(position, new_pos, vt);

	/* 对旋转插值 */
	Vector3D rt = Vector3D::ZERO;
	Vector3D new_rot = cmkf->rotation;

	/* 在贝塞尔曲线上采样 */
	rt.x = this->IsLinear(crx_0, crx_1) ? t : this->SampleBezier(crx_0, crx_1, t);
	rt.y = this->IsLinear(cry_0, cry_1) ? t : this->SampleBezier(cry_0, cry_1, t);
	rt.z = this->IsLinear(crz_0, crz_1) ? t : this->SampleBezier(crz_0, crz_1, t);

	kfd.euler = Vector3D::Lerp(rotation, new_rot, vt);

	/* 对其它属性插值 */
	kfd.fov = fov + (cmkf->fov - fov) * t;
	kfd.focal_length = focal_length + (cmkf->focal_length - focal_length) * t;

	return kfd;
}

//-------------------------------------------------------------------
// 关键帧动画
// KeyFrameAnimation
//-------------------------------------------------------------------
KeyFrameAnimation::KeyFrameAnimation(const std::wstring& name) : name(name)
{
}

KeyFrameAnimation::~KeyFrameAnimation()
{
	this->ReleaseKeyFrame();
}

void KeyFrameAnimation::AddKeyFrame(KeyFrame* kf)
{
	key_frames.push_back(kf);
}

void KeyFrameAnimation::ReleaseKeyFrame()
{
	for ( int i = 0; i < key_frames.size(); i++ )
	{
		delete key_frames[i];
	}
}

void KeyFrameAnimation::CreateIndexTable()
{
	/* 对关键帧按时间先后排序 */
	std::sort(key_frames.begin(), key_frames.end(), [] (KeyFrame* a, KeyFrame* b) -> bool {
		return a->Frame() < b->Frame();
	});

	/* 创建索引表，能够快速查找某一帧处于哪两个关键帧之间 */
	int idx = 0;
	int separate = key_frames[idx]->Frame();
	int it_size = key_frames[key_frames.size() - 1]->Frame();

	for ( int i = 0; i < it_size; i++ )
	{
		if ( i > separate )
		{
			separate = key_frames[++idx]->Frame();
		}
		index_table.push_back(idx);
	}
}

KeyFrameData KeyFrameAnimation::GetKeyFrameData(int nframe)
{
	/* 越界返回最后一帧 */
	if ( nframe >= index_table.size() )
	{
		return key_frames[key_frames.size() - 1]->GetData();
	}

	/* 刚好处于关键帧，返回该关键帧 */
	int next_frame = key_frames[index_table[nframe]]->Frame();
	if ( next_frame == nframe )
	{
		return key_frames[index_table[nframe]]->GetData();
	}

	/* 位于两关键帧之间，进行关键帧的插值 */
	KeyFrame* prev_kf = key_frames[index_table[nframe] - 1];
	KeyFrame* next_kf = key_frames[index_table[nframe]];

	int prev_frame = prev_kf->Frame();
	float t = float(nframe - prev_frame) / float(next_frame - prev_frame);

	/* 对两关键帧进行插值运算 */
	return prev_kf->Slerp(next_kf, t);
}

int KeyFrameAnimation::GetMaxFrame()
{
	if ( key_frames.size() == 0 ) return 0;
	return key_frames[key_frames.size() - 1]->Frame();
}


//-------------------------------------------------------------------
// 动画片段
// AnimationClip
//-------------------------------------------------------------------
AnimationClip::AnimationClip(mmd::mmd_motion* motion)
{
	frame_count = -1;
	camera_animation = nullptr;

	this->ParseBoneAnimation(motion->bones);
	this->ParseMorphAnimation(motion->morphs);
	this->ParseCameraAnimation(motion->camera);
	this->CreateIndexTable();
}

AnimationClip::~AnimationClip()
{
	/* 释放 BoneAnimations */
	for ( auto it = bone_animations.begin(); it != bone_animations.end(); ++it )
	{
		delete it->second;
	}
	bone_animations.clear();

	/* 释放 MorphAnimations */
	for ( auto it = morph_animations.begin(); it != morph_animations.end(); ++it )
	{
		delete it->second;
	}
	morph_animations.clear();

	delete camera_animation;
}

/* 解析骨骼动画 */
void AnimationClip::ParseBoneAnimation(const std::vector<mmd::vmd_bone>& bones)
{
	KeyFrameAnimation* key_frame_anim = nullptr;

	/* 创建关键帧 */
	for ( int i = 0; i < bones.size(); i++ )
	{
		/* 获取某一个骨节对应的骨骼结点动画 */
		auto it = bone_animations.find(bones[i].bone_name);
		if ( it == bone_animations.end() )
		{
			key_frame_anim = new KeyFrameAnimation(bones[i].bone_name);
			bone_animations.insert(std::make_pair(bones[i].bone_name, key_frame_anim));
		}
		else
		{
			key_frame_anim = it->second;
		}

		/* 创建关键帧 */
		MotionKeyFrame* mkf = new MotionKeyFrame(bones[i].nframe);
		key_frame_anim->AddKeyFrame(mkf);

		mkf->SetBezier(( char* ) bones[i].interpolator);
		mkf->SetPosition(
			bones[i].translation.x,
			bones[i].translation.y,
			bones[i].translation.z);
		mkf->SetRotation(
			bones[i].rotation.x,
			bones[i].rotation.y,
			bones[i].rotation.z,
			bones[i].rotation.w);
	}
}

/* 解析变形动画 */
void AnimationClip::ParseMorphAnimation(const std::vector<mmd::vmd_morph>& morphs)
{
	/* 创建关键帧 */
	KeyFrameAnimation* key_frame_anim = nullptr;
	for ( int i = 0; i < morphs.size(); i++ )
	{
		auto it = morph_animations.find(morphs[i].morph_name);
		if ( it == morph_animations.end() )
		{
			key_frame_anim = new KeyFrameAnimation(morphs[i].morph_name);
			morph_animations.insert(std::make_pair(morphs[i].morph_name, key_frame_anim));
		}
		else
		{
			key_frame_anim = it->second;
		}

		FloatKeyFrame* fkf = new FloatKeyFrame(morphs[i].nframe);
		key_frame_anim->AddKeyFrame(fkf);

		fkf->SetValue(morphs[i].weight);
	}
}

void AnimationClip::ParseCameraAnimation(const std::vector<mmd::vmd_camera>& cameras)
{
	if ( cameras.size() == 0 ) return;

	/* 创建关键帧 */
	camera_animation = new KeyFrameAnimation(L"Camera");
	for ( int i = 0; i < cameras.size(); i++ )
	{
		CameraMotionKeyFrame* cmkf = new CameraMotionKeyFrame(cameras[i].nframe);

		cmkf->SetFov(cameras[i].fov);
		cmkf->SetFocalLength(cameras[i].focal_length);

		cmkf->SetBezier(( char* ) cameras[i].interpolator);
		cmkf->SetPosition(
			cameras[i].position.x,
			cameras[i].position.y,
			cameras[i].position.z);
		cmkf->SetRotation(
			cameras[i].rotation.x,
			cameras[i].rotation.y,
			cameras[i].rotation.z);

		camera_animation->AddKeyFrame(cmkf);
	}
	camera_animation->CreateIndexTable();
}

void AnimationClip::CreateIndexTable()
{
	for ( auto it = bone_animations.begin(); it != bone_animations.end(); ++it )
	{
		it->second->CreateIndexTable();
	}

	for ( auto it = morph_animations.begin(); it != morph_animations.end(); ++it )
	{
		it->second->CreateIndexTable();
	}
}

int AnimationClip::GetFrameCount()
{
	if ( frame_count == -1 )
	{
		for ( auto it = bone_animations.begin(); it != bone_animations.end(); ++it )
		{
			frame_count = max(frame_count, it->second->GetMaxFrame());
		}
	}
	return frame_count;
}

std::map<std::wstring, KeyFrameAnimation*>* AnimationClip::GetBoneAnimations()
{
	return &bone_animations;
}

std::map<std::wstring, KeyFrameAnimation*>* AnimationClip::GetMorphAnimations()
{
	return &morph_animations;
}

KeyFrameAnimation* AnimationClip::GetCameraAnimation()
{
	return camera_animation;
}