#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class cityTimeWeatherLighting;
    extern class sdlPage16;

    // External declarations
    extern class gfxTexture;
    extern class Stream;

    namespace $
    {
        namespace sdlCommon
        {
            HOOK_API AGEHook<0x45CBC0>::Func<bool> BACKFACE;
            HOOK_API AGEHook<0x448090>::Func<void> UpdateLighting;
        }
        namespace sdlPage16
        {
            HOOK_API AGEHook<0x45A4E0>::MemberFunc<void> $$ctor;

            HOOK_API AGEHook<0x448330>::MemberFunc<void> Draw;

            HOOK_API AGEHook<0x45A560>::MemberFunc<void> ArcMap;
            HOOK_API AGEHook<0x45A760>::MemberFunc<void> WallMap;
            HOOK_API AGEHook<0x45A900>::MemberFunc<bool> PointInPerimeter;

            HOOK_API AGEHook<0x45A9F0>::MemberFunc<void> GetCentroid;
            HOOK_API AGEHook<0x45D110>::MemberFunc<int> GetPerimeterCount;
            HOOK_API AGEHook<0x45D120>::MemberFunc<int> GetPerimeterVertexIndex;
            HOOK_API AGEHook<0x45D140>::MemberFunc<const MM2::Vector3 &> GetCodedVertex;
            HOOK_API AGEHook<0x45D160>::MemberFunc<float> GetFloat;
            HOOK_API AGEHook<0x45D170>::MemberFunc<MM2::gfxTexture *> GetTexture;

            HOOK_API AGEHook<0x450880>::Func<uint> GetShadedColor$1;
            HOOK_API AGEHook<0x450910>::Func<uint> GetShadedColor$2;

            HOOK_API AGEHook<0x45BF90>::Func<MM2::sdlPage16 *> LoadBinary;
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
        static AGEHook<0x62B090>::Type<Vector3> sm_CamPos;
        static AGEHook<0x62B0A0>::Type<uint> sm_LightTable;

        AGE_API static void UpdateLighting(void) {
            $::sdlCommon::UpdateLighting();
        }

        AGE_API static bool BACKFACE(const Vector3 &vec1, const Vector3 &vec2) {
            return $::sdlCommon::BACKFACE(&vec1, &vec2);
        }
    };

    class sdlPage16 {
    public:
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

    HOOK_API AGEHook<0x445820>::MemberFunc<void> $cityLevel_DrawRooms;
    HOOK_API AGEHook<0x443E50>::MemberFunc<void> $cityLevel_SetObjectDetail;

    HOOK_API AGEHook<0x6299A8>::Type<cityTimeWeatherLighting[16]> timeWeathers;
}