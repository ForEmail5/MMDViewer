#ifndef MMD_MATH_H
#define MMD_MATH_H

#include <math.h>

#define PI 3.1415926f

#define EPSINON 1e-6f

#define DEGREE_TO_RADIAN 0.017453f
#define RADIAN_TO_DEGREE 57.29578f

namespace Math
{
	/* 相当于 1.0f / sqrt(x) */
	float InvSqrt(float x);

	float Clamp(float val, float min, float max);

	/* 浮点型数据不能直接使用 '=='、'!=' 比较 */
	bool Equal(float v1, float v2);
}

//---------------------------------------------------------------------
// Vertor2D
//---------------------------------------------------------------------
class Vector2D
{
public:
	float x, y;

	Vector2D() : x(0), y(0)
	{
	}

	Vector2D(float x, float y) : x(x), y(y) 
	{
	}

	void Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Normalize()
	{
		*this = this->GetNormalize();
	}

	Vector2D GetNormalize()
	{
		if ( Length() == 0 ) return *this;

		float inv_length = Math::InvSqrt(x * x + y * y);
		return Vector2D(x * inv_length, y * inv_length);
	}

	float Length()
	{
		return sqrt(x * x + y * y);
	}

	Vector2D operator- ()
	{
		return Vector2D(-x, -y);
	}

	static float Dot(const Vector2D& v1, const Vector2D &v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	static Vector2D Lerp(const Vector2D& v1, const Vector2D &v2, float t)
	{
		return Vector2D(
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t);
	}
};

/* + */
inline Vector2D operator + (const Vector2D &v1, const Vector2D &v2) { return Vector2D(v1.x + v2.x, v1.y + v2.y); }
inline Vector2D operator + (float s, const Vector2D &v) { return Vector2D(v.x + s, v.y + s); }
inline Vector2D operator + (const Vector2D &v, float s) { return Vector2D(v.x + s, v.y + s); }

/* - */
inline Vector2D operator - (const Vector2D &v1, const Vector2D &v2) { return Vector2D(v1.x - v2.x, v1.y - v2.y); }
inline Vector2D operator - (float s, const Vector2D &v) { return Vector2D(s - v.x, s - v.y); }
inline Vector2D operator - (const Vector2D &v, float s) { return Vector2D(v.x - s, v.y - s); }

/* * */
inline Vector2D operator * (const Vector2D &v1, const Vector2D &v2) { return Vector2D(v1.x * v2.x, v1.y * v2.y); }
inline Vector2D operator * (float s, const Vector2D &v) { return Vector2D(v.x * s, v.y * s); }
inline Vector2D operator * (const Vector2D &v, float s) { return Vector2D(v.x * s, v.y * s); }

/* / */
inline Vector2D operator / (const Vector2D &v1, const Vector2D &v2) { return Vector2D(v1.x / v2.x, v1.y / v2.y); }
inline Vector2D operator / (float s, const Vector2D &v) { return Vector2D(s / v.x, s / v.y); }
inline Vector2D operator / (const Vector2D &v, float s) { return Vector2D(v.x / s, v.y / s); }


//---------------------------------------------------------------------
// Vertor3D
//---------------------------------------------------------------------
class Vector3D
{
public:
	float x, y, z;

	Vector3D() : x(0), y(0), z(0)
	{
	}

	Vector3D(const Vector2D& v) : x(v.x), y(v.y), z(0)
	{
	}

	Vector3D(float x, float y, float z) : x(x), y(y), z(z)
	{
	}

	void Set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Normalize()
	{
		*this = this->GetNormalize();
	}

	Vector3D GetNormalize() const
	{
		if ( Length() == 0.0f ) return *this;

		float inv_length = Math::InvSqrt(x * x + y * y + z * z);
		return Vector3D(x * inv_length, y * inv_length, z * inv_length);
	}

	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline Vector3D operator - () const
	{
		return Vector3D(-x, -y, -z);
	}

	static float Angle(const Vector3D& v1, const Vector3D &v2)
	{
		float len = v1.Length() * v2.Length();
		if ( len < EPSINON ) len = EPSINON;

		return acosf(Math::Clamp(Dot(v1, v2) / len, -1.0f, 1.0f));
	}

	static float Length(const Vector3D& v1, const Vector3D &v2);

	static float Dot(const Vector3D& v1, const Vector3D &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	static Vector3D Cross(const Vector3D& v1, const Vector3D &v2)
	{
		/* 推导公式
		** a x b = (x1, y1, z1) x (x2, y2, z2)
		**
		** |i  j  k |
		** |x1 y1 z1| -> (y1*z2 - y2*z1)i - (x1*z2 - x2*z1)j + (x1*y2 - x2*y1)k;
		** |x2 y2 z2|
		**
		** a x b = [y1*z2 - y2*z1, -(x1*z2 - x2*z1), x1*y2 - x2*y1);
		*/
		return Vector3D(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}

	static Vector3D Lerp(const Vector3D& v1, const Vector3D &v2, float t)
	{
		return Vector3D(
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t,
			v1.z + (v2.z - v1.z) * t);
	}

	static Vector3D Lerp(const Vector3D& v1, const Vector3D &v2, const Vector3D &t)
	{
		return Vector3D(
			v1.x + (v2.x - v1.x) * t.x,
			v1.y + (v2.y - v1.y) * t.y,
			v1.z + (v2.z - v1.z) * t.z);
	}

	static const Vector3D UNIT_X;
	static const Vector3D UNIT_Y;
	static const Vector3D UNIT_Z;
	static const Vector3D ONE;
	static const Vector3D ZERO;
	static const Vector3D UP;
	static const Vector3D DOWN;
	static const Vector3D LEFT;
	static const Vector3D RIGHT;
	static const Vector3D FRONT;
	static const Vector3D BACK;
};

/* + */
inline Vector3D operator + (const Vector3D &v1, const Vector3D &v2) { return Vector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline Vector3D operator + (float s, const Vector3D &v) { return Vector3D(v.x + s, v.y + s, v.z + s); }
inline Vector3D operator + (const Vector3D &v, float s) { return Vector3D(v.x + s, v.y + s, v.z + s); }

/* - */
inline Vector3D operator - (const Vector3D &v1, const Vector3D &v2) { return Vector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline Vector3D operator - (float s, const Vector3D &v) { return Vector3D(s - v.x, s - v.y, s - v.z); }
inline Vector3D operator - (const Vector3D &v, float s) { return Vector3D(v.x - s, v.y - s, v.z - s); }

/* * */
inline Vector3D operator * (const Vector3D &v1, const Vector3D &v2) { return Vector3D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
inline Vector3D operator * (float s, const Vector3D &v) { return Vector3D(v.x * s, v.y * s, v.z * s); }
inline Vector3D operator * (const Vector3D &v, float s) { return Vector3D(v.x * s, v.y * s, v.z * s); }

/* / */
inline Vector3D operator / (const Vector3D &v1, const Vector3D &v2) { return Vector3D(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
inline Vector3D operator / (float s, const Vector3D &v) { return Vector3D(s / v.x, s / v.y, s / v.z); }
inline Vector3D operator / (const Vector3D &v, float s) { return Vector3D(v.x / s, v.y / s, v.z / s); }


//---------------------------------------------------------------------
// Vector4D
//---------------------------------------------------------------------
class Vector4D
{
public:
	float x, y, z, w;

	Vector4D() : x(0), y(0), z(0), w(0) 
	{
	}

	Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
	{
	}

	Vector4D(const Vector3D& v) : x(v.x), y(v.y), z(v.z), w(1)
	{
	}
};


//---------------------------------------------------------------------
// Matrix4
//---------------------------------------------------------------------
class Matrix4
{
public:
	union
	{
		float m[4][4];
		float _m[16];
	};

	Matrix4()
	{
	}

	Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;
		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;
		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;
		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	Matrix4 Transpose() const
	{
		return Matrix4(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	Matrix4 Matrix4::Inverse() const
	{
		float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
		float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
		float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
		float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

		float v0 = m20 * m31 - m21 * m30;
		float v1 = m20 * m32 - m22 * m30;
		float v2 = m20 * m33 - m23 * m30;
		float v3 = m21 * m32 - m22 * m31;
		float v4 = m21 * m33 - m23 * m31;
		float v5 = m22 * m33 - m23 * m32;

		float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
		float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
		float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
		float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

		float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		float d00 = t00 * invDet;
		float d10 = t10 * invDet;
		float d20 = t20 * invDet;
		float d30 = t30 * invDet;

		float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		return Matrix4(
			d00, d01, d02, d03,
			d10, d11, d12, d13,
			d20, d21, d22, d23,
			d30, d31, d32, d33);
	}

	inline Vector3D operator* (const Vector3D& v) const
	{
		Vector3D result;
		float inv_w = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

		result.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * inv_w;
		result.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * inv_w;
		result.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * inv_w;

		return result;
	}

	inline Matrix4 operator*(const Matrix4 &m2) const
	{
		Matrix4 r;
		r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
		r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
		r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
		r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

		r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
		r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
		r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
		r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

		r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
		r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
		r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
		r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

		r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
		r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
		r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
		r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

		return r;
	}

	static inline Matrix4 LookAtLH(Vector3D eye, Vector3D target, Vector3D up)
	{
		Matrix4 matrix = Matrix4::IDENTITY;

		/* U: 相机的右方, V: 相机的上方, N: 相机的后方 */
		Vector3D N = (target - eye).GetNormalize();
		Vector3D U = Vector3D::Cross(up, N).GetNormalize();
		Vector3D V = Vector3D::Cross(N, U).GetNormalize();

		matrix.m[0][0] = U.x;
		matrix.m[1][0] = U.y;
		matrix.m[2][0] = U.z;

		matrix.m[0][1] = V.x;
		matrix.m[1][1] = V.y;
		matrix.m[2][1] = V.z;

		matrix.m[0][2] = N.x;
		matrix.m[1][2] = N.y;
		matrix.m[2][2] = N.z;

		matrix.m[3][0] = -Vector3D::Dot(U, eye);
		matrix.m[3][1] = -Vector3D::Dot(V, eye);
		matrix.m[3][2] = -Vector3D::Dot(N, eye);

		return matrix;
	}

	static inline Matrix4 LookAtRH(Vector3D eye, Vector3D target, Vector3D up)
	{
		Matrix4 matrix = Matrix4::IDENTITY;

		/* U: 相机的右方, V: 相机的上方, N: 相机的后方 */
		Vector3D N = (eye - target).GetNormalize();
		Vector3D U = Vector3D::Cross(up, N).GetNormalize();
		Vector3D V = Vector3D::Cross(U, N).GetNormalize();

		matrix.m[0][0] = U.x;
		matrix.m[1][0] = U.y;
		matrix.m[2][0] = U.z;

		matrix.m[0][1] = V.x;
		matrix.m[1][1] = V.y;
		matrix.m[2][1] = V.z;

		matrix.m[0][2] = N.x;
		matrix.m[1][2] = N.y;
		matrix.m[2][2] = N.z;

		matrix.m[3][0] = -Vector3D::Dot(U, eye);
		matrix.m[3][1] = -Vector3D::Dot(V, eye);
		matrix.m[3][2] = -Vector3D::Dot(N, eye);

		return matrix;
	}

	static inline Matrix4 perspectiveLH(float fov, float aspect, float near_plane, float far_plane)
	{
		float tan_half_fovy = tan(fov / 2);
		Matrix4 matrix = Matrix4::ZERO;

		matrix.m[0][0] = 1 / (aspect * tan_half_fovy);
		matrix.m[1][1] = 1 / tan_half_fovy;
		matrix.m[2][3] = 1;

		matrix.m[2][2] = far_plane / (far_plane - near_plane);
		matrix.m[3][2] = -(far_plane * near_plane) / (far_plane - near_plane);

		return matrix;
	}

	static inline Matrix4 perspectiveRH(float fov, float aspect, float near_plane, float far_plane)
	{
		float tan_half_fovy = tan(fov / 2);
		Matrix4 matrix = Matrix4::ZERO;

		matrix.m[0][0] = 1 / (aspect * tan_half_fovy);
		matrix.m[1][1] = 1 / tan_half_fovy;
		matrix.m[2][3] = -1;

		matrix.m[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
		matrix.m[3][2] = -(2 * far_plane * near_plane) / (far_plane - near_plane);

		return matrix;
	}

	static inline Matrix4 MakeTransform(const Vector3D& position, const Vector3D& scale)
	{
		Matrix4 mat = Matrix4::IDENTITY;

		mat.m[0][0] = scale.x;
		mat.m[1][1] = scale.y;
		mat.m[2][2] = scale.z;

		mat.m[0][3] = scale.z * position.x;
		mat.m[1][3] = scale.z * position.y;
		mat.m[2][3] = scale.z * position.z;

		mat.m[3][3] = 1;

		return mat;
	}

	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;
};


//---------------------------------------------------------------------
// Quaternion
//---------------------------------------------------------------------
class Quaternion
{
public:
	float x, y, z, w;

public:
	Quaternion() : w(0), x(0), y(0), z(0) {}

	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	void SetRotationMatrx(const Matrix4& mat);

	Matrix4 ToRotationMatrx();

	static Quaternion FromAxes(const Vector3D& x_axis, const Vector3D& y_axis, const Vector3D& z_axis);

	void ToAxes(Vector3D& x_axis, Vector3D& y_axis, Vector3D& z_axis);

	static Quaternion AngleAxis(float rad, const Vector3D& axis);

	void ToAngleAxis(float& rad, Vector3D& axis);

	Vector3D ToEular();

	void ToEular(float& euler_x, float& euler_y, float& euler_z);

	float GetPitch();

	float GetYaw();

	float GetRoll();

	static Quaternion EulerXYZ(float euler_x, float euler_y, float euler_z);

	static Quaternion EulerXZY(float euler_x, float euler_y, float euler_z);

	static Quaternion EulerZXY(float euler_x, float euler_y, float euler_z);

	static Quaternion EulerZYX(float euler_x, float euler_y, float euler_z);

	static Quaternion EulerYXZ(float euler_x, float euler_y, float euler_z);

	static Quaternion EulerYZX(float euler_x, float euler_y, float euler_z);

	static Quaternion LookRotation(const Vector3D& view, const Vector3D& up = Vector3D::UP);

	static Quaternion FromToRotation(const Vector3D& from_direction, const Vector3D& to_direction);

	static Quaternion Identity();

	static float Dot(const Quaternion& q1, const Quaternion& q2);

	static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t, bool shortest_path = false);

	Quaternion Inverse() const;

	Quaternion Conjugate() const;

	Quaternion GetNormalize() const;

	Quaternion operator+ (const Quaternion& q) const;
	Quaternion operator- (const Quaternion& q) const;
	Quaternion operator* (const Quaternion& q) const;
	Quaternion operator* (float scale) const;

	Vector3D operator* (const Vector3D& v) const;

	Quaternion operator- () const;

	bool operator==(const Quaternion& q) const;
	bool operator!=(const Quaternion& q) const;
};

//---------------------------------------------------------------------
// Color
//---------------------------------------------------------------------
class Color
{
public:
	float r, g, b, a;

	Color() : r(0), g(0), b(0), a(1)
	{
	}

	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
	{
	}
};


//---------------------------------------------------------------------
// Rect
//---------------------------------------------------------------------
class Rect
{
public:
	float x, y, w, h;

	Rect() : x(0), y(0), w(0), h(0)
	{
	}

	Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h)
	{
	}
};


struct Vector2Di { int x, y; };
struct Vector4Di { int x, y, z, w; };
struct Vector2Dui { unsigned int x, y; };
struct Vector4Dui { unsigned int x, y, z, w; };

#endif