#ifndef CAMERA_H
#define CAMERA_H

#include "MathLib.h"
#include "Event\Event.h"

class Model;

//----------------------------------------------------------------------------
// 旋转球
// ArcBall
//----------------------------------------------------------------------------
class ArcBall : public MouseEventListener
{
public:
	ArcBall(Model* model);

	void OnBegin(int x, int y);

	void OnMove(int x, int y);

	void OnEnd();

	void SetQuaternionNow(const Quaternion& quat);

	Quaternion GetQuaternionNow();

	void SetWindowSize(int w, int h);

	void MouseMove(const MouseEvent& event) override;

	void MousePressed(const MouseEvent& event) override;

	void MouseReleased(const MouseEvent& event) override;

private:
	Vector3D ScreenToVector(float screen_x, float screen_y);

private:
	Quaternion quat_down;
	Quaternion quat_now;

	Vector3D down_point;
	Vector3D current_point;

	bool drag;

	int width;
	int height;

	Model* model;
};


//----------------------------------------------------------------------------
// 摄像机
// Camera
//----------------------------------------------------------------------------
class Camera
{
public:
	Camera();

	virtual ~Camera();

	virtual void LookAt(const Vector3D& pos, const Vector3D& up = Vector3D::UP);

	virtual void Translate(const Vector3D& offset);

	void SetFov(float fov);

	float GetFov() { return fov; }

	virtual void SetTarget(const Vector3D& tar) { /* Nothing */ }

	virtual Vector3D GetTarget() { return Vector3D::ZERO; }

	virtual void SetDistance(float distance) { /* Nothing */ }

	virtual float GetDistance() { return 0; }

	virtual Vector3D GetPosition() { return position; }

	virtual void SetPosition(const Vector3D& pos) { position = pos; }

	virtual Quaternion GetRotation() { return rotation; }

	virtual void SetRotation(const Quaternion& rot) { rotation = rot; }

	Matrix4 GetVPMatrix() { return proj_matrix * view_matrix; }

	Matrix4 GetViewMatrix() { return view_matrix; }

	Matrix4 GetProjMatrix() { return proj_matrix; }

	void UpdateViewMatrix();

protected:
	Matrix4 proj_matrix;
	Matrix4 view_matrix;

	float fov;
	Vector3D position;
	Quaternion rotation;
};


//----------------------------------------------------------------------------
// 第三人称摄像机
// ThirdPersonCamera
//----------------------------------------------------------------------------
class ThirdPersonCamera : public Camera, public KeyEventListener, public MouseEventListener
{
public:
	ThirdPersonCamera();

	void KeyPressed(const KeyEvent& event) override;

	void MouseMove(const MouseEvent& event) override;

	void MousePressed(const MouseEvent& event) override;

	void MouseReleased(const MouseEvent& event) override;

	void MouseWheel(const MouseEvent& event) override;

	void SetRotation(const Quaternion& rot) override;

	Vector3D GetTarget() { return target; }

	void SetTarget(const Vector3D& tar);

	void SetDistance(float distance);

	float GetDistance() { return distance; }

	void UpdatePositionAndRotation();

private:
	Vector3D target;
	Vector3D offset;
	Quaternion target_rotation;

	bool button_pressed;

	float distance;
	float mouse_x;
	float mouse_y;
	float euler_x;
	float euler_y;
	float prev_euler_x;
	float prev_euler_y;
};

#endif