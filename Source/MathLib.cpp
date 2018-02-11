#include "MathLib.h"

const Vector3D Vector3D::ONE	= Vector3D(1, 1, 1);
const Vector3D Vector3D::ZERO	= Vector3D(0, 0, 0);
const Vector3D Vector3D::UNIT_X = Vector3D(1, 0, 0);
const Vector3D Vector3D::UNIT_Y = Vector3D(0, 1, 0);
const Vector3D Vector3D::UNIT_Z = Vector3D(0, 0, 1);
const Vector3D Vector3D::RIGHT	=  UNIT_X;
const Vector3D Vector3D::UP		=  UNIT_Y;
const Vector3D Vector3D::FRONT	=  UNIT_Z;
const Vector3D Vector3D::DOWN	= -UNIT_Y;;
const Vector3D Vector3D::LEFT	= -UNIT_X;
const Vector3D Vector3D::BACK	= -UNIT_Z;

const Matrix4 Matrix4::ZERO = Matrix4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
const Matrix4 Matrix4::IDENTITY = Matrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);


namespace Math
{
	float InvSqrt(float x)
	{
		float xhalf = 0.5f * x;
		int i = *( int* ) &x;
		i = 0x5f375a86 - (i >> 1);
		x = *( float* ) &i;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}

	float Clamp(float val, float min, float max)
	{
		return fmaxf(min, fminf(val, max));
	}

	bool Equal(float v1, float v2)
	{
		float o = v1 - v2;
		return (o >= -EPSINON && o <= EPSINON);
	}
}

float Vector3D::Length(const Vector3D& v1, const Vector3D &v2)
{
	Vector3D offset = v2 - v1;
	return offset.Length();
}

void Quaternion::SetRotationMatrx(const Matrix4& mat)
{
	float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	float root;

	if ( trace > 0.0f )
	{
		root = sqrtf(trace + 1.0f);
		w = root * 0.5f;
		root = 0.5f / root;
		x = (mat.m[2][1] - mat.m[1][2]) * root;
		y = (mat.m[0][2] - mat.m[2][0]) * root;
		z = (mat.m[1][0] - mat.m[0][1]) * root;
	}
	else
	{
		static size_t s_i_next[3] = { 1, 2, 0 };
		size_t i = 0;

		if ( mat.m[1][1] > mat.m[0][0] ) i = 1;
		if ( mat.m[2][2] > mat.m[i][i] ) i = 1;

		size_t j = s_i_next[i];
		size_t k = s_i_next[j];

		root = sqrtf(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f);
		float* apk_quat[3] = { &x, &y, &z };
		*apk_quat[i] = 0.5f * root;
		root = 0.5f / root;
		*apk_quat[j] = (mat.m[j][i] + mat.m[i][j]) * root;
		*apk_quat[k] = (mat.m[k][i] + mat.m[i][k]) * root;
		w = (mat.m[k][j] - mat.m[j][k]) * root;
	}
}

Matrix4 Quaternion::ToRotationMatrx()
{
	Matrix4 mat = Matrix4::IDENTITY;

	/* 四元数对应的旋转矩阵
	** ________________________________________________
	** |1-2(y^2 + z^2)	2(xy - zw)		2(xz + yw)	  |
	** |2(xy + zw)		1-2(x^2 + z^2)	2(yz - xw)	  |
	** |2(xz - yw)		2(yz + xw)		1-2(x^2 + y^2)|
	** ------------------------------------------------
	*/

	float tx = x + x;
	float ty = y + y;
	float tz = z + z;
	float twx = tx * w;
	float twy = ty * w;
	float twz = tz * w;
	float txx = tx * x;
	float txy = ty * x;
	float txz = tz * x;
	float tyy = ty * y;
	float tyz = tz * y;
	float tzz = tz * z;

	mat.m[0][0] = 1.0f - (tyy + tzz);
	mat.m[0][1] = txy - twz;
	mat.m[0][2] = txz + twy;
	mat.m[1][0] = txy + twz;
	mat.m[1][1] = 1.0f - (txx + tzz);
	mat.m[1][2] = tyz - twx;
	mat.m[2][0] = txz - twy;
	mat.m[2][1] = tyz + twx;
	mat.m[2][2] = 1.0f - (txx + tyy);

	return mat;
}

Quaternion Quaternion::AngleAxis(float rad, const Vector3D& axis)
{
	Quaternion quat;
	/* 确保旋转轴单位化 */
	Vector3D normalize_v = axis.GetNormalize();

	float half_radian = rad * 0.5f;
	float sin_val = sinf(half_radian);

	quat.w = cosf(half_radian);
	quat.x = sin_val * normalize_v.x;
	quat.y = sin_val * normalize_v.y;
	quat.z = sin_val * normalize_v.z;

	return quat;
}

void Quaternion::ToAngleAxis(float& rad, Vector3D& axis)
{
	float sqr_length = x * x + y * y + z * z;
	if ( sqr_length > 0.0f )
	{
		rad = acosf(w) * 2.0f;
		float inv_length = Math::InvSqrt(sqr_length);
		axis.x = x * inv_length;
		axis.y = y * inv_length;
		axis.z = z * inv_length;
	}
	else
	{
		rad = 0.0f;
		axis.x = 1.0f;
		axis.y = 0.0f;
		axis.z = 0.0f;
	}
}

Quaternion Quaternion::FromAxes(const Vector3D& x_axis, const Vector3D& y_axis, const Vector3D& z_axis)
{
	Matrix4 krot;
	Quaternion quat;

	krot.m[0][0] = x_axis.x;
	krot.m[1][0] = x_axis.y;
	krot.m[2][0] = x_axis.z;

	krot.m[0][1] = y_axis.x;
	krot.m[1][1] = y_axis.y;
	krot.m[2][1] = y_axis.z;

	krot.m[0][2] = z_axis.x;
	krot.m[1][2] = z_axis.y;
	krot.m[2][2] = z_axis.z;

	quat.SetRotationMatrx(krot);
	return quat;
}

void Quaternion::ToAxes(Vector3D& x_axis, Vector3D& y_axis, Vector3D& z_axis)
{
	Matrix4 krot = ToRotationMatrx();

	x_axis.x = krot.m[0][0];
	x_axis.y = krot.m[1][0];
	x_axis.z = krot.m[2][0];

	y_axis.x = krot.m[0][1];
	y_axis.y = krot.m[1][1];
	y_axis.z = krot.m[2][1];

	z_axis.x = krot.m[0][2];
	z_axis.y = krot.m[1][2];
	z_axis.z = krot.m[2][2];
}

Quaternion Quaternion::EulerXYZ(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz - sx * sy * sz;
	quat.x = sx * cy * cz + cx * sy * sz;
	quat.y = cx * sy * cz - sx * cy * sz;
	quat.z = sx * sy * cz + cx * cy * sz;

	return quat;
}

Quaternion Quaternion::EulerXZY(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz + sx * sy * sz;
	quat.x = sx * cy * cz - cx * sy * sz;
	quat.y = cx * sy * cz - sx * cy * sz;
	quat.z = cx * cy * sz + sx * sy * cz;

	return quat;
}

Quaternion Quaternion::EulerZXY(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz - sx * sy * sz;
	quat.x = sx * cy * cz - cx * sy * sz;
	quat.y = cx * sy * cz + sx * cy * sz;
	quat.z = cx * cy * sz + sx * sy * cz;

	return quat;
}

Quaternion Quaternion::EulerZYX(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz + sx * sy * sz;
	quat.x = sx * cy * cz - cx * sy * sz;
	quat.y = cx * sy * cz + sx * cy * sz;
	quat.z = cx * cy * sz - sx * sy * cz;

	return quat;
}

Quaternion Quaternion::EulerYXZ(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz + sx * sy * sz;
	quat.x = sx * cy * cz + cx * sy * sz;
	quat.y = cx * sy * cz - sx * cy * sz;
	quat.z = cx * cy * sz - sx * sy * cz;

	return quat;
}

Quaternion Quaternion::EulerYZX(float euler_x, float euler_y, float euler_z)
{
	Quaternion quat;

	float cx = cosf(euler_x * 0.5f);
	float cy = cosf(euler_y * 0.5f);
	float cz = cosf(euler_z * 0.5f);
	float sx = sinf(euler_x * 0.5f);
	float sy = sinf(euler_y * 0.5f);
	float sz = sinf(euler_z * 0.5f);

	quat.w = cx * cy * cz - sx * sy * sz;
	quat.x = sx * cy * cz + cx * sy * sz;
	quat.y = cx * sy * cz + sx * cy * sz;
	quat.z = cx * cy * sz - sx * sy * cz;

	return quat;
}

Vector3D Quaternion::ToEular()
{
	Vector3D result;
	ToEular(result.x, result.y, result.z);
	return result;
}

void Quaternion::ToEular(float& euler_x, float& euler_y, float& euler_z)
{
	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float ww = w * w;

	euler_x = atan2f(2.0f * (y*z + w*x), ww - xx - yy + zz);
	euler_y = asinf( 2.0f * (x*z + w*y));
	euler_z = atan2f(2.0f * (x*y + w*z), ww + xx - yy - zz);
}

float Quaternion::GetPitch()
{
	return atan2f(2.0f * (y*z + w*x), w*w - x*x - y*y + z*z);
}

float Quaternion::GetYaw()
{
	return asinf(2.0f * (x * z + w * y));
}

float Quaternion::GetRoll()
{
	return atan2f(2.0f * (x*y + w*z), w*w + x*x - y*y - z*z);
}

Quaternion Quaternion::LookRotation(const Vector3D& view, const Vector3D& up)
{
	Vector3D right = Vector3D::Cross(up, view);
	Vector3D local_up = Vector3D::Cross(view, right);

	return Quaternion::FromAxes(right.GetNormalize(), local_up.GetNormalize(), view.GetNormalize());
}

Quaternion Quaternion::FromToRotation(const Vector3D& from_direction, const Vector3D& to_direction)
{
	Vector3D v1 = from_direction.GetNormalize();
	Vector3D v2 = to_direction.GetNormalize();

	float d = Vector3D::Dot(v1, v2);

	/* 如果 d == 1，表示两个同样的向量 */
	if ( d >= 1.0f )
	{
		return Quaternion::Identity();
	}
	else
	{
		float s = sqrtf((1 + d) * 2);
		float invs = 1.0f / s;

		Vector3D axis = Vector3D::Cross(v1, v2);

		Quaternion q;
		q.x = axis.x * invs;
		q.y = axis.y * invs;
		q.z = axis.z * invs;
		q.w = s * 0.5f;
		return q.GetNormalize();
	}

	//return Quaternion::AngleAxis(
	//	Vector3D::Angle(from_direction, to_direction),
	//	Vector3D::Cross(from_direction, to_direction));
}

float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

Quaternion Quaternion::Identity()
{
	return Quaternion(0, 0, 0, 1);
}

Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, float t, bool shortest_path)
{
	float fcos = Quaternion::Dot(from, to);
	Quaternion result;

	if ( fcos < 0.0f && shortest_path )
	{
		fcos = -fcos;
		result = -to;
	}
	else
	{
		result = to;
	}

	float epsilon = 1e-03;
	if ( abs(fcos) < 1 - epsilon )
	{
		float fsin = sqrtf(1 - fcos * fcos);
		float angle = atan2f(fsin, fcos);
		float inv_sin = 1.0f / fsin;
		float coeff0 = sinf((1.0f - t) * angle) * inv_sin;
		float coeff1 = sinf(t * angle) * inv_sin;
		return from * coeff0 + result * coeff1;
	}
	else
	{
		Quaternion tmp = from * (1.0f - t) + result * t;
		return tmp.GetNormalize();
	}
}

Quaternion Quaternion::GetNormalize() const
{
	float inv_length = Math::InvSqrt(x * x + y * y + z * z + w * w);
	return Quaternion(x * inv_length, y * inv_length, z * inv_length, w * inv_length);
}

Quaternion Quaternion::Inverse() const
{
	return this->Conjugate() * (1 / (x * x + y * y + z * z + w * w));
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::operator+ (const Quaternion& q) const
{
	return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::operator- (const Quaternion& q) const
{
	return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
}

Quaternion Quaternion::operator* (const Quaternion& q) const
{
	return Quaternion(
		w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w + z * q.x - x * q.z,
		w * q.z + z * q.w + x * q.y - y * q.x,
		w * q.w - x * q.x - y * q.y - z * q.z);
}

Quaternion Quaternion::operator* (float scale) const
{
	return Quaternion(x * scale, y * scale, z * scale, w * scale);
}

Vector3D Quaternion::operator* (const Vector3D& v) const
{
	Vector3D qvec(x, y, z);
	Vector3D uv = Vector3D::Cross(qvec, v);
	Vector3D uuv = Vector3D::Cross(qvec, uv);

	return v + uv * (2.0f * w) + uuv * 2.0f;
}

Quaternion Quaternion::operator- () const
{
	return Quaternion(-x, -y, -z, -w);
}

bool Quaternion::operator==(const Quaternion& q) const
{
	return (x == q.x) && (y == q.y) && (z == q.z) && (w == q.w);
}

bool Quaternion::operator!=(const Quaternion& q) const
{
	return (x != q.x) || (y != q.y) || (z != q.z) || (w != q.w);
}