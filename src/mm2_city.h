#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class cityTimeWeatherLighting;
    class sdlPage16;

    // External declarations
    extern class gfxTexture;
    extern class Stream;

    namespace $
    {
        HOOK_EXPORT(0x6299A8, _Type<cityTimeWeatherLighting[16]>, timeWeathers);

        namespace cityLevel
        {
            HOOK_EXPORT(0x445820, _Func<void>::ThisCall, DrawRooms);
            HOOK_EXPORT(0x443E50, _Func<void>::ThisCall, SetObjectDetail);
        }
        namespace sdlCommon
        {
            HOOK_EXPORT(0x45CBC0, _Func<bool>, BACKFACE);
            HOOK_EXPORT(0x448090, _Func<void>, UpdateLighting);
        }
        namespace sdlPage16
        {
            HOOK_EXPORT(0x45A4E0, _MemberFunc<void>, $$ctor);

            HOOK_EXPORT(0x448330, _MemberFunc<void>, Draw);

            HOOK_EXPORT(0x45A560, _MemberFunc<void>, ArcMap);
            HOOK_EXPORT(0x45A760, _MemberFunc<void>, WallMap);
            HOOK_EXPORT(0x45A900, _MemberFunc<bool>, PointInPerimeter);

            HOOK_EXPORT(0x45A9F0, _MemberFunc<void>, GetCentroid);
            HOOK_EXPORT(0x45D110, _MemberFunc<int>, GetPerimeterCount);
            HOOK_EXPORT(0x45D120, _MemberFunc<int>, GetPerimeterVertexIndex);
            HOOK_EXPORT(0x45D140, _MemberFunc<const MM2::Vector3 &>, GetCodedVertex);
            HOOK_EXPORT(0x45D160, _MemberFunc<float>, GetFloat);
            HOOK_EXPORT(0x45D170, _MemberFunc<MM2::gfxTexture *>, GetTexture);

            HOOK_EXPORT(0x450880, _Func<uint>, GetShadedColor$1);
            HOOK_EXPORT(0x450910, _Func<uint>, GetShadedColor$2);

            HOOK_EXPORT(0x45BF90, _Func<MM2::sdlPage16 *>, LoadBinary);
        }
    }

    class cityTimeWeatherLighting {
    public:
        /*0x00*/float KeyHeading;
        /*0x04*/float KeyPitch;
        /*0x08*/Vector3 KeyColor;

        /*0x14*/float Fill1Heading;
        /*0x18*/float Fill1Pitch;
        /*0x1C*/Vector3 Fill1Color;

        /*0x28*/float Fill2Heading;
        /*0x2C*/float Fill2Pitch;
        /*0x30*/Vector3 Fill2Color;

        // "ambient light levels" that get calculated using the Ambient color?
        // doesn't care for any of the Key/Pitch values
        /*0x3C*/int AmbientLevel3; // fullbright
        /*0x40*/int AmbientLevel2;
        /*0x44*/int AmbientLevel1;

        /*0x48*/int Ambient; // looks like the alpha is ignored
    };

    class sdlCommon {
    public:
        static ageHook::Type<Vector3> sm_CamPos;
        static ageHook::Type<uint> sm_LightTable;

        AGE_API static void UpdateLighting(void) {
            $::sdlCommon::UpdateLighting();
        }

        AGE_API static bool BACKFACE(const Vector3 &vec1, const Vector3 &vec2) {
            return $::sdlCommon::BACKFACE(&vec1, &vec2);
        }
    };

    class sdlPage16 {
        char unk_00;
        char PerimeterCount;
        char unk_02;
        char unk_03;
        Vector3 *CodedVertices;
        float *Floats;
        int unk_0C;
        unsigned short *PerimeterVertices;
        unsigned short *Attributes;
        gfxTexture **Textures;
        uint unk_1C;
    public:
        AGE_API sdlPage16(int p1, int p2) {
            $::sdlPage16::$$ctor(this, p1, p2);
        }

        AGE_API static sdlPage16 * LoadBinary(Stream *stream) {
            return $::sdlPage16::LoadBinary(stream);
        }

        AGE_API void Draw(int lod, uint baseColor) const {
            $::sdlPage16::Draw(this, lod, baseColor);
        }

        AGE_API void ArcMap(float *p1, const ushort *p2, int p3, int p4, int p5) const {
            $::sdlPage16::ArcMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API void WallMap(float *p1, const ushort *p2, float p3, int p4, int p5) const {
            $::sdlPage16::WallMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API bool PointInPerimeter(float p1, float p2) const {
            return $::sdlPage16::PointInPerimeter(this, p1, p2);
        }

        AGE_API void GetCentroid(Vector3 &p1) const {
            $::sdlPage16::GetCentroid(this, &p1);
        }

        AGE_API int GetPerimeterCount(void) const {
            return $::sdlPage16::GetPerimeterCount(this);
        }

        AGE_API int GetPerimeterVertexIndex(int p1) const {
            return $::sdlPage16::GetPerimeterVertexIndex(this, p1);
        }

        AGE_API const Vector3 & GetCodedVertex(int p1) const {
            return $::sdlPage16::GetCodedVertex(this, p1);
        }

        AGE_API float GetFloat(int p1) const {
            return $::sdlPage16::GetFloat(this, p1);
        }

        AGE_API gfxTexture * GetTexture(int p1) const {
            return $::sdlPage16::GetTexture(this, p1);
        }

        /* these are originally private, but they're pretty helpful */

        AGE_API static uint GetShadedColor(uint p1, uint p2) {
            return $::sdlPage16::GetShadedColor$1(p1, p2);
        }

        AGE_API static uint GetShadedColor(uint p1, uint p2, uint p3) {
            return $::sdlPage16::GetShadedColor$2(p1, p2, p3);
        }
    };
}