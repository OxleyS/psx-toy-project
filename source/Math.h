#ifndef _MATH_H_
#define _MATH_H_

#include "Global.h"

class Vec3Long;
class Vec3Short : public SVECTOR
{
    public:
        Vec3Short() {}
        Vec3Short(short x, short y, short z) { vx = x; vy = y; vz = z; }
        Vec3Short(const SVECTOR& svec) { ((SVECTOR&)*this) = svec; }

        inline static Vec3Short FromLong(const Vec3Long& longVec);

        int LengthSq() const { return (vx * vx) + (vy * vy) + (vz * vz); }
        int Dot(const Vec3Short& rhs) const { return (vx * rhs.vx) + (vy * rhs.vy) + (vz * rhs.vz); }

        // Beware of overflow! It will cause strangely negative results, or crashes!
        Vec3Short GetNormalized() const { Vec3Short ret; VectorNormalSS((SVECTOR*)this, &ret); return ret; }
        void Normalize() { VectorNormalSS(this, this); }

        static const Vec3Short& Zero;
        static const Vec3Short& Right;
        static const Vec3Short& Up;
        static const Vec3Short& Forward;

        Vec3Short& operator=(const Vec3Short& rhs) { vx = rhs.vx; vy = rhs.vy; vz = rhs.vz; return *this; }
        Vec3Short operator+(const Vec3Short& rhs) const { return Vec3Short(vx + rhs.vx, vy + rhs.vy, vz + rhs.vz); }
        Vec3Short& operator+=(const Vec3Short& rhs) { vx += rhs.vx; vy += rhs.vy; vz += rhs.vz; return *this; }
        Vec3Short operator-(const Vec3Short& rhs) const { return Vec3Short(vx - rhs.vx, vy - rhs.vy, vz - rhs.vz); }
        Vec3Short& operator-=(const Vec3Short& rhs) { vx -= rhs.vx; vy -= rhs.vy; vz -= rhs.vz; return *this; }
        Vec3Short operator*(short scalar) const { return Vec3Short(vx * scalar, vy * scalar, vz * scalar); }
        Vec3Short& operator*=(short scalar) { vx *= scalar; vy *= scalar; vz *= scalar; return *this; }
        Vec3Short operator/(short scalar) const { return Vec3Short(vx / scalar, vy / scalar, vz / scalar); }
        Vec3Short& operator/=(short scalar) { vx /= scalar; vy /= scalar; vz /= scalar; return *this; }
        Vec3Short operator-() const { return Vec3Short(-vx, -vy, -vz); }
};

class Vec3Long : public VECTOR
{
    public:
        Vec3Long() {}
        Vec3Long(long x, long y, long z) { vx = x; vy = y; vz = z; }

        inline static Vec3Long FromShort(const Vec3Short& shortVec);

        // Beware of overflow!
        int Dot(const Vec3Long& rhs) const { return (vx * rhs.vx) + (vy * rhs.vy) + (vz * rhs.vz); }

        // Beware of overflow! It will cause strangely negative results, or crashes!
        Vec3Short GetNormalShort() const { Vec3Short ret; VectorNormalS((VECTOR*)this, &ret); return ret; }
        Vec3Long GetNormalized() const { Vec3Long ret; VectorNormal((VECTOR*)this, &ret); return ret; }
        void Normalize() { VectorNormal(this, this); }

        Vec3Long CrossFixed(const Vec3Long& other) const { Vec3Long ret; OuterProduct12((VECTOR*)this, (VECTOR*)&other, &ret); return ret; }
        Vec3Long CrossIntegral(const Vec3Long& other) const { Vec3Long ret; OuterProduct0((VECTOR*)this, (VECTOR*)&other, &ret); return ret; }

        static const Vec3Long& Zero;

        Vec3Long& operator=(const Vec3Long& rhs) { vx = rhs.vx; vy = rhs.vy; vz = rhs.vz; return *this; }
        Vec3Long operator+(const Vec3Long& rhs) const { return Vec3Long(vx + rhs.vx, vy + rhs.vy, vz + rhs.vz); }
        Vec3Long& operator+=(const Vec3Long& rhs) { vx += rhs.vx; vy += rhs.vy; vz += rhs.vz; return *this; }
        Vec3Long operator-(const Vec3Long& rhs) const { return Vec3Long(vx - rhs.vx, vy - rhs.vy, vz - rhs.vz); }
        Vec3Long& operator-=(const Vec3Long& rhs) { vx -= rhs.vx; vy -= rhs.vy; vz -= rhs.vz; return *this; }
        Vec3Long operator*(int scalar) const { return Vec3Long(vx * scalar, vy * scalar, vz * scalar); }
        Vec3Long& operator*=(int scalar) { vx *= scalar; vy *= scalar; vz *= scalar; return *this; }
        Vec3Long operator/(int scalar) const { return Vec3Long(vx / scalar, vy / scalar, vz / scalar); }
        Vec3Long& operator/=(int scalar) { vx /= scalar; vy /= scalar; vz /= scalar; return *this; }
        Vec3Long operator-() const { return Vec3Long(-vx, -vy, -vz); }
};

Vec3Short Vec3Short::FromLong(const Vec3Long& longVec) { return Vec3Short((short)longVec.vx, (short)longVec.vy, (short)longVec.vz); }
Vec3Long Vec3Long::FromShort(const Vec3Short& shortVec) { return Vec3Long((long)shortVec.vx, (long)shortVec.vy, (long)shortVec.vz); }

class Matrix : public MATRIX
{
    public:
        void ToIdentityRot();

        void MulInPlace(const Matrix& other);
        void Mul(const Matrix& other, Matrix& out);

        void TransposeRot() { TransposeMatrix(this, this); }

        Vec3Short GetRight() const { return Vec3Short(m[0][0], m[1][0], m[2][0]); }
        void SetRight(const Vec3Short& right) { m[0][0] = right.vx; m[1][0] = right.vy; m[2][0] = right.vz; }
        Vec3Short GetUp() const { return Vec3Short(m[0][1], m[1][1], m[2][1]); }
        void SetUp(const Vec3Short& up) { m[0][1] = up.vx; m[1][1] = up.vy; m[2][1] = up.vz; }
        Vec3Short GetForward() const { return Vec3Short(m[0][2], m[1][2], m[2][2]); }
        void SetForward(const Vec3Short& forward) { m[0][2] = forward.vx; m[1][2] = forward.vy; m[2][2] = forward.vz; }

        Vec3Long GetTrans() const { return Vec3Long(t[0], t[1], t[2]); }
        void SetTrans(const Vec3Long& trans) { t[0] = trans.vx; t[1] = trans.vy; t[2] = trans.vz; }

        static const Matrix& Identity;
};

namespace Math
{
    inline short Min(short a, short b) { return a < b ? a : b; }
    inline short Max(short a, short b) { return a > b ? a : b; }
}

#endif
