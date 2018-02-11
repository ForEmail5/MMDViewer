#include "Camera.h"
#include "Model.h"
#include "RenderWindow.h"

//----------------------------------------------------------------------------
// ArcBall
//----------------------------------------------------------------------------
ArcBall::ArcBall(Model* model)
{
	this->model = model;

	quat_down = Quaternion::Identity();
	quat_now = Quaternion::Identity();

	down_point = Vector3D::ZERO;
	current_point = Vector3D::ZERO;

	this->drag = false;

	RECT rc;
	GetClientRect(GetForegroundWindow(), &rc);
	this->SetWindowSize(rc.right, rc.bottom);
}

void ArcBall::OnBegin(int x, int y)
{
	if ( x >= 0 && x < width && y > 0 && y < height )
	{
		drag = true;
		quat_down = quat_now;
		down_point = this->ScreenToVector(( float ) x, ( float ) y);
	}
}

void ArcBall::OnMove(int x, int y)
{
	if ( drag )
	{
		current_point = this->ScreenToVector(( float ) x, ( float ) y);
		quat_now = Quaternion::FromToRotation(current_point, down_point) * quat_down;
	}
}

void ArcBall::OnEnd()
{
	this->drag = false;
}

void ArcBall::SetQuaternionNow(const Quaternion& quat)
{
	this->quat_now = quat;
}

Quaternion ArcBall::GetQuaternionNow()
{
	return quat_now;
}

void ArcBall::SetWindowSize(int w, int h)
{
	this->width = w;
	this->height = h;
}

void ArcBall::MouseMove(const MouseEvent& event)
{
	if ( event.button_type == ButtonType::LEFT_BUTTON )
	{
		this->OnMove(event.x, event.y);
	}
}

void ArcBall::MousePressed(const MouseEvent& event)
{
	if ( event.button_type == ButtonType::LEFT_BUTTON )
	{
		this->SetQuaternionNow(model->GetRotation());
		this->OnBegin(event.x, event.y);
	}
}

void ArcBall::MouseReleased(const MouseEvent& event)
{
	if ( event.button_type == ButtonType::LEFT_BUTTON )
	{
		this->OnEnd();
	}
}

Vector3D ArcBall::ScreenToVector(float screen_x, float screen_y)
{
	/* 将屏幕坐标转换到 [-1, 1] 范围内 */
	float x = (screen_x - width * 0.5f) / (width * 0.5f);
	float y = (screen_y - height * 0.5f) / (height * 0.5f);

	float z = 0.0f;
	float length = x * x + y * y;

	if ( length > 1.0f )
	{
		/* 坐标点位于单位球外 */
		float scale = Math::InvSqrt(length);
		x *= scale;
		y *= scale;
	}
	else
	{
		/* 坐标点位于单位球上，根据 x^2 + y^2 + z^2 = 1 计算出 z */
		z = sqrtf(1.0f - length);
	}
	return Vector3D(x, y, z);
}

//----------------------------------------------------------------------------
// Camera
//----------------------------------------------------------------------------
Camera::Camera()
{
	fov = 45;
	rotation = Quaternion::Identity();
	position = Vector3D(0, 0, -20);

	this->proj_matrix = Matrix4::perspectiveLH(fov * DEGREE_TO_RADIAN, RenderWindow::GetAspect(), 0.1f, 1000.0f).Transpose();
	this->UpdateViewMatrix();
}

Camera::~Camera()
{

}

void Camera::SetFov(float fov)
{ 
	if ( Math::Equal(this->fov, fov) ) return;

	this->fov = fov; 
	this->proj_matrix = Matrix4::perspectiveLH(fov * DEGREE_TO_RADIAN, RenderWindow::GetAspect(), 0.1f, 1000.0f).Transpose();
}

void Camera::LookAt(const Vector3D& target, const Vector3D& up)
{
	rotation = Quaternion::LookRotation(target - position, up);
	this->UpdateViewMatrix();
}

void Camera::Translate(const Vector3D& offset)
{
	position = position + offset;
	this->UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	Vector3D view_dir = rotation * Vector3D::FRONT;
	Vector3D up = rotation * Vector3D::UP;
	Vector3D target = position + view_dir;

	view_matrix = Matrix4::LookAtLH(position, target, up).Transpose();
}


//----------------------------------------------------------------------------
// ThirdPersonCamera
//----------------------------------------------------------------------------
ThirdPersonCamera::ThirdPersonCamera()
{
	target = Vector3D(0, 0, 0);
	offset = Vector3D::ZERO;

	distance = 20;
	button_pressed = false;
	euler_x = prev_euler_x = 0;
	euler_y = prev_euler_y = 0;

	target_rotation = Quaternion::Identity();

	this->SetPosition(target + target_rotation * Vector3D::BACK * distance);
	this->LookAt(target);
}

void ThirdPersonCamera::KeyPressed(const KeyEvent& event)
{
	if ( event.keys[KeyType::Key_Left] )
	{
		this->SetTarget(this->GetTarget() + Vector3D::LEFT);
	}
	if ( event.keys[KeyType::Key_Right] )
	{
		this->SetTarget(this->GetTarget() + Vector3D::RIGHT);
	}
	if ( event.keys[KeyType::Key_Up] )
	{
		this->SetTarget(this->GetTarget() + Vector3D::UP);
	}
	if ( event.keys[KeyType::Key_Down] )
	{
		this->SetTarget(this->GetTarget() + Vector3D::DOWN);
	}
}

void ThirdPersonCamera::MouseMove(const MouseEvent& event)
{
	if ( button_pressed && event.button_type == ButtonType::RIGHT_BUTTON )
	{
		float scale = 0.01;

		euler_x = prev_euler_x + (event.x - mouse_x) * scale;
		euler_y = prev_euler_y + (event.y - mouse_y) * scale;
		euler_y = Math::Clamp(euler_y, -PI * 0.4f, PI * 0.4f);

		this->SetRotation(Quaternion::EulerYXZ(-euler_y, euler_x, 0));
	}
}

void ThirdPersonCamera::MousePressed(const MouseEvent& event)
{
	if ( event.button_type == ButtonType::RIGHT_BUTTON )
	{
		button_pressed = true;
		mouse_x = event.x;
		mouse_y = event.y;

		prev_euler_x = euler_x;
		prev_euler_y = euler_y;
	}
}

void ThirdPersonCamera::MouseReleased(const MouseEvent& event)
{
	button_pressed = false;
}

void ThirdPersonCamera::MouseWheel(const MouseEvent& event)
{
	if ( event.delta != 0 )
	{
		int dir = -event.delta / 120;
		this->SetDistance(this->GetDistance() + dir);
	}
}

void ThirdPersonCamera::SetRotation(const Quaternion& rot)
{
	target_rotation = rot;
	this->UpdatePositionAndRotation();
}

void ThirdPersonCamera::SetTarget(const Vector3D& tar)
{
	target = tar;
	this->UpdatePositionAndRotation();
}

void ThirdPersonCamera::SetDistance(float distance)
{
	this->distance = distance;
	this->UpdatePositionAndRotation();
}

void ThirdPersonCamera::UpdatePositionAndRotation()
{
	/* 计算摄像机位置 */
	this->SetPosition(target + target_rotation * Vector3D::BACK * distance);

	/* 计算摄像机朝向 */
	this->LookAt(target, target_rotation * Vector3D::UP);
}