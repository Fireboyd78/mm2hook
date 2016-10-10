#pragma once

#include "mm2_utils.h"

const double cosNum = 1.570796;

namespace MM2
{
    // Forward declarations
    extern class Matrix34;
    extern class Matrix44;
    extern class Vector2;
    extern class Vector3;
    extern class Vector4;

    class Matrix34 {
    public:
        float m11;
        float m12;
        float m13;
        float m14;

        float m21;
        float m22;
        float m23;
        float m24;

        float m31;
        float m32;
        float m33;
        float m34;
    };

    class Matrix44 {
    public:
        float m11;
        float m12;
        float m13;
        float m14;

        float m21;
        float m22;
        float m23;
        float m24;

        float m31;
        float m32;
        float m33;
        float m34;

        float m41;
        float m42;
        float m43;
        float m44;
    };

    class Vector2 {
    public:
        float X;
        float Y;
    };

    class Vector3 {
    public:
        float X;
        float Y;
        float Z;
    };

    class Vector4 {
    public:
        float X;
        float Y;
        float Z;
        float W;
    };

    static Vector3 addPitch(Vector3 *vec, float pitch)
    {
        float p = (float) fmod(pitch, 3.14159);
        bool pitchIsZero = (pitch >= 0.0f);

        return {
            (float) ((!pitchIsZero) ? vec->X * cos(pitch + cosNum) : 0.0f),
            (float) ((!pitchIsZero) ? vec->Y * cos(pitch + cosNum) : 0.0f),
            (float) ((!pitchIsZero) ? vec->Z * cos(pitch + cosNum) : 0.0f),
        };
    }

    static float normalize(float value)
    {
        if (value >= 2.0f)
            value = 1.0f;

        return (value > 1.0f) ? (value - (value - 1.0f)) : value;
    };
}