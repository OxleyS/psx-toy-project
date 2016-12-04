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
};

class Vec3Long : public VECTOR
{
    public:
        Vec3Long() {}
        Vec3Long(long x, long y, long z) { vx = x; vy = y; vz = z; }

        inline static Vec3Long FromShort(const Vec3Short& shortVec);
};

Vec3Short Vec3Short::FromLong(const Vec3Long& longVec) { return Vec3Short((short)longVec.vx, (short)longVec.vy, (short)longVec.vz); }
Vec3Long Vec3Long::FromShort(const Vec3Short& shortVec) { return Vec3Long((long)shortVec.vx, (long)shortVec.vy, (long)shortVec.vz); }

class Matrix : public MATRIX
{
    public:
        void ToIdentityRot();
        void ToZeroTrans() { t[0] = 0; t[1] = 0; t[2] = 0; }

        void MulInPlace(const Matrix& other);
        void Mul(const Matrix& other, Matrix& out);

        static const Matrix& Identity;
};

#endif