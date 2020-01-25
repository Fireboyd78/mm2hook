#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class Matrix34;
    class Matrix44;
    class Vector2;
    class Vector3;
    class Vector4;

    namespace $
    {
        namespace Matrix34 {
            declhook(0x4BBF50, _MemberFunc<void>, Identity);
        }
        namespace Vector2
        {
            declhook(0x45CC60, _MemberFunc<float>, Mag);
            declhook(0x45CC80, _MemberFunc<float>, Mag2);
        }
        namespace Vector3
        {
            declhook(0x45CDC0, _MemberFunc<float>, InvMag);
            declhook(0x466490, _MemberFunc<float>, Mag);
            declhook(0x46C990, _MemberFunc<float>, Mag2);

            declhook(0x45CCB0, _MemberFunc<void>, Set$1);
            declhook(0x480AA0, _MemberFunc<void>, Set$2);

            declhook(0x45CE00, _MemberFunc<void>, Add);
            declhook(0x484420, _MemberFunc<void>, AddScaled);
            declhook(0x45CD30, _MemberFunc<void>, Subtract);
            declhook(0x474000, _MemberFunc<void>, SubtractScaled);
            declhook(0x42EB40, _MemberFunc<void>, Scale$1);
            declhook(0x45CE40, _MemberFunc<void>, Scale$2);
            declhook(0x595B70, _MemberFunc<void>, InvScale);
            declhook(0x4C0390, _MemberFunc<void>, RotateX);
            declhook(0x4C03D0, _MemberFunc<void>, RotateY);
            declhook(0x4C0400, _MemberFunc<void>, RotateZ);
            declhook(0x4C0020, _MemberFunc<void>, RotateAboutAxis);

            declhook(0x4C0430, _MemberFunc<float>, Angle);
            declhook(0x4C0500, _MemberFunc<float>, FastAngle);
            declhook(0x4BFF20, _MemberFunc<bool>, Approach);
            declhook(0x45CE80, _MemberFunc<void>, Cross$1);
            declhook(0x4429C0, _MemberFunc<void>, Cross$2);
            declhook(0x45CEC0, _MemberFunc<float>, Dist);
            declhook(0x45CF00, _MemberFunc<float>, FlatDist);
            declhook(0x466470, _MemberFunc<float>, Dot$1);
            declhook(0x43BBB0, _MemberFunc<void>, Dot$2);
            declhook(0x479560, _MemberFunc<void>, Dot3x3);
            declhook(0x4795C0, _MemberFunc<void>, Dot3x3Transpose);
            declhook(0x494C00, _MemberFunc<void>, Lerp);
            declhook(0x480AC0, _MemberFunc<void>, Negate);

            declhook(0x522B80, _MemberFunc<bool>, IsEqual);

            declhook(0x45CCD0, _MemberFunc<void>, $$op_e_add$1);
            declhook(0x45CD00, _MemberFunc<void>, $$op_e_sub$1);
            declhook(0x43DD50, _MemberFunc<void>, $$op_e_mul);
            declhook(0x479520, _MemberFunc<void>, $$op_e_add$2);
            declhook(0x470180, _MemberFunc<void>, $$op_e_sub$2);

            declhook(0x491860, _MemberFunc<MM2::Vector3>, $$op_sub);
            declhook(0x4918A0, _MemberFunc<MM2::Vector3>, $$op_div);
            declhook(0x541770, _MemberFunc<MM2::Vector3>, $$op_mod);
        }
        namespace Vector4
        {
            declhook(0x43DD80, _MemberFunc<void>, Set);
            declhook(0x494C90, _MemberFunc<void>, Cross);
            declhook(0x494C40, _MemberFunc<void>, Subtract);
            declhook(0x43DDA0, _MemberFunc<float>, Dot$1);
            declhook(0x494C70, _MemberFunc<float>, Dot3);
            declhook(0x4C1170, _MemberFunc<MM2::Vector4 &>, Dot$2);
            declhook(0x4C1200, _MemberFunc<MM2::Vector4 &>, Dot3x3);
            declhook(0x4C52D0, _MemberFunc<void>, ComputePlane$1);
            declhook(0x4C53B0, _MemberFunc<void>, ComputePlane$2);
            declhook(0x4C53F0, _MemberFunc<void>, Min);
            declhook(0x4C5460, _MemberFunc<void>, Max);
        }
    }

    class Matrix34 {
    public:
        float m00;
        float m01;
        float m02;
        
        float m10;
        float m11;
        float m12;

        float m20;
        float m21;
        float m22;

        float m30;
        float m31;
        float m32;

		void Matrix34::Dot(Matrix34* rhs)
		{
			ageHook::Thunk<0x4BC400>::Call(this, rhs);
		}

        void Scale(float amount) {
            this->m00 = amount * this->m00;
            this->m01 = amount * this->m01;
            this->m02 = amount * this->m02;
            this->m10 = amount * this->m10;
            this->m11 = amount * this->m11;
            this->m12 = amount * this->m12;
            this->m20 = amount * this->m20;
            this->m21 = amount * this->m21;
            this->m22 = amount * this->m22;
        }

        void Identity() {
            this->m00 = 1.0;
            this->m01 = 0.0;
            this->m02 = 0.0;
            this->m10 = 0.0;
            this->m11 = 1.0;
            this->m12 = 0.0;
            this->m20 = 0.0;
            this->m21 = 0.0;
            this->m22 = 1.0;
            this->m30 = 0.0;
            this->m31 = 0.0;
            this->m32 = 0.0;
        }

        void Identity3x3() {
            this->m00 = 1.0;
            this->m01 = 0.0;
            this->m02 = 0.0;
            this->m10 = 0.0;
            this->m11 = 1.0;
            this->m12 = 0.0;
            this->m20 = 0.0;
            this->m21 = 0.0;
            this->m22 = 1.0;
        }

        AGE_API void Set(const Matrix34* a1)                { ageHook::Thunk<0x4BBFB0>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Matrix34>("Matrix34")
                .addFactory([](float m00 = 0.0, float m01 = 0.0, float m02 = 0.0,
                               float m10 = 0.0, float m11 = 0.0, float m12 = 0.0, 
                               float m20 = 0.0, float m21 = 0.0, float m22 = 0.0, 
                               float m30 = 0.0, float m31 = 0.0, float m32 = 0.0) {
                    auto mtx = new Matrix34();
                    mtx->m00 = m00;
                    mtx->m01 = m01;
                    mtx->m02 = m02;

                    mtx->m10 = m10;
                    mtx->m11 = m11;
                    mtx->m12 = m12;
                  
                    mtx->m20 = m20;
                    mtx->m21 = m21;
                    mtx->m22 = m22;

                    mtx->m30 = m30;
                    mtx->m31 = m31;
                    mtx->m32 = m32;
                    return mtx;
                }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>,
                        _opt<float>, _opt<float>, _opt<float>, _opt<float>,
                        _opt<float>, _opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("m00", &Matrix34::m00)
                .addVariableRef("m01", &Matrix34::m01)
                .addVariableRef("m02", &Matrix34::m02)
                .addVariableRef("m10", &Matrix34::m10)
                .addVariableRef("m11", &Matrix34::m11)
                .addVariableRef("m12", &Matrix34::m12)
                .addVariableRef("m20", &Matrix34::m20)
                .addVariableRef("m21", &Matrix34::m21)
                .addVariableRef("m22", &Matrix34::m22)
                .addVariableRef("m30", &Matrix34::m30)
                .addVariableRef("m31", &Matrix34::m31)
                .addVariableRef("m32", &Matrix34::m32)
                .addFunction("Identity", &Matrix34::Identity)
                .addFunction("Identity3x3", &Matrix34::Identity3x3)
            .endClass();
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

        AGE_API static void Convert(Matrix44* output, const Matrix34* input)
                                                            { ageHook::StaticThunk<0x42FCF0>::Call<void>(output, input); }
        AGE_API void Identity(void)                         { ageHook::Thunk<0x4C0730>::Call<void>(this); }
        AGE_API void Transpose(void)                        { ageHook::Thunk<0x4C0929>::Call<void>(this); }
        AGE_API void Zero(void)                             { ageHook::Thunk<0x4C0770>::Call<void>(this); }
        AGE_API void Hermite(void)                          { ageHook::Thunk<0x4C0870>::Call<void>(this); }
        AGE_API void Bezier(void)                           { ageHook::Thunk<0x4C0820>::Call<void>(this); }
        AGE_API void CatmullRom(void)                       { ageHook::Thunk<0x4C08C0>::Call<void>(this); }
        AGE_API float Determinant(void)                     { return ageHook::Thunk<0x4C134A>::Call<float>(this); }
        AGE_API void Add(const Matrix44* a1)                { ageHook::Thunk<0x4C0A30>::Call<void>(this, a1); }
        AGE_API void AddScaled(const Matrix44* a1, float a2){ ageHook::Thunk<0x4C0CB0>::Call<void>(this, a1, a2); }
        AGE_API void Dot(const Matrix44* a1)                { ageHook::Thunk<0x4C0D50>::Call<void>(this, a1); }
        AGE_API void FastInverse(const Matrix44* a1)        { ageHook::Thunk<0x4C1260>::Call<void>(this, a1); }
        AGE_API void FromMatrix34(const Matrix34* a1)       { ageHook::Thunk<0x4C0970>::Call<void>(this, a1); }
        AGE_API void InvertTo(const Matrix44* a1)           { ageHook::Thunk<0x4C1510>::Call<void>(this, a1); }
        AGE_API void MakeRotX(float a1)                     { ageHook::Thunk<0x4C1C00>::Call<void>(this, a1); }
        AGE_API void MakeRotY(float a1)                     { ageHook::Thunk<0x4C1C50>::Call<void>(this, a1); }
        AGE_API void MakeRotZ(float a1)                     { ageHook::Thunk<0x4C1CA0>::Call<void>(this, a1); }
        AGE_API void Print(LPCSTR name)                     { ageHook::Thunk<0x4C1CF0>::Call<void>(this, name); }
        AGE_API void Set(const Matrix44* a1)                { ageHook::Thunk<0x4C07B0>::Call<void>(this, a1); }
        AGE_API void Subtract(const Matrix44* a1)           { ageHook::Thunk<0x4C0B70>::Call<void>(this, a1); }
        AGE_API void ToMatrix34(const Matrix34* a1)         { ageHook::Thunk<0x4C09E0>::Call<void>(this, a1); }
        AGE_API void Transform4(const Vector3* a1, const Vector4* a2, int a3)        
                                                            { ageHook::Thunk<0x4C1D60>::Call<void>(this, a1, a2, a3); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Matrix44>("Matrix44")
                .addFactory([](float m11 = 0.0, float m12 = 0.0, float m13 = 0.0, float m14 = 0.0,
                            float m21 = 0.0, float m22 = 0.0, float m23 = 0.0, float m24 = 0.0,
                            float m31 = 0.0, float m32 = 0.0, float m33 = 0.0, float m34 = 0.0,
                            float m41 = 0.0, float m42 = 0.0, float m43 = 0.0, float m44 = 0.0) {
                    auto mtx = new Matrix44();
                    mtx->m11 = m11;
                    mtx->m12 = m12;
                    mtx->m13 = m13;
                    mtx->m14 = m14;

                    mtx->m21 = m21;
                    mtx->m22 = m22;
                    mtx->m23 = m23;
                    mtx->m24 = m24;

                    mtx->m31 = m31;
                    mtx->m32 = m32;
                    mtx->m33 = m33;
                    mtx->m34 = m34;

                    mtx->m41 = m41;
                    mtx->m42 = m42;
                    mtx->m43 = m43;
                    mtx->m44 = m44;
                    return mtx;
                }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>,
                        _opt<float>, _opt<float>, _opt<float>, _opt<float>,
                        _opt<float>, _opt<float>, _opt<float>, _opt<float>,
                        _opt<float>, _opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("m11", &Matrix44::m11)
                .addVariableRef("m12", &Matrix44::m12)
                .addVariableRef("m13", &Matrix44::m13)
                .addVariableRef("m14", &Matrix44::m14)
                .addVariableRef("m21", &Matrix44::m21)
                .addVariableRef("m22", &Matrix44::m22)
                .addVariableRef("m23", &Matrix44::m23)
                .addVariableRef("m24", &Matrix44::m24)
                .addVariableRef("m31", &Matrix44::m31)
                .addVariableRef("m32", &Matrix44::m32)
                .addVariableRef("m33", &Matrix44::m33)
                .addVariableRef("m34", &Matrix44::m34)
                .addVariableRef("m41", &Matrix44::m41)
                .addVariableRef("m42", &Matrix44::m42)
                .addVariableRef("m43", &Matrix44::m43)
                .addVariableRef("m44", &Matrix44::m44)
                .addStaticFunction("Convert", &Convert)
                .addFunction("Identity", &Identity)
                .addFunction("Transpose", &Transpose)
                .addFunction("Add", &Add)
                .addFunction("Subtract", &Subtract)
                .addFunction("Bezier", &Bezier)
                .addFunction("CatmullRom", &CatmullRom)
                .addFunction("Hermite", &Hermite)
                .addFunction("Zero", &Zero)
                .addFunction("AddScaled", &AddScaled)
                .addFunction("Dot", &Dot)
                .addFunction("FastInverse", &FastInverse)
                .addFunction("FromMatrix34", &FromMatrix34)
                .addFunction("InvertTo", &InvertTo)
                .addFunction("MakeRotX", &MakeRotX)
                .addFunction("MakeRotY", &MakeRotY)
                .addFunction("MakeRotZ", &MakeRotZ)
                .addFunction("Print", &Print)
                .addFunction("Set", &Set)
                .addFunction("Transform4", &Transform4)
                .addFunction("Determinant", &Determinant)
            .endClass();
        }
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

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector2>("Vector2")
                .addFactory([](float x = 0.0, float y = 0.0) {
                    auto vec = new Vector2;
                    vec->X = x;
                    vec->Y = y;
                    return vec;
                }, LUA_ARGS(_opt<float>, _opt<float>))
                .addVariableRef("x", &Vector2::X)
                .addVariableRef("y", &Vector2::Y)
            .endClass();
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

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector3>("Vector3")
                .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0) {
                    auto vec = new Vector3;
                    vec->X = x;
                    vec->Y = y;
                    vec->Z = z;
                    return vec;
                }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("x", &Vector3::X)
                .addVariableRef("y", &Vector3::Y)
                .addVariableRef("z", &Vector3::Z)
                .addFunction("Mag", &Mag)
                .addFunction("Mag2", &Mag2)
                .addFunction("InvMag", &InvMag)
                .addFunction("RotateX", &RotateX)
                .addFunction("RotateY", &RotateY)
                .addFunction("RotateZ", &RotateZ)
                .addFunction("RotateAboutAxis", &RotateAboutAxis)
                .addFunction("Lerp", &Lerp)
                .addFunction("Dist", &Dist)
                .addFunction("Angle", &Angle)
                .addFunction("FastAngle", &FastAngle)
                .addFunction("FlatDist", &FlatDist)
                .addFunction("Approach", &Approach)
                .addFunction("Negate", &Negate)                
                    
                    
            .endClass();
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

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector4>("Vector4")
                .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) {
                    auto vec = new Vector4;
                    vec->X = x;
                    vec->Y = y;
                    vec->Z = z;
                    vec->W = w;
                    return vec;
                }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("x", &Vector4::X)
                .addVariableRef("y", &Vector4::Y)
                .addVariableRef("z", &Vector4::Z)
                .addVariableRef("w", &Vector4::W)
            .endClass();
        }
    };
}