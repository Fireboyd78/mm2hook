#pragma once
#include "mm2_common.h"

#include <tuple>

namespace MM2
{
    struct Vector2 {
        float x, y;
    };

    struct Vector3 {
        float x, y, z;

#define __Vec3Op(op)                                                                                             \
Vector3  operator op    (const Vector3 rhs) { return { this->x op rhs.x, this->y op rhs.y, this->z op rhs.z }; } \
Vector3& operator op##= (const Vector3 rhs) { return *this = *this op rhs; }                                     \
Vector3  operator op    (const float   rhs) { return *this op    Vector3 { rhs, rhs, rhs }; }                    \
Vector3& operator op##= (const float   rhs) { return *this op##= Vector3 { rhs, rhs, rhs }; }

        __Vec3Op(+)
        __Vec3Op(-)
        __Vec3Op(*)
        __Vec3Op(/)

#undef __Vec3Op

        Vector3 Lerp(Vector3 rhs, float scale) {
            return (rhs - *this) * scale + *this;
        }

        float Dist(Vector3 rhs) {
            Vector3 delta = *this - rhs;
            return sqrt((delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z));
        }

        float InvMag()
        {
            float mag = this->x * this->x + this->y * this->y + this->z * this->z;

            if (mag == 0.0f)
            {
                return 0.0f;
            }
            else
            {
                return 1.0f / sqrt(mag);
            }
        }
    };

    struct Vector4 {
        float x, y, z, w;
    };

    struct Matrix34 {
        float
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34;
    };

    struct Matrix44 {
        float
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44;
    };
}
