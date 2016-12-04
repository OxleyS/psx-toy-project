#include "Math.h"

static Matrix CreateIdentityMatrix()
{
	Matrix mtx;
	Memory::Set(&mtx, 0, sizeof(Matrix));
	mtx.m[0][0] = 0x1000;
	mtx.m[1][1] = 0x1000;
	mtx.m[2][2] = 0x1000;
	return mtx;
}

const Vec3Short g_Vec3ShortZero = Vec3Short(0, 0, 0);
const Vec3Short g_Vec3ShortRight = Vec3Short(1, 0, 0);
const Vec3Short g_Vec3ShortUp = Vec3Short(0, -1, 0);
const Vec3Short g_Vec3ShortForward = Vec3Short(0, 0, 1);

const Vec3Short& Vec3Short::Zero = g_Vec3ShortZero;
const Vec3Short& Vec3Short::Right = g_Vec3ShortRight;
const Vec3Short& Vec3Short::Up = g_Vec3ShortUp;
const Vec3Short& Vec3Short::Forward = g_Vec3ShortForward;

const Matrix g_MatrixIdentity = CreateIdentityMatrix();

const Matrix& Matrix::Identity = g_MatrixIdentity;

void Matrix::ToIdentityRot()
{
	Memory::Copy(m, &g_MatrixIdentity, sizeof(m));
}

void Matrix::MulInPlace(const Matrix& other)
{
	// Multiplies just the rotation part of the matrix
	MulMatrix(this, (Matrix*)&other);

	// Handle the translation part
	ApplyMatrixLV((Matrix*)&other, (VECTOR*)t, (VECTOR*)t);
}

void Matrix::Mul(const Matrix& other, Matrix& out)
{
	// Multiplies just the rotation part of the matrix
	MulMatrix0(this, (Matrix*)&other, &out);

	// Handle the translation part
	ApplyMatrixLV((Matrix*)&other, (VECTOR*)t, (VECTOR*)out.t);
}
