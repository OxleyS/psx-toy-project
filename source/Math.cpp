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
const Vec3Short g_Vec3ShortRight = Vec3Short(GTE_ONE, 0, 0);
const Vec3Short g_Vec3ShortUp = Vec3Short(0, -GTE_ONE, 0);
const Vec3Short g_Vec3ShortForward = Vec3Short(0, 0, GTE_ONE);

const Vec3Short& Vec3Short::Zero = g_Vec3ShortZero;
const Vec3Short& Vec3Short::Right = g_Vec3ShortRight;
const Vec3Short& Vec3Short::Up = g_Vec3ShortUp;
const Vec3Short& Vec3Short::Forward = g_Vec3ShortForward;

const Vec3Long g_Vec3LongZero = Vec3Long(0, 0, 0);

const Vec3Long& Vec3Long::Zero = g_Vec3LongZero;

const Matrix g_MatrixIdentity = CreateIdentityMatrix();

const Matrix& Matrix::Identity = g_MatrixIdentity;

void Matrix::ToIdentityRot()
{
	Memory::Copy(m, &g_MatrixIdentity, sizeof(m));
}

void Matrix::MulInPlace(const Matrix& other)
{
	CompMatrixLV((MATRIX*)this, (MATRIX*)&other, this);
}

void Matrix::Mul(const Matrix& other, Matrix& out)
{
	CompMatrixLV((MATRIX*)this, (MATRIX*)&other, &out);
}
