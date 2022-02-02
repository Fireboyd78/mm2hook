#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class Vector2;
    class Vector3;
    class Vector4;
    class Matrix34;
    class Matrix44;

    class Vector2 {
    public:
        float X;
        float Y;

        AGE_API Vector2() {}
        AGE_API Vector2(float x, float y) : X(x), Y(y) {}

        AGE_API float Mag(void) const {
            return sqrtf(this->Mag2());
        }
        AGE_API float Mag2(void) const {
            return (X * X + Y * Y);
        }

        AGE_API void Set(float x, float y) {
            X = x;
            Y = y;
        }

        bool IsEqual(const Vector2& vec) const {
            return this->X == vec.X && this->Y == vec.Y;
        }

        AGE_API void Normalize()
        {
            float len = this->Mag2();
            float mul = (len != 00.f) ? 1.0f / sqrtf(len) : 0.0f;

            this->X *= mul;
            this->Y *= mul;
        }

        Vector2 operator+(Vector2& vec) {
            return Vector2(this->X + vec.X, this->Y + vec.Y);
        }
        Vector2 operator-(const Vector2 vec) const {
            return Vector2(vec.X - this->X, vec.Y - this->Y);
        }
        Vector2 operator/(float value) const {
            return Vector2(this->X / value, this->Y / value);
        }
        Vector2 operator*(float value) const {
            return Vector2(this->X * value, this->Y * value);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector2>("Vector2")
                .addFactory([](float x = 0.0, float y = 0.0) {
                auto vec = Vector2();
                vec.X = x;
                vec.Y = y;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>))
                .addVariableRef("x", &Vector2::X)
                .addVariableRef("y", &Vector2::Y)

                .addFunction("__add", &Vector2::operator+)
                .addFunction("__sub", &Vector2::operator-)
                .addFunction("__div", &Vector2::operator/)
                .addFunction("__mul", &Vector2::operator*)
                .addFunction("__eq", &Vector2::IsEqual)

                .addFunction("Mag", &Mag)
                .addFunction("Mag2", &Mag2)
                .addFunction("Normalize", &Normalize)
                .endClass();
        }
    };

    ASSERT_SIZEOF(Vector2, 0x8);

    class Vector3 {
    public:
        float X;
        float Y;
        float Z;

        AGE_API Vector3(void) {}
        AGE_API Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

        //operators


        //functions
        AGE_API float InvMag(void) const {
            return 1.0f / this->Mag();
        }
        AGE_API float Mag(void) const {
            return sqrtf(this->Mag2());
        }
        AGE_API float Mag2(void) const {
            return (X * X + Y * Y + Z * Z);
        }
        AGE_API void Set(float x, float y, float z) {
            this->X = x;
            this->Y = y;
            this->Z = z;
        }
        AGE_API void Set(const Vector3& vec) {
            this->X = vec.X;
            this->Y = vec.Y;
            this->Z = vec.Z;
        }

        AGE_API void Add(const Vector3& vec) {
            this->X += vec.X;
            this->Y += vec.Y;
            this->Z += vec.Z;
        }
        AGE_API void AddScaled(const Vector3& vec, float scale) {
            this->X += (vec.X * scale);
            this->Y += (vec.Y * scale);
            this->Z += (vec.Z * scale);
        }
        AGE_API void Subtract(const Vector3& vec) {
            this->X -= vec.X;
            this->Y -= vec.Y;
            this->Z -= vec.Z;
        }
        AGE_API void SubtractScaled(const Vector3& vec, float scale) {
            this->X -= (vec.X * scale);
            this->Y -= (vec.Y * scale);
            this->Z -= (vec.Z * scale);
        }
        AGE_API void Scale(float scale) {
            this->X *= scale;
            this->Y *= scale;
            this->Z *= scale;
        }
        AGE_API void InvScale(float scale) {
            this->X *= 1.0f / scale;
            this->Y *= 1.0f / scale;
            this->Z *= 1.0f / scale;
        }
        AGE_API void RotateX(float angle) {
            hook::Thunk<0x4C0390>::Call<void>(this, angle);
        }
        AGE_API void RotateY(float angle) {
            hook::Thunk<0x4C03D0>::Call<void>(this, angle);
        }
        AGE_API void RotateZ(float angle) {
            hook::Thunk<0x4C0400>::Call<void>(this, angle);
        }
        AGE_API void RotateAboutAxis(float angle, int axis) {
            hook::Thunk<0x4C0020>::Call<void>(this, angle, axis);
        }
        AGE_API float Angle(const Vector3& vec) const {
            return hook::Thunk<0x4C0430>::Call<float>(this, &vec);
        }
        AGE_API float FastAngle(const Vector3& vec) const {
            return hook::Thunk<0x4C0500>::Call<float>(this, &vec);
        }
        AGE_API bool Approach(const Vector3& vec, float p2, float p3) {
            return hook::Thunk<0x4BFF20>::Call<bool>(this, &vec, p2, p3);
        }
        AGE_API void Cross(const Vector3& vec) {
            hook::Thunk<0x45CE80>::Call<void>(this, &vec);
        }
        AGE_API void Cross(const Vector3& vec1, const Vector3& vec2) {
            hook::Thunk<0x4429C0>::Call<void>(this, &vec1, &vec2);
        }
        AGE_API float Dist(const Vector3& vec) const {
            return (vec - *this).Mag();
        }
        AGE_API float FlatDist(const Vector3& vec) const {
            float dX = vec.X - this->X;
            float dY = vec.Y - this->Y;
            return sqrtf(dX * dX + dY * dY);
        }
        AGE_API float Dot(const Vector3& vec) const {
            return hook::Thunk<0x466470>::Call<float>(this, &vec);
        }
        AGE_API void Dot(const Vector3& vec, const Matrix34& mtx) {
            hook::Thunk<0x43BBB0>::Call<void>(this, &vec, &mtx);
        }
        AGE_API void Dot3x3(const Matrix34& mtx) {
            hook::Thunk<0x479560>::Call<void>(this, &mtx);
        }
        AGE_API void Dot3x3Transpose(const Matrix34& mtx) {
            hook::Thunk<0x4795C0>::Call<void>(this, &mtx);
        }
        AGE_API void Lerp(float p1, const Vector3& vec1, const Vector3& vec2) {
            hook::Thunk<0x494C00>::Call<void>(this, p1, &vec1, &vec2);
        }
        AGE_API void Negate(const Vector3& vec) {
            hook::Thunk<0x480AC0>::Call<void>(this, &vec);
        }
        AGE_API void Normalize()
        {
            float len = this->Mag2();
            float mul = (len != 00.f) ? 1.0f / sqrtf(len) : 0.0f;

            this->X *= mul;
            this->Y *= mul;
            this->Z *= mul;
        }

        AGE_API void HSVtoRGB() {
            hook::Thunk<0x4C0270>::Call<void>(this);
        }

        AGE_API void RGBtoHSV() {
            hook::Thunk<0x4C0160>::Call<void>(this);
        }

        AGE_API bool IsEqual(const Vector3& vec) const {
            return vec.X == this->X && vec.Y == this->Y && vec.Z == this->Z;
        }

        AGE_API void operator*=(const Vector3& vec) {
            hook::Thunk<0x43DD50>::Call<void>(this, &vec);
        }
        AGE_API void operator+=(const Vector3& vec) {
            hook::Thunk<0x45CCD0>::Call<void>(this, &vec);
        }
        AGE_API void operator-=(const Vector3& vec) {
            hook::Thunk<0x45CD00>::Call<void>(this, &vec);
        }
        AGE_API void operator+=(Vector3& vec) {
            hook::Thunk<0x479520>::Call<void>(this, &vec);
        }
        AGE_API void operator-=(Vector3& vec) {
            hook::Thunk<0x470180>::Call<void>(this, &vec);
        }

        AGE_API Vector3 operator+(Vector3& vec) const {
            return Vector3(this->X + vec.X, this->Y + vec.Y, this->Z + vec.Z);
        }
        AGE_API Vector3 operator-(const Vector3& vec) const {
            return hook::Thunk<0x491860>::Call<Vector3>(this, &vec);
        }
        AGE_API Vector3 operator/(float value) const {
            return Vector3(this->X / value, this->Y / value, this->Z / value);
        }
        AGE_API Vector3 operator*(float value) const {
            return Vector3(this->X * value, this->Y * value, this->Z * value);
        }
        AGE_API Vector3 operator%(const Vector3& vec) const {
            return hook::Thunk<0x541770>::Call<Vector3>(this, &vec);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector3>("Vector3")
                .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0) {
                auto vec = Vector3();
                vec.X = x;
                vec.Y = y;
                vec.Z = z;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("x", &Vector3::X)
                .addVariableRef("y", &Vector3::Y)
                .addVariableRef("z", &Vector3::Z)

                .addFunction("__add", &Vector3::operator+)
                .addFunction("__sub", &Vector3::operator-)
                .addFunction("__div", &Vector3::operator/)
                .addFunction("__mul", &Vector3::operator*)
                .addFunction("__eq", &Vector3::IsEqual)

                .addFunction("Cross", static_cast<void(Vector3::*)(const Vector3&)>(&Vector3::Cross))
                .addFunction("Normalize", &Normalize)
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

    ASSERT_SIZEOF(Vector3, 0xC);

    class Vector4 {
    public:
        float X;
        float Y;
        float Z;
        float W;

        AGE_API Vector4() {}
        AGE_API Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

        AGE_API void Set(float x, float y, float z, float w) {
            this->X = x;
            this->Y = y;
            this->Z = z;
            this->W = w;
        }
        AGE_API void Cross(const Vector4& vec1, const Vector4& vec2)                     { hook::Thunk<0x494C90>::Call<void>(this, &vec1, &vec2); }
        AGE_API void Subtract(const Vector3& vec1, const Vector3& vec2)                  { hook::Thunk<0x494C40>::Call<void>(this, &vec1, &vec2); }
        AGE_API float Dot(const Vector4& vec) const                                      { return hook::Thunk<0x43DDA0>::Call<float>(this, &vec); }
        AGE_API float Dot3(const Vector4& vec) const                                     { return hook::Thunk<0x494C70>::Call<float>(&vec); }
        AGE_API Vector4& Dot(const Vector4& vec, Matrix44 const& mtx)                    { return hook::Thunk<0x4C1170>::Call<Vector4&>(this, &vec, &mtx); }
        AGE_API Vector4& Dot3x3(const Vector4& vec, Matrix44 const& mtx)                 { return hook::Thunk<0x4C1200>::Call<Vector4&>(this, &vec, &mtx); }
        AGE_API void ComputePlane(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3)
                                                                                         { hook::Thunk<0x4C52D0>::Call<void>(this, &vec1, &vec2, &vec3); }
        AGE_API void ComputePlane(const Vector3& vec1, const Vector3& vec2)              { hook::Thunk<0x4C53B0>::Call<void>(this, &vec1, &vec2); }
        AGE_API void Min(const Vector4& vec1, const Vector4& vec2)                       { hook::Thunk<0x4C53F0>::Call<void>(this, &vec1, &vec2); }
        AGE_API void Max(const Vector4& vec1, const Vector4& vec2)                       { hook::Thunk<0x4C5460>::Call<void>(this, &vec1, &vec2); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Vector4>("Vector4")
                .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) {
                auto vec = Vector4();
                vec.X = x;
                vec.Y = y;
                vec.Z = z;
                vec.W = w;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("x", &Vector4::X)
                .addVariableRef("y", &Vector4::Y)
                .addVariableRef("z", &Vector4::Z)
                .addVariableRef("w", &Vector4::W)
                .endClass();
        }
    };

    ASSERT_SIZEOF(Vector4, 0x10);

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

        //operators


        //function
        void MakeRotate(Vector3 const& axis, float angle)
        {
            hook::Thunk<0x4BCFA0>::Call<void>(this, &axis, angle);
        }

        void MakeScale(float xScale, float yScale, float zScale) {
            this->m00 = xScale;
            this->m01 = 0.0;
            this->m02 = 0.0;
            this->m10 = 0.0;
            this->m11 = yScale;
            this->m12 = 0.0;
            this->m20 = 0.0;
            this->m21 = 0.0;
            this->m22 = zScale;
        }

        void MakeScale(float scale) {
            this->m00 = scale;
            this->m01 = 0.0;
            this->m02 = 0.0;
            this->m10 = 0.0;
            this->m11 = scale;
            this->m12 = 0.0;
            this->m20 = 0.0;
            this->m21 = 0.0;
            this->m22 = scale;
        }

        void MakeRotateZ(float angle) {
            this->m02 = 0.0;
            float v2 = sinf(angle);
            float v3 = cosf(angle);
            this->m00 = v3;
            this->m01 = v2;
            this->m10 = -v2;
            this->m12 = 0.0;
            this->m11 = v3;
            this->m20 = 0.0;
            this->m21 = 0.0;
            this->m22 = 1.0;
        }

        void MakeRotateY(float angle) {
            float v2 = cosf(angle);
            this->m01 = 0.0;
            float v3 = sinf(angle);
            this->m00 = v2;
            this->m02 = -v3;
            this->m10 = 0.0;
            this->m11 = 1.0;
            this->m12 = 0.0;
            this->m21 = 0.0;
            this->m20 = v3;
            this->m22 = v2;
        }

        void MakeRotateX(float angle) {
            this->m00 = 1.0;
            this->m01 = 0.0;
            this->m02 = 0.0;
            this->m10 = 0.0;
            float v2 = sinf(angle);
            float v3 = cosf(angle);
            this->m11 = v3;
            this->m12 = v2;
            this->m21 = -v2;
            this->m20 = 0.0;
            this->m22 = v3;
        }

        void Rotate(Vector3 const& axis, float angle)
        {
            hook::Thunk<0x4BCD70>::Call<void>(this, &axis, angle);
        }

        void RotateFull(Vector3 const& axis, float angle)
        {
            hook::Thunk<0x4BCE60>::Call<void>(this, &axis, angle);
        }

        void RotateX(float angle) {
            Matrix34 rotMatrix = Matrix34();
            rotMatrix.MakeRotateX(angle);
            this->Dot(rotMatrix);
        }

        void RotateY(float angle) {
            Matrix34 rotMatrix = Matrix34();
            rotMatrix.MakeRotateY(angle);
            this->Dot(rotMatrix);
        }

        void RotateZ(float angle) {
            Matrix34 rotMatrix = Matrix34();
            rotMatrix.MakeRotateZ(angle);
            this->Dot(rotMatrix);
        }

        void Add(Matrix34* values) {
            this->m00 = values->m00 + this->m00;
            this->m01 = values->m01 + this->m01;
            this->m02 = values->m02 + this->m02;
            this->m10 = values->m10 + this->m10;
            this->m11 = values->m11 + this->m11;
            this->m12 = values->m12 + this->m12;
            this->m20 = values->m20 + this->m20;
            this->m21 = values->m21 + this->m21;
            this->m22 = values->m22 + this->m22;
            this->m30 = values->m30 + this->m30;
            this->m31 = values->m31 + this->m31;
            this->m32 = values->m32 + this->m32;
        }

        void Normalize() {
            hook::Thunk<0x4BEE90>::Call<void>(this);
        }

        void LookAt(const Vector3& from, const Vector3& to)
        {
            hook::Thunk<0x4BEB20>::Call<void>(this, &from, &to);
        }

        void Zero() {
            this->m02 = 0.0;
            this->m01 = 0.0;
            this->m00 = 0.0;
            this->m12 = 0.0;
            this->m11 = 0.0;
            this->m10 = 0.0;
            this->m22 = 0.0;
            this->m21 = 0.0;
            this->m20 = 0.0;
            this->m32 = 0.0;
            this->m31 = 0.0;
            this->m30 = 0.0;
        }

        void Dot(const Matrix34& rhs) {
            float v3 = rhs.m11 * this->m01 + this->m02 * rhs.m21 + rhs.m01 * this->m00;
            float v15 = rhs.m12 * this->m01 + rhs.m22 * this->m02 + rhs.m02 * this->m00;
            float v14 = rhs.m10 * this->m11 + this->m12 * rhs.m20 + this->m10 * rhs.m00;
            float v13 = rhs.m11 * this->m11 + this->m12 * rhs.m21 + rhs.m01 * this->m10;
            float v12 = rhs.m12 * this->m11 + rhs.m02 * this->m10 + rhs.m22 * this->m12;
            float v11 = rhs.m10 * this->m21 + this->m20 * rhs.m00 + rhs.m20 * this->m22;
            float v10 = rhs.m11 * this->m21 + rhs.m01 * this->m20 + rhs.m21 * this->m22;
            float v9 = rhs.m22 * this->m22 + rhs.m12 * this->m21 + rhs.m02 * this->m20;
            float v8 = rhs.m10 * this->m31 + this->m30 * rhs.m00 + this->m32 * rhs.m20 + rhs.m30;
            float v7 = rhs.m11 * this->m31 + rhs.m01 * this->m30 + this->m32 * rhs.m21 + rhs.m31;
            float v6 = rhs.m22 * this->m32 + rhs.m12 * this->m31 + rhs.m02 * this->m30 + rhs.m32;
            float v4 = rhs.m10 * this->m01 + rhs.m00 * this->m00;
            float v5 = this->m02 * rhs.m20;
            this->m02 = v15;
            this->m00 = v4 + v5;
            this->m01 = v3;
            this->m10 = v14;
            this->m11 = v13;
            this->m12 = v12;
            this->m20 = v11;
            this->m21 = v10;
            this->m22 = v9;
            this->m30 = v8;
            this->m31 = v7;
            this->m32 = v6;
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

        void Scale(float xAmount, float yAmount, float zAmount) {
            this->m00 = xAmount * this->m00;
            this->m10 = xAmount * this->m10;
            this->m20 = xAmount * this->m20;
            this->m01 = yAmount * this->m01;
            this->m11 = yAmount * this->m11;
            this->m21 = yAmount * this->m21;
            this->m02 = zAmount * this->m02;
            this->m12 = zAmount * this->m12;
            this->m22 = zAmount * this->m22;
        }

        void ScaleFull(float amount) {
            this->m00 = amount * this->m00;
            this->m01 = amount * this->m01;
            this->m02 = amount * this->m02;
            this->m10 = amount * this->m10;
            this->m11 = amount * this->m11;
            this->m12 = amount * this->m12;
            this->m20 = amount * this->m20;
            this->m21 = amount * this->m21;
            this->m22 = amount * this->m22;
            this->m30 = amount * this->m30;
            this->m31 = amount * this->m31;
            this->m32 = amount * this->m32;
        }

        void ScaleFull(float xAmount, float yAmount, float zAmount) {
            this->m00 = xAmount * this->m00;
            this->m10 = xAmount * this->m10;
            this->m20 = xAmount * this->m20;
            this->m30 = xAmount * this->m30;
            this->m01 = yAmount * this->m01;
            this->m11 = yAmount * this->m11;
            this->m21 = yAmount * this->m21;
            this->m31 = yAmount * this->m31;
            this->m02 = zAmount * this->m02;
            this->m12 = zAmount * this->m12;
            this->m22 = zAmount * this->m22;
            this->m32 = zAmount * this->m32;
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

        void Transform(const Vector3& vector, Vector3& out) const
        {
            out.X = this->m00 * vector.X + this->m10 * vector.Y + this->m20 * vector.Z + this->m30;
            out.Y = this->m01 * vector.X + this->m11 * vector.Y + this->m21 * vector.Z + this->m31;
            out.Z = this->m02 * vector.X + this->m12 * vector.Y + this->m22 * vector.Z + this->m32;
        }

        Vector3 Transform(const Vector3& vector) const
        {
            Vector3 returnVec;
            Transform(vector, returnVec);
            return returnVec;
        }

        Vector4 GetColumn(int column) const
        {
            switch (column)
            {
            case 0:
                return Vector4(this->m00, this->m10, this->m20, this->m30);
            case 1:
                return Vector4(this->m01, this->m11, this->m21, this->m31);
            case 2:
                return Vector4(this->m02, this->m12, this->m22, this->m32);
            default:
                return Vector4();
            }
        }

        Vector3 GetRow(int row) const
        {
            switch (row)
            {
            case 0:
                return Vector3(this->m00, this->m01, this->m02);
            case 1:
                return Vector3(this->m10, this->m11, this->m12);
            case 2:
                return Vector3(this->m20, this->m21, this->m22);
            case 3:
                return Vector3(this->m30, this->m31, this->m32);
            default:
                return Vector3();
            }
        }

        void SetColumn(int column, Vector4 value)
        {
            switch (column)
            {
            case 0:
                this->m00 = value.X;
                this->m10 = value.Y;
                this->m20 = value.Z;
                this->m30 = value.W;
                break;
            case 1:
                this->m01 = value.X;
                this->m11 = value.Y;
                this->m21 = value.Z;
                this->m31 = value.W;
                break;
            case 2:
                this->m02 = value.X;
                this->m12 = value.Y;
                this->m22 = value.Z;
                this->m32 = value.W;
                break;
            }
        }

        void SetRow(int row, Vector3 value)
        {
            switch (row)
            {
            case 0:
                this->m00 = value.X;
                this->m01 = value.Y;
                this->m02 = value.Z;
                break;
            case 1:
                this->m10 = value.X;
                this->m11 = value.Y;
                this->m12 = value.Z;
                break;
            case 2:
                this->m20 = value.X;
                this->m21 = value.Y;
                this->m22 = value.Z;
                break;
            case 3:
                this->m30 = value.X;
                this->m31 = value.Y;
                this->m32 = value.Z;
                break;
            }
        }

        AGE_API void Set(const Matrix34& a1) { hook::Thunk<0x4BBFB0>::Call<void>(this, &a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Matrix34>("Matrix34")
                .addFactory([](float m00 = 1.0, float m01 = 0.0, float m02 = 0.0,
                    float m10 = 0.0, float m11 = 1.0, float m12 = 0.0,
                    float m20 = 0.0, float m21 = 0.0, float m22 = 1.0,
                    float m30 = 0.0, float m31 = 0.0, float m32 = 0.0) {
                auto mtx = Matrix34();
                mtx.m00 = m00;
                mtx.m01 = m01;
                mtx.m02 = m02;

                mtx.m10 = m10;
                mtx.m11 = m11;
                mtx.m12 = m12;

                mtx.m20 = m20;
                mtx.m21 = m21;
                mtx.m22 = m22;

                mtx.m30 = m30;
                mtx.m31 = m31;
                mtx.m32 = m32;
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

                .addFunction("GetColumn", &GetColumn)
                .addFunction("GetRow", &GetRow)
                .addFunction("SetColumn", &SetColumn)
                .addFunction("SetRow", &SetRow)

                .addFunction("Identity", &Matrix34::Identity)
                .addFunction("Identity3x3", &Matrix34::Identity3x3)
                .addFunction("Scale", static_cast<void(Matrix34::*)(float, float, float)>(&Matrix34::Scale))
                .addFunction("Normalize", &Matrix34::Normalize)
                .addFunction("Dot", &Matrix34::Dot)
                .addFunction("Rotate", &Matrix34::Rotate)
                .addFunction("RotateFull", &Matrix34::RotateFull)

                .addFunction("Zero", &Matrix34::Zero)
                .addFunction("MakeRotateX", &Matrix34::MakeRotateX)
                .addFunction("MakeRotateY", &Matrix34::MakeRotateY)
                .addFunction("MakeRotateZ", &Matrix34::MakeRotateZ)
                .addFunction("MakeScale", static_cast<void(Matrix34::*)(float, float, float)>(&Matrix34::MakeScale))
                .addFunction("MakeRotate", &Matrix34::MakeRotate)
                .addFunction("Transform", static_cast<Vector3(Matrix34::*)(const Vector3&)const>(&Matrix34::Transform))
                .addFunction("RotateX", &Matrix34::RotateX)
                .addFunction("RotateY", &Matrix34::RotateY)
                .addFunction("RotateZ", &Matrix34::RotateZ)
                .addFunction("LookAt", &Matrix34::LookAt)

                .addFunction("Set", &Matrix34::Set)
                .endClass();
        }
    };

    ASSERT_SIZEOF(Matrix34, 0x30);

    class Matrix44 {
    private:
        Matrix34 toMatrix34Lua() const
        {
            Matrix34 returnMtx = Matrix34();
            this->ToMatrix34(returnMtx);
            return returnMtx;
        }
    public:
        float m00;
        float m01;
        float m02;
        float m03;

        float m10;
        float m11;
        float m12;
        float m13;

        float m20;
        float m21;
        float m22;
        float m23;

        float m30;
        float m31;
        float m32;
        float m33;

        Vector4 GetColumn(int column) const
        {
            switch (column)
            {
            case 0:
                return Vector4(this->m00, this->m10, this->m20, this->m30);
            case 1:
                return Vector4(this->m01, this->m11, this->m21, this->m31);
            case 2:
                return Vector4(this->m02, this->m12, this->m22, this->m32);
            default:
                return Vector4();
            }
        }

        Vector4 GetRow(int row) const
        {
            switch (row)
            {
            case 0:
                return Vector4(this->m00, this->m01, this->m02, this->m03);
            case 1:
                return Vector4(this->m10, this->m11, this->m12, this->m13);
            case 2:
                return Vector4(this->m20, this->m21, this->m22, this->m23);
            case 3:
                return Vector4(this->m30, this->m31, this->m32, this->m33);
            default:
                return Vector4();
            }
        }

        void SetColumn(int column, Vector4 value)
        {
            switch (column)
            {
            case 0:
                this->m00 = value.X;
                this->m10 = value.Y;
                this->m20 = value.Z;
                this->m30 = value.W;
                break;
            case 1:
                this->m01 = value.X;
                this->m11 = value.Y;
                this->m21 = value.Z;
                this->m31 = value.W;
                break;
            case 2:
                this->m02 = value.X;
                this->m12 = value.Y;
                this->m22 = value.Z;
                this->m32 = value.W;
                break;
            }
        }

        void SetRow(int row, Vector4 value)
        {
            switch (row)
            {
            case 0:
                this->m00 = value.X;
                this->m01 = value.Y;
                this->m02 = value.Z;
                this->m03 = value.W;
                break;
            case 1:
                this->m10 = value.X;
                this->m11 = value.Y;
                this->m12 = value.Z;
                this->m13 = value.W;
                break;
            case 2:
                this->m20 = value.X;
                this->m21 = value.Y;
                this->m22 = value.Z;
                this->m23 = value.W;
                break;
            case 3:
                this->m30 = value.X;
                this->m31 = value.Y;
                this->m32 = value.Z;
                this->m33 = value.W;
                break;
            }
        }

        AGE_API void Identity(void)                                            { hook::Thunk<0x4C0730>::Call<void>(this); }
        AGE_API void Transpose(void)                                           { hook::Thunk<0x4C0920>::Call<void>(this); }
        AGE_API void Zero(void)                                                { hook::Thunk<0x4C0770>::Call<void>(this); }
        AGE_API void Hermite(void)                                             { hook::Thunk<0x4C0870>::Call<void>(this); }
        AGE_API void Bezier(void)                                              { hook::Thunk<0x4C0820>::Call<void>(this); }
        AGE_API void CatmullRom(void) const                                    { hook::Thunk<0x4C08C0>::Call<void>(this); }
        AGE_API float Determinant(void) const                                  { return hook::Thunk<0x4C1320>::Call<float>(this); }
        AGE_API void Add(const Matrix44& a1)                                   { hook::Thunk<0x4C0A30>::Call<void>(this, &a1); }
        AGE_API void AddScaled(const Matrix44& a1, float a2)                   { hook::Thunk<0x4C0CB0>::Call<void>(this, &a1, a2); }
        AGE_API Matrix44 & Dot(const Matrix44& a1)                             { return hook::Thunk<0x4C0D50>::Call<Matrix44&>(this, &a1); }
        AGE_API void FastInverse(const Matrix44& a1)                           { hook::Thunk<0x4C1260>::Call<void>(this, &a1); }
        AGE_API void FromMatrix34(const Matrix34& a1)                          { hook::Thunk<0x4C0970>::Call<void>(this, &a1); }
        AGE_API void InvertTo(const Matrix44& a1) const                        { hook::Thunk<0x4C1510>::Call<void>(this, &a1); }
        AGE_API Matrix44 & MakeRotX(float a1)                                  { return hook::Thunk<0x4C1C00>::Call<Matrix44&>(this, a1); }
        AGE_API Matrix44 & MakeRotY(float a1)                                  { return hook::Thunk<0x4C1C50>::Call<Matrix44&>(this, a1); }
        AGE_API Matrix44 & MakeRotZ(float a1)                                  { return hook::Thunk<0x4C1CA0>::Call<Matrix44&>(this, a1); }
        AGE_API void Print(LPCSTR name) const                                  { hook::Thunk<0x4C1CF0>::Call<void>(this, name); }
        AGE_API void Set(const Matrix44& a1)                                   { hook::Thunk<0x4C07B0>::Call<void>(this, &a1); }
        AGE_API void Subtract(const Matrix44& a1)                              { hook::Thunk<0x4C0B70>::Call<void>(this, &a1); }
        AGE_API void ToMatrix34(Matrix34& a1) const                            { hook::Thunk<0x4C09E0>::Call<void>(this, &a1); }
        AGE_API void Transform4(const Vector3* a1, Vector4* a2, int a3) const  { hook::Thunk<0x4C1D60>::Call<void>(this, a1, a2, a3); }
        AGE_API static Matrix44 & Convert(Matrix44& output, const Matrix34& input)
                                                                               { return hook::StaticThunk<0x42FCF0>::Call<Matrix44&>(&output, &input); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Matrix44>("Matrix44")
                .addFactory([](float m00 = 0.0, float m01 = 0.0, float m02 = 0.0, float m03 = 0.0,
                    float m10 = 0.0, float m11 = 0.0, float m12 = 0.0, float m13 = 0.0,
                    float m20 = 0.0, float m21 = 0.0, float m22 = 0.0, float m23 = 0.0,
                    float m30 = 0.0, float m31 = 0.0, float m32 = 0.0, float m33 = 0.0) {
                auto mtx = Matrix44();
                mtx.m00 = m00;
                mtx.m01 = m01;
                mtx.m02 = m02;
                mtx.m03 = m03;

                mtx.m10 = m10;
                mtx.m11 = m11;
                mtx.m12 = m12;
                mtx.m13 = m13;

                mtx.m20 = m20;
                mtx.m21 = m21;
                mtx.m22 = m22;
                mtx.m23 = m23;

                mtx.m30 = m30;
                mtx.m31 = m31;
                mtx.m32 = m32;
                mtx.m33 = m33;
                return mtx;
            }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>,
                _opt<float>, _opt<float>, _opt<float>, _opt<float>,
                _opt<float>, _opt<float>, _opt<float>, _opt<float>,
                _opt<float>, _opt<float>, _opt<float>, _opt<float>))
                .addVariableRef("m00", &Matrix44::m00)
                .addVariableRef("m01", &Matrix44::m01)
                .addVariableRef("m02", &Matrix44::m02)
                .addVariableRef("m03", &Matrix44::m03)
                .addVariableRef("m10", &Matrix44::m10)
                .addVariableRef("m11", &Matrix44::m11)
                .addVariableRef("m12", &Matrix44::m12)
                .addVariableRef("m13", &Matrix44::m13)
                .addVariableRef("m20", &Matrix44::m20)
                .addVariableRef("m21", &Matrix44::m21)
                .addVariableRef("m22", &Matrix44::m22)
                .addVariableRef("m23", &Matrix44::m23)
                .addVariableRef("m30", &Matrix44::m30)
                .addVariableRef("m31", &Matrix44::m31)
                .addVariableRef("m32", &Matrix44::m32)
                .addVariableRef("m33", &Matrix44::m33)
                .addStaticFunction("Convert", &Convert)

                .addFunction("GetColumn", &GetColumn)
                .addFunction("GetRow", &GetRow)
                .addFunction("SetColumn", &SetColumn)
                .addFunction("SetRow", &SetRow)

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
                .addFunction("ToMatrix34", &toMatrix34Lua)
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

    ASSERT_SIZEOF(Matrix44, 0x40);

}