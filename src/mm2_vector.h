#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class Matrix34;
    extern class Matrix44;
    extern class Vector2;
    extern class Vector3;
    extern class Vector4;

    namespace $
    {
        namespace Matrix34 {
            HOOK_API AGEHook<0x004BBF50>::MemberFunc<void> Identity;
        }
        namespace Vector2
        {
            HOOK_API AGEHook<0x45CC60>::MemberFunc<float> Mag;
            HOOK_API AGEHook<0x45CC80>::MemberFunc<float> Mag2;
        }
        namespace Vector3
        {
            HOOK_API AGEHook<0x45CDC0>::MemberFunc<float> InvMag;
            HOOK_API AGEHook<0x466490>::MemberFunc<float> Mag;
            HOOK_API AGEHook<0x46C990>::MemberFunc<float> Mag2;

            HOOK_API AGEHook<0x45CCB0>::MemberFunc<void> Set$1;
            HOOK_API AGEHook<0x480AA0>::MemberFunc<void> Set$2;

            HOOK_API AGEHook<0x45CE00>::MemberFunc<void> Add;
            HOOK_API AGEHook<0x484420>::MemberFunc<void> AddScaled;
            HOOK_API AGEHook<0x45CD30>::MemberFunc<void> Subtract;
            HOOK_API AGEHook<0x474000>::MemberFunc<void> SubtractScaled;
            HOOK_API AGEHook<0x42EB40>::MemberFunc<void> Scale$1;
            HOOK_API AGEHook<0x45CE40>::MemberFunc<void> Scale$2;
            HOOK_API AGEHook<0x595B70>::MemberFunc<void> InvScale;
            HOOK_API AGEHook<0x4C0390>::MemberFunc<void> RotateX;
            HOOK_API AGEHook<0x4C03D0>::MemberFunc<void> RotateY;
            HOOK_API AGEHook<0x4C0400>::MemberFunc<void> RotateZ;
            HOOK_API AGEHook<0x4C0020>::MemberFunc<void> RotateAboutAxis;

            HOOK_API AGEHook<0x4C0430>::MemberFunc<float> Angle;
            HOOK_API AGEHook<0x4C0500>::MemberFunc<float> FastAngle;
            HOOK_API AGEHook<0x4BFF20>::MemberFunc<bool> Approach;
            HOOK_API AGEHook<0x45CE80>::MemberFunc<void> Cross$1;
            HOOK_API AGEHook<0x4429C0>::MemberFunc<void> Cross$2;
            HOOK_API AGEHook<0x45CEC0>::MemberFunc<float> Dist;
            HOOK_API AGEHook<0x45CF00>::MemberFunc<float> FlatDist;
            HOOK_API AGEHook<0x466470>::MemberFunc<float> Dot$1;
            HOOK_API AGEHook<0x43BBB0>::MemberFunc<void> Dot$2;
            HOOK_API AGEHook<0x479560>::MemberFunc<void> Dot3x3;
            HOOK_API AGEHook<0x4795C0>::MemberFunc<void> Dot3x3Transpose;
            HOOK_API AGEHook<0x494C00>::MemberFunc<void> Lerp;
            HOOK_API AGEHook<0x480AC0>::MemberFunc<void> Negate;

            HOOK_API AGEHook<0x522B80>::MemberFunc<bool> IsEqual;

            HOOK_API AGEHook<0x45CCD0>::MemberFunc<void> $$op_e_add$1;
            HOOK_API AGEHook<0x45CD00>::MemberFunc<void> $$op_e_sub$1;
            HOOK_API AGEHook<0x43DD50>::MemberFunc<void> $$op_e_mul;
            HOOK_API AGEHook<0x479520>::MemberFunc<void> $$op_e_add$2;
            HOOK_API AGEHook<0x470180>::MemberFunc<void> $$op_e_sub$2;

            HOOK_API AGEHook<0x491860>::MemberFunc<MM2::Vector3> $$op_sub;
            HOOK_API AGEHook<0x4918A0>::MemberFunc<MM2::Vector3> $$op_div;
            HOOK_API AGEHook<0x541770>::MemberFunc<MM2::Vector3> $$op_mod;
        }
        namespace Vector4
        {
            HOOK_API AGEHook<0x43DD80>::MemberFunc<void> Set;
            HOOK_API AGEHook<0x494C90>::MemberFunc<void> Cross;
            HOOK_API AGEHook<0x494C40>::MemberFunc<void> Subtract;
            HOOK_API AGEHook<0x43DDA0>::MemberFunc<float> Dot$1;
            HOOK_API AGEHook<0x494C70>::MemberFunc<float> Dot3;
            HOOK_API AGEHook<0x4C1170>::MemberFunc<MM2::Vector4 &> Dot$2;
            HOOK_API AGEHook<0x4C1200>::MemberFunc<MM2::Vector4 &> Dot3x3;
            HOOK_API AGEHook<0x4C52D0>::MemberFunc<void> ComputePlane$1;
            HOOK_API AGEHook<0x4C53B0>::MemberFunc<void> ComputePlane$2;
            HOOK_API AGEHook<0x4C53F0>::MemberFunc<void> Min;
            HOOK_API AGEHook<0x4C5460>::MemberFunc<void> Max;
        }
    }

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

        void Identity() {
            $::Matrix34::Identity(this);
        }
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

        AGE_API Vector2() {}
        AGE_API Vector2(float x, float y) : X(x), Y(y) {}

        AGE_API float Mag(void) const {
            return $::Vector2::Mag(this);
        }
        AGE_API float Mag2(void) const {
            return $::Vector2::Mag2(this);
        }

        AGE_API void Set(float x, float y) {
            X = x;
            Y = y;
        }
    };

    class Vector3 {
    public:
        float X;
        float Y;
        float Z;

        AGE_API Vector3(void) {}
        AGE_API Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

        AGE_API float InvMag(void) const {
            return $::Vector3::InvMag(this);
        }
        AGE_API float Mag(void) const {
            return $::Vector3::Mag(this);
        }
        AGE_API float Mag2(void) const {
            return $::Vector3::Mag2(this);
        }

        AGE_API void Set(float x, float y, float z) {
            $::Vector3::Set$1(this, x, y, z);
        }
        AGE_API void Set(const Vector3 &vec) {
            $::Vector3::Set$2(this, &vec);
        }

        AGE_API void Add(const Vector3 &vec) {
            $::Vector3::Add(this, &vec);
        }
        AGE_API void AddScaled(const Vector3 &vec1, const Vector3 &vec2, float scale) {
            $::Vector3::AddScaled(this, &vec1, &vec2, scale);
        }
        AGE_API void Subtract(const Vector3 &vec) {
            $::Vector3::Subtract(this, &vec);
        }
        AGE_API void SubtractScaled(const Vector3 &vec, float scale) {
            $::Vector3::SubtractScaled(this, &vec, scale);
        }
        AGE_API void Scale(float scale) {
            $::Vector3::Scale$1(this, scale);
        }
        AGE_API void Scale(const Vector3 &vec, float scale) {
            $::Vector3::Scale$2(this, &vec, scale);
        }
        AGE_API void InvScale(float scale) {
            $::Vector3::InvScale(this, scale);
        }
        AGE_API void RotateX(float angle) {
            $::Vector3::RotateX(this, angle);
        }
        AGE_API void RotateY(float angle) {
            $::Vector3::RotateY(this, angle);
        }
        AGE_API void RotateZ(float angle) {
            $::Vector3::RotateZ(this, angle);
        }
        AGE_API void RotateAboutAxis(float angle, int axis) {
            $::Vector3::RotateAboutAxis(this, angle, axis);
        }

        AGE_API float Angle(const Vector3 &vec) const {
            return $::Vector3::Angle(this, &vec);
        }
        AGE_API float FastAngle(const Vector3 &vec) const {
            return $::Vector3::FastAngle(this, &vec);
        }
        AGE_API bool Approach(const Vector3 &vec, float p2, float p3) {
            return $::Vector3::Approach(this, &vec, p2, p3);
        }
        AGE_API void Cross(const Vector3 &vec) {
            $::Vector3::Cross$1(this, &vec);
        }
        AGE_API void Cross(const Vector3 &vec1, const Vector3 &vec2) {
            $::Vector3::Cross$2(this, &vec1, &vec2);
        }
        AGE_API float Dist(const Vector3 &vec) const {
            return $::Vector3::Dist(this, &vec);
        }
        AGE_API float FlatDist(const Vector3 &vec) const {
            return $::Vector3::FlatDist(this, &vec);
        }
        AGE_API float Dot(const Vector3 &vec) const {
            return $::Vector3::Dot$1(this, &vec);
        }
        AGE_API void Dot(const Vector3 &vec, const Matrix34 &mtx) {
            $::Vector3::Dot$2(this, &vec, &mtx);
        }
        AGE_API void Dot3x3(const Matrix34 &mtx) {
            $::Vector3::Dot3x3(this, &mtx);
        }
        AGE_API void Dot3x3Transpose(const Matrix34 &mtx) {
            $::Vector3::Dot3x3Transpose(this, &mtx);
        }
        AGE_API void Lerp(float p1, const Vector3 &vec1, const Vector3 &vec2) {
            $::Vector3::Lerp(this, p1, &vec1, &vec2);
        }
        AGE_API void Negate(const Vector3 &vec) {
            $::Vector3::Negate(this, &vec);
        }

        AGE_API bool IsEqual(const Vector3 &vec) const {
            return $::Vector3::IsEqual(this, &vec);
        }

        AGE_API void operator*=(const Vector3 &vec) {
            $::Vector3::$$op_e_mul(this, &vec);
        }
        AGE_API void operator+=(const Vector3 &vec) {
            $::Vector3::$$op_e_add$1(this, &vec);
        }
        AGE_API void operator-=(const Vector3 &vec) {
            $::Vector3::$$op_e_sub$1(this, &vec);
        }
        AGE_API void operator+=(Vector3 &vec) {
            $::Vector3::$$op_e_add$2(this, &vec);
        }
        AGE_API void operator-=(Vector3 &vec) {
            $::Vector3::$$op_e_sub$2(this, &vec);
        }

        AGE_API Vector3 operator-(const Vector3 &vec) const {
            return $::Vector3::$$op_sub(this, &vec);
        }
        AGE_API Vector3 operator/(float value) const {
            return $::Vector3::$$op_div(this, value);
        }
        AGE_API Vector3 operator%(const Vector3 &vec) const {
            return $::Vector3::$$op_mod(this, &vec);
        }
    };

    class Vector4 {
    public:
        float X;
        float Y;
        float Z;
        float W;

        AGE_API Vector4() {}
        AGE_API Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

        AGE_API void Set(float x, float y, float z, float w) {
            $::Vector4::Set(this, x, y, z, w);
        }
        AGE_API void Cross(const Vector4 &vec1, const Vector4 &vec2) {
            $::Vector4::Cross(this, &vec1, &vec2);
        }
        AGE_API void Subtract(const Vector3 &vec1, const Vector3 &vec2) {
            $::Vector4::Subtract(this, &vec1, &vec2);
        }
        AGE_API float Dot(const Vector4 &vec) const {
            return $::Vector4::Dot$1(this, &vec);
        }
        AGE_API float Dot3(const Vector4 &vec) const {
            return $::Vector4::Dot3(this, &vec);
        }
        AGE_API Vector4 & Dot(const Vector4 &vec, Matrix44 const &mtx) {
            return $::Vector4::Dot$2(this, &vec, &mtx);
        }
        AGE_API Vector4 & Dot3x3(const Vector4 &vec, Matrix44 const &mtx) {
            return $::Vector4::Dot3x3(this, &vec, &mtx);
        }
        AGE_API void ComputePlane(const Vector3 &vec1, const Vector3 &vec2, const Vector3 &vec3) {
            $::Vector4::ComputePlane$1(this, &vec1, &vec2, &vec3);
        }
        AGE_API void ComputePlane(const Vector3 &vec1, const Vector3 &vec2) {
            $::Vector4::ComputePlane$2(this, &vec1, &vec2);
        }
        AGE_API void Min(const Vector4 &vec1, const Vector4 &vec2) {
            $::Vector4::Min(this, &vec1, &vec2);
        }
        AGE_API void Max(const Vector4 &vec1, const Vector4 &vec2) {
            $::Vector4::Max(this, &vec1, &vec2);
        }
    };
}