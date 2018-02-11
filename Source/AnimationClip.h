#ifndef ANIMATION_CLIP_H
#define ANIMATION_CLIP_H

#include <map>
#include <vector>
#include <algorithm>

#include "MathLib.h"
#include "ModelBone.h"

#include "MMD\MmdCommon.h"


struct KeyFrameData
{
	float fov;
	float focal_length;

	Vector3D euler;
	Vector3D position;
	Quaternion rotation;

	Matrix4 ToMatrix()
	{
		return Matrix4::MakeTransform(position, Vector3D::ONE) * rotation.ToRotationMatrx();
	}
};


//-------------------------------------------------------------------
// 关键帧基类
// KeyFrame
//-------------------------------------------------------------------
class KeyFrame
{
public:
	KeyFrame(int nframe) : nframe(nframe) 
	{
	}

	virtual ~KeyFrame()
	{
	}

	int Frame() 
	{ 
		return nframe; 
	}

	bool IsLinear(const Vector2D& p1, const Vector2D& p2)
	{
		return Math::Equal(p1.x, p1.y) && Math::Equal(p2.x, p2.y);
	}

	float SampleBezier(const Vector2D& p1, const Vector2D& p2, float t)
	{
		static Vector2D p0 = Vector2D(0, 0);
		static Vector2D p3 = Vector2D(1, 1);

		Vector2D q0 = Vector2D::Lerp(p0, p1, t);
		Vector2D q1 = Vector2D::Lerp(p1, p2, t);
		Vector2D q2 = Vector2D::Lerp(p2, p3, t);

		Vector2D r0 = Vector2D::Lerp(q0, q1, t);
		Vector2D r1 = Vector2D::Lerp(q1, q2, t);

		return Vector2D::Lerp(r0, r1, t).y;

		// B(t) = P0 (1-t)^3 + 3 P1 t(1-t)^2 + 3 p2 t^2 (1-t) + p3 t^3;
		//return 3 * p1.y*t*(1 - t)*(1 - t) + 3 * p2.y*t*t*(1 - t) + p3.y*t*t*t;
	}

	virtual KeyFrameData GetData() = 0;

	virtual KeyFrameData Slerp(KeyFrame* kf, float t) = 0;

protected:
	int nframe;
};


//-------------------------------------------------------------------
// 浮点型关键帧
// FloatKeyFrame
//-------------------------------------------------------------------
class FloatKeyFrame : public KeyFrame
{
public:
	FloatKeyFrame(int nframe);

	void SetValue(float value);

	float GetValue();

	KeyFrameData GetData() override;

	KeyFrameData Slerp(KeyFrame* kf, float t) override;

private:
	float value;
};

//-------------------------------------------------------------------
// 动作关键帧
// MotionKeyFrame
//-------------------------------------------------------------------
class MotionKeyFrame : public KeyFrame
{
public:
	MotionKeyFrame(int nframe);

	void SetPosition(float x, float y, float z);

	void SetRotation(float x, float y, float z, float w);

	void SetBezier(char* interpolator);

	KeyFrameData GetData() override;

	KeyFrameData Slerp(KeyFrame* kf, float t) override;

private:
	/* 创建贝塞尔曲线的两个柄点 */
	Vector2D cx_0, cx_1;
	Vector2D cy_0, cy_1;
	Vector2D cz_0, cz_1;
	Vector2D cr_0, cr_1;

	/* 关键帧记录位移信息和旋转信息 */
	Vector3D position;
	Quaternion rotation;
};


//-------------------------------------------------------------------
// 摄像机动作关键帧
// CameraMotionKeyFrame
//-------------------------------------------------------------------
class CameraMotionKeyFrame : public KeyFrame
{
public:
	CameraMotionKeyFrame(int nframe);

	void SetFov(float fov);

	void SetFocalLength(float length);

	void SetPosition(float x, float y, float z);

	void SetRotation(float x, float y, float z);

	void SetBezier(char* interpolator);

	KeyFrameData GetData() override;

	KeyFrameData Slerp(KeyFrame* kf, float t) override;

private:
	/* 创建贝塞尔曲线的两个柄点 */
	Vector2D cx_0, cx_1;
	Vector2D cy_0, cy_1;
	Vector2D cz_0, cz_1;
	Vector2D crx_0, crx_1;
	Vector2D cry_0, cry_1;
	Vector2D crz_0, crz_1;

	/* 关键帧记录位移信息和旋转信息 */
	Vector3D position;
	Vector3D rotation;

	float fov;
	float focal_length;
};

//-------------------------------------------------------------------
// 关键帧动画
// KeyFrameAnimation
//-------------------------------------------------------------------
class KeyFrameAnimation
{
public:
	KeyFrameAnimation(const std::wstring& name);

	~KeyFrameAnimation();

	void AddKeyFrame(KeyFrame* kf);

	void ReleaseKeyFrame();

	void CreateIndexTable();

	KeyFrameData GetKeyFrameData(int nframe);

	int GetMaxFrame();

	int GetOutputIndex();

private:
	std::wstring name;
	std::vector<KeyFrame*> key_frames;
	std::vector<unsigned short> index_table;
};

//-------------------------------------------------------------------
// 动画片段
// AnimationClip
//-------------------------------------------------------------------
class AnimationClip
{
public:
	AnimationClip(mmd::mmd_motion* motion);

	~AnimationClip();

	/* 解析骨骼动画 */
	void ParseBoneAnimation(const std::vector<mmd::vmd_bone>& bones);

	/* 解析变形动画 */
	void ParseMorphAnimation(const std::vector<mmd::vmd_morph>& morphs);

	/* 解析摄像机动画 */
	void ParseCameraAnimation(const std::vector<mmd::vmd_camera>& cameras);

	void CreateIndexTable();

	int GetFrameCount();

	std::map<std::wstring, KeyFrameAnimation*>* GetBoneAnimations();

	std::map<std::wstring, KeyFrameAnimation*>* GetMorphAnimations();

	KeyFrameAnimation* GetCameraAnimation();

private:
	int frame_count;
	KeyFrameAnimation* camera_animation;

	std::map<std::wstring, KeyFrameAnimation*> morph_animations;
	std::map<std::wstring, KeyFrameAnimation*> bone_animations;
};

#endif