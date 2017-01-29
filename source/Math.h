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

        int Dot(const Vec3Long& rhs) const { return (vx * rhs.vx) + (vy * rhs.vy) + (vz * rhs.vz); }

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

        Vec3Short GetRight() const { return Vec3Short(m[0][0], m[1][0], m[2][0]); }

        Vec3Short GetUp() const { return Vec3Short(m[0][1], m[1][1], m[2][1]); }

        Vec3Short GetForward() const { return Vec3Short(m[0][2], m[1][2], m[2][2]); }

        Vec3Long GetTrans() const { return Vec3Long(t[0], t[1], t[2]); }
        void SetTrans(const Vec3Long& trans) { t[0] = trans.vx; t[1] = trans.vy; t[2] = trans.vz; }

        static const Matrix& Identity;
};

#endif
